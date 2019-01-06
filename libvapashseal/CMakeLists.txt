file(GLOB sources "*.cpp" "*.h")

add_library(vapashseal ${sources})
target_include_directories(vapashseal PRIVATE ../utils)
target_link_libraries(vapashseal PUBLIC vapory vapcore vapash)
