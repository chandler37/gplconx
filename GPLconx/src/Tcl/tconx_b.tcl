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


# This file contains commands for handling the three model windows and
# the main window, plus some utility functions.

# Shortcuts so that we can refer to the models' windows easily:
proc pd { } { return [tconx_widg pd] }
proc kd { } { return [tconx_widg kd] }
proc puhp { } { return [tconx_widg puhp] }

proc tconx_version { } {
# This program's version number.
    global tconx_globls
    return $tconx_globls(Package_Version)
}

proc tconx_package { } {
# This program is part of a package whose name is returned.
    global tconx_globls
    return $tconx_globls(Package)
}

proc tconx_mother_widget_status { msg } {
    global tconx_globls
    set tconx_globls(bottom_entry) $msg
    update idletasks; # see Welch book.
}

proc tconx_current_mouse_semantics { } {
# Returns a ConxMenuChoices label for the current left click semantics, e.g.
# CONXCMD_MGETFS for the
# press-to-select-focus1-and-then-release-to-select-focus2 action.
    global tconx_globls
    if [info exists tconx_globls(click_action)] {
        return $tconx_globls(click_action)
    } else {
        return CONXCMD_MGETFS
    }
}

proc tconx_cca { action } {
# Sets a ConxMenuChoices label for the current left click semantics, e.g.
# CONXCMD_MGETFS for the
# press-to-select-focus1-and-then-release-to-select-focus2 action.
    global tconx_globls
    set tconx_globls(click_action) $action
}

proc tconx_finally_show_main_window { root } {
    Debugloc "tconx_finally_show_main_window"
    
    wm geometry $root +0+340
    wm deiconify $root
}

proc tconx_mother_widget_pathname { } {
    global tconx_globls
    return $tconx_globls(saved_root)
}

proc tconx_base { } {
# This is the base pathname for all widgets.  A new toplevel would be
# `[tconx_base].toplevel', e.g.
    global tconx_globls
    return $tconx_globls(saved_base)
}

proc tconx_b1_press { x y W } {
    Debugloc "tconx_b1_press { $x $y $W }";
    $W mouse down $x $y [tconx_current_mouse_semantics]
}

proc tconx_b1_release { x y W } {
    Debugloc "tconx_b1_release { $x $y $W }";
    $W mouse up $x $y [tconx_current_mouse_semantics]
    $W render
}

# proc tconx_b1_motion { x y W } {
#     Debugloc "tconx_b1_motion { $x $y $W }";
# }

proc tconx_b3_press { x y W } {
    Debugloc "tconx_b3_press $x $y $W";
    tconx_catch_error {
        tk_popup [tconx_menuname2path mouse_popup] \
            [expr [winfo rootx $W] + $x] \
            [expr [winfo rooty $W] + $y]
    }
}

proc tconx_exit_is_confirmed { {parent_window .} } {
    if {[tconx_confirms_exit]} {
        # Ask for confirmation.
        if {[tk_messageBox -parent $parent_window \
                      -title "Exit GPLconx?" \
                      -type yesno -icon question -message "Exit GPLconx?"]
            == "no"} {
            return 0
        }
    }
    return 1
}

proc tconx_exit { W } {
# Call this before closing a window $W.
# Returns 1 if the window remains open (after the user intervened)

    Debugloc "tconx_exit $W";
    if {$W == [tconx_mother_widget_pathname]} {
        if {![tconx_exit_is_confirmed]} {
            return 1
        }
    }
    set ow [tconx_open_windows]
    if {$ow == [list $W]} {
        Debug_assert { [llength $ow] == 1 }
        # We cannot close the last open model window because
        # we communicate through Togl commands, not Tcl commands.
        # How would we change the log level without a togl widget
        # to communicate through?
        if {![tconx_exit_is_confirmed]} {
            return 1
        }
        exit 0
    }
    destroy $W
    return 0
}

proc tconx_change_cd { newcd } {
# DLC experiment only; too unwieldy for production use.
    tconx_change_geometry CONXCMD_GETCD $newcd
    tconx_draw_int 0 CONXCMD_RESETTHISONE
    tconx_draw_int 0 CONXCMD_BCIRCLE
}

proc tconx_mother_widget_init { W } {
    # $base is an important invisible parameter to this script, allowing for
    # placing all widgets under different ownership
    Debugloc "tconx_mother_widget_init $W";
    global tconx_globls
    set tconx_globls(saved_root) $W
    if {"$W" == "."} {
        set base ""
    } else {
        set base $W
    }
    set tconx_globls(saved_base) $base

    foreach model {puhp kd pd} {
        set tconx_globls(check_$model) 1
        tconx_mother_check $model
    }

    bind all <ButtonPress-3> {tconx_b3_press %x %y %W}
    bind all [tconx_kill_key] {tconx_exit %W}
    bind $W <Alt-Key-u> "$base.checkbutton\#1 invoke; break;"
    bind $W <Alt-Key-k> "$base.checkbutton\#2 invoke; break;"
    bind $W <Alt-Key-p> "$base.checkbutton\#3 invoke; break;"

    wm protocol $W WM_DELETE_WINDOW [list tconx_exit $W]

    if {[tconx_focus_model_is_click_to_type]} {
        bind all <Button> {focus %W;}
    } else {
        bind all <Enter> {focus %W;}
    }

    $base.bottom_entry configure -state disabled
    tconx_mother_widget_status "This is GPLconx"
    wm title $W [tconx_get_window_title]
}

proc tconx_simple_initialize { W } {
    Debugloc "tconx_simple_initialize $W"
}

proc tconx_b2_press_action { x y W } {
    Debugloc "tconx_b2_press_action $x $y $W"
    tconx_catch_error {
        tk_popup [tconx_menuname2path choose_click_action_popup] \
            [expr [winfo rootx $W] + $x] \
            [expr [winfo rooty $W] + $y]
    }
}

proc tconx_b2_release_action { x y W } {
    Debugloc "tconx_b2_release_action $x $y $W"
}

proc tconx_b2_motion_action { x y W } {
    Debugloc "tconx_b2_motion_action $x $y $W"
}

proc tconx_new_viz_window { W {ttl "n/a"} {ident Joe} {xcoord 0} {ycoord 0}
                            {w_width 300} {w_height 300}
                            {one_to_one_aspect 1} } {
# Create a new window for visualization.
    tconx_new_unshown_window $W $ttl not_transient
    wm geometry $W +$xcoord+$ycoord
    if {$one_to_one_aspect} {
        wm aspect $W 1 1 1 1
        set w_height $w_width
    }
    simple_ui $W
    global tconx_globls
    togl $W.togl_wig -rgba true -width $w_width -height $w_height \
        -double $tconx_globls(double_buffered) \
        -depth false -accum false -stencil false \
        -privatecmap false -ident $ident

    grid rowconfigure $W.the_frame 1 -weight 1 -minsize 100
    grid columnconfigure $W.the_frame 1 -weight 1 -minsize 100
    pack $W.togl_wig -in $W.the_frame -fill both -expand 1
    bind $W.togl_wig <ButtonPress-1> {tconx_b1_press %x %y %W}
    #bind $W.togl_wig <B1-Motion> {tconx_b1_motion %x %y %W}
    bind $W.togl_wig <ButtonRelease-1> {tconx_b1_release %x %y %W}
    bind $W.togl_wig <ButtonPress-2> {tconx_b2_press_action %x %y %W}
    bind $W.togl_wig <B2-Motion> {tconx_b2_motion_action %x %y %W}
    bind $W.togl_wig <ButtonRelease-2> {
        tconx_b2_release_action %x %y %W
    }
    wm protocol $W WM_DELETE_WINDOW [list tconx_exit $W]
    wm deiconify $W
}

proc tconx_open_windows { } {
# Returns a list of toplevels that may include [pd], [kd], and [puhp]
    set W {}
    foreach w [list [kd] [puhp] [pd]] {
        if [winfo exists $w] {
            lappend W $w
        }
    }
    return $W
}

proc tconx_change_geometry { a args } {
# Changes parameters that affect the mathematics that is being visualized.
    set W [tconx_open_windows]
    foreach w $W {
        eval [list $w.togl_wig data_entry $a] $args
    }
    if {$W == {}} {
        return 1;
    } else {
        return 0;
    }
}

proc tconx_verbosity_level { e } {
    case $e {
        LOGG_TEXINFO { return 25.1 }
        LOGG_QUICK   { return 50.1 }
        LOGG_FULL    { return 100.1 }
        LOGG_BRES2   { return 800.1 }
        LOGG_OFF     { return -0.1 }
    }
}

proc tconx_do_conxcmd_only_once { a args } {
# Usually we do a viz command for each model simultaneously.  This
# function allows us to do a command only once in a model that cannot
# be predetermined.
    eval "[list [lindex [tconx_open_windows] 0].togl_wig data_entry $a] $args"
}

proc tconx_widg { id {base .} } {
# Returns the path for the widget identified by $id; the widget is
# a child of $base.
    case $id {
        widge_slider_CONXCMD_GETCD { return ${base}ws_CONXCMD_GETCD }
        widge_slider_CONXCMD_GETTOL { return ${base}ws_CONXCMD_GETTOL }
        widge_slider_CONXCMD_GETA { return ${base}ws_CONXCMD_GETA }
        widge_slider_CONXCMD_GETR { return ${base}ws_CONXCMD_GETR }
        widge_slider_CONXCMD_GETLOGLEVEL { return ${base}ws_CONXCMD_GETLOGLEVEL }
        widge_slider_CONXCMD_GETTSTEP { return ${base}ws_CONXCMD_GETTSTEP }
        kd { return ${base}modelkd }
        pd { return ${base}modelpd }
        puhp { return ${base}modelpuhp }
        * {
            # DLC
            Debugloc "tconx_wig <$id> <$base>"
        }
    }
}

proc tconx_get_viz_parameter { id {model kd} } {
# Returns the value of a parameter such as the conic distance or the viewing
# rectangle.  `CONXCMD_SHOWCD', e.g.
    Debugloc "tconx_get_viz_parameter $id";
    return [[tconx_widg $model].togl_wig query $id]
}

proc tconx_draw_one { windows cmd } {
# Do one drawing action in the selected windows.
    foreach w $windows {
        $w.togl_wig data_entry $cmd
    }
}

proc tconx_render_all { } {
# Cause a redisplay in all Togl widgets.
    tconx_cause_render [tconx_open_windows]
}

proc tconx_cause_render { windows } {
    foreach w $windows {
        $w.togl_wig render
    }
}

proc tconx_draw { args } {
# Does a redisplay.
    eval "tconx_draw_int 1 $args"
}

proc tconx_draw_int { redisplay args } {
    # Draw in one window (in which the mouse is?) or in all windows.
    set wins {}
    if [winfo exists [kd]] {
        lappend wins [kd]
    }
    if [winfo exists [pd]] {
        lappend wins [pd]
    }
    if [winfo exists [puhp]] {
        lappend wins [puhp]
    }
    foreach cmd $args {
        tconx_draw_one $wins $cmd
    }
    if {$redisplay} {
        tconx_cause_render $wins
    }
}

proc tconx_verbosity { a } {
    Debug_assert { [tconx_is_real $a] }
    tconx_do_conxcmd_only_once CONXCMD_GETLOGLEVEL $a
}

proc tconx_get_viz_input { conxgetcmd conxshowcmd } {
    Debugloc "@tconx_get_viz_input"
    tconx_slider_new [tconx_widg widge_slider_$conxgetcmd] \
        [list tconx_change_geometry $conxgetcmd] \
        [tconx_get_viz_parameter $conxshowcmd]
    Debugloc "@end tconx_get_viz_input"
}

proc tconx_model_description { model } {
    case $model {
        puhp { return "Poincare Upper Half Plane" }
        kd { return "Beltrami-Klein Disk" }
        pd { return "Poincare Disk" }
        * { return "" }
    }
}

proc tconx_model_geometry { model } {
# Returns a list {xpos ypos width height}, all are pixels.
    set sz 300
    case $model {
        puhp { return "0 5 $sz $sz" }
        kd { return "[expr $sz+40] 5 $sz $sz" }
        pd { return "[expr 2*($sz+40)] 5 $sz $sz" }
    }
}

proc tconx_model_number { model } {
    case $model {
        puhp { return 0 }
        kd { return 1 }
        pd { return 2 }
        * { return -1 }
    }
}

proc tconx_mother_check { model } {
    Debugloc "tconx_mother_check $model"
    if [winfo exists [tconx_widg $model]] {
        if [tconx_exit [tconx_widg $model]] {
            global tconx_globls
            set tconx_globls(check_$model) 1
        }
    } else {
        eval "[list tconx_new_viz_window [tconx_widg $model] \
                      [tconx_model_description $model] $model] \
              [tconx_model_geometry $model]"
    }
    after idle {focus [tconx_mother_widget_pathname]}
}

proc q { args } {
# DLC debugging only.
# Keyboard shorcuts.
# Type `q vb s' to quickly debug the bresenham2 method and then sync, e.g.
    set result {}
    foreach a $args {
        lappend result [tconx_alias $a]
    }
    return $result
}

proc tconx_alias { cmd } {
# This just allows for quickly typed commands.
    set cmd [string toupper $cmd]
    case $cmd {
        VB { tconx_verbosity [tconx_verbosity_level LOGG_BRES2] }
        VQ { tconx_verbosity [tconx_verbosity_level LOGG_QUICK] }
        VF { tconx_verbosity [tconx_verbosity_level LOGG_FULL] }
        VT { tconx_verbosity [tconx_verbosity_level LOGG_TEXINFO] }
        V0 { tconx_verbosity [tconx_verbosity_level LOGG_OFF] }
        QCD { return [[puhp].togl_wig query CONXCMD_SHOWCD] }
        . { tconx_do_conxcmd_only_once CONXCMD_OUTPUT }
        S { sync }
        * { puts "unrecognized alias" }
    }
}

proc p { a } {
    return [[puhp].togl_wig yap "$a\n"]
}
