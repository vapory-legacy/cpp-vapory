file(GLOB sources "*.cpp" "*.h")

add_library(vapcore ${sources})

target_include_directories(vapcore PRIVATE ../utils)
target_link_libraries(vapcore PUBLIC devcrypto devcore)
