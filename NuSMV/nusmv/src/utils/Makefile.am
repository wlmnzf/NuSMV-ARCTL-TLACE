## Makefile.am generated by the makefiler script
##
# FileName	[ Makefile ]
#
# PackageName	[ NuSMV ]
#
# Synopsis	[ Package-wide Makefile ]
#
# Description	[ Makefile template for the NuSMV executable ]
#
# Author	[ Roberto Cavada ]
#
# Copyright	[Copyright (C) 2003 by ITC-irst ]
#
# NuSMV version 2 is free software; you can redistribute it and/or 
# modify it under the terms of the GNU Lesser General Public 
# License as published by the Free Software Foundation; either 
# version 2 of the License, or (at your option) any later version.
#
# NuSMV version 2 is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public 
# License along with this library; if not, write to the Free Software 
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.
#
# For more information of NuSMV see <http://nusmv.irst.itc.it>
# or email to <nusmv-users@irst.itc.it>.
# Please report bugs to <nusmv-users@irst.itc.it>.
#
# To contact the NuSMV development board, email to <nusmv@irst.itc.it>.]
#
# Revision	[1.1.2.4 2003/11/13 13:42:48]

AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = -I. -I.. -I$(top_srcdir)/src -I$(top_srcdir)/../cudd-2.3.0.1/include

PKG_SOURCES = array.h assoc.h avl.h error.h heap.h list.h lsort.h \
	object.h object_private.h ucmd.h ustring.h utils.h \
	utils_io.h array.c assoc.c avl.c error.c heap.c list.c \
	object.c ucmd.c ustring.c utils.c utils_io.c \
	NodeList.h NodeList.c range.h range.c 

utilsdir = $(includedir)/nusmv/utils
utils_HEADERS = array.h error.h lsort.h ustring.h assoc.h heap.h \
	NodeList.h range.h utils.h avl.h list.h object.h ucmd.h \
	utils_io.h


# -------------------------------------------------------

noinst_LTLIBRARIES = libutils.la
libutils_la_SOURCES = $(PKG_SOURCES)

