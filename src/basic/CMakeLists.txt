###############################################################################
#                                                                             #
# CMakeLists.txt                                                              #
# ==============                                                              #
#                                                                             #
# Date   : 18.10.15                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_basic/                                #
###############################################################################

### projekt name
set(sub_lib_1 wepet_basic)
set(sub_lib_2 wepet_progressbar)

### additional libraries
#<none>

### include header files
include_directories(include)

### create libraries
add_library(${sub_lib_1} src/${sub_lib_1}.cpp)

add_library(${sub_lib_2} src/${sub_lib_2}.cpp)

### create executables
add_executable(test_progressbar src/test_progressbar.cpp)
add_dependencies(test_progressbar wepet_progressbar)
target_link_libraries(test_progressbar wepet_progressbar)

