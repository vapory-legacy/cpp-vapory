set(
    sources
    AccountManager.cpp AccountManager.h
    main.cpp
    MinerAux.h
)

add_executable(eth ${sources})
target_link_libraries(
    eth
    PRIVATE ethereum ethashseal evm web3jsonrpc webthree devcore Boost::program_options C:/dev/libboost_context-vc141-mt-1_64.lib C:/dev/libboost_fiber-vc141-mt-1_64.lib
)

target_include_directories(eth PRIVATE ../utils)

if(MINIUPNPC)
    target_compile_definitions(eth PRIVATE ETH_MINIUPNPC)
endif()

install(TARGETS eth DESTINATION bin)
