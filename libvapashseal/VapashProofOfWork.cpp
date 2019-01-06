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
/** @file VapashProofOfWork.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 *
 * Determines the PoW algorithm.
 */

#include "VapashProofOfWork.h"
#include "Vapash.h"
using namespace std;
using namespace chrono;
using namespace dev;
using namespace vap;

const unsigned VapashProofOfWork::defaultLocalWorkSize = 64;
const unsigned VapashProofOfWork::defaultGlobalWorkSizeMultiplier = 4096; // * CL_DEFAULT_LOCAL_WORK_SIZE
const unsigned VapashProofOfWork::defaultMSPerBatch = 0;
const VapashProofOfWork::WorkPackage VapashProofOfWork::NullWorkPackage = VapashProofOfWork::WorkPackage();

VapashProofOfWork::WorkPackage::WorkPackage(BlockHeader const& _bh):
	boundary(Vapash::boundary(_bh)),
	seedHash(Vapash::seedHash(_bh)),
	m_headerHash(_bh.hash(WithoutSeal))
{}

VapashProofOfWork::WorkPackage::WorkPackage(WorkPackage const& _other):
	boundary(_other.boundary),
	seedHash(_other.seedHash),
	m_headerHash(_other.headerHash())
{}

VapashProofOfWork::WorkPackage& VapashProofOfWork::WorkPackage::operator=(VapashProofOfWork::WorkPackage const& _other)
{
	if (this == &_other)
		return *this;
	boundary = _other.boundary;
	seedHash = _other.seedHash;
	h256 headerHash = _other.headerHash();
	{
		Guard l(m_headerHashLock);
		m_headerHash = std::move(headerHash);
	}
	return *this;
}
