/******************************************************************************
*                                                                             *
* test_flash.cpp                                                              *
* ==============                                                              *
*                                                                             *
* Version: 1.0.2                                                              *
* Date   : 18.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_avr_downloader                        *
******************************************************************************/

// local headers
#include "wepet_flash.h"

// wepet headers

// standard headers
#include <stdint.h>

#include <iostream>

// additional headers



//**************************[main]*********************************************
int main(int argc, char **argv) {

    std::cout << "Simple test program for cFlash" << std::endl;
    std::cout << std::endl;

    if (argc != 3) {
        std::cout << "usage: " << std::string(argv[0]) <<
          " <hexfile_old>  <hexfile_new>" << std::endl;

        return -1;
    }

    std::cout << "wepet::cFlash flash;" << std::endl;
    wepet::cFlash flash;

    if (! flash.HasError()) {
        std::cout << "flash.LoadFromHexFile(\"" << argv[1] << "\") == ";
        if (flash.LoadFromHexFile(std::string(argv[1]))) {
            std::cout << "true" << std::endl;
        } else {
            std::cout << "false" << std::endl;
        }
    }

    if (! flash.HasError()) {
        std::cout << "flash.Reset;" << std::endl;
        flash.Reset();
    }

    if (! flash.HasError()) {
        std::cout << "flash.LoadFromHexFile(\"" << argv[2] << "\") == ";
        if (flash.LoadFromHexFile(std::string(argv[2]))) {
            std::cout << "true" << std::endl;
        } else {
            std::cout << "false" << std::endl;
        }
    }

    if (! flash.HasError()) {
        std::cout << "flash.SaveToHexFile(\"merged.hex\", fsGetKnown) == ";
        if (flash.SaveToHexFile("merged.hex", wepet::fsGetKnown)) {
            std::cout << "true" << std::endl;
        } else {
            std::cout << "false" << std::endl;
        }
    }

    if (! flash.HasError()) {
        std::cout << "flash.SaveToHexFile(\"diff.hex\", fsGetChanged) == ";
        if (flash.SaveToHexFile("diff.hex", wepet::fsGetChanged)) {
            std::cout << "true" << std::endl;
        } else {
            std::cout << "false" << std::endl;
        }
    }

    if (flash.HasError()) {
        std::cout << "flash.HasError() == true" << std::endl;

        std::cout << "flash.ReturnLastError() == " <<
          flash.ReturnLastError() << std::endl;
    }

    std::cout << "finished :-)" << std::endl;

    return 0;
}
