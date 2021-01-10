cmake_minimum_required(VERSION 3.16)
project(tihijson)

set(CMAKE_VERBOSE_MAKEFILE ON)

#判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -std=c++11 ${CMAKE_CXX_FLAGS}") 
endif(CMAKE_COMPILER_IS_GNUCXX)

set(LIB_SRC
    src/tihijson.cc
)
# redefine_file_macro(tihijson)

add_library(tihijson SHARED ${LIB_SRC})
set(LIB_LIB
        tihijson
)

add_executable(test tests/test.cc)
add_dependencies(test tihijson)
# redefine_file_macro(test)
target_link_libraries(test ${LIB_LIB})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)