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


# This file contains preferences that a user might wish to change.
# The facilities for this are not currently present (except for a hacker).

proc tconx_get_window_title { } {
# The title for the main window, the mother widget's containing toplevel.
    return {GPLconx}
}

proc tconx_focus_model_is_click_to_type { } {
# Is the focus model "click to type" or "mouse over to type"?
    return 1
}

proc tconx_slider_logarithm_base { } {
# Make this larger for bigger jumps when you press the slider widget's
# "lower" and "higher" buttons.
    return 10.0
}

proc tconx_confirms_exit { } {
# Returns true if you must answer a dialog before exiting the program (via
# closing the window through the facilities of the window manager or
# otherwise.
    return 1
}

proc tconx_kill_key { } {
# Returns the name of the keystroke that closes windows.
    return <Control-Key-c>
}
