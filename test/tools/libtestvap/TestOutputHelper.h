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
/** @file
 * Fixture class for boost output when running testvap
 */

#pragma once
#include <test/tools/libtestutils/Common.h>
#include <test/tools/libtestvap/JsonSpiritHeaders.h>

namespace dev
{
namespace test
{

class TestOutputHelper
{
public:
	TestOutputHelper(size_t _maxTests = 1) { TestOutputHelper::initTest(_maxTests); }
	static bool checkTest(std::string const& _testName);

	// Display percantage of completed tests to std::out. Has to be called before execution of every test.
	static void showProgress();
	static void setMaxTests(int _count) { m_maxTests = _count; }
	static void setCurrentTestFileName(std::string const& _name) { m_currentTestFileName = _name; }
	static void setCurrentTestName(std::string const& _name) { m_currentTestName = _name; }
	static std::string const& testName() { return m_currentTestName; }
	static std::string const& caseName() { return m_currentTestCaseName; }
	static std::string const& testFileName() { return m_currentTestFileName; }
	static void printTestExecStats();
	~TestOutputHelper() { TestOutputHelper::finishTest(); }
private:
	static void initTest(size_t _maxTests = 1);
	static void finishTest();
	static Timer m_timer;
	static size_t m_currTest;
	static size_t m_maxTests;
	static std::string m_currentTestName;
	static std::string m_currentTestCaseName;
	static std::string m_currentTestFileName;
	typedef std::pair<double, std::string> execTimeName;
	static std::vector<execTimeName> m_execTimeResults;
};

} //namespace test
} //namespace dev
