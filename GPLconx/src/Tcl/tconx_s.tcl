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


# This is code for the slider widget, i.e. the widget which is generated
# by SpecTcl 1.1 from `slider.ui'.

# Code in this file depends on `tconx_w.tcl', `tconx_h.tcl',
# `slider.ui.tcl', `tconx_d.tcl', `tconx_b.tcl' and `tconx_p.tcl'

proc tconx_slider_bare_minimum { } {
    # DLC do the numerical analysis.
    return 0.0001
}

proc tconx_slider_increment { Smin Smax {N 25} } {
# Returns the correct increment to get from Smin to Smax on the
# scale in N hops.
    return [expr "($Smax - $Smin) / ($N - 1)"]
}

proc tconx_slider_new { W cmd initial_value {window_title {Slider Entry}} } {
# Creates a new toplevel $W that will call `eval $cmd 1.03'
# when the user presses `Apply'.  If the toplevel already exists, it will
# be raised and the scale value will be updated to $initial_value.  (The
# command $cmd is updated, too, if the window already exists.)
# The toplevel's title (not updated, just used at creation) is $window_title.

    Debugloc "[list tconx_slider_new $W $cmd $initial_value $window_title]"

    global $W.apply_cmd $W.scale_var
    if [winfo exists $W] {
        set $W.apply_cmd $cmd
        set $W.scale_var $initial_value
        tconx_raise $W
    } else {
        tconx_new_unshown_window $W $window_title
        Debugloc "tconx_slider_new 100"
        slider_ui $W
        Debugloc "tconx_slider_new 200"
        # DLC 
        set Smin [tconx_slider_bare_minimum]
        set Smax [expr $initial_value*2.0]
        Debugloc "tconx_slider_new 300"
        $W.scale\#1 configure -resolution 0
        Debugloc "tconx_slider_new 400"
# DLC -by [tconx_slider_increment $Smin $Smax]
        tconx_slider_set_scale_min $W $Smin
        Debugloc "tconx_slider_new 500"
        tconx_slider_set_scale_max $W $Smax
        Debugloc "tconx_slider_new 600"
        set $W.apply_cmd $cmd
        Debugloc "tconx_slider_new 700"


        # We don't want this to trigger the motion command:
        #set saved_cmd [$W.scale\#1 cget -command]
        #$W.scale\#1 configure -command ""
        set $W.scale_var $initial_value
        #$W.scale\#1 configure -command $saved_cmd


        Debugloc "tconx_slider_new 800"
        
        # DLC Center the window over another window.

        wm deiconify $W
        Debugloc "tconx_slider_new 900"
    }
    Debug_assert { [info   exists $W.apply_cmd] }
}

proc tconx_slider_set_scale_min { W Smin } {
# Sets the scale's minimum value.
    $W.label\#1 configure -text [tconx_slider_labelready $Smin]
    $W.scale\#1 configure -from $Smin
}

proc tconx_slider_scale_motion_matters { W } {
    global $W.first_motion
    if {![info exists $W.first_motion]} {
        Debugloc "$W's first motion"
        set $W.first_motion now_it_does_matter
        return 0
    }
# Returns true if we care about the motion of $W's scale, false if we don't.
    global $W.scale_var
    # return false during initialization at least #DLC
    return 1
}

proc tconx_slider_scale_motion { W newvalue } {
    Debugloc "tconx_slider_scale_motion $W $newvalue"
    if [tconx_slider_scale_motion_matters $W] {
        tconx_slider_button apply $W
        tconx_render_all
        # rerender. DLC
    }
}

proc tconx_slider_set_scale_max { W Smax } {
# Sets the scale's maximum value.
    $W.label\#2 configure -text [tconx_slider_labelready $Smax]
    $W.scale\#1 configure -to $Smax
}

proc tconx_slider_init { W } {
# Initialize the slider widget.
# This does general initialization, nothing specific (e.g., setting
# the low and high values).

    Debugloc "tconx_slider_init $W"

    # button#1 has accelerator key `h', for example.
    set buttons_with_bindings \
        [list [list button\#1 h] \
             [list button\#2 a] \
             [list button\#3 c] \
             [list button\#6 i] \
             [list button\#7 l] \
             [list button\#8 s] \
             [list button\#9 g] \
             [list button\#10 v] \
             [list button\#11 w] \
             [list button\#12 m]]
    tconx_set_up_button_bindings $W $buttons_with_bindings
}

proc tconx_slider_labelready { n } {
# Returns a formatted version of $n so that the numbers look however we
# want them to.
    return [format {%.4e} $n]
}

proc tconx_slider_scale_min { W } {
# Returns the minimum value on $W's scale.
# DLC why not -from, it isn't formatted!
    return [$W.label\#1 cget -text]
}

proc tconx_slider_scale_max { W } {
# Returns the maximum value on $W's scale.
    return [$W.label\#2 cget -text]
}

proc tconx_slider_button { button W } {
# A slider widget $W has just had button $button pressed.  Handle it.

# The slider covers the interval [A, B].  Neither A nor B can go below
# [tconx_slider_bare_minimum], and A <= B at all times.  We wish to
# increase and decrease A and B logarithmically.


# DLC  Instead of using A and B, use A and slider value, slider value and B.

    Debugloc "tconx_slider_button ${W}::$button"

    set baremin [tconx_slider_bare_minimum]
    set base [tconx_slider_logarithm_base]
    Debug_assert {$base > 1.0}
    upvar \#0 $W.scale_var slider_value

    case $button {
        {left_higher left_set left_lower set_manually} {
            set A [tconx_slider_scale_min $W]
            set B $slider_value
        }
        {right_higher right_set right_lower set_manually} {
                set A $slider_value
                set B [tconx_slider_scale_max $W]
        }
    }
    
    case $button {
        left_higher {
            # Increase the minimum scale value.
            # Set A to (B-(B-A)/base) so that the two are now $base
            # times closer together.
            tconx_slider_set_scale_min $W [expr $B-($B-$A)/$base]
        }
        left_set {
            # Set the minimum scale value.
            # DLC
        }
        left_lower {
            # Decrease the minimum scale value.
            # Set A to (B-(base-1)*(B-A)) so that the two are now $base
            # times farther apart.
            set diff [tconx_max [expr ($B-$A)] $baremin]
            tconx_slider_set_scale_min $W \
                [tconx_max [expr $B-($base-1.0)*$diff] $baremin]
        }
        right_higher {
            # Increase the maximum scale value.
            # Set B to (A+($base-1)*(B-A)) so that the two are now $base
            # times farther apart.

            # If B is the current scale value, handle it separately:
            set diff [tconx_max [expr ($B-$A)] $baremin]
            tconx_slider_set_scale_max $W [expr $A+($base-1.0)*$diff]
        }
        right_set {
            # Set the maximum scale value.
            # DLC
        }
        right_lower {
            # Decrease the maximum scale value.
            # Set B to (A+(1/$base)*(B-A)) so that the two are now $base
            # times closer together.  Solve "B-A=base*(B'-A)" for B', that is.
            tconx_slider_set_scale_max $W [expr $A+(1.0/$base)*($B-$A)]
        }
        set_manually {
            # Set the scale to the value, and change the minimum
            # and maximum scale values.
            # DLC
        }
        help {
            tconx_display_help [tconx_get_help $W] $W
        }
        apply {
            # Change the associated datum to the scale's value after clamping.
            global $W.apply_cmd $W.scale_var
            upvar \#0 $W.apply_cmd acmd $W.scale_var svar
            set sv $svar
            set sbm $baremin
            if {$sv < $sbm} {
                set sv $sbm
            }
            eval "$acmd $sv"
        }
        close {
            destroy $W
        }
    }
}
