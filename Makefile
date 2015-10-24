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

NAME_GIT_THIS=main

URL_GIT_BASE=https://github.com/peterweissig/
URL_GIT_THIS=$(URL_GIT_BASE)cpp_$(NAME_GIT_THIS).git

PATH_BUILD=build/
PATH_SOURCE=src/


URL_GIT="https://github.com/peterweissig/cpp_main.git"
NAME_GIT="main"

SUB_MAKEFILES = $(wildcard $(PATH_SOURCE)*/Makefile)
.PHONY : $(SUB_MAKEFILES)

.PHONY : all clean eclipse update status push update_init status_init

all:
	mkdir -p $(PATH_BUILD)

	@echo ""
	@echo "### starting cmake ###"
	cd $(PATH_BUILD) && cmake ../$(PATH_SOURCE)

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
	cd $(PATH_BUILD) && cmake ../$(PATH_SOURCE) \
	  -G"Eclipse CDT4 - Unix Makefiles"

	@echo ""
	@echo "### finished :-) ###"

update: update_init $(SUB_MAKEFILES)

status: status_init $(SUB_MAKEFILES)

push:
	@echo ""
	@echo "### pushing of $(NAME_GIT_THIS) ###"
	git push "$(URL_GIT_THIS)"

update_init:
	@echo ""
	@echo "### update $(NAME_GIT_THIS) ###"
	git pull "$(URL_GIT_THIS)"
	$(eval MAKEFILE_COMMAND=update)

status_init:
	@echo ""
	@echo "### status of $(NAME_GIT_THIS) ###"
	@git status --untracked-files
	$(eval MAKEFILE_COMMAND=status)

$(SUB_MAKEFILES):
	@cd $(dir $@) && make $(MAKEFILE_COMMAND)
