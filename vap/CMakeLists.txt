set(
    sources
    AccountManager.cpp AccountManager.h
    main.cpp
    MinerAux.h
)

add_executable(vap ${sources})
target_link_libraries(
    vap
    PRIVATE vapory vapashseal vvm web3jsonrpc webthree devcore
)

target_include_directories(vap PRIVATE ../utils)

install(TARGETS vap DESTINATION bin)
