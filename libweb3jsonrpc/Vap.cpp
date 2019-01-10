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
/** @file Vap.cpp
 * @authors:
 *   Gav Wood <i@gavwood.com>
 *   Marek Kotewicz <marek@ethdev.com>
 * @date 2014
 */

#include <csignal>
#include <jsonrpccpp/common/exception.h>
#include <libdevcore/CommonData.h>
#include <libvapory/Client.h>
#include <libvapashseal/VapashClient.h>
#include <libwebthree/WebThree.h>
#include <libvapcore/CommonJS.h>
#include <libweb3jsonrpc/JsonHelper.h>
#include "Vap.h"
#include "AccountHolder.h"

using namespace std;
using namespace jsonrpc;
using namespace dev;
using namespace vap;
using namespace shh;
using namespace dev::rpc;

Vap::Vap(vap::Interface& _vap, vap::AccountHolder& _vapAccounts):
	m_vap(_vap),
	m_vapAccounts(_vapAccounts)
{
}

string Vap::vap_protocolVersion()
{
	return toJS(vap::c_protocolVersion);
}

string Vap::vap_coinbase()
{
	return toJS(client()->author());
}

string Vap::vap_hashrate()
{
	try
	{
		return toJS(asVapashClient(client())->hashrate());
	}
	catch (InvalidSealEngine&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

bool Vap::vap_mining()
{
	try
	{
		return asVapashClient(client())->isMining();
	}
	catch (InvalidSealEngine&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_gasPrice()
{
	return toJS(client()->gasBidPrice());
}

Json::Value Vap::vap_accounts()
{
	return toJson(m_vapAccounts.allAccounts());
}

string Vap::vap_blockNumber()
{
	return toJS(client()->number());
}


string Vap::vap_getBalance(string const& _address, string const& _blockNumber)
{
	try
	{
		return toJS(client()->balanceAt(jsToAddress(_address), jsToBlockNumber(_blockNumber)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_getStorageAt(string const& _address, string const& _position, string const& _blockNumber)
{
	try
	{
		return toJS(toCompactBigEndian(client()->stateAt(jsToAddress(_address), jsToU256(_position), jsToBlockNumber(_blockNumber)), 32));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_getStorageRoot(string const& _address, string const& _blockNumber)
{
	try
	{
		return toString(client()->stateRootAt(jsToAddress(_address), jsToBlockNumber(_blockNumber)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_pendingTransactions()
{
	//Return list of transaction that being sent by local accounts
	Transactions ours;
	for (Transaction const& pending:client()->pending())
	{
		for (Address const& account:m_vapAccounts.allAccounts())
		{
			if (pending.sender() == account)
			{
				ours.push_back(pending);
				break;
			}
		}
	}

	return toJS(ours);
}

string Vap::vap_getTransactionCount(string const& _address, string const& _blockNumber)
{
	try
	{
		return toJS(client()->countAt(jsToAddress(_address), jsToBlockNumber(_blockNumber)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getBlockTransactionCountByHash(string const& _blockHash)
{
	try
	{
		h256 blockHash = jsToFixed<32>(_blockHash);
		if (!client()->isKnown(blockHash))
			return Json::Value(Json::nullValue);

		return toJS(client()->transactionCount(blockHash));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getBlockTransactionCountByNumber(string const& _blockNumber)
{
	try
	{
		BlockNumber blockNumber = jsToBlockNumber(_blockNumber);
		if (!client()->isKnown(blockNumber))
			return Json::Value(Json::nullValue);

		return toJS(client()->transactionCount(jsToBlockNumber(_blockNumber)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getUncleCountByBlockHash(string const& _blockHash)
{
	try
	{
		h256 blockHash = jsToFixed<32>(_blockHash);
		if (!client()->isKnown(blockHash))
			return Json::Value(Json::nullValue);

		return toJS(client()->uncleCount(blockHash));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getUncleCountByBlockNumber(string const& _blockNumber)
{
	try
	{
		BlockNumber blockNumber = jsToBlockNumber(_blockNumber);
		if (!client()->isKnown(blockNumber))
			return Json::Value(Json::nullValue);

		return toJS(client()->uncleCount(blockNumber));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_getCode(string const& _address, string const& _blockNumber)
{
	try
	{
		return toJS(client()->codeAt(jsToAddress(_address), jsToBlockNumber(_blockNumber)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

void Vap::setTransactionDefaults(TransactionSkeleton& _t)
{
	if (!_t.from)
		_t.from = m_vapAccounts.defaultTransactAccount();
}

string Vap::vap_sendTransaction(Json::Value const& _json)
{
	try
	{
		TransactionSkeleton t = toTransactionSkeleton(_json);
		setTransactionDefaults(t);
		TransactionNotification n = m_vapAccounts.authenticate(t);
		switch (n.r)
		{
		case TransactionRepercussion::Success:
			return toJS(n.hash);
		case TransactionRepercussion::ProxySuccess:
			return toJS(n.hash);// TODO: give back something more useful than an empty hash.
		case TransactionRepercussion::UnknownAccount:
			BOOST_THROW_EXCEPTION(JsonRpcException("Account unknown."));
		case TransactionRepercussion::Locked:
			BOOST_THROW_EXCEPTION(JsonRpcException("Account is locked."));
		case TransactionRepercussion::Refused:
			BOOST_THROW_EXCEPTION(JsonRpcException("Transaction rejected by user."));
		case TransactionRepercussion::Unknown:
			BOOST_THROW_EXCEPTION(JsonRpcException("Unknown reason."));
		}
	}
	catch (JsonRpcException&)
	{
		throw;
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
	BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	return string();
}

string Vap::vap_signTransaction(Json::Value const& _json)
{
	try
	{
		TransactionSkeleton t = toTransactionSkeleton(_json);
		setTransactionDefaults(t);
		TransactionNotification n = m_vapAccounts.authenticate(t);
		switch (n.r)
		{
		case TransactionRepercussion::Success:
			return toJS(n.hash);
		case TransactionRepercussion::ProxySuccess:
			return toJS(n.hash);// TODO: give back something more useful than an empty hash.
		default:
			// TODO: provide more useful information in the exception.
			BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
		}
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_inspectTransaction(std::string const& _rlp)
{
	try
	{
		return toJson(Transaction(jsToBytes(_rlp, OnFailed::Throw), CheckTransaction::Everything));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_sendRawTransaction(std::string const& _rlp)
{
	try
	{
		if (client()->injectTransaction(jsToBytes(_rlp, OnFailed::Throw)) == ImportResult::Success)
		{
			Transaction tx(jsToBytes(_rlp, OnFailed::Throw), CheckTransaction::None);
			return toJS(tx.sha3());
		}
		else
			return toJS(h256());
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_call(Json::Value const& _json, string const& _blockNumber)
{
	try
	{
		TransactionSkeleton t = toTransactionSkeleton(_json);
		setTransactionDefaults(t);
		ExecutionResult er = client()->call(t.from, t.value, t.to, t.data, t.gas, t.gasPrice, jsToBlockNumber(_blockNumber), FudgeFactor::Lenient);
		return toJS(er.output);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_estimateGas(Json::Value const& _json)
{
	try
	{
		TransactionSkeleton t = toTransactionSkeleton(_json);
		setTransactionDefaults(t);
		int64_t gas = static_cast<int64_t>(t.gas);
		return toJS(client()->estimateGas(t.from, t.value, t.to, t.data, gas, t.gasPrice, PendingBlock).first);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

bool Vap::vap_flush()
{
	client()->flushTransactions();
	return true;
}

Json::Value Vap::vap_getBlockByHash(string const& _blockHash, bool _includeTransactions)
{
	try
	{
		h256 h = jsToFixed<32>(_blockHash);
		if (!client()->isKnown(h))
			return Json::Value(Json::nullValue);

		if (_includeTransactions)
			return toJson(client()->blockInfo(h), client()->blockDetails(h), client()->uncleHashes(h), client()->transactions(h), client()->sealEngine());
		else
			return toJson(client()->blockInfo(h), client()->blockDetails(h), client()->uncleHashes(h), client()->transactionHashes(h), client()->sealEngine());
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getBlockByNumber(string const& _blockNumber, bool _includeTransactions)
{
	try
	{
		BlockNumber h = jsToBlockNumber(_blockNumber);
		if (!client()->isKnown(h))
			return Json::Value(Json::nullValue);

		if (_includeTransactions)
			return toJson(client()->blockInfo(h), client()->blockDetails(h), client()->uncleHashes(h), client()->transactions(h), client()->sealEngine());
		else
			return toJson(client()->blockInfo(h), client()->blockDetails(h), client()->uncleHashes(h), client()->transactionHashes(h), client()->sealEngine());
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getTransactionByHash(string const& _transactionHash)
{
	try
	{
		h256 h = jsToFixed<32>(_transactionHash);
		if (!client()->isKnownTransaction(h))
			return Json::Value(Json::nullValue);

		return toJson(client()->localisedTransaction(h));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getTransactionByBlockHashAndIndex(string const& _blockHash, string const& _transactionIndex)
{
	try
	{
		h256 bh = jsToFixed<32>(_blockHash);
		unsigned ti = jsToInt(_transactionIndex);
		if (!client()->isKnownTransaction(bh, ti))
			return Json::Value(Json::nullValue);

		return toJson(client()->localisedTransaction(bh, ti));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getTransactionByBlockNumberAndIndex(string const& _blockNumber, string const& _transactionIndex)
{
	try
	{
		BlockNumber bn = jsToBlockNumber(_blockNumber);
		h256 bh = client()->hashFromNumber(bn);
		unsigned ti = jsToInt(_transactionIndex);
		if (!client()->isKnownTransaction(bh, ti))
			return Json::Value(Json::nullValue);

		return toJson(client()->localisedTransaction(bh, ti));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getTransactionReceipt(string const& _transactionHash)
{
	try
	{
		h256 h = jsToFixed<32>(_transactionHash);
		if (!client()->isKnownTransaction(h))
			return Json::Value(Json::nullValue);

		return toJson(client()->localisedTransactionReceipt(h));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getUncleByBlockHashAndIndex(string const& _blockHash, string const& _uncleIndex)
{
	try
	{
		return toJson(client()->uncle(jsToFixed<32>(_blockHash), jsToInt(_uncleIndex)), client()->sealEngine());
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getUncleByBlockNumberAndIndex(string const& _blockNumber, string const& _uncleIndex)
{
	try
	{
		return toJson(client()->uncle(jsToBlockNumber(_blockNumber), jsToInt(_uncleIndex)), client()->sealEngine());
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_newFilter(Json::Value const& _json)
{
	try
	{
		return toJS(client()->installWatch(toLogFilter(_json, *client())));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_newFilterEx(Json::Value const& _json)
{
	try
	{
		return toJS(client()->installWatch(toLogFilter(_json)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_newBlockFilter()
{
	h256 filter = dev::vap::ChainChangedFilter;
	return toJS(client()->installWatch(filter));
}

string Vap::vap_newPendingTransactionFilter()
{
	h256 filter = dev::vap::PendingChangedFilter;
	return toJS(client()->installWatch(filter));
}

bool Vap::vap_uninstallFilter(string const& _filterId)
{
	try
	{
		return client()->uninstallWatch(jsToInt(_filterId));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getFilterChanges(string const& _filterId)
{
	try
	{
		int id = jsToInt(_filterId);
		auto entries = client()->checkWatch(id);
//		if (entries.size())
//			cnote << "FIRING WATCH" << id << entries.size();
		return toJson(entries);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getFilterChangesEx(string const& _filterId)
{
	try
	{
		int id = jsToInt(_filterId);
		auto entries = client()->checkWatch(id);
//		if (entries.size())
//			cnote << "FIRING WATCH" << id << entries.size();
		return toJsonByBlock(entries);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getFilterLogs(string const& _filterId)
{
	try
	{
		return toJson(client()->logs(jsToInt(_filterId)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getFilterLogsEx(string const& _filterId)
{
	try
	{
		return toJsonByBlock(client()->logs(jsToInt(_filterId)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getLogs(Json::Value const& _json)
{
	try
	{
		return toJson(client()->logs(toLogFilter(_json, *client())));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getLogsEx(Json::Value const& _json)
{
	try
	{
		return toJsonByBlock(client()->logs(toLogFilter(_json)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_getWork()
{
	try
	{
		Json::Value ret(Json::arrayValue);
		auto r = asVapashClient(client())->getVapashWork();
		ret.append(toJS(get<0>(r)));
		ret.append(toJS(get<1>(r)));
		ret.append(toJS(get<2>(r)));
		return ret;
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_syncing()
{
	dev::vap::SyncStatus sync = client()->syncStatus();
	if (sync.state == SyncState::Idle || !sync.majorSyncing)
		return Json::Value(false);

	Json::Value info(Json::objectValue);
	info["startingBlock"] = sync.startBlockNumber;
	info["highestBlock"] = sync.highestBlockNumber;
	info["currentBlock"] = sync.currentBlockNumber;
	return info;
}

bool Vap::vap_submitWork(string const& _nonce, string const&, string const& _mixHash)
{
	try
	{
		return asVapashClient(client())->submitVapashWork(jsToFixed<32>(_mixHash), jsToFixed<Nonce::size>(_nonce));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

bool Vap::vap_submitHashrate(string const& _hashes, string const& _id)
{
	try
	{
		asVapashClient(client())->submitExternalHashrate(jsToInt<32>(_hashes), jsToFixed<32>(_id));
		return true;
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

string Vap::vap_register(string const& _address)
{
	try
	{
		return toJS(m_vapAccounts.addProxyAccount(jsToAddress(_address)));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

bool Vap::vap_unregister(string const& _accountId)
{
	try
	{
		return m_vapAccounts.removeProxyAccount(jsToInt(_accountId));
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

Json::Value Vap::vap_fetchQueuedTransactions(string const& _accountId)
{
	try
	{
		auto id = jsToInt(_accountId);
		Json::Value ret(Json::arrayValue);
		// TODO: throw an error on no account with given id
		for (TransactionSkeleton const& t: m_vapAccounts.queuedTransactions(id))
			ret.append(toJson(t));
		m_vapAccounts.clearQueue(id);
		return ret;
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}
