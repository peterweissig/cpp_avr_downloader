/******************************************************************************
*                                                                             *
* test_avrdude_file.cpp                                                       *
* =====================                                                       *
*                                                                             *
* Version: 1.0.0                                                              *
* Date   : 17.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_avr_downloader                        *
******************************************************************************/

// local headers
#include "wepet_avrdude_file.h"

// wepet headers

// standard headers
#include <stdint.h>

#include <iostream>

// additional headers



//**************************[main]*********************************************
int main(int argc, char **argv) {

    std::cout << "Simple test program for cAvrdudeFile" << std::endl;
    std::cout << std::endl;

    if (argc != 1) {
        std::cout << "usage: " << std::string(argv[0]) << std::endl;

        return -1;
    }

    std::cout << "wepet::cAvrdudeFile adfile(\"avrdude.conf\");" << std::endl;
    wepet::cAvrdudeFile adfile("avrdude.conf");

    std::string mcu = "atmega64";
    while (! adfile.HasError()) {
        std::cout << "adfile.SetController(\"" << mcu << "\") == ";
        if (adfile.SetController(mcu)) {
            std::cout << "true" << std::endl;
        } else {
            std::cout << "false" << std::endl;
            continue;
        }

        std::cout << "  adfile.GetId()          == " <<
          adfile.GetId() << std::endl;
        std::cout << "  adfile.GetDescription() == " <<
          adfile.GetDescription() << std::endl;
        std::cout << "  adfile.GetSignature()   ~~";
        std::string temp = adfile.GetSignature();
        for (int i = 0; i < temp.size(); i++) {
            std::cout << " " << wepet::IntToHex((int) temp[i], -2);
        }
        std::cout << std::endl;
        std::cout << "  adfile.GetPageCount()   == " <<
          adfile.GetPageCount() << std::endl;
        std::cout << "  adfile.GetPageSize()    == " <<
          adfile.GetPageSize() << std::endl;

        std::cout << std::endl;
        std::cout << "Check Controller: ";
        std::cin >> mcu;
    }

    if (adfile.HasError()) {
        std::cout << "adfile.HasError() == true" << std::endl;

        std::cout << "adfile.ReturnLastError() == " <<
          adfile.ReturnLastError() << std::endl;
    }

    std::cout << "finished :-)" << std::endl;

    return 0;
}
