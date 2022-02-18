/******************************************************************************
*                                                                             *
* test_hexfile_reader.cpp                                                     *
* =======================                                                     *
*                                                                             *
* Version: 1.0.3                                                              *
* Date   : 18.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_file/                                 *
******************************************************************************/

// local headers
#include "wepet_hexfile.h"

// wepet headers
#include "wepet_basic.h"

// standard headers
#include <stdint.h>

#include <iostream>

// additional headers



//**************************[main]*********************************************
int main(int argc, char **argv) {

    std::cout << "Simple test program for cHexFileReader" << std::endl;
    std::cout << std::endl;

    if (argc != 2) {
        std::cout << "usage: " << std::string(argv[0]) <<
          " <hexfile-name>" << std::endl;

        return -1;
    }

    std::cout << "cHexfileReader reader;" << std::endl;
    wepet::cHexfileReader reader;

    std::cout << "reader.LoadFromFile(\"" << argv[1] << "\") == ";
    if (reader.LoadFromFile(std::string(argv[1]))) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }

    if (! reader.HasError()) {
        std::cout << std::endl;

        for (int i = 0; i < 20; i++) {
            int address;
            uint8_t data;

            std::cout << "reader.GetData(";
            bool ok = reader.GetData(address, data);

            std::cout << wepet::IntToStr(address, 3) << ", " <<
              wepet::IntToStr(data, 3) << ") == ";
            if (! ok) {
                std::cout << "false;" << std::endl;
                break;
            }
            std::cout << "true;";

            std::cout << " ++reader;" << std::endl;
            ++reader;
            if (reader.HasError()) { break; }
        }
    }

    if (reader.HasError()) {
        std::cout << std::endl;
        std::cout << "reader.HasError() == true" << std::endl;

        std::cout << "reader.ReturnLastError() == " <<
          reader.ReturnLastError() << std::endl;
    }


    std::cout << std::endl;
    std::cout << "finished :-)" << std::endl;

    return 0;
}
