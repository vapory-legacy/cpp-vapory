
set(sources
    ExtVMFace.cpp ExtVMFace.h
    Instruction.cpp Instruction.h
    VMFace.h
    VMConfig.h
    VM.cpp VM.h
    VMCalls.cpp
    VMOpt.cpp
    VMSIMD.cpp
    VMValidate.cpp
    VMFactory.cpp VMFactory.h
)

if(VVMJIT OR HERA)
    list(APPEND sources
        VVMC.cpp VVMC.h
        HeraPlusVM.cpp HeraPlusVM.h
        SmartVM.cpp SmartVM.h
    )
endif()

add_library(vvm ${sources})

target_link_libraries(vvm PUBLIC vapcore devcore PRIVATE jsoncpp_lib_static Boost::program_options)
target_include_directories(vvm PUBLIC ${CMAKE_SOURCE_DIR}/vvmjit/include)
target_include_directories(vvm PUBLIC ${CMAKE_SOURCE_DIR}/hera/src)

if(VVMJIT)
    target_link_libraries(vvm PRIVATE vvmjit)
    target_compile_definitions(vvm PRIVATE VAP_VVMJIT)
endif()

if(HERA)
    target_link_libraries(vvm PRIVATE hera)
    target_compile_definitions(vvm PRIVATE VAP_HERA)
endif()
