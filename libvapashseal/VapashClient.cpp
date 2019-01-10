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
/** @file VapashClient.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2015
 */

#include "VapashClient.h"
#include "Vapash.h"
#include <boost/filesystem/path.hpp>
using namespace std;
using namespace dev;
using namespace dev::vap;
using namespace p2p;
namespace fs = boost::filesystem;

VapashClient& dev::vap::asVapashClient(Interface& _c)
{
	if (dynamic_cast<Vapash*>(_c.sealEngine()))
		return dynamic_cast<VapashClient&>(_c);
	throw InvalidSealEngine();
}

VapashClient* dev::vap::asVapashClient(Interface* _c)
{
	if (dynamic_cast<Vapash*>(_c->sealEngine()))
		return &dynamic_cast<VapashClient&>(*_c);
	throw InvalidSealEngine();
}

DEV_SIMPLE_EXCEPTION(ChainParamsNotVapash);

VapashClient::VapashClient(
	ChainParams const& _params,
	int _networkID,
	p2p::Host* _host,
	std::shared_ptr<GasPricer> _gpForAdoption,
	fs::path const& _dbPath,
	WithExisting _forceAction,
	TransactionQueue::Limits const& _limits
):
	Client(_params, _networkID, _host, _gpForAdoption, _dbPath, _forceAction, _limits)
{
	// will throw if we're not an Vapash seal engine.
	asVapashClient(*this);
}

VapashClient::~VapashClient()
{
	terminate();
}

Vapash* VapashClient::vapash() const
{
	return dynamic_cast<Vapash*>(Client::sealEngine());
}

bool VapashClient::isMining() const
{
	return vapash()->farm().isMining();
}

WorkingProgress VapashClient::miningProgress() const
{
	if (isMining())
		return vapash()->farm().miningProgress();
	return WorkingProgress();
}

u256 VapashClient::hashrate() const
{
	u256 r = externalHashrate();
	if (isMining())
		r += miningProgress().rate();
	return r;
}

std::tuple<h256, h256, h256> VapashClient::getVapashWork()
{
	// lock the work so a later submission isn't invalidated by processing a transaction elsewhere.
	// this will be reset as soon as a new block arrives, allowing more transactions to be processed.
	bool oldShould = shouldServeWork();
	m_lastGetWork = chrono::system_clock::now();

	if (!sealEngine()->shouldSeal(this))
		return std::tuple<h256, h256, h256>();

	// if this request has made us bother to serve work, prep it now.
	if (!oldShould && shouldServeWork())
		onPostStateChanged();
	else
		// otherwise, set this to true so that it gets prepped next time.
		m_remoteWorking = true;
	vapash()->manuallySetWork(m_sealingInfo);
	return std::tuple<h256, h256, h256>(m_sealingInfo.hash(WithoutSeal), Vapash::seedHash(m_sealingInfo), Vapash::boundary(m_sealingInfo));
}

bool VapashClient::submitVapashWork(h256 const& _mixHash, h64 const& _nonce)
{
	vapash()->manuallySubmitWork(_mixHash, _nonce);
	return true;
}

void VapashClient::setShouldPrecomputeDAG(bool _precompute)
{
	bytes trueBytes {1};
	bytes falseBytes {0};
	sealEngine()->setOption("precomputeDAG", _precompute ? trueBytes: falseBytes);
}

void VapashClient::submitExternalHashrate(u256 const& _rate, h256 const& _id)
{
	WriteGuard writeGuard(x_externalRates);
	m_externalRates[_id] = make_pair(_rate, chrono::steady_clock::now());
}

u256 VapashClient::externalHashrate() const
{
	u256 ret = 0;
	WriteGuard writeGuard(x_externalRates);
	for (auto i = m_externalRates.begin(); i != m_externalRates.end();)
		if (chrono::steady_clock::now() - i->second.second > chrono::seconds(5))
			i = m_externalRates.erase(i);
		else
			ret += i++->second.first;
	return ret;
}
