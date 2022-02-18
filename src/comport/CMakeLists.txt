###############################################################################
#                                                                             #
# CMakeLists.txt                                                              #
# ==============                                                              #
#                                                                             #
# Date   : 11.10.15                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_comport/                              #
###############################################################################

### projekt name
project(wepet_comport)

### additional libraries
#<none>

### include header files
include_directories(include)

### create libraries
add_library(${PROJECT_NAME} src/${PROJECT_NAME}.cpp)

### create executables
#<none>
