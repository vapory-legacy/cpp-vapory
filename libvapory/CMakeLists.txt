file(GLOB sources "*.cpp" "*.h")

add_library(vapory ${sources})

target_include_directories(vapory PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(vapory PUBLIC vvm vapcore p2p devcrypto devcore PRIVATE jsoncpp_lib_static Snappy::snappy)
