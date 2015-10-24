###############################################################################
#                                                                             #
# Makefile                                                                    #
# ========                                                                    #
#                                                                             #
# Version: 1.1.0                                                              #
# Date   : 24.10.15                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_main/                                 #
###############################################################################

.PHONY : all clean eclipse update status update_init status_init

PATH_BUILD="build/"
PATH_SOURCE="src/"

URL_GIT="https://github.com/peterweissig/cpp_main.git"
NAME_GIT="main"

SUB_MAKEFILES = $(wildcard src/*/Makefile)
.PHONY : $(SUB_MAKEFILES)

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
	@echo ""
	@echo "### cleaning build ###"
	rm -rf $(PATH_BUILD)

eclipse:
	mkdir -p $(PATH_BUILD)

	@echo ""
	@echo "### starting cmake (eclipse) ###"
	cd $(PATH_BUILD) && cmake ../src -G"Eclipse CDT4 - Unix Makefiles"

	@echo ""
	@echo "### finished :-) ###"

update: update_init $(SUB_MAKEFILES)

status: status_init $(SUB_MAKEFILES)

update_init:
	@echo ""
	@echo "### update $(NAME_GIT) ###"
	git pull "$(URL_GIT)"
	$(eval MAKEFILE_COMMAND=update)

status_init:
	@echo ""
	@echo "### status of $(NAME_GIT) ###"
	@git status --untracked-files
	$(eval MAKEFILE_COMMAND=status)

$(SUB_MAKEFILES):
	@cd $(dir $@) && make $(MAKEFILE_COMMAND)
