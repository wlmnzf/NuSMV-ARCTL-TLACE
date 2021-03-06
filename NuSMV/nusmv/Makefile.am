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
# Revision	[1.1.2.14 2003/11/10 14:15:42]

CUDD_NAME = $(cuddname)
CUDD_DIR = ../$(CUDD_NAME)

INCLUDES = -I. -I./src -I$(top_srcdir)/src -I$(top_srcdir)/$(CUDD_DIR)/include $(expatuserinc)

SUBDIRS = src doc examples contrib share

TESTS = helpers/runtests.sh

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" @CPPFLAGS@

READMES = LGPL-2.1 README README_PLATFORMS README.zChaff README.MiniSat

MINISAT_PATCH = ../MiniSat_1.12-nusmv.patch

OTHER_DIST_FILES = nusmv-2.2.pc.in $(MINISAT_PATCH)

# ----------------------------------------------------------------------

bin_PROGRAMS = NuSMV

lib_LTLIBRARIES = libnusmv.la

NuSMV_SOURCES = src/sm/smMain.c 
NuSMV_LDADD = libnusmv.la
NuSMV_DEPENDENCIES = libnusmv.la


# Convenience lib for external linking:
libnusmv_la_SOURCES = src/sm/smVers.c  # to make 'ar' on mac osx work
libnusmv_la_LIBADD = $(top_builddir)/src/libnusmvcore.la \
	-L$(top_srcdir)/$(CUDD_DIR)/lib \
	-lcudd -lcudd_util -lmtr -lst   \
        $(readline_libs) $(zchaff_libs) $(minisat_libs) $(expat_libs)

libnusmv_la_DEPENDENCIES = $(top_builddir)/src/libnusmvcore.la $(CUDD_DEPENDENCY)


EXTRA_DIST = $(CUDD_DIR) $(TESTS) examples contrib share  \
	helpers/regression_test helpers/extdoc helpers/extindex \
	helpers/extract_doc.in \
	$(READMES) $(OTHER_DIST_FILES)

nusmvincdir = $(includedir)/nusmv
nusmvinc_HEADERS = config.h

readmedir = $(datadir)/nusmv
readme_DATA = $(READMES)

cuddincdir = $(includedir)/$(CUDD_NAME)
cuddinc_HEADERS = $(CUDD_DIR)/include/cudd.h  $(CUDD_DIR)/include/dddmp.h \
	$(CUDD_DIR)/include/mnemosyne.h  $(CUDD_DIR)/include/mtr.h  \
	$(CUDD_DIR)/include/node.h  $(CUDD_DIR)/include/st.h  \
	$(CUDD_DIR)/include/util.h 

pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = nusmv-2.2.pc

dist-hook:
	(test ! -d __tmp_nusmv__ || rm -rf __tmp_nusmv__)
	mkdir __tmp_nusmv__ 
	mv $(distdir)/*  __tmp_nusmv__
	cd $(distdir)/$(CUDD_DIR) && $(MAKE) distclean
	mv $(distdir)/$(CUDD_DIR) $(distdir)
	mv $(distdir)/$(MINISAT_PATCH) $(distdir)
	mv __tmp_nusmv__ $(distdir)/nusmv
	rm -rf `find $(distdir) -name CVS`
	rm -rf `find $(distdir) -name '*~'`	
	cd $(distdir)/nusmv/src/parser && rm -f grammar.h grammar.c input.c
	cd $(distdir)/nusmv/src/parser/ord && rm -f ord_grammar.h ord_grammar.c ord_input.c
	cd $(distdir)/nusmv/src/ltl/ltl2smv && rm -f lex.c yacc.c yacc.h
	rm -rf $(distdir)/nusmv/examples/TipSuite

distcleancheck_listfiles = \
	find -type f -exec sh -c 'test -f $(srcdir)/{} || echo {}' ';'

distcheck: dist
	$(am__remove_distdir)
	GZIP=$(GZIP_ENV) gunzip -c $(distdir).tar.gz | $(AMTAR) xf -
	chmod a+w $(distdir)	
	mkdir $(distdir)/=build
	mkdir $(distdir)/=inst
	cp -r $(distdir)/nusmv/helpers $(distdir)/=build
	cp -r $(distdir)/nusmv/examples $(distdir)/=build

	dc_install_base=`$(am__cd) $(distdir)/=inst && pwd` \
	  && dc_src_base=`$(am__cd) $(distdir)/nusmv && pwd` \
	  && $(am__cd) $(distdir)/=build \
	  && ../nusmv/configure --srcdir=$(dc_src_base) --prefix=$$dc_install_base \
	    $(DISTCHECK_CONFIGURE_FLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && $(MAKE) $(AM_MAKEFLAGS) uninstall \
	  && (test `find $$dc_install_base -type f -print | wc -l` -le 1 \
	      || { echo "ERROR: files left after uninstall:" ; \
	           find $$dc_install_base -type f -print ; \
	           exit 1; } >&2 ) \
	  && $(MAKE) $(AM_MAKEFLAGS) dist-gzip \
	  && rm -f $(distdir).tar.gz \
	  && $(MAKE) $(AM_MAKEFLAGS) distcleancheck
	$(am__remove_distdir)
	@echo "$(distdir).tar.gz is ready for distribution" | \
	  sed 'h;s/./=/g;p;x;p;x'


# To build ps and pdf versions of the info file
docs:
	cd $(top_builddir)/doc && $(MAKE) docs

# To extract commands/helps and source code documentation
allDocs: docs


# The used did not build the cudd package, or the distcheck 
# target is performing its work
if HAVE_NOT_CUDD
CUDD_DEPENDENCY = cudd
endif
cudd:
	cd $(top_srcdir)/$(CUDD_DIR) && $(MAKE)



if HAVE_ZCHAFF
distbindir =  $(PACKAGE)-zchaff-$(VERSION)-$(host_triplet)
else
distbindir =  $(PACKAGE)-$(VERSION)-$(host_triplet)
endif

remove_distbindir = \
  { test ! -d $(distbindir) || { rm -fr $(distbindir); }; }

do_distbindir: 
	$(remove_distbindir)
	mkdir $(distbindir)
	chmod a+w $(distbindir)


distbin: distbin_gzip


distbin_gzip: do_distbindir
	$(MAKE) prefix=$(shell cd $(distbindir) && pwd) install
	$(AMTAR) chof - $(distbindir) | GZIP=$(GZIP_ENV) gzip -c >$(distbindir).tar.gz
	$(remove_distbindir)



