file(GLOB_RECURSE sources "*.cpp" "*.h" "*.sol")

# search for test names and create ctest tests
set(excludeSuites jsonrpc \"customTestSuite\" BlockQueueSuite)
set(allSuites jsonrpc)
foreach(file ${sources})
	file(STRINGS ${file} test_list_raw REGEX "BOOST_.*TEST_(SUITE|CASE|SUITE_END)")
	set(TestSuite "DEFAULT")
	set(TestSuitePath "")
	foreach(test_raw ${test_list_raw})
		string(REGEX REPLACE ".*TEST_(SUITE|CASE)\\(([^ ,\\)]*).*" "\\1 \\2" test ${test_raw})

		#skip disabled
		if (";${excludeSuites};" MATCHES ";${TestSuite};")
			continue()
		endif()

		if(test MATCHES "^SUITE .*")

			string(SUBSTRING ${test} 6 -1 TestSuite)
			set(TestSuitePath "${TestSuitePath}/${TestSuite}")

			if(FASTCTEST)
				if (";${excludeSuites};" MATCHES ";${TestSuite};")
					continue()
				endif()
				if (NOT ";${allSuites};" MATCHES ";${TestSuite};")
					string(SUBSTRING ${TestSuitePath} 1 -1 TestSuitePathFixed)
					list(APPEND allSuites ${TestSuite})
					separate_arguments(TESTVAP_ARGS)
					set(TestVapArgs -t ${TestSuitePathFixed} -- ${TESTVAP_ARGS})
					add_test(NAME ${TestSuitePathFixed} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test COMMAND testvap ${TestVapArgs})
				endif()
			endif()
		elseif(test MATCHES "^CASE .*")
			if(NOT FASTCTEST)
				if(NOT test MATCHES "^CASE &createRandom.*")
					string(SUBSTRING ${test} 5 -1 TestCase)
					string(SUBSTRING ${TestSuitePath} 1 -1 TestSuitePathFixed)
					separate_arguments(TESTVAP_ARGS)
					set(TestVapArgs -t ${TestSuitePathFixed}/${TestCase} -- ${TESTVAP_ARGS})
					add_test(NAME ${TestSuitePathFixed}/${TestCase} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test COMMAND testvap ${TestVapArgs})
				endif()
			endif()
		elseif (";${test_raw};" MATCHES "BOOST_AUTO_TEST_SUITE_END()")
			#encountered SUITE_END block. remove one test suite from the suite path.
			string(FIND ${TestSuitePath} "/" Position REVERSE)
			string(SUBSTRING ${TestSuitePath} 0 ${Position} TestSuitePath)
		endif()
	endforeach(test_raw)
endforeach(file)

hunter_add_package(yaml-cpp)
find_package(yaml-cpp CONFIG REQUIRED)

add_executable(testvap ${sources})
target_include_directories(testvap PRIVATE ${UTILS_INCLUDE_DIR})
target_link_libraries(testvap PRIVATE vapory vapashseal web3jsonrpc devcrypto devcore cryptopp-static yaml-cpp::yaml-cpp binaryen::binaryen)

enable_testing()
set(CTEST_OUTPUT_ON_FAILURE TRUE)

include(VapUtils)

vap_add_test(ClientBase
	ARGS --vap_testfile=BlockTests/bcJS_API_Test --vap_threads=1
	ARGS --vap_testfile=BlockTests/bcJS_API_Test --vap_threads=3
	ARGS --vap_testfile=BlockTests/bcJS_API_Test --vap_threads=10
	ARGS --vap_testfile=BlockTests/bcValidBlockTest --vap_threads=1
	ARGS --vap_testfile=BlockTests/bcValidBlockTest --vap_threads=3
	ARGS --vap_testfile=BlockTests/bcValidBlockTest --vap_threads=10
)

#Does not work
#vap_add_test(JsonRpc
#	ARGS --vap_testfile=BlockTests/bcJS_API_Test
#	ARGS --vap_testfile=BlockTests/bcValidBlockTest
#)
