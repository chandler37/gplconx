# GPLconx -- visualize 2-D hyperbolic geometry.
# Copyright (C) 1996-2001  David L. Chandler

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

THE_HTML = $(srcdir)/htdocs/index.html $(srcdir)/README
EXTRA_DIST = $(THE_HTML)

MAINTAINER_USERNAME = dchandler
publish:
	scp $(THE_HTML) $(MAINTAINER_USERNAME)@gplconx.sourceforge.net:/home/groups/g/gp/gplconx/htdocs
