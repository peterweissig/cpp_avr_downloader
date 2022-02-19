###############################################################################
#                                                                             #
# CMakeLists.txt                                                              #
# ==============                                                              #
#                                                                             #
# Date   : 18.02.22                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_avr_downloader                        #
###############################################################################

### projekt name
set(sub_lib_1 wepet_flash)
set(sub_lib_2 wepet_avrdude_file)
set(sub_lib_3 wepet_avr_downloader)

### additional libraries
#<none>

### include header files
include_directories(
  include
  ../basic/include
  ../file/include
  ../comport/include
)

### create libraries
add_library(${sub_lib_1} src/${sub_lib_1}.cpp)
add_dependencies(${sub_lib_1} wepet_hexfile wepet_basic)
target_link_libraries(${sub_lib_1} wepet_hexfile wepet_basic)

add_library(${sub_lib_2} src/${sub_lib_2}.cpp)
add_dependencies(${sub_lib_2} wepet_textfile wepet_basic)
target_link_libraries(${sub_lib_2} wepet_textfile wepet_basic)

add_library(${sub_lib_3} src/${sub_lib_3}.cpp)
add_dependencies(${sub_lib_3}
  wepet_basic
  wepet_progressbar
  wepet_comport
  wepet_flash
  wepet_avrdude_file
)
target_link_libraries(${sub_lib_3}
  wepet_basic
  wepet_progressbar
  wepet_comport
  wepet_flash
  wepet_avrdude_file
)

### create executables
add_executable(${PROJECT_NAME} src/main.cpp)
add_dependencies(${PROJECT_NAME} wepet_avr_downloader)
target_link_libraries(${PROJECT_NAME} wepet_avr_downloader)

add_executable(test_flash src/test_flash.cpp)
add_dependencies(test_flash wepet_flash)
target_link_libraries(test_flash wepet_flash)

add_executable(test_avrdude_file src/test_avrdude_file.cpp)
add_dependencies(test_avrdude_file wepet_avrdude_file)
target_link_libraries(test_avrdude_file wepet_avrdude_file)
