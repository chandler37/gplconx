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


# This is code for the text widget, i.e. the widget which is generated
# by SpecTcl 1.1 from `text.ui'.

proc tconx_new_text { W text {window_title {GPLconx Info}} } {
# Creates a new toplevel $W that will show $text until the user presses
# `Close'.  If the toplevel already exists, it will be raised and the text
# will be updated.
# The toplevel's title (not updated, just used at creation) is $window_title.

    Debugloc "[list tconx_new_text $W $text $window_title]"

    global $W.apply_cmd $W.scale_var
    if [winfo exists $W] {
        tconx_text_set_text $W $text
        tconx_raise $W
    } else {
        tconx_new_unshown_window $W $window_title
        text_ui $W
        tconx_bind_return_key $W button\#1
        tconx_bind_button_accelerators $W button\#1 c
        tconx_text_set_text $W $text
        wm deiconify $W
    }
}

proc tconx_text_set_text { W text } {
    $W.text\#1 configure -state normal
    $W.text\#1 delete 1.0 end
    $W.text\#1 insert 1.0 $text
    $W.text\#1 configure -state disabled
# Set the size of the text to accommodate this. DLC
}
