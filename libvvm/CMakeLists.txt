
set(SOURCES
	ExtVMFace.cpp
	Instruction.cpp
	VM.cpp
	VMOpt.cpp
	VMCalls.cpp
	VMSIMD.cpp
	VMValidate.cpp
	VMFactory.cpp
)

if (VVMJIT)
	list(APPEND SOURCES
		JitVM.cpp
		SmartVM.cpp
	)
endif()

file(GLOB HEADERS "*.h")

add_library(vvm ${SOURCES} ${HEADERS})

target_link_libraries(vvm PUBLIC vapcore devcore PRIVATE jsoncpp_lib_static)
target_include_directories(vvm PUBLIC ../vvmjit/include PRIVATE ../utils)

if (VVMJIT)
	target_link_libraries(vvm PRIVATE vvmjit)
endif()
