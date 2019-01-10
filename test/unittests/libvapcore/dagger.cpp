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
/** @file dagger.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 * Dashimoto test functions.
 */

#include <fstream>
#include <json_spirit/JsonSpiritHeaders.h>
#include <libdevcore/CommonIO.h>
#include <libvapashseal/Vapash.h>
#include <libvapashseal/VapashAux.h>
#include <test/tools/libtestvap/TestHelper.h>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
using namespace dev;
using namespace dev::vap;
using namespace dev::test;

namespace fs = boost::filesystem;
namespace js = json_spirit;

using dev::operator <<;

BOOST_FIXTURE_TEST_SUITE(DashimotoTests, TestOutputHelperFixture)

BOOST_AUTO_TEST_CASE(basic_test)
{
	fs::path const testPath = test::getTestPath() / fs::path("PoWTests");;

	cnote << "Testing Proof of Work...";
	js::mValue v;
	string const s = contentsString(testPath / fs::path("vapash_tests.json"));
	BOOST_REQUIRE_MESSAGE(s.length() > 0, "Contents of 'vapash_tests.json' is empty. Have you cloned the 'tests' repo branch develop?");
	js::read_string(s, v);
	for (auto& i: v.get_obj())
	{
		cnote << i.first;
		js::mObject& o = i.second.get_obj();
		vector<pair<string, string>> ss;
		BlockHeader header(fromHex(o["header"].get_str()), HeaderData);
		h256 headerHash(o["header_hash"].get_str());
		vap::Nonce nonce(o["nonce"].get_str());
		BOOST_REQUIRE_EQUAL(headerHash, header.hash(WithoutSeal));
		BOOST_REQUIRE_EQUAL(nonce, Vapash::nonce(header));

		unsigned cacheSize(o["cache_size"].get_int());
		h256 cacheHash(o["cache_hash"].get_str());
		BOOST_REQUIRE_EQUAL(VapashAux::get()->light(Vapash::seedHash(header))->size, cacheSize);
		BOOST_REQUIRE_EQUAL(sha3(VapashAux::get()->light(Vapash::seedHash(header))->data()), cacheHash);

#if TEST_FULL
		unsigned fullSize(o["full_size"].get_int());
		h256 fullHash(o["full_hash"].get_str());
		BOOST_REQUIRE_EQUAL(VapashAux::get()->full(Vapash::seedHash(header))->size(), fullSize);
		BOOST_REQUIRE_EQUAL(sha3(VapashAux::get()->full(Vapash::seedHash(header))->data()), fullHash);
#endif

		h256 result(o["result"].get_str());
		VapashProofOfWork::Result r = VapashAux::eval(Vapash::seedHash(header), header.hash(WithoutSeal), Vapash::nonce(header));
		BOOST_REQUIRE_EQUAL(r.value, result);
		BOOST_REQUIRE_EQUAL(r.mixHash, Vapash::mixHash(header));
	}
}

BOOST_AUTO_TEST_SUITE_END()
