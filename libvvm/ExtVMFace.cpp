/*
	This file is part of cpp-vapory.

	cpp-vapory is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-vapory is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-vapory.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ExtVMFace.h"

namespace dev
{
namespace vap
{
namespace
{

static_assert(sizeof(Address) == sizeof(vvm_address), "Address types size mismatch");
static_assert(alignof(Address) == alignof(vvm_address), "Address types alignment mismatch");

inline Address fromVvmC(vvm_address const& _addr)
{
	return reinterpret_cast<Address const&>(_addr);
}

static_assert(sizeof(h256) == sizeof(vvm_uint256be), "Hash types size mismatch");
static_assert(alignof(h256) == alignof(vvm_uint256be), "Hash types alignment mismatch");

inline u256 fromVvmC(vvm_uint256be const& _n)
{
	return fromBigEndian<u256>(_n.bytes);
}

int accountExists(vvm_context* _context, vvm_address const* _addr) noexcept
{
	auto& env = static_cast<ExtVMFace&>(*_context);
	Address addr = fromVvmC(*_addr);
	return env.exists(addr) ? 1 : 0;
}

void getStorage(
	vvm_uint256be* o_result,
	vvm_context* _context,
	vvm_address const* _addr,
	vvm_uint256be const* _key
) noexcept
{
	(void) _addr;
	auto& env = static_cast<ExtVMFace&>(*_context);
	assert(fromVvmC(*_addr) == env.myAddress);
	u256 key = fromVvmC(*_key);
	*o_result = toVvmC(env.store(key));
}

void setStorage(
	vvm_context* _context,
	vvm_address const* _addr,
	vvm_uint256be const* _key,
	vvm_uint256be const* _value
) noexcept
{
	(void) _addr;
	auto& env = static_cast<ExtVMFace&>(*_context);
	assert(fromVvmC(*_addr) == env.myAddress);
	u256 index = fromVvmC(*_key);
	u256 value = fromVvmC(*_value);
	if (value == 0 && env.store(index) != 0)                   // If delete
		env.sub.refunds += env.vvmSchedule().sstoreRefundGas;  // Increase refund counter

	env.setStore(index, value);    // Interface uses native endianness
}

void getBalance(
	vvm_uint256be* o_result,
	vvm_context* _context,
	vvm_address const* _addr
) noexcept
{
	auto& env = static_cast<ExtVMFace&>(*_context);
	*o_result = toVvmC(env.balance(fromVvmC(*_addr)));
}

size_t getCode(byte const** o_code, vvm_context* _context, vvm_address const* _addr)
{
	auto& env = static_cast<ExtVMFace&>(*_context);
	Address addr = fromVvmC(*_addr);
	if (o_code != nullptr)
	{
		auto& code = env.codeAt(addr);
		*o_code = code.data();
		return code.size();
	}
	return env.codeSizeAt(addr);
}

void selfdestruct(
	vvm_context* _context,
	vvm_address const* _addr,
	vvm_address const* _beneficiary
) noexcept
{
	(void) _addr;
	auto& env = static_cast<ExtVMFace&>(*_context);
	assert(fromVvmC(*_addr) == env.myAddress);
	env.suicide(fromVvmC(*_beneficiary));
}


void log(
	vvm_context* _context,
	vvm_address const* _addr,
	uint8_t const* _data,
	size_t _dataSize,
	vvm_uint256be const _topics[],
	size_t _numTopics
) noexcept
{
	(void) _addr;
	auto& env = static_cast<ExtVMFace&>(*_context);
	assert(fromVvmC(*_addr) == env.myAddress);
	h256 const* pTopics = reinterpret_cast<h256 const*>(_topics);
	env.log(h256s{pTopics, pTopics + _numTopics},
			bytesConstRef{_data, _dataSize});
}

void getTxContext(vvm_tx_context* result, vvm_context* _context) noexcept
{
	auto& env = static_cast<ExtVMFace&>(*_context);
	result->tx_gas_price = toVvmC(env.gasPrice);
	result->tx_origin = toVvmC(env.origin);
	result->block_coinbase = toVvmC(env.envInfo().author());
	result->block_number = static_cast<int64_t>(env.envInfo().number());
	result->block_timestamp = static_cast<int64_t>(env.envInfo().timestamp());
	result->block_gas_limit = static_cast<int64_t>(env.envInfo().gasLimit());
	result->block_difficulty = toVvmC(env.envInfo().difficulty());
}

void getBlockHash(vvm_uint256be* o_hash, vvm_context* _envPtr, int64_t _number)
{
	auto& env = static_cast<ExtVMFace&>(*_envPtr);
	*o_hash = toVvmC(env.blockHash(_number));
}

void create(vvm_result* o_result, ExtVMFace& _env, vvm_message const* _msg) noexcept
{
	u256 gas = _msg->gas;
	u256 value = fromVvmC(_msg->value);
	bytesConstRef init = {_msg->input, _msg->input_size};
	// ExtVM::create takes the sender address from .myAddress.
	assert(fromVvmC(_msg->sender) == _env.myAddress);

	h160 addr;
	owning_bytes_ref output;
	std::tie(addr, output) = _env.create(
		value, gas, init, Instruction::CREATE, u256(0), {}
	);
	o_result->gas_left = static_cast<int64_t>(gas);
	o_result->release = nullptr;
	if (addr)
	{
		o_result->status_code = VVM_SUCCESS;
		o_result->create_address = toVvmC(addr);
		o_result->output_data = nullptr;
		o_result->output_size = 0;
	}
	else
	{
		o_result->status_code = VVM_REVERT;

		// Pass the output to the VVM without a copy. The VVM will delete it
		// when finished with it.

		// First assign reference. References are not invalidated when vector
		// of bytes is moved. See `.takeBytes()` below.
		o_result->output_data = output.data();
		o_result->output_size = output.size();

		// Place a new vector of bytes containing output in result's reserved memory.
		auto* data = vvm_get_optional_data(o_result);
		static_assert(sizeof(bytes) <= sizeof(*data), "Vector is too big");
		new(data) bytes(output.takeBytes());
		// Set the destructor to delete the vector.
		o_result->release = [](vvm_result const* _result)
		{
			auto* data = vvm_get_const_optional_data(_result);
			auto& output = reinterpret_cast<bytes const&>(*data);
			// Explicitly call vector's destructor to release its data.
			// This is normal pattern when placement new operator is used.
			output.~bytes();
		};
	}
}

void call(vvm_result* o_result, vvm_context* _context, vvm_message const* _msg) noexcept
{
	assert(_msg->gas >= 0 && "Invalid gas value");
	auto& env = static_cast<ExtVMFace&>(*_context);

	// Handle CREATE separately.
	if (_msg->kind == VVM_CREATE)
		return create(o_result, env, _msg);

	CallParameters params;
	params.gas = _msg->gas;
	params.apparentValue = fromVvmC(_msg->value);
	params.valueTransfer =
		_msg->kind == VVM_DELEGATECALL ? 0 : params.apparentValue;
	params.senderAddress = fromVvmC(_msg->sender);
	params.codeAddress = fromVvmC(_msg->address);
	params.receiveAddress =
		_msg->kind == VVM_CALL ? params.codeAddress : env.myAddress;
	params.data = {_msg->input, _msg->input_size};
	params.staticCall = (_msg->flags & VVM_STATIC) != 0;
	params.onOp = {};

	bool success = false;
	owning_bytes_ref output;
	std::tie(success, output) = env.call(params);
	// FIXME: We have a mess here. It is hard to distinguish reverts from failures.
	// In first case we want to keep the output, in the second one the output
	// is optional and should not be passed to the contract, but can be useful
	// for VVM in general.
	o_result->status_code = success ? VVM_SUCCESS : VVM_REVERT;
	o_result->gas_left = static_cast<int64_t>(params.gas);

	// Pass the output to the VVM without a copy. The VVM will delete it
	// when finished with it.

	// First assign reference. References are not invalidated when vector
	// of bytes is moved. See `.takeBytes()` below.
	o_result->output_data = output.data();
	o_result->output_size = output.size();

	// Place a new vector of bytes containing output in result's reserved memory.
	auto* data = vvm_get_optional_data(o_result);
	static_assert(sizeof(bytes) <= sizeof(*data), "Vector is too big");
	new(data) bytes(output.takeBytes());
	// Set the destructor to delete the vector.
	o_result->release = [](vvm_result const* _result)
	{
		auto* data = vvm_get_const_optional_data(_result);
		auto& output = reinterpret_cast<bytes const&>(*data);
		// Explicitly call vector's destructor to release its data.
		// This is normal pattern when placement new operator is used.
		output.~bytes();
	};
}

vvm_context_fn_table const fnTable = {
	accountExists,
	getStorage,
	setStorage,
	getBalance,
	getCode,
	selfdestruct,
	vap::call,
	getTxContext,
	getBlockHash,
	vap::log
};

}

ExtVMFace::ExtVMFace(
	EnvInfo const& _envInfo,
	Address _myAddress,
	Address _caller,
	Address _origin,
	u256 _value,
	u256 _gasPrice,
	bytesConstRef _data,
	bytes _code,
	h256 const& _codeHash,
	unsigned _depth,
	bool _staticCall
):
	vvm_context{&fnTable},
	m_envInfo(_envInfo),
	myAddress(_myAddress),
	caller(_caller),
	origin(_origin),
	value(_value),
	gasPrice(_gasPrice),
	data(_data),
	code(std::move(_code)),
	codeHash(_codeHash),
	depth(_depth),
	staticCall(_staticCall)
{}

}
}
