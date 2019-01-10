#------------------------------------------------------------------------------
# Top-level CMake file for cpp-vapory.
#
# The documentation for cpp-vapory is hosted at http://cpp-vapory.co
#
# ------------------------------------------------------------------------------
# This file is part of cpp-vapory.
#
# cpp-vapory is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cpp-vapory is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cpp-vapory.  If not, see <http://www.gnu.org/licenses/>
#
# (c) 2014-2016 cpp-ethereum contributors.
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5.1)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    # The default toolchain file configures compilers and build environment.
    # This configuration is also used by hunter to build dependencies.
    # CMake will cache this value, not need to explictly specify CACHE param.
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain.cmake)
endif()

set(VAP_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${VAP_CMAKE_DIR})

set(CPP_VAPORY_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Path to the root directory for cpp-vapory")

# set cmake_policies
include(VapPolicy)
vap_policy()

if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Map current configuration to configurations of imported targets.
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release)
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)

set(HUNTER_CONFIGURATION_TYPES Release)
set(HUNTER_JOBS_NUMBER 4)
set(HUNTER_CACHE_SERVERS "https://github.com/vaporyco/hunter-cache")
include(HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.173.tar.gz"
    SHA1 "750fb1d621af971fad6f303356b13017ad9f5e15"
    LOCAL
)

if(HUNTER_ENABLED)
    # Find Python executable,
    # prefer version 3 that has requests module included.
    set(Python_ADDITIONAL_VERSIONS 3)
    find_package(PythonInterp)
    if(PYTHONINTERP_FOUND)
        hunter_gate_self(
            "${HUNTER_CACHED_ROOT}"
            "${HUNTER_VERSION}"
            "${HUNTER_SHA1}"
            hunter_dir
        )
        set(hunter_upload_script "${hunter_dir}/maintenance/upload-cache-to-github.py")
        set(hunter_cache_dir "${HUNTER_GATE_ROOT}/_Base/Cache")
        set(hunter_tmp_dir "${HUNTER_GATE_ROOT}/tmp")
        add_custom_target(
            hunter_upload_cache
            ${PYTHON_EXECUTABLE} ${hunter_upload_script}
            --username hunter-cache-bot
            --repo-owner vapory
            --repo hunter-cache
            --cache-dir ${hunter_cache_dir}
            --temp-dir ${hunter_tmp_dir}
        )
    endif()
endif()

# project name and version should be set after cmake_policy CMP0048
project(cpp-vapory VERSION "1.3.0")

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/vvmjit/.git)
    message(FATAL_ERROR "Git submodules not initialized, execute:\n  git submodule update --init")
endif()

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY On)

include(VapCcache)
include(VapOptions)
include(VapCompilerSettings)
include(VapExecutableHelper)
include(VapDependencies)
include(VapUtils)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
hunter_add_package(Boost COMPONENTS program_options filesystem system thread)
find_package(Boost CONFIG REQUIRED program_options filesystem system thread)

hunter_add_package(jsoncpp)
find_package(jsoncpp CONFIG REQUIRED)

hunter_add_package(Snappy)
find_package(Snappy CONFIG REQUIRED)

hunter_add_package(cryptopp)
find_package(cryptopp CONFIG REQUIRED)

hunter_add_package(libjson-rpc-cpp)
find_package(libjson-rpc-cpp CONFIG REQUIRED)

include(ProjectSecp256k1)
include(ProjectLibFF)

# Import binaryen library before hera to make it available also to testvap.
include(hera/cmake/ProjectBinaryen.cmake)

find_package(Threads)

if(MINIUPNPC)
    find_package(Miniupnpc 1.8.2013 REQUIRED)
endif()

set(UTILS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/utils")

configure_project()

if (HERA)
    add_subdirectory(hera)
endif()

#Global include path for all libs.
include_directories("${CMAKE_SOURCE_DIR}")

add_subdirectory(libdevcore)
add_subdirectory(libdevcrypto)
add_subdirectory(libp2p)

add_subdirectory(libvapash)

add_subdirectory(libvapcore)
add_subdirectory(libvvm)
add_subdirectory(libvapory)
add_subdirectory(libvapashseal)

add_subdirectory(libwebthree)
add_subdirectory(libweb3jsonrpc)

if (VVMJIT)
    add_subdirectory(vvmjit)
endif()

add_subdirectory(vap)

if (TOOLS)
    add_subdirectory(vapkey)
    add_subdirectory(vapvm)
    add_subdirectory(rlp)
endif()

if (TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# TODO: Split out json_spirit, libscrypt and sec256k1.
add_subdirectory(utils)
