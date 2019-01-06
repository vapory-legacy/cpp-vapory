add_executable(vapvm main.cpp)

target_link_libraries(vapvm PRIVATE vapory vvm vapashseal devcore)

target_include_directories(vapvm PRIVATE ../utils)

install(TARGETS vapvm DESTINATION bin OPTIONAL)
