#pragma once

#ifndef unlikely
#  ifdef __GNUC__
#    define unlikely(x) __builtin_expect(!!(x), 0)
#  else
#    define unlikely(x) x
#  endif
#endif
