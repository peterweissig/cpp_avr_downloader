###############################################################################
#                                                                             #
# CMakeLists.txt                                                              #
# ==============                                                              #
#                                                                             #
# Date   : 22.02.17                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/                                          #
###############################################################################

### projekt name
project(xbee_config)

set(sub_lib wepet_xbee_config)

### additional libraries
#<none>

### include header files
include_directories(
  include
  ../basic/include
  ../comport/include
)

### create libraries
add_library(${sub_lib} src/${sub_lib}.cpp)
add_dependencies(${sub_lib}
  wepet_basic
  wepet_comport
)
target_link_libraries(${sub_lib}
  wepet_basic
  wepet_comport
)

### create executables
add_executable(${PROJECT_NAME} src/main.cpp)
add_dependencies(${PROJECT_NAME} wepet_xbee_config)
target_link_libraries(${PROJECT_NAME} wepet_xbee_config)