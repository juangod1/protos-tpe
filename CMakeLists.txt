cmake_minimum_required(VERSION 3.10)
project(protos_tpe C)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread --std=c99 -pedantic -pedantic-errors -Wall -Wextra -Wno-unused-parameter -D_POSIX_C_SOURCE=200112L -O1 -fsanitize=address")

AUX_SOURCE_DIRECTORY(Shared SHARED_SOURCE_FILES)

AUX_SOURCE_DIRECTORY(Proxy PROXY_SOURCE_FILES)
add_executable(proxy ${SHARED_SOURCE_FILES} ${PROXY_SOURCE_FILES})

AUX_SOURCE_DIRECTORY(Control CONTROL_SOURCE_FILES)
add_executable(control ${SHARED_SOURCE_FILES} ${CONTROL_SOURCE_FILES})

AUX_SOURCE_DIRECTORY(Parser PARSER_SOURCE_FILES)
add_executable(parser ${SHARED_SOURCE_FILES} ${PARSER_SOURCE_FILES})