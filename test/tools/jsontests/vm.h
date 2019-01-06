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
/** @file vm.h
 * @author Christoph Jentzsch <jentzsch.simulationsoftware@gmail.com>
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 * vm test functions.
 */

#pragma once

#include <test/tools/libtestvap/TestHelper.h>
#include <libdevcore/Log.h>
#include <libdevcore/CommonIO.h>
#include <libvvm/ExtVMFace.h>
#include <libvvm/VM.h>
#include <libvapory/Transaction.h>
#include <libvapory/ExtVM.h>
#include <libvapory/State.h>

#include <json_spirit/json_spirit.h>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <cstdint>

namespace dev
{
namespace vap
{
class LastBlockHashesFace;
}

namespace test
{

class FakeExtVM: public vap::ExtVMFace
{
public:
	FakeExtVM() = delete;
	FakeExtVM(vap::EnvInfo const& _envInfo, unsigned _depth = 0);

	virtual u256 store(u256 _n) override { return std::get<2>(addresses[myAddress])[_n]; }
	virtual void setStore(u256 _n, u256 _v) override { std::get<2>(addresses[myAddress])[_n] = _v; }
	virtual bool exists(Address _a) override { return !!addresses.count(_a); }
	virtual u256 balance(Address _a) override { return std::get<0>(addresses[_a]); }
	virtual void suicide(Address _a) override { std::get<0>(addresses[_a]) += std::get<0>(addresses[myAddress]); addresses.erase(myAddress); }
	virtual bytes const& codeAt(Address _a) override { return std::get<3>(addresses[_a]); }
	virtual size_t codeSizeAt(Address _a) override { return std::get<3>(addresses[_a]).size(); }
	virtual std::pair<h160, vap::owning_bytes_ref> create(u256 _endowment, u256& io_gas, bytesConstRef _init, vap::Instruction _op, u256 _salt, vap::OnOpFunc const&) override;
	virtual std::pair<bool, vap::owning_bytes_ref> call(vap::CallParameters&) override;
	virtual h256 blockHash(u256 _number) override;
	void setTransaction(Address _caller, u256 _value, u256 _gasPrice, bytes const& _data);
	void setContract(Address _myAddress, u256 _myBalance, u256 _myNonce, std::map<u256, u256> const& _storage, bytes const& _code);
	void set(Address _a, u256 _myBalance, u256 _myNonce, std::map<u256, u256> const& _storage, bytes const& _code);
	void reset(u256 _myBalance, u256 _myNonce, std::map<u256, u256> const& _storage);
	u256 doPosts();
	json_spirit::mObject exportEnv();
	static dev::vap::EnvInfo importEnv(json_spirit::mObject const& _o, vap::LastBlockHashesFace const& _lastBlockHashes);
	json_spirit::mObject exportState();
	void importState(json_spirit::mObject const& _object);
	json_spirit::mObject exportExec();
	void importExec(json_spirit::mObject const& _o);
	json_spirit::mArray exportCallCreates();
	void importCallCreates(json_spirit::mArray const& _callcreates);

	vap::OnOpFunc simpleTrace() const;

	std::map<Address, std::tuple<u256, u256, std::map<u256, u256>, bytes>> addresses;
	vap::Transactions callcreates;
	bytes thisTxData;
	bytes thisTxCode;
	u256 gas;
	u256 execGas;
};

} } // Namespace Close
