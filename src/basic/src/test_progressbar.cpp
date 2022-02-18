/******************************************************************************
*                                                                             *
* test_progressbar.cpp                                                        *
* ====================                                                        *
*                                                                             *
* Version: 1.0.1                                                              *
* Date   : 23.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_basic/                                *
******************************************************************************/

// local headers
#include "wepet_progressbar.h"

// wepet headers

// standard headers
#include <iostream>

#include <unistd.h>

// additional headers



//**************************[main]*********************************************
int main(int argc, char **argv) {

    std::cout << "Simple test program for cProgressbar" << std::endl;
    std::cout << std::endl;

    if (argc != 1) {
        std::cout << "usage: " << std::string(argv[0]) << std::endl;

        return -1;
    }

    std::cout << "cProgressbar progressbar;" << std::endl;
    wepet::cProgressbar progressbar;

    std::cout << "progressbar.Init(\"Doing Work! \")" << std::endl;
    progressbar.Init("Doing Work! ");

    std::cout << "progressbar.Update()" << std::endl;
    for (int i = 0; i < 50; i++) {
        progressbar.Update(i);

        usleep(1000 * 50);
    }

    progressbar.Skip();
    std::cout << "//New Line for interruption was produced with " <<
      "\"progressbar.Skip()\"" << std::endl;

    for (int i = 50; i <= 100; i++) {
        progressbar.Update(i);

        usleep(1000 * 50);
    }

    std::cout << std::endl;
    std::cout << "finished :-)" << std::endl;

    return 0;
}
