###############################################################################
#                                                                             #
# Makefile                                                                    #
# ========                                                                    #
#                                                                             #
# Version: 1.3.2                                                              #
# Date   : 19.02.22                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_avr_downloader                        #
###############################################################################

NAME_GIT_THIS=main

PATH_BUILD=build/
PATH_SOURCE=src/

.PHONY : all build clean

all: build

build:
	mkdir -p $(PATH_BUILD)

	@echo ""
	@echo "### starting cmake ###"
	cd $(PATH_BUILD) && cmake ../$(PATH_SOURCE)

	@echo ""
	@echo "### starting build process ###"
	cd $(PATH_BUILD) && make all

	@echo ""
	@echo "### copy executables ###"
	ln --symbolic --force "$(PATH_BUILD)avr_downloader/avr_downloader"
	ln --symbolic --force "$(PATH_BUILD)xbee_config/xbee_config"

	@echo ""
	@echo "### finished :-) ###"

clean:
	@echo ""
	@echo "### cleaning build ###"
	rm -rf $(PATH_BUILD) avr_downloader xbee_config
