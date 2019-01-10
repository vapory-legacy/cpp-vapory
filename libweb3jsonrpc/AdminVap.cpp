#include <jsonrpccpp/common/exception.h>
#include <libdevcore/CommonJS.h>
#include <libvapcore/KeyManager.h>
#include <libvapory/Client.h>
#include <libvapory/Executive.h>
#include <libvapashseal/VapashClient.h>
#include "AdminVap.h"
#include "SessionManager.h"
#include "JsonHelper.h"
using namespace std;
using namespace dev;
using namespace dev::rpc;
using namespace dev::vap;

AdminVap::AdminVap(vap::Client& _vap, vap::TrivialGasPricer& _gp, vap::KeyManager& _keyManager, SessionManager& _sm):
	m_vap(_vap),
	m_gp(_gp),
	m_keyManager(_keyManager),
	m_sm(_sm)
{}

bool AdminVap::admin_vap_setMining(bool _on, string const& _session)
{
	RPC_ADMIN;
	if (_on)
		m_vap.startSealing();
	else
		m_vap.stopSealing();
	return true;
}

Json::Value AdminVap::admin_vap_blockQueueStatus(string const& _session)
{
	RPC_ADMIN;
	Json::Value ret;
	BlockQueueStatus bqs = m_vap.blockQueue().status();
	ret["importing"] = (int)bqs.importing;
	ret["verified"] = (int)bqs.verified;
	ret["verifying"] = (int)bqs.verifying;
	ret["unverified"] = (int)bqs.unverified;
	ret["future"] = (int)bqs.future;
	ret["unknown"] = (int)bqs.unknown;
	ret["bad"] = (int)bqs.bad;
	return ret;
}

bool AdminVap::admin_vap_setAskPrice(string const& _wei, string const& _session)
{
	RPC_ADMIN;
	m_gp.setAsk(jsToU256(_wei));
	return true;
}

bool AdminVap::admin_vap_setBidPrice(string const& _wei, string const& _session)
{
	RPC_ADMIN;
	m_gp.setBid(jsToU256(_wei));
	return true;
}

Json::Value AdminVap::admin_vap_findBlock(string const& _blockHash, string const& _session)
{
	RPC_ADMIN;
	h256 h(_blockHash);
	if (m_vap.blockChain().isKnown(h))
		return toJson(m_vap.blockChain().info(h));
	switch(m_vap.blockQueue().blockStatus(h))
	{
		case QueueStatus::Ready:
			return "ready";
		case QueueStatus::Importing:
			return "importing";
		case QueueStatus::UnknownParent:
			return "unknown parent";
		case QueueStatus::Bad:
			return "bad";
		default:
			return "unknown";
	}
}

string AdminVap::admin_vap_blockQueueFirstUnknown(string const& _session)
{
	RPC_ADMIN;
	return m_vap.blockQueue().firstUnknown().hex();
}

bool AdminVap::admin_vap_blockQueueRetryUnknown(string const& _session)
{
	RPC_ADMIN;
	m_vap.retryUnknown();
	return true;
}

Json::Value AdminVap::admin_vap_allAccounts(string const& _session)
{
	RPC_ADMIN;
	Json::Value ret;
	u256 total = 0;
	u256 pendingtotal = 0;
	Address beneficiary;
	for (auto const& address: m_keyManager.accounts())
	{
		auto pending = m_vap.balanceAt(address, PendingBlock);
		auto latest = m_vap.balanceAt(address, LatestBlock);
		Json::Value a;
		if (address == beneficiary)
			a["beneficiary"] = true;
		a["address"] = toJS(address);
		a["balance"] = toJS(latest);
		a["nicebalance"] = formatBalance(latest);
		a["pending"] = toJS(pending);
		a["nicepending"] = formatBalance(pending);
		ret["accounts"][m_keyManager.accountName(address)] = a;
		total += latest;
		pendingtotal += pending;
	}
	ret["total"] = toJS(total);
	ret["nicetotal"] = formatBalance(total);
	ret["pendingtotal"] = toJS(pendingtotal);
	ret["nicependingtotal"] = formatBalance(pendingtotal);
	return ret;
}

Json::Value AdminVap::admin_vap_newAccount(Json::Value const& _info, string const& _session)
{
	RPC_ADMIN;
	if (!_info.isMember("name"))
		throw jsonrpc::JsonRpcException("No member found: name");
	string name = _info["name"].asString();
	KeyPair kp = KeyPair::create();
	h128 uuid;
	if (_info.isMember("password"))
	{
		string password = _info["password"].asString();
		string hint = _info["passwordHint"].asString();
		uuid = m_keyManager.import(kp.secret(), name, password, hint);
	}
	else
		uuid = m_keyManager.import(kp.secret(), name);
	Json::Value ret;
	ret["account"] = toJS(kp.pub());
	ret["uuid"] = toUUID(uuid);
	return ret;
}

bool AdminVap::admin_vap_setMiningBenefactor(string const& _uuidOrAddress, string const& _session)
{
	RPC_ADMIN;
	return miner_setVaporbase(_uuidOrAddress);
}

Json::Value AdminVap::admin_vap_inspect(string const& _address, string const& _session)
{
	RPC_ADMIN;
	if (!isHash<Address>(_address))
		throw jsonrpc::JsonRpcException("Invalid address given.");
	
	Json::Value ret;
	auto h = Address(fromHex(_address));
	ret["storage"] = toJson(m_vap.storageAt(h, PendingBlock));
	ret["balance"] = toJS(m_vap.balanceAt(h, PendingBlock));
	ret["nonce"] = toJS(m_vap.countAt(h, PendingBlock));
	ret["code"] = toJS(m_vap.codeAt(h, PendingBlock));
	return ret;
}

h256 AdminVap::blockHash(string const& _blockNumberOrHash) const
{
	if (isHash<h256>(_blockNumberOrHash))
		return h256(_blockNumberOrHash.substr(_blockNumberOrHash.size() - 64, 64));
	try
	{
		return m_vap.blockChain().numberHash(stoul(_blockNumberOrHash));
	}
	catch (...)
	{
		throw jsonrpc::JsonRpcException("Invalid argument");
	}
}

Json::Value AdminVap::admin_vap_reprocess(string const& _blockNumberOrHash, string const& _session)
{
	RPC_ADMIN;
	Json::Value ret;
	PopulationStatistics ps;
	m_vap.block(blockHash(_blockNumberOrHash), &ps);
	ret["enact"] = ps.enact;
	ret["verify"] = ps.verify;
	ret["total"] = ps.verify + ps.enact;
	return ret;
}

Json::Value AdminVap::admin_vap_vmTrace(string const& _blockNumberOrHash, int _txIndex, string const& _session)
{
	RPC_ADMIN;
	
	Json::Value ret;
	
	if (_txIndex < 0)
		throw jsonrpc::JsonRpcException("Negative index");
	Block block = m_vap.block(blockHash(_blockNumberOrHash));
	if ((unsigned)_txIndex < block.pending().size())
	{
		Transaction t = block.pending()[_txIndex];
		State s(State::Null);
		Executive e(s, block, _txIndex, m_vap.blockChain());
		try
		{
			StandardTrace st;
			st.setShowMnemonics();
			e.initialize(t);
			if (!e.execute())
				e.go(st.onOp());
			e.finalize();
			Json::Reader().parse(st.json(), ret);
		}
		catch(Exception const& _e)
		{
			cwarn << diagnostic_information(_e);
		}
	}
	
	return ret;
}

Json::Value AdminVap::admin_vap_getReceiptByHashAndIndex(string const& _blockNumberOrHash, int _txIndex, string const& _session)
{
	RPC_ADMIN;
	if (_txIndex < 0)
		throw jsonrpc::JsonRpcException("Negative index");
	auto h = blockHash(_blockNumberOrHash);
	if (!m_vap.blockChain().isKnown(h))
		throw jsonrpc::JsonRpcException("Invalid/unknown block.");
	auto rs = m_vap.blockChain().receipts(h);
	if ((unsigned)_txIndex >= rs.receipts.size())
		throw jsonrpc::JsonRpcException("Index too large.");
	return toJson(rs.receipts[_txIndex]);
}

bool AdminVap::miner_start(int)
{
	m_vap.startSealing();
	return true;
}

bool AdminVap::miner_stop()
{
	m_vap.stopSealing();
	return true;
}

bool AdminVap::miner_setVaporbase(string const& _uuidOrAddress)
{
	Address a;
	h128 uuid = fromUUID(_uuidOrAddress);
	if (uuid)
		a = m_keyManager.address(uuid);
	else if (isHash<Address>(_uuidOrAddress))
		a = Address(_uuidOrAddress);
	else
		throw jsonrpc::JsonRpcException("Invalid UUID or address");

	if (m_setMiningBenefactor)
		m_setMiningBenefactor(a);
	else
		m_vap.setAuthor(a);
	return true;
}

bool AdminVap::miner_setExtra(string const& _extraData)
{
	m_vap.setExtraData(asBytes(_extraData));
	return true;
}

bool AdminVap::miner_setGasPrice(string const& _gasPrice)
{
	m_gp.setAsk(jsToU256(_gasPrice));
	return true;
}

string AdminVap::miner_hashrate()
{
	VapashClient const* client = nullptr;
	try
	{
		client = asVapashClient(&m_vap);
	}
	catch (...)
	{
		throw jsonrpc::JsonRpcException("Hashrate not available - blockchain does not support mining.");
	}
	return toJS(client->hashrate());
}
