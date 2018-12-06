cmake_minimum_required(VERSION 3.5)

project(ioctl_tests C)

set(MUSL_PREFIX_RELATIVE ../../musl/prefix)
get_filename_component(MUSL_PREFIX
	"${CMAKE_CURRENT_LIST_DIR}/${MUSL_PREFIX_RELATIVE}"
						                          ABSOLUTE)
SET(CMAKE_C_COMPILER ${MUSL_PREFIX}/bin/musl-gcc)

add_executable(ioctl_unopt ioctl.c)
target_compile_options(ioctl_unopt PRIVATE -g -O0)
#target_link_options(ioctl_unopt PRIVATE -static -nostdlib)
target_link_libraries(ioctl_unopt -static -nostdlib ${MUSL_PREFIX}/lib/crt1.o ${MUSL_PREFIX}/lib/libc.a)


add_executable(ioctl ioctl.c)
target_compile_options(ioctl PRIVATE -g -O3)
#target_link_options(ioctl PRIVATE -static -nostdlib)
target_link_libraries(ioctl -static -nostdlib ${MUSL_PREFIX}/lib/crt1.o ${MUSL_PREFIX}/lib/libc.a)
