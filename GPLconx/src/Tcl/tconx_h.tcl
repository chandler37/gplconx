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


# This file contains only help messages and utility functions for help
# messages.

proc tconx_get_help { widget } {
# Get the help text for the widget $widget.
    case $widget {
        *CONXCMD_GETCD {
            # the "get the conic parameter" widget.
            return "This does not affect parabolas.  It is the radius of a\
                    circle; it is ... DLC\n\nNote that the scale is linear, but the increases and decreases in the scale's endpoints are logarithmic."
        }
        * {
            #DLC
            return "help text for widget $widget"
        }
    }
}

proc tconx_help_help { } {
    # DLC
    tconx_new_text [tconx_base].text_help "None yet DLC " "Help for GPLconx"
}

proc tconx_help_about { } {
    # DLC
    tconx_new_text [tconx_base].text_about "[tconx_package] version [tconx_version], Copyright (C) 1996-2001 David L. Chandler\n[tconx_package] comes with ABSOLUTELY NO WARRANTY\nThis is free software, and you are welcome to redistribute it\nunder certain conditions; read the COPYING file for details." "About [tconx_package]"
}
