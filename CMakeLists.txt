cmake_minimum_required(VERSION 3.5)

project(open-cpp-utils)

set(OPENCPPUTILS_HEADERS
        any.h
        directed_tree.h
        object_pool.h
        optional.h
        startup.h
        template_utils.h
        unique_id.h
        dynarray.h
        redec.h
        hash_table.h
        math.h
        set.h
        map.h
)

add_library(open-cpp-utils INTERFACE
        ${OPENCPPUTILS_HEADERS}
)

if (GTest_FOUND)

# Set CPP Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

find_package(GTest)

add_executable(open-cpp-utils-test
        Test/Test.cpp

        ${OPENCPPUTILS_HEADERS}
)

target_link_libraries(open-cpp-utils-test PRIVATE
        GTest::gtest
)

endif ()

# DOXYGEN ==============================================================================================================
# https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

find_package(Doxygen)

if(DOXYGEN_FOUND)
	get_filename_component(DOXYGEN_PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
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