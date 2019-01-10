file(GLOB sources "*.cpp" "*.h")

add_library(vapcore ${sources})

target_include_directories(vapcore PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(vapcore PUBLIC devcrypto devcore)
