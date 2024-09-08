// util/test-platform.h - Tests platform sizes at compile time

#ifndef LIBNBAR_PLATFORM_TEST_H
#   define LIBNBAR_PLATFORM_TEST_H
#   if defined(__x86_64__) || defined(_M_X64)
#       define REGISTER_SIZE 64
#   elif defined(__i386__) || defined(_M_IX86)
#       define REGISTER_SIZE 32
#   elif defined(__SIZEOF_POINTER__)
#       if __SIZEOF_POINTER__ == 8
#           define REGISTER_SIZE 64
#       elif __SIZEOF_POINTER__ == 4
#           define REGISTER_SIZE 32
#       else
#           error "Unknown pointer size"
#       endif
#   else
#       error "Cannot determine register size"
#   endif
#endif