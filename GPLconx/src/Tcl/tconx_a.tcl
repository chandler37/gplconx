#    GPLconx -- visualize 2-D hyperbolic geometry.
#    Copyright (C) 1996-2001  David L. Chandler

#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# Other Tcl source files are loaded and reloaded via the facilities provided
# in this file.

proc tconx_do_most_sourcing { } {
    # load files.
    Debugloc "tconx_do_most_sourcing start";
    tconx_source tconxt.ui.tcl;
    tconx_source tconx_h.tcl;
    tconx_source tconx_b.tcl;
    tconx_source tconx_w.tcl;
    tconx_source simple.ui.tcl;
    tconx_source text.ui.tcl;
    tconx_source tconx_t.tcl;
    tconx_source tconx_m.tcl;
    tconx_source tconx_l.tcl;
    tconx_source slider.ui.tcl;
    tconx_source tconx_s.tcl;
    tconx_source tconx_u.tcl;
    tconx_source tconx_p.tcl;
    Debugloc "tconx_do_most_sourcing end";
}

proc sync { } {
    # re-load as many files as is possible.
    Debugloc "@sync";
    tconx_source tconxt.ui.tcl;
    tconx_source tconx_d.tcl;
    tconx_source tconx_h.tcl;
    tconx_source tconx_b.tcl;
    tconx_source tconx_w.tcl;
    tconx_source simple.ui.tcl;
    tconx_source text.ui.tcl;
    tconx_source tconx_t.tcl;
    tconx_source tconx_m.tcl;
    tconx_source tconx_l.tcl;
    tconx_source slider.ui.tcl;
    tconx_source tconx_s.tcl;
    tconx_source tconx_u.tcl;
    tconx_source tconx_p.tcl;
    tconx_menus_init
    Debugloc "@end sync";
}
