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


# This is code that is useful for widgets in general.

# Code in this file depends on `tconx_d.tcl' and `tconx_b.tcl'

proc tconx_button_invoke { button } {
# Returns code that will safely invoke the button $button.
    return "$button flash; $button invoke; break;"
}

proc tconx_bind_return_key { widget buttons } {
# Bind the Return key (aka the Enter key) to the list of $buttons that
# are children of $widget.  The binding just does what a mouse click
# would.
    foreach button $buttons {
        bind ${widget}.$button <Key-Return> \
            [tconx_button_invoke ${widget}.$button]
    }
}

proc tconx_bind_button_accelerators { widget buttons keys } {
# Bind the list of $buttons in $widget so that the Nth button uses
# the nth accelerator key.  There must be a key for each button,
# such as `j', or the empty string if no binding should be set up
# for a key.
    set i 0
    Debug_assert {[llength $buttons] == [llength $keys]}
    foreach button $buttons {
        tconx_bind_button_accelerator $widget $button [lindex $keys $i]
        incr i 1
    }
}

proc tconx_bind_button_accelerator { widget button accel } {
# Bind a button in a widget to an Alt-Key accelerator if $accel != ""
        if {"$accel" != ""} {
            bind ${widget} <Alt-Key-$accel> \
                [tconx_button_invoke ${widget}.$button]
        }
}

proc tconx_set_up_button_bindings { widget buttons_with_bindings } {
# Pass in the parent widget and a list of lists of length 2,
# [list buttonpath accelerator] where accelerator is {} if there is
# no Alt-Key accelerator binding.

    tconx_bind_return_key $widget \
        [tconx_lapply_after lindex $buttons_with_bindings 0]
    foreach bwb $buttons_with_bindings {
        tconx_bind_button_accelerator $widget [lindex $bwb 0] [lindex $bwb 1]
    }
}

proc tconx_display_help { help_message related_widget } {
    # Display help for $related_widget.  The text to display is $help_message.
    tconx_new_text $related_widget.cshelp $help_message
}

proc tconx_raise { widget {above_whom {}} } {
# Raise window $widget [above $above_whom]
    if {"[wm state $widget]" == "iconic"} {
        wm deiconify $widget
    }
    eval "raise $widget $above_whom"
}

proc tconx_new_unshown_window {W window_title {is_transient "1"}} {
    # Creates a new toplevel $W with title $window_title, but you must
    # deiconify $W to see it.
    toplevel $W
    wm withdraw $W
    if {"$is_transient" == "1"} {
        wm transient $W
    }
    if {$window_title != ""} {
        wm title $W $window_title
    }
}
