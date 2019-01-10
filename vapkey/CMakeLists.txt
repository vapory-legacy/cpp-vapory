add_executable(vapkey KeyAux.h main.cpp)
target_link_libraries(vapkey PRIVATE vapcore devcore Boost::program_options)
