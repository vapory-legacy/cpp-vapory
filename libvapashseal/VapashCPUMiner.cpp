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
/** @file VapashCPUMiner.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 *
 * Determines the PoW algorithm.
 */

#include "VapashCPUMiner.h"
#include <thread>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <random>

using namespace std;
using namespace dev;
using namespace vap;

unsigned VapashCPUMiner::s_numInstances = 0;


VapashCPUMiner::VapashCPUMiner(GenericMiner<VapashProofOfWork>::ConstructionInfo const& _ci):
	GenericMiner<VapashProofOfWork>(_ci), Worker("miner" + toString(index()))
{
}

VapashCPUMiner::~VapashCPUMiner()
{
	terminate();
}

void VapashCPUMiner::kickOff()
{
	stopWorking();
	startWorking();
}

void VapashCPUMiner::pause()
{
	stopWorking();
}

void VapashCPUMiner::workLoop()
{
	auto tid = std::this_thread::get_id();
	static std::mt19937_64 s_eng((utcTime() + std::hash<decltype(tid)>()(tid)));

	uint64_t tryNonce = s_eng();
	vapash_return_value vapashReturn;

	WorkPackage w = work();

	VapashAux::FullType dag;
	while (!shouldStop() && !dag)
	{
		while (!shouldStop() && VapashAux::computeFull(w.seedHash, true) != 100)
			this_thread::sleep_for(chrono::milliseconds(500));
		dag = VapashAux::full(w.seedHash, false);
	}

	h256 boundary = w.boundary;
	unsigned hashCount = 1;
	for (; !shouldStop(); tryNonce++, hashCount++)
	{
		vapashReturn = vapash_full_compute(dag->full, *(vapash_h256_t*)w.headerHash().data(), tryNonce);
		h256 value = h256((uint8_t*)&vapashReturn.result, h256::ConstructFromPointer);
		if (value <= boundary && submitProof(VapashProofOfWork::Solution{(h64)(u64)tryNonce, h256((uint8_t*)&vapashReturn.mix_hash, h256::ConstructFromPointer)}))
			break;
		if (!(hashCount % 100))
			accumulateHashes(100);
	}
}

std::string VapashCPUMiner::platformInfo()
{
	string baseline = toString(std::thread::hardware_concurrency()) + "-thread CPU";
	return baseline;
}
