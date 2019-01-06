# based on: http://stackoverflow.com/questions/11813271/embed-resources-eg-shader-code-images-into-executable-library-with-cmake
#
# example:
# cmake -DVAP_RES_FILE=test.cmake -P resources.cmake
#
# where test.cmake is:
# 
# # BEGIN OF cmake.test
# 
# set(copydlls "copydlls.cmake")
# set(conf "configure.cmake")
# 
# # this three properties must be set!
#
# set(VAP_RESOURCE_NAME "VapResources")
# set(VAP_RESOURCE_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}")
# set(VAP_RESOURCES "copydlls" "conf")
#
# # END of cmake.test
#

# should define VAP_RESOURCES
include(${VAP_RES_FILE})

set(VAP_RESULT_DATA "")
set(VAP_RESULT_INIT "")

# resource is a name visible for cpp application 
foreach(resource ${VAP_RESOURCES})

	# filename is the name of file which will be used in app
	set(filename ${${resource}})

	# filedata is a file content
	file(READ ${filename} filedata HEX)

	# read full name of the file
	file(GLOB filename ${filename})

	# Convert hex data for C compatibility
	string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})

	# append static variables to result variable
	set(VAP_RESULT_DATA "${VAP_RESULT_DATA}	static const unsigned char vap_${resource}[] = {\n	// ${filename}\n	${filedata}\n};\n")

	# append init resources
	set(VAP_RESULT_INIT "${VAP_RESULT_INIT}	m_resources[\"${resource}\"] = (char const*)vap_${resource};\n")
	set(VAP_RESULT_INIT "${VAP_RESULT_INIT}	m_sizes[\"${resource}\"]     = sizeof(vap_${resource});\n")

endforeach(resource)

set(VAP_DST_NAME "${VAP_RESOURCE_LOCATION}/${VAP_RESOURCE_NAME}")

configure_file("${CMAKE_CURRENT_LIST_DIR}/resource.hpp.in" "${VAP_DST_NAME}.hpp.tmp")

include("${CMAKE_CURRENT_LIST_DIR}/../VapUtils.cmake")
replace_if_different("${VAP_DST_NAME}.hpp.tmp" "${VAP_DST_NAME}.hpp")
