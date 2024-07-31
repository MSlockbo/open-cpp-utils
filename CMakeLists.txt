cmake_minimum_required(VERSION 3.5)

project(open-cpp-utils)

add_library(open-cpp-utils INTERFACE
        any.h
        directed_tree.h
        startup.h
        optional.h
        template_utils.h
        unique_id.h
        array.h
        types.h
)

# DOXYGEN ==============================================================================================================
# https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

find_package(Doxygen)

if(DOXYGEN_FOUND)
	set(DOXYGEN_CONFIG_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
	set(DOXYGEN_CONFIG_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

	configure_file(${DOXYGEN_CONFIG_IN} ${DOXYGEN_CONFIG_OUT} @ONLY)
	message("Doxygen Build Started.")

	if(WIN32)
		add_custom_target(open-cpp-utils-documentation ALL
				COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_OUT}
				COMMAND start firefox "${CMAKE_CURRENT_SOURCE_DIR}/Documentation/html/index.html"
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Generating Doxygen Documentation"
				VERBATIM)
	else()
		add_custom_target(open-cpp-utils-documentation ALL
				COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_OUT}
				COMMAND firefox "${CMAKE_CURRENT_SOURCE_DIR}/Documentation/html/index.html"
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Generating Doxygen Documentation"
				VERBATIM)
	endif()
else()
	message("Doxygen not found.")
endif()