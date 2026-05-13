#
# (C) Copyright 2012
# Texas Instruments Incorporated - http://www.ti.com/
# Aneesh V <aneesh@ti.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#
# Based on common/Makefile.
#

include $(TOPDIR)/config.mk

LIB	= $(obj)libjpeg.o

COBJS-y += jpeg_decode.o
COBJS-y += jdapimin.o
COBJS-y += jdapistd.o
COBJS-y += jcomapi.o
COBJS-y += jdtrans.o
COBJS-y += jdmaster.o
COBJS-y += jdinput.o
COBJS-y += jdmainct.o
COBJS-y += jdcoefct.o
COBJS-y += jdpostct.o
COBJS-y += jdmarker.o
COBJS-y += jdhuff.o
COBJS-y += jdarith.o
COBJS-y += jddctmgr.o
COBJS-y += jidctint.o
COBJS-y += jidctfst.o
COBJS-y += jidctflt.o
COBJS-y += jdsample.o
COBJS-y += jdcolor.o
COBJS-y += jdmerge.o
COBJS-y += jquant1.o
COBJS-y += jquant2.o
COBJS-y += jdatasrc.o
COBJS-y += jaricom.o
COBJS-y += jerror.o
COBJS-y += jmemmgr.o
COBJS-y += jutils.o
COBJS-y += jmemnobs.o

COBJS	:= $(sort $(COBJS-y))
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS-y))
CFLAGS  += -I./
all:	$(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
