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

#pragma once

#include <libdevcore/Common.h>
#include <libdevcore/Exceptions.h>
#include <libdevcore/FixedHash.h>

#include <memory>

namespace dev
{

namespace vap
{

DEV_SIMPLE_EXCEPTION(FailedToReadSnapshotManifestFile);
DEV_SIMPLE_EXCEPTION(FailedToReadChunkFile);
DEV_SIMPLE_EXCEPTION(ChunkIsTooBig);
DEV_SIMPLE_EXCEPTION(ChunkDataCorrupted);
DEV_SIMPLE_EXCEPTION(FailedToGetUncompressedLength);
DEV_SIMPLE_EXCEPTION(FailedToUncompressedSnapshotChunk);

class SnapshotStorageFace
{
public:
	virtual ~SnapshotStorageFace() = default;

	virtual bytes readManifest() const = 0;

	virtual std::string readChunk(h256 const& _chunkHash) const = 0;
};


std::unique_ptr<SnapshotStorageFace> createSnapshotStorage(std::string const& _snapshotDirPath);

}
}
