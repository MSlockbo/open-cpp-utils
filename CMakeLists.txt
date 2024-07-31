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