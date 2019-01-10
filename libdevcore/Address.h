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
/// @file
/// This file defined Address alias for FixedHash of 160 bits and some
/// special Address constants.

#pragma once

#include "FixedHash.h"

namespace dev
{

/// An Vapory address: 20 bytes.
/// @NOTE This is not endian-specific; it's just a bunch of bytes.
using Address = h160;

/// A vector of Vapory addresses.
using Addresses = h160s;

/// A hash set of Vapory addresses.
using AddressHash = std::unordered_set<h160>;

/// The zero address.
extern Address const ZeroAddress;

/// The last address.
extern Address const MaxAddress;

/// The SYSTEM address.
extern Address const SystemAddress;

}

