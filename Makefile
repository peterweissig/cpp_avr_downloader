###############################################################################
#                                                                             #
# Makefile                                                                    #
# ========                                                                    #
#                                                                             #
# Version: 1.1.1                                                              #
# Date   : 24.10.15                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_avr_downloader/                       #
###############################################################################

NAME_GIT_THIS=avr_downloader

URL_GIT_BASE=https://github.com/peterweissig/
URL_GIT_THIS=$(URL_GIT_BASE)cpp_$(NAME_GIT_THIS).git

.PHONY : update status push

update:
	@echo ""
	@echo "### update $(NAME_GIT_THIS) ###"
	git pull "$(URL_GIT_THIS)"

status:
	@echo ""
	@echo "### status of $(NAME_GIT_THIS) ###"
	@git status --untracked-files

push:
	@echo ""
	@echo "### pushing of $(NAME_GIT_THIS) ###"
	git push "$(URL_GIT_THIS)"
