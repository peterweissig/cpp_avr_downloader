###############################################################################
#                                                                             #
# Makefile                                                                    #
# ========                                                                    #
#                                                                             #
# Version: 1.0.0                                                              #
# Date   : 11.10.15                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_main/                                 #
###############################################################################

.PHONY : all clean eclipse

PATH_BUILD="build/"

all:
	mkdir -p $(PATH_BUILD)

	@echo ""
	@echo "### starting cmake ###"
	cd $(PATH_BUILD) && cmake ../src

	@echo ""
	@echo "### starting build process ###"
	cd $(PATH_BUILD) && make all

	@echo ""
	@echo "### finished :-) ###"

clean:
	rm -rf $(PATH_BUILD)

eclipse:
	mkdir -p $(PATH_BUILD)

	@echo ""
	@echo "### starting cmake (eclipse) ###"
	cd $(PATH_BUILD) && cmake ../src -G"Eclipse CDT4 - Unix Makefiles"

	@echo ""
	@echo "### finished :-) ###"
