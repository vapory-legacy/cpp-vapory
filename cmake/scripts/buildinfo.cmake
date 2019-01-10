# generates BuildInfo.h
#
# this module expects
# VAP_SOURCE_DIR - main CMAKE_SOURCE_DIR
# VAP_DST_DIR - main CMAKE_BINARY_DIR
# VAP_BUILD_TYPE
# VAP_BUILD_PLATFORM
# VAP_BUILD_NUMBER
# VAP_VERSION_SUFFIX
#
# example usage:
# cmake -DVAP_SOURCE_DIR=. -DVAP_DST_DIR=build -DVAP_BUILD_TYPE=Debug -DVAP_BUILD_PLATFORM=mac -DVAP_BUILD_NUMBER=45 -DVAP_VERSION_SUFFIX="-rc1" -P scripts/buildinfo.cmake

if (NOT VAP_BUILD_TYPE)
	set(VAP_BUILD_TYPE "unknown")
endif()

if (NOT VAP_BUILD_PLATFORM)
	set(VAP_BUILD_PLATFORM "unknown")
endif()

execute_process(
	COMMAND git --git-dir=${VAP_SOURCE_DIR}/.git --work-tree=${VAP_SOURCE_DIR} rev-parse HEAD
	OUTPUT_VARIABLE VAP_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
)

if (NOT VAP_COMMIT_HASH)
	set(VAP_COMMIT_HASH 0)
endif()

execute_process(
	COMMAND git --git-dir=${VAP_SOURCE_DIR}/.git --work-tree=${VAP_SOURCE_DIR} diff HEAD --shortstat
	OUTPUT_VARIABLE VAP_LOCAL_CHANGES OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
)

if (VAP_LOCAL_CHANGES)
	set(VAP_CLEAN_REPO 0)
else()
	set(VAP_CLEAN_REPO 1)
endif()

set(TMPFILE "${VAP_DST_DIR}/BuildInfo.h.tmp")
set(OUTFILE "${VAP_DST_DIR}/BuildInfo.h")

configure_file("${VAP_BUILDINFO_IN}" "${TMPFILE}")

include("${VAP_CMAKE_DIR}/VapUtils.cmake")
replace_if_different("${TMPFILE}" "${OUTFILE}" CREATE)
