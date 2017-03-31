###############################################################################
#                                                                             #
# Makefile                                                                    #
# ========                                                                    #
#                                                                             #
# Version: 1.2.0                                                              #
# Date   : 31.03.17                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_avr_downloader/                       #
###############################################################################

NAME_GIT_THIS=avr_downloader

.PHONY : update status pull push

update: pull

status:
	@echo ""
	@echo "### status of $(NAME_GIT_THIS) ###"
	@git status --untracked-files

pull:
	@echo ""
	@echo "### pulling $(NAME_GIT_THIS) ###"
	@git pull

push:
	@echo ""
	@echo "### pushing $(NAME_GIT_THIS) ###"
	@git push
