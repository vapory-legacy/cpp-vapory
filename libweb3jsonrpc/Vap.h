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
/** @file Vap.h
 * @authors:
 *   Gav Wood <i@gavwood.com>
 *   Marek Kotewicz <marek@ethdev.com>
 * @date 2014
 */

#pragma once

#include <memory>
#include <iosfwd>
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/common/exception.h>
#include <libdevcore/Common.h>
#include "SessionManager.h"
#include "VapFace.h"


namespace dev
{
class NetworkFace;
class KeyPair;
namespace vap
{
class AccountHolder;
struct TransactionSkeleton;
class Interface;
}

}

namespace dev
{

namespace rpc
{

/**
 * @brief JSON-RPC api implementation
 */
class Vap: public dev::rpc::VapFace
{
public:
	Vap(vap::Interface& _vap, vap::AccountHolder& _vapAccounts);

	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"vap", "1.0"}};
	}

	vap::AccountHolder const& vapAccounts() const { return m_vapAccounts; }

	virtual std::string vap_protocolVersion() override;
	virtual std::string vap_hashrate() override;
	virtual std::string vap_coinbase() override;
	virtual bool vap_mining() override;
	virtual std::string vap_gasPrice() override;
	virtual Json::Value vap_accounts() override;
	virtual std::string vap_blockNumber()override;
	virtual std::string vap_getBalance(std::string const& _address, std::string const& _blockNumber) override;
	virtual std::string vap_getStorageAt(std::string const& _address, std::string const& _position, std::string const& _blockNumber) override;
	virtual std::string vap_getStorageRoot(std::string const& _address, std::string const& _blockNumber) override;
	virtual std::string vap_getTransactionCount(std::string const& _address, std::string const& _blockNumber) override;
	virtual std::string vap_pendingTransactions() override;
	virtual Json::Value vap_getBlockTransactionCountByHash(std::string const& _blockHash) override;
	virtual Json::Value vap_getBlockTransactionCountByNumber(std::string const& _blockNumber) override;
	virtual Json::Value vap_getUncleCountByBlockHash(std::string const& _blockHash) override;
	virtual Json::Value vap_getUncleCountByBlockNumber(std::string const& _blockNumber) override;
	virtual std::string vap_getCode(std::string const& _address, std::string const& _blockNumber) override;
	virtual std::string vap_sendTransaction(Json::Value const& _json) override;
	virtual std::string vap_call(Json::Value const& _json, std::string const& _blockNumber) override;
	virtual std::string vap_estimateGas(Json::Value const& _json) override;
	virtual bool vap_flush() override;
	virtual Json::Value vap_getBlockByHash(std::string const& _blockHash, bool _includeTransactions) override;
	virtual Json::Value vap_getBlockByNumber(std::string const& _blockNumber, bool _includeTransactions) override;
	virtual Json::Value vap_getTransactionByHash(std::string const& _transactionHash) override;
	virtual Json::Value vap_getTransactionByBlockHashAndIndex(std::string const& _blockHash, std::string const& _transactionIndex) override;
	virtual Json::Value vap_getTransactionByBlockNumberAndIndex(std::string const& _blockNumber, std::string const& _transactionIndex) override;
	virtual Json::Value vap_getTransactionReceipt(std::string const& _transactionHash) override;
	virtual Json::Value vap_getUncleByBlockHashAndIndex(std::string const& _blockHash, std::string const& _uncleIndex) override;
	virtual Json::Value vap_getUncleByBlockNumberAndIndex(std::string const& _blockNumber, std::string const& _uncleIndex) override;
	virtual std::string vap_newFilter(Json::Value const& _json) override;
	virtual std::string vap_newFilterEx(Json::Value const& _json) override;
	virtual std::string vap_newBlockFilter() override;
	virtual std::string vap_newPendingTransactionFilter() override;
	virtual bool vap_uninstallFilter(std::string const& _filterId) override;
	virtual Json::Value vap_getFilterChanges(std::string const& _filterId) override;
	virtual Json::Value vap_getFilterChangesEx(std::string const& _filterId) override;
	virtual Json::Value vap_getFilterLogs(std::string const& _filterId) override;
	virtual Json::Value vap_getFilterLogsEx(std::string const& _filterId) override;
	virtual Json::Value vap_getLogs(Json::Value const& _json) override;
	virtual Json::Value vap_getLogsEx(Json::Value const& _json) override;
	virtual Json::Value vap_getWork() override;
	virtual bool vap_submitWork(std::string const& _nonce, std::string const&, std::string const& _mixHash) override;
	virtual bool vap_submitHashrate(std::string const& _hashes, std::string const& _id) override;
	virtual std::string vap_register(std::string const& _address) override;
	virtual bool vap_unregister(std::string const& _accountId) override;
	virtual Json::Value vap_fetchQueuedTransactions(std::string const& _accountId) override;
	virtual std::string vap_signTransaction(Json::Value const& _transaction) override;
	virtual Json::Value vap_inspectTransaction(std::string const& _rlp) override;
	virtual std::string vap_sendRawTransaction(std::string const& _rlp) override;
	virtual bool vap_notePassword(std::string const&) override { return false; }
	virtual Json::Value vap_syncing() override;
	
	void setTransactionDefaults(vap::TransactionSkeleton& _t);
protected:

	vap::Interface* client() { return &m_vap; }
	
	vap::Interface& m_vap;
	vap::AccountHolder& m_vapAccounts;

};

}
} //namespace dev
