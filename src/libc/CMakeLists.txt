# So CMake can tell whether or not it can process this file
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
 
# Require Perl (for whatever reason)
#FIND_PACKAGE(PERL REQUIRED)
 
MESSAGE(STATUS "Hi, from src/libc!")
 
# Grab all of the C files in src; store in C_SRCS
FILE(GLOB C_SRCS_libc ./*.c)
 
# Note how we obtain this variable's value!
ADD_EXECUTABLE(foo ${C_SRCS})
