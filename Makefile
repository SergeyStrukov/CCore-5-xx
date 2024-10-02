# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: CCore 5.01
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2024 Sergey Strukov. All rights reserved.
#
#----------------------------------------------------------------------------------------

LIGHT_FORGE ?= ~/LightForge

.PHONY: WIN64 delete

WIN64:
	cp Target/WIN64/TARGET_ Target/TARGET
	cp Target/WIN64/test/TARGET_ Target/test/TARGET
	$(LIGHT_FORGE)/manager.exe add . ccore

delete:
	$(LIGHT_FORGE)/manager.exe del . ccore


