#include "JitVM.h"

#include <libdevcore/Log.h>
#include <libvvm/VM.h>
#include <libvvm/VMFactory.h>

namespace dev
{
namespace vap
{

namespace
{

/// RAII wrapper for an vvm instance.
class VVM
{
public:
	VVM():
		m_instance(vvmjit_get_factory().create())
	{}

	~VVM()
	{
		m_instance->destroy(m_instance);
	}

	VVM(VVM const&) = delete;
	VVM& operator=(VVM) = delete;

	class Result
	{
	public:
		explicit Result(vvm_result const& _result):
			m_result(_result)
		{}

		~Result()
		{
			if (m_result.release)
				m_result.release(&m_result);
		}

		Result(Result&& _other) noexcept:
			m_result(_other.m_result)
		{
			// Disable releaser of the rvalue object.
			_other.m_result.release = nullptr;
		}

		Result(Result const&) = delete;
		Result& operator=(Result const&) = delete;

		vvm_result_code code() const
		{
			return m_result.code;
		}

		int64_t gasLeft() const
		{
			return m_result.gas_left;
		}

		bytesConstRef output() const
		{
			return {m_result.output_data, m_result.output_size};
		}

	private:
		vvm_result m_result;
	};

	/// Handy wrapper for vvm_execute().
	Result execute(ExtVMFace& _ext, int64_t gas)
	{
		auto mode = JitVM::toRevision(_ext.vvmSchedule());
		uint32_t flags = _ext.staticCall ? VVM_STATIC : 0;
		vvm_message msg = {toVvmC(_ext.myAddress), toVvmC(_ext.caller),
						   toVvmC(_ext.value), _ext.data.data(),
						   _ext.data.size(), toVvmC(_ext.codeHash), gas,
						   static_cast<int32_t>(_ext.depth), VVM_CALL, flags};
		return Result{m_instance->execute(
			m_instance, &_ext, mode, &msg, _ext.code.data(), _ext.code.size()
		)};
	}

	bool isCodeReady(vvm_revision _mode, uint32_t _flags, h256 _codeHash)
	{
		return m_instance->get_code_status(m_instance, _mode, _flags, toVvmC(_codeHash)) == VVM_READY;
	}

	void compile(vvm_revision _mode, uint32_t _flags, bytesConstRef _code, h256 _codeHash)
	{
		m_instance->prepare_code(
			m_instance, _mode, _flags, toVvmC(_codeHash), _code.data(), _code.size()
		);
	}

private:
	/// The VM instance created with m_interface.create().
	vvm_instance* m_instance = nullptr;
};

VVM& getJit()
{
	// Create VVM JIT instance by using VVM-C interface.
	static VVM jit;
	return jit;
}

}  // End of anonymous namespace.

owning_bytes_ref JitVM::exec(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp)
{
	bool rejected = false;
	// TODO: Rejecting transactions with gas limit > 2^63 can be used by attacker to take JIT out of scope
	rejected |= io_gas > std::numeric_limits<int64_t>::max(); // Do not accept requests with gas > 2^63 (int64 max)
	rejected |= _ext.envInfo().number() > std::numeric_limits<int64_t>::max();
	rejected |= _ext.envInfo().timestamp() > std::numeric_limits<int64_t>::max();
	rejected |= _ext.envInfo().gasLimit() > std::numeric_limits<int64_t>::max();
	if (rejected)
	{
		cwarn << "Execution rejected by VVM JIT (gas limit: " << io_gas << "), executing with interpreter";
		return VMFactory::create(VMKind::Interpreter)->exec(io_gas, _ext, _onOp);
	}

	auto gas = static_cast<int64_t>(io_gas);
	auto r = getJit().execute(_ext, gas);

	// TODO: Add VVM-C result codes mapping with exception types.
	if (r.code() == VVM_FAILURE)
		BOOST_THROW_EXCEPTION(OutOfGas());

	io_gas = r.gasLeft();
	// FIXME: Copy the output for now, but copyless version possible.
	owning_bytes_ref output{r.output().toVector(), 0, r.output().size()};

	if (r.code() == VVM_REVERT)
		throw RevertInstruction(std::move(output));

	return output;
}

vvm_revision JitVM::toRevision(VVMSchedule const& _schedule)
{
	if (_schedule.haveCreate2)
		return VVM_CONSTANTINOPLE;
	if (_schedule.haveRevert)
		return VVM_BYZANTIUM;
	if (_schedule.eip158Mode)
		return VVM_SPURIOUS_DRAGON;
	if (_schedule.eip150Mode)
		return VVM_TANGERINE_WHISTLE;
	if (_schedule.haveDelegateCall)
		return VVM_HOMESTEAD;
	return VVM_FRONTIER;
}

bool JitVM::isCodeReady(vvm_revision _mode, uint32_t _flags, h256 _codeHash)
{
	return getJit().isCodeReady(_mode, _flags, _codeHash);
}

void JitVM::compile(vvm_revision _mode, uint32_t _flags, bytesConstRef _code, h256 _codeHash)
{
	getJit().compile(_mode, _flags, _code, _codeHash);
}

}
}
