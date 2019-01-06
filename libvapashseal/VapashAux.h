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
/** @file VapashAux.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <condition_variable>
#include <libvapash/vapash.h>
#include <libdevcore/Log.h>
#include <libdevcore/Worker.h>
#include "VapashProofOfWork.h"
#include "Vapash.h"

namespace dev
{
namespace vap
{

struct DAGChannel: public LogChannel { static const char* name(); static const int verbosity = 1; };

class BlockHeader;

class VapashAux
{
public:
	~VapashAux();

	static VapashAux* get();

	struct LightAllocation
	{
		LightAllocation(h256 const& _seedHash);
		~LightAllocation();
		bytesConstRef data() const;
		VapashProofOfWork::Result compute(h256 const& _headerHash, Nonce const& _nonce) const;
		vapash_light_t light;
		uint64_t size;
	};

	struct FullAllocation
	{
		FullAllocation(vapash_light_t _light, vapash_callback_t _cb);
		~FullAllocation();
		VapashProofOfWork::Result compute(h256 const& _headerHash, Nonce const& _nonce) const;
		bytesConstRef data() const;
		uint64_t size() const { return vapash_full_dag_size(full); }
		vapash_full_t full;
	};

	using LightType = std::shared_ptr<LightAllocation>;
	using FullType = std::shared_ptr<FullAllocation>;

	static h256 seedHash(unsigned _number);
	static uint64_t number(h256 const& _seedHash);
	static uint64_t cacheSize(BlockHeader const& _header);
	static uint64_t dataSize(uint64_t _blockNumber);

	static LightType light(h256 const& _seedHash);

	static const uint64_t NotGenerating = (uint64_t)-1;
	/// Kicks off generation of DAG for @a _seedHash and @returns false or @returns true if ready.
	static unsigned computeFull(h256 const& _seedHash, bool _createIfMissing = true);
	/// Information on the generation progress.
	static std::pair<uint64_t, unsigned> fullGeneratingProgress() { return std::make_pair(get()->m_generatingFullNumber, get()->m_fullProgress); }
	/// Kicks off generation of DAG for @a _blocknumber and blocks until ready; @returns result or empty pointer if not existing and _createIfMissing is false.
	static FullType full(h256 const& _seedHash, bool _createIfMissing = false, std::function<int(unsigned)> const& _f = std::function<int(unsigned)>());

	static VapashProofOfWork::Result eval(h256 const& _seedHash, h256 const& _headerHash, Nonce const& _nonce);

private:
	VapashAux() {}

	/// Kicks off generation of DAG for @a _blocknumber and blocks until ready; @returns result.

	void killCache(h256 const& _s);

	static VapashAux* s_this;

	SharedMutex x_lights;
	std::unordered_map<h256, std::shared_ptr<LightAllocation>> m_lights;

	Mutex x_fulls;
	std::condition_variable m_fullsChanged;
	std::unordered_map<h256, std::weak_ptr<FullAllocation>> m_fulls;
	FullType m_lastUsedFull;
	std::unique_ptr<std::thread> m_fullGenerator;
	uint64_t m_generatingFullNumber = NotGenerating;
	unsigned m_fullProgress;

	Mutex x_epochs;
	std::unordered_map<h256, unsigned> m_epochs;
	h256s m_seedHashes;
};

}
}
