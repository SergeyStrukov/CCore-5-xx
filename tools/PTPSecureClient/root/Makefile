# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: PTPSecureClient 1.00
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2022 Sergey Strukov. All rights reserved.
#
#----------------------------------------------------------------------------------------

LIGHT_FORGE ?= ~/LightForge

Client = $(LIGHT_FORGE)/build/ccore/CCore/PTPSecureClient/CCore-PTPSecureClient.exe

.PHONY : run

run: $(Client)
	$(Client) Config.ddl

