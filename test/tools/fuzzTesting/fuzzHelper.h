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
/** @file fuzzHelper.h
 * @author Dimitry Khokhlov <winsvega@mail.ru>
 * @date 2015
 */

#include <string>
#include <random>
#include <boost/filesystem/path.hpp>

#include <test/tools/libtestvap/TestHelper.h>
#include <libdevcore/CommonIO.h>
#include <libdevcore/CommonData.h>
#include <libvvm/Instruction.h>
#include <test/tools/libtestvap/TestSuite.h>

#pragma once

//Test Templates
extern std::string const c_testExampleStateTest;
extern std::string const c_testExampleTransactionTest;
extern std::string const c_testExampleVMTest;
extern std::string const c_testExampleBlockchainTest;
extern std::string const c_testExampleRLPTest;

namespace dev
{
namespace test
{

using IntDistrib = std::uniform_int_distribution<>;
using DescreteDistrib = std::discrete_distribution<>;
using IntGenerator = std::function<int()>;

struct RandomCodeOptions
{
public:
	enum AddressType{
		Precompiled,
		StateAccount,
		PrecompiledOrStateOrCreate,
		All
	};
	RandomCodeOptions();
	void setWeight(dev::vap::Instruction _opCode, int _weight);
	void addAddress(dev::Address const& _address, AddressType _type);
	dev::Address getRandomAddress(AddressType _type = AddressType::All) const;
	int getWeightedRandomOpcode() const;

	bool useUndefinedOpCodes;
	int smartCodeProbability;
	int randomAddressProbability;
	int emptyCodeProbability;
	int emptyAddressProbability;
	int precompiledAddressProbability;

private:
	std::map<int, int> mapWeights;
	std::vector<dev::Address> precompiledAddressList;
	std::vector<dev::Address> stateAddressList;
};

enum class SizeStrictness
{
	Strict,
	Random
};

struct RlpDebug
{
	std::string rlp;
	int insertions;
};

class RandomCode
{
public:
	/// Generate random vm code
	static std::string generate(int _maxOpNumber, RandomCodeOptions const& _options);

	/// Replace keywords in given string with values
	static void parseTestWithTypes(std::string& _test, std::map<std::string, std::string> const& _varMap, RandomCodeOptions const& _options);
	static void parseTestWithTypes(std::string& _test, std::map<std::string, std::string> const& _varMap)
	{
		RandomCodeOptions defaultOptions;
		parseTestWithTypes(_test, _varMap, defaultOptions);
	}

	// Returns empty string if there was an error, a filled test otherwise.
	// prints test to the std::out or std::error if error when filling
	static std::string fillRandomTest(dev::test::TestSuite const& _testSuite, std::string const& _testFillerTemplate, dev::test::RandomCodeOptions const& _options);

	/// Generate random byte string of a given length
	static std::string rndByteSequence(int _length = 1, SizeStrictness _sizeType = SizeStrictness::Strict);

	/// Generate random rlp byte sequence of a given depth (e.g [[[]],[]]). max depth level = 20.
	/// The _debug string contains returned rlp string with analysed sections
	/// [] - length section/ or single byte rlp encoding
	/// () - decimal representation of length
	/// {1} - Array
	/// {2} - Array more than 55
	/// {3} - List
	/// {4} - List more than 55
	static std::string rndRLPSequence(int _depth, std::string& _debug);

	/// Generate random
	static std::string randomUniIntHex(u256 const& _minVal = 0, u256 const& _maxVal = std::numeric_limits<uint64_t>::max());
	static u256 randomUniInt(u256 const& _minVal = 0, u256 const& _maxVal = std::numeric_limits<uint64_t>::max());
	static int randomPercent() { refreshSeed(); return percentDist(gen); }
	static int weightedOpcode(std::vector<int>& _weights);

private:
	static std::string fillArguments(dev::vap::Instruction _opcode, RandomCodeOptions const& _options);
	static std::string getPushCode(int _value);
	static std::string getPushCode(std::string const& _hex);
	static int recursiveRLP(std::string& _result, int _depth, std::string& _debug);
	static void refreshSeed();
	static std::vector<std::string> getTypes();

	static std::mt19937_64 gen;				///< Random generator
	static IntDistrib opCodeDist;			///< 0..255 opcodes
	static IntDistrib percentDist;			///< 0..100 percent
	static IntDistrib opLengDist;			///< 1..32  byte string
	static IntDistrib opMemrDist;			///< 1..10MB  byte string
	static IntDistrib uniIntDist;			///< 0..0x7fffffff

	static IntGenerator randUniIntGen;		///< Generate random UniformInt from uniIntDist
	static IntGenerator randOpCodeGen;		///< Generate random value from opCodeDist
	static IntGenerator randOpLengGen;		///< Generate random length from opLengDist
	static IntGenerator randOpMemrGen;		///< Generate random length from opMemrDist
};

}
}
