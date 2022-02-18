/******************************************************************************
*                                                                             *
* test_hexfile_writer.cpp                                                     *
* =======================                                                     *
*                                                                             *
* Version: 1.0.2                                                              *
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

    std::cout << "Simple test program for cHexFileWriter" << std::endl;
    std::cout << std::endl;

    if (argc != 2) {
        std::cout << "usage: " << std::string(argv[0]) <<
          " <hexfile-name>" << std::endl;

        return -1;
    }

    std::cout << "cHexfileWriter writer;" << std::endl;
    wepet::cHexfileWriter writer;

    std::string data("Hello this is just data :-)");
    if (! writer.HasError()) {
        std::cout << std::endl;

        for (int i = 0; i < data.size(); i++) {
            std::cout << "writer.SetData(" << wepet::IntToStr(i + 42, 3) << ", '";
            std::cout << data[i] << "') == ";

            if (! writer.SetData(i + 42, data[i])) {
                std::cout << "false" << std::endl;
                break;
            } else {
                std::cout << "true" << std::endl;
            }

            if (writer.HasError()) { break; }
        }
    }

    data = "Text2";
    if (! writer.HasError()) {
        std::cout << std::endl;

        for (int i = 0; i < data.size(); i++) {
            std::cout << "writer.SetData(" << wepet::IntToStr(i + 256, 3) << ", '";
            std::cout << data[i] << "') == ";

            if (! writer.SetData(i + 256, data[i])) {
                std::cout << "false" << std::endl;
                break;
            } else {
                std::cout << "true" << std::endl;
            }

            if (writer.HasError()) { break; }
        }
    }

    if (! writer.HasError()) {
        std::cout << std::endl;
        std::cout << "writer.SaveToFile(\"" << argv[1] << "\") == ";
        if (writer.SaveToFile(std::string(argv[1]))) {
            std::cout << "true" << std::endl;
        } else {
            std::cout << "false" << std::endl;
        }
    }

    if (writer.HasError()) {
        std::cout << std::endl;
        std::cout << "writer.HasError() == true" << std::endl;

        std::cout << "writer.ReturnLastError() == " <<
          writer.ReturnLastError() << std::endl;
    }

    std::cout << std::endl;
    std::cout << "finished :-)" << std::endl;

    return 0;
}
