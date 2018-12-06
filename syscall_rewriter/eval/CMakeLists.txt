cmake_minimum_required(VERSION 3.5)

project(rewriting_eval C)

set(MUSL_PREFIX ${CMAKE_SOURCE_DIR}/../../musl/prefix)

set(CMAKE_C_COMPILER ${MUSL_PREFIX}/bin/musl-gcc)

add_executable(eval eval.c)
target_compile_options(eval PRIVATE -g -O0)
target_link_libraries(eval -static -nostdlib ${MUSL_PREFIX}/lib/crt1.o ${MUSL_PREFIX}/lib/libc.a)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES eval_fast)
