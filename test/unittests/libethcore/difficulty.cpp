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
/** @file difficulty.cpp
 * @author Christoph Jentzsch <cj@ethdev.com>
 * @date 2015
 * difficulty calculation tests.
 */

#include <test/tools/libtestvap/TestHelper.h>
#include <test/tools/fuzzTesting/fuzzHelper.h>
#include <libvapashseal/Vapash.h>
#include <libvapashseal/GenesisInfo.h>
#include <libvapory/ChainParams.h>
#include <boost/filesystem/path.hpp>
using namespace std;
using namespace dev;
using namespace dev::vap;

namespace fs = boost::filesystem;
namespace js = json_spirit;
std::string const c_testDifficulty = R"(
 "DifficultyTest[N]" : {
		"parentTimestamp" : "[PSTAMP]",
		"parentDifficulty" : "[PDIFF]",
		"parentUncles" : "[PUNCLS]",
		"currentTimestamp" : "[СSTAMP]",
		"currentBlockNumber" : "[CNUM]",
		"currentDifficulty" : "[CDIFF]"
	},
)";
h256 const nonzeroHash = sha3("whatever nonempty string");

void fillDifficulty(boost::filesystem::path const& _testFileFullName, Vapash& _sealEngine)
{
	int testN = 0;
	ostringstream finalTest;
	finalTest << "{\n";
	dev::test::TestOutputHelper testOutputHelper(900);

	for (int stampDelta = 0; stampDelta < 45; stampDelta+=2)
	{
		for (int pUncles = 0; pUncles < 2; pUncles++)
		{
			for (u256 blockNumber = 100000; blockNumber < 5000000; blockNumber += 100000)
			{
				testN++;
				string testName = "DifficultyTest"+toString(testN);
				if (!dev::test::TestOutputHelper::checkTest(testName))
					continue;

				u256 pStamp = dev::test::RandomCode::randomUniInt();
				u256 pDiff = dev::test::RandomCode::randomUniInt();
				u256 cStamp = pStamp + stampDelta;
				u256 cNum = blockNumber;

				BlockHeader parent;
				parent.setTimestamp(pStamp);
				parent.setDifficulty(pDiff);
				parent.setNumber(cNum - 1);
				parent.setSha3Uncles((pUncles == 0) ? EmptyListSHA3 : nonzeroHash);

				BlockHeader current;
				current.setTimestamp(cStamp);
				current.setNumber(cNum);

				string tmptest = c_testDifficulty;
				std::map<string, string> replaceMap;
				replaceMap["[N]"] = toString(testN);
				replaceMap["[PDIFF]"] = toCompactHexPrefixed(pDiff);
				replaceMap["[PSTAMP]"] = toCompactHexPrefixed(pStamp);
				replaceMap["[PUNCLS]"] = toCompactHexPrefixed(parent.sha3Uncles());
				replaceMap["[СSTAMP]"] = toCompactHexPrefixed(cStamp);
				replaceMap["[CNUM]"] = toCompactHexPrefixed(cNum);
				replaceMap["[CDIFF]"] = toCompactHexPrefixed(_sealEngine.calculateDifficulty(current, parent));

				dev::test::RandomCode::parseTestWithTypes(tmptest, replaceMap);
				finalTest << tmptest;
			}
		}
	}

	finalTest << "\n}";
	string testFile = finalTest.str();
	testFile = testFile.replace(testFile.find_last_of(","), 1, "");
	writeFile(_testFileFullName, asBytes(testFile));
}

void testDifficulty(fs::path const& _testFileFullName, Vapash& _sealEngine)
{
	//Test File
	js::mValue v;
	string s = contentsString(_testFileFullName);
	BOOST_REQUIRE_MESSAGE(s.length() > 0, "Contents of '" << _testFileFullName << "' is empty. Have you cloned the 'tests' repo branch develop?");
	js::read_string(s, v);
	dev::test::TestOutputHelper testOutputHelper(v.get_obj().size());

	for (auto& i: v.get_obj())
	{
		js::mObject o = i.second.get_obj();
		string testname = i.first;
		if (!dev::test::TestOutputHelper::checkTest(testname))
			continue;

		BOOST_REQUIRE_MESSAGE(o.count("parentTimestamp") > 0, testname + " missing parentTimestamp field");
		BOOST_REQUIRE_MESSAGE(o.count("parentDifficulty") > 0, testname + " missing parentDifficulty field");
		BOOST_REQUIRE_MESSAGE(o.count("currentBlockNumber") > 0, testname + " missing currentBlockNumber field");
		BOOST_REQUIRE_MESSAGE(o.count("parentUncles") > 0, testname + " missing parentUncles field");
		BOOST_REQUIRE_MESSAGE(o.count("currentTimestamp") > 0, testname + " missing currentTimestamp field");
		BOOST_REQUIRE_MESSAGE(o.count("currentBlockNumber") > 0, testname + " missing currentBlockNumber field");
		BOOST_REQUIRE_MESSAGE(o.count("currentDifficulty") > 0, testname + " missing currentDifficulty field");

		BlockHeader parent;
		parent.setTimestamp(test::toInt(o["parentTimestamp"]));
		parent.setDifficulty(test::toInt(o["parentDifficulty"]));
		parent.setNumber(test::toInt(o["currentBlockNumber"]) - 1);
		parent.setSha3Uncles(h256(o["parentUncles"].get_str()));

		BlockHeader current;
		current.setTimestamp(test::toInt(o["currentTimestamp"]));
		current.setNumber(test::toInt(o["currentBlockNumber"]));

		u256 difficulty = _sealEngine.calculateDifficulty(current, parent);
		BOOST_CHECK_EQUAL(difficulty, test::toInt(o["currentDifficulty"]));
	}
}

BOOST_AUTO_TEST_SUITE(DifficultyTests)

BOOST_AUTO_TEST_CASE(difficultyTestsFrontier)
{
	fs::path const testFileFullName = test::getTestPath() / fs::path("BasicTests/difficultyFrontier.json");

	Vapash sealEngine;
	sealEngine.setChainParams(ChainParams(genesisInfo(vap::Network::FrontierTest)));

	if (dev::test::Options::get().filltests)
		fillDifficulty(testFileFullName, sealEngine);

	testDifficulty(testFileFullName, sealEngine);
}

BOOST_AUTO_TEST_CASE(difficultyTestsRopsten)
{
	fs::path const testFileFullName = test::getTestPath() / fs::path("BasicTests/difficultyRopsten.json");

	Vapash sealEngine;
	sealEngine.setChainParams(ChainParams(genesisInfo(vap::Network::Ropsten)));

	if (dev::test::Options::get().filltests)
		fillDifficulty(testFileFullName, sealEngine);

	testDifficulty(testFileFullName, sealEngine);
}

BOOST_AUTO_TEST_CASE(difficultyTestsHomestead)
{
	fs::path const testFileFullName = test::getTestPath() / fs::path("BasicTests/difficultyHomestead.json");

	Vapash sealEngine;
	sealEngine.setChainParams(ChainParams(genesisInfo(vap::Network::HomesteadTest)));

	if (dev::test::Options::get().filltests)
		fillDifficulty(testFileFullName, sealEngine);

	testDifficulty(testFileFullName, sealEngine);
}

BOOST_AUTO_TEST_CASE(difficultyByzantium)
{
	fs::path const testFileFullName = test::getTestPath() / fs::path("BasicTests/difficultyByzantium.json");

	Vapash sealEngine;
	sealEngine.setChainParams(ChainParams(genesisInfo(vap::Network::ByzantiumTest)));

	if (dev::test::Options::get().filltests)
		fillDifficulty(testFileFullName, sealEngine);

	testDifficulty(testFileFullName, sealEngine);
}

BOOST_AUTO_TEST_CASE(difficultyTestsMainNetwork)
{
	fs::path const testFileFullName = test::getTestPath() / fs::path("BasicTests/difficultyMainNetwork.json");

	Vapash sealEngine;
	sealEngine.setChainParams(ChainParams(genesisInfo(vap::Network::MainNetworkTest)));

	if (dev::test::Options::get().filltests)
		fillDifficulty(testFileFullName, sealEngine);

	testDifficulty(testFileFullName, sealEngine);
}

BOOST_AUTO_TEST_CASE(difficultyTestsCustomMainNetwork)
{
	fs::path const testFileFullName = test::getTestPath() / fs::path("BasicTests/difficultyCustomMainNetwork.json");

	Vapash sealEngine;
	sealEngine.setChainParams(ChainParams(genesisInfo(vap::Network::MainNetworkTest)));

	if (dev::test::Options::get().filltests)
	{
		u256 byzantiumBlockNumber = 4370000;
		std::vector<u256> blockNumberVector = {byzantiumBlockNumber - 100000, byzantiumBlockNumber, byzantiumBlockNumber + 100000};
		std::vector<u256> parentDifficultyVector = {1000, 2048, 4000, 1000000};
		std::vector<int> timestampDeltaVector = {0, 1, 8, 10, 13, 20, 100, 800, 1000, 1500};

		int testN = 0;
		ostringstream finalTest;
		finalTest << "{\n";

		for (size_t bN = 0; bN < blockNumberVector.size(); bN++)
			for (size_t pdN = 0; pdN < parentDifficultyVector.size(); pdN++)
				for (int pUncles = 0; pUncles < 3; pUncles++)
					for (size_t tsN = 0; tsN < timestampDeltaVector.size(); tsN++)
					{
						testN++;
						int stampDelta = timestampDeltaVector.at(tsN);
						u256 blockNumber = blockNumberVector.at(bN);
						u256 pDiff = parentDifficultyVector.at(pdN);

						u256 pStamp = dev::test::RandomCode::randomUniInt();
						u256 cStamp = pStamp + stampDelta;
						u256 cNum = blockNumber;

						BlockHeader parent;
						parent.setTimestamp(pStamp);
						parent.setDifficulty(pDiff);
						parent.setNumber(cNum - 1);

						parent.setSha3Uncles((pUncles == 0) ? EmptyListSHA3 : nonzeroHash);

						BlockHeader current;
						current.setTimestamp(cStamp);
						current.setNumber(cNum);

						string tmptest = c_testDifficulty;
						std::map<string, string> replaceMap;
						replaceMap["[N]"] = toString(testN);
						replaceMap["[PDIFF]"] = toCompactHexPrefixed(pDiff);
						replaceMap["[PSTAMP]"] = toCompactHexPrefixed(pStamp);
						replaceMap["[PUNCLS]"] = toCompactHexPrefixed(parent.sha3Uncles());
						replaceMap["[СSTAMP]"] = toCompactHexPrefixed(cStamp);
						replaceMap["[CNUM]"] = toCompactHexPrefixed(cNum);
						replaceMap["[CDIFF]"] = toCompactHexPrefixed(sealEngine.calculateDifficulty(current, parent));

						dev::test::RandomCode::parseTestWithTypes(tmptest, replaceMap);
						finalTest << tmptest;
					}

		finalTest << "\n}";
		string testFile = finalTest.str();
		testFile = testFile.replace(testFile.find_last_of(","), 1, "");
		writeFile(testFileFullName, asBytes(testFile));
	}

	testDifficulty(testFileFullName, sealEngine);
}

BOOST_AUTO_TEST_CASE(basicDifficultyTest)
{
	fs::path const testPath = test::getTestPath() / fs::path("BasicTests/difficulty.json");

	Vapash sealEngine;
	sealEngine.setChainParams(ChainParams(genesisInfo(vap::Network::MainNetwork)));

	testDifficulty(testPath, sealEngine);
}

BOOST_AUTO_TEST_SUITE_END()
