#pragma once
#include "AdminVapFace.h"

namespace dev
{

namespace vap
{
class Client;
class TrivialGasPricer;
class KeyManager;
}

namespace rpc
{

class SessionManager;

class AdminVap: public AdminVapFace
{
public:
	AdminVap(vap::Client& _vap, vap::TrivialGasPricer& _gp, vap::KeyManager& _keyManager, SessionManager& _sm);

	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"admin", "1.0"}, RPCModule{"miner", "1.0"}};
	}

	virtual bool admin_vap_setMining(bool _on, std::string const& _session) override;
	virtual Json::Value admin_vap_blockQueueStatus(std::string const& _session) override;
	virtual bool admin_vap_setAskPrice(std::string const& _wei, std::string const& _session) override;
	virtual bool admin_vap_setBidPrice(std::string const& _wei, std::string const& _session) override;
	virtual Json::Value admin_vap_findBlock(std::string const& _blockHash, std::string const& _session) override;
	virtual std::string admin_vap_blockQueueFirstUnknown(std::string const& _session) override;
	virtual bool admin_vap_blockQueueRetryUnknown(std::string const& _session) override;
	virtual Json::Value admin_vap_allAccounts(std::string const& _session) override;
	virtual Json::Value admin_vap_newAccount(const Json::Value& _info, std::string const& _session) override;
	virtual bool admin_vap_setMiningBenefactor(std::string const& _uuidOrAddress, std::string const& _session) override;
	virtual Json::Value admin_vap_inspect(std::string const& _address, std::string const& _session) override;
	virtual Json::Value admin_vap_reprocess(std::string const& _blockNumberOrHash, std::string const& _session) override;
	virtual Json::Value admin_vap_vmTrace(std::string const& _blockNumberOrHash, int _txIndex, std::string const& _session) override;
	virtual Json::Value admin_vap_getReceiptByHashAndIndex(std::string const& _blockNumberOrHash, int _txIndex, std::string const& _session) override;
	virtual bool miner_start(int _threads) override;
	virtual bool miner_stop() override;
	virtual bool miner_setVaporbase(std::string const& _uuidOrAddress) override;
	virtual bool miner_setExtra(std::string const& _extraData) override;
	virtual bool miner_setGasPrice(std::string const& _gasPrice) override;
	virtual std::string miner_hashrate() override;

	virtual void setMiningBenefactorChanger(std::function<void(Address const&)> const& _f) { m_setMiningBenefactor = _f; }
private:
	vap::Client& m_vap;
	vap::TrivialGasPricer& m_gp;
	vap::KeyManager& m_keyManager;
	SessionManager& m_sm;
	std::function<void(Address const&)> m_setMiningBenefactor;

	h256 blockHash(std::string const& _blockNumberOrHash) const;
};

}
}
