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
/** @file VapashClient.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <libvapory/Client.h>
#include <boost/filesystem/path.hpp>
#include <tuple>

namespace dev
{
namespace vap
{

class Vapash;

DEV_SIMPLE_EXCEPTION(InvalidSealEngine);

class VapashClient: public Client
{
public:
	/// Trivial forwarding constructor.
	VapashClient(
		ChainParams const& _params,
		int _networkID,
		p2p::Host* _host,
		std::shared_ptr<GasPricer> _gpForAdoption,
		boost::filesystem::path const& _dbPath = boost::filesystem::path(),
		WithExisting _forceAction = WithExisting::Trust,
		TransactionQueue::Limits const& _l = TransactionQueue::Limits{1024, 1024}
	);

	Vapash* vapash() const;

	/// Enable/disable precomputing of the DAG for next epoch
	void setShouldPrecomputeDAG(bool _precompute);

	/// Are we mining now?
	bool isMining() const;

	/// The hashrate...
	u256 hashrate() const;

	/// Check the progress of the mining.
	WorkingProgress miningProgress() const;

	/// @returns true only if it's worth bothering to prep the mining block.
	bool shouldServeWork() const { return m_bq.items().first == 0 && (isMining() || remoteActive()); }

	/// Update to the latest transactions and get hash of the current block to be mined minus the
	/// nonce (the 'work hash') and the difficulty to be met.
	/// @returns Tuple of hash without seal, seed hash, target boundary.
	std::tuple<h256, h256, h256> getVapashWork();

	/** @brief Submit the proof for the proof-of-work.
	 * @param _s A valid solution.
	 * @return true if the solution was indeed valid and accepted.
	 */
	bool submitVapashWork(h256 const& _mixHash, h64 const& _nonce);

	void submitExternalHashrate(u256 const& _rate, h256 const& _id);

protected:
	u256 externalHashrate() const;

	// external hashrate
	mutable std::unordered_map<h256, std::pair<u256, std::chrono::steady_clock::time_point>> m_externalRates;
	mutable SharedMutex x_externalRates;
};

VapashClient& asVapashClient(Interface& _c);
VapashClient* asVapashClient(Interface* _c);

}
}
