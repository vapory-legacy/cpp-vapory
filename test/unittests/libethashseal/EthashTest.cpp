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
/** @file vapash.cpp
 * Vapash class testing.
 */

#include <boost/test/unit_test.hpp>
#include <test/tools/libtestvap/TestOutputHelper.h>
#include <libvapashseal/Vapash.h>

using namespace std;
using namespace dev;
using namespace dev::vap;
using namespace dev::test;

BOOST_FIXTURE_TEST_SUITE(VapashTests, TestOutputHelper)

BOOST_AUTO_TEST_CASE(calculateDifficultyByzantiumWithoutUncles)
{
	ChainOperationParams params;
	params.byzantiumForkBlock = u256(0x1000);

	Vapash vapash;
	vapash.setChainParams(params);

	BlockHeader parentHeader;
	parentHeader.clear();
	parentHeader.setNumber(0x2000);
	parentHeader.setTimestamp(100);
	parentHeader.setDifficulty(1000000);

	BlockHeader header;
	header.clear();
	header.setNumber(0x2001);
	header.setTimestamp(130);

	BOOST_REQUIRE_EQUAL(vapash.calculateDifficulty(header, parentHeader), 999024);
}

BOOST_AUTO_TEST_CASE(calculateDifficultyByzantiumWithUncles)
{
	ChainOperationParams params;
	params.byzantiumForkBlock = u256(0x1000);

	Vapash vapash;
	vapash.setChainParams(params);

	BlockHeader parentHeader;
	parentHeader.clear();
	parentHeader.setNumber(0x2000);
	parentHeader.setTimestamp(100);
	parentHeader.setDifficulty(1000000);
	parentHeader.setSha3Uncles(h256("0x949d991d685738352398dff73219ab19c62c06e6f8ce899fbae755d5127ed1ef"));

	BlockHeader header;
	header.clear();
	header.setNumber(0x2001);
	header.setTimestamp(130);

	BOOST_REQUIRE_EQUAL(vapash.calculateDifficulty(header, parentHeader), 999512);
}

BOOST_AUTO_TEST_CASE(calculateDifficultyByzantiumMaxAdjustment)
{
	ChainOperationParams params;
	params.byzantiumForkBlock = u256(0x1000);

	Vapash vapash;
	vapash.setChainParams(params);

	BlockHeader parentHeader;
	parentHeader.clear();
	parentHeader.setNumber(0x2000);
	parentHeader.setTimestamp(100);
	parentHeader.setDifficulty(1000000);

	BlockHeader header;
	header.clear();
	header.setNumber(0x2001);
	header.setTimestamp(1100);

	BOOST_REQUIRE_EQUAL(vapash.calculateDifficulty(header, parentHeader), 951688);
}

BOOST_AUTO_TEST_SUITE_END()
