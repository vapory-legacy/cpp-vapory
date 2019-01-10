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
/** @file
 * Helper class for managing data when running state tests
 */

#pragma once
#include <test/tools/libtestutils/Common.h>
#include <libvapashseal/GenesisInfo.h>
#include <test/tools/libtestvap/JsonSpiritHeaders.h>
#include <libvapory/State.h>

namespace dev
{
namespace test
{

class ImportTest
{
public:
	ImportTest(json_spirit::mObject const& _input, json_spirit::mObject& _output);

	// imports
	void importEnv(json_spirit::mObject const& _o);
	static void importState(json_spirit::mObject const& _o, vap::State& _state);
	static void importState(json_spirit::mObject const& _o, vap::State& _state, vap::AccountMaskMap& o_mask);
	static void importTransaction (json_spirit::mObject const& _o, vap::Transaction& o_tr);
	void importTransaction(json_spirit::mObject const& _o);
	static json_spirit::mObject makeAllFieldsHex(json_spirit::mObject const& _o, bool _isHeader = false);
	static void parseJsonStrValueIntoVector(json_spirit::mValue const& _json, std::vector<std::string>& _out);

	//check functions
	//check that networks in the vector are allowed
	static void checkAllowedNetwork(std::vector<std::string> const& _networks);
	static void checkBalance(vap::State const& _pre, vap::State const& _post, bigint _miningReward = 0);

	bytes executeTest();
	int exportTest();
	static int compareStates(vap::State const& _stateExpect, vap::State const& _statePost, vap::AccountMaskMap const _expectedStateOptions = vap::AccountMaskMap(), WhenError _throw = WhenError::Throw);
	bool checkGeneralTestSection(json_spirit::mObject const& _expects, std::vector<size_t>& _errorTransactions, std::string const& _network="") const;
	void traceStateDiff();

	vap::State m_statePre;
	vap::State m_statePost;

private:
	using ExecOutput = std::pair<vap::ExecutionResult, vap::TransactionReceipt>;
	std::tuple<vap::State, ExecOutput, vap::ChangeLog> executeTransaction(vap::Network const _sealEngineNetwork, vap::EnvInfo const& _env, vap::State const& _preState, vap::Transaction const& _tr);

	std::unique_ptr<vap::LastBlockHashesFace const> m_lastBlockHashes;
	std::unique_ptr<vap::EnvInfo> m_envInfo;
	vap::Transaction m_transaction;

	//General State Tests
	struct transactionToExecute
	{
		transactionToExecute(int d, int g, int v, vap::Transaction const& t):
			dataInd(d), gasInd(g), valInd(v), transaction(t), postState(0), netId(vap::Network::MainNetwork),
			output(std::make_pair(vap::ExecutionResult(), vap::TransactionReceipt(h256(), u256(), vap::LogEntries()))) {}
		int dataInd;
		int gasInd;
		int valInd;
		vap::Transaction transaction;
		vap::State postState;
		vap::ChangeLog changeLog;
		vap::Network netId;
		ExecOutput output;
	};
	std::vector<transactionToExecute> m_transactions;
	using StateAndMap = std::pair<vap::State, vap::AccountMaskMap>;
	using TrExpectSection = std::pair<transactionToExecute, StateAndMap>;
	bool checkGeneralTestSectionSearch(json_spirit::mObject const& _expects, std::vector<size_t>& _errorTransactions, std::string const& _network = "", TrExpectSection* _search = NULL) const;

    /// Create blockchain test fillers for specified _networks and test information (env, pre, txs)
    /// of Importtest then fill blockchain fillers into tests.
    void makeBlockchainTestFromStateTest(std::vector<vap::Network> const& _networks) const;

    json_spirit::mObject const& m_testInputObject;
	json_spirit::mObject& m_testOutputObject;
};

template<class T>
bool inArray(std::vector<T> const& _array, const T& _val)
{
	for (auto const& obj: _array)
		if (obj == _val)
			return true;
	return false;
}

} //namespace test
} //namespace dev
