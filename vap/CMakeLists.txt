set(
    sources
    AccountManager.cpp AccountManager.h
    main.cpp
    MinerAux.h
)

add_executable(vap ${sources})
target_link_libraries(
    vap
    PRIVATE vapory vapashseal vvm web3jsonrpc webthree devcore Boost::program_options
)

target_include_directories(vap PRIVATE ../utils)

if(MINIUPNPC)
    target_compile_definitions(vap PRIVATE VAP_MINIUPNPC)
endif()

install(TARGETS vap DESTINATION bin)
