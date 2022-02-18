###############################################################################
#                                                                             #
# CMakeLists.txt                                                              #
# ==============                                                              #
#                                                                             #
# Version: 1.1.0                                                              #
# Date   : 18.02.22                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_avr_downloader/                       #
###############################################################################

cmake_minimum_required(VERSION 2.8)
project(avr_downloader)

# check all subdirectories and add those with a "CMakeLists.txt" file
file(GLOB file_list "*/CMakeLists.txt")
foreach (file ${file_list})
    string(REPLACE "//" "/" file ${file})
    string(REGEX REPLACE "(.*/)([^ /]*)/[^ /]*$" "\\2" project ${file})
    message("adding project \"${project}\"")
    string(REGEX REPLACE "(.*/[^ /]*)/[^ /]*$" "\\1"  path ${file})
    message("  path: \"${path}\"")

    add_subdirectory("${path}")
endforeach()
