###############################################################################
#                                                                             #
# CMakeLists.txt                                                              #
# ==============                                                              #
#                                                                             #
# Date   : 17.10.15                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_file/                                 #
###############################################################################

### projekt name
set(sub_lib_1 wepet_fileparser)
set(sub_lib_2 wepet_textfile)
set(sub_lib_3 wepet_hexfile)

### additional libraries
#<none>

### include header files
include_directories(
  include
  ../basic/include
)

### create libraries
add_library(${sub_lib_1} src/${sub_lib_1}.cpp)

add_library(${sub_lib_2} src/${sub_lib_2}.cpp)
add_dependencies(${sub_lib_2} wepet_basic)
target_link_libraries(${sub_lib_2} wepet_basic)

add_library(${sub_lib_3} src/${sub_lib_3}.cpp)
add_dependencies(${sub_lib_3} wepet_textfile wepet_basic)
target_link_libraries(${sub_lib_3} wepet_textfile wepet_basic)

### create executables
add_executable(test_hexfile_reader src/test_hexfile_reader.cpp)
add_dependencies(test_hexfile_reader wepet_hexfile wepet_basic)
target_link_libraries(test_hexfile_reader wepet_hexfile wepet_basic)

add_executable(test_hexfile_writer src/test_hexfile_writer.cpp)
add_dependencies(test_hexfile_writer wepet_hexfile wepet_basic)
target_link_libraries(test_hexfile_writer wepet_hexfile wepet_basic)

