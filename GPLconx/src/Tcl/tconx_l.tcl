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


# Tcl code to set up the menus.

#DLC do ^-C binding for widgets

proc tconx_init_menu_names { } {
    tconx_define_named_menu choose_click_action .mouse_mb.m
    tconx_define_named_menu file .file_mb.m
    tconx_define_named_menu help .help_mb.m
    tconx_define_named_menu draw .draw_mb.m
    tconx_define_named_menu input .input_mb.m
    tconx_define_named_menu choose_click_action_popup .cca_popup
    tconx_define_named_menu mouse_popup .mouse_popup
}

proc tconx_menus_init { } {
    tconx_init_menu_names

    tconx_init_file_menu [tconx_menuname2path file]
    tconx_init_input_menu [tconx_menuname2path input]
    tconx_init_help_menu [tconx_menuname2path help]
    tconx_init_draw_menu [tconx_menuname2path mouse_popup]
    tconx_init_draw_menu [tconx_menuname2path draw]
    tconx_init_choose_click_action_menu \
        [tconx_menuname2path choose_click_action_popup]
    tconx_init_choose_click_action_menu \
        [tconx_menuname2path choose_click_action]
    
    # DLC Set up some menu item accelerator keys with bindings.
}

proc tconx_init_help_menu { m } {
    tconx_new_nameless_menu $m
    $m add command -label "Help" -underline 0 -command {tconx_help_help}
    $m add command -label "About" -underline 0 -command {tconx_help_about}
}

proc tconx_init_file_menu { m } {
    tconx_new_nameless_menu $m
    $m add command -command {tconx_read_in_st_commands_ui} \
        -label {Execute Text File of Commands} \
        -underline 0
    $m add command -command {tconx_draw CONXCMD_RESETTHISONE} \
        -label {Clear windows, retain changes} -underline 1
    $m add command -command {tconx_draw CONXCMD_CLEAR} -label {Reset program} \
        -underline 0
    $m add separator
    $m add command -command {tconx_exit [tconx_mother_widget_pathname]} \
        -label Exit -underline 1
}

proc tconx_init_input_menu { m } {
    tconx_new_nameless_menu $m

    global tconx_globls
    if ![info exists tconx_globls(prw_wrap)] {
        set tconx_globls(prw_wrap) word; #DLC .GPLconx entry
        tconx_set_prw_wrapping $tconx_globls(prw_wrap)
    }
    $m add checkbutton -label "Word-wrapping for parse results" -underline 0 \
        -offvalue none -onvalue word -variable tconx_globls(prw_wrap) \
        -command {tconx_set_prw_wrapping $tconx_globls(prw_wrap)}

#DLC radio button for button2 "mouse" menu

    $m add separator
    $m add checkbutton -label "Poincare Upper Half Plane" -underline 9 \
        -command {tconx_mother_check puhp} -variable tconx_globls(check_puhp)
    $m add checkbutton -label "Beltrami-Klein Disk" -underline 9 \
        -command {tconx_mother_check kd} -variable tconx_globls(check_kd)
    $m add checkbutton -label "Poincare Disk" -underline 0 \
        -command {tconx_mother_check pd} -variable tconx_globls(check_pd)

    if {![tconx_we_are_cxxconx]} {
        $m add separator
        $m add command -label "Conic distance" -underline 0 \
            -command {tconx_get_viz_input CONXCMD_GETCD CONXCMD_SHOWCD}
        $m add command -label "Tolerance for slow method" -underline 0 \
            -command {tconx_get_viz_input CONXCMD_GETTOL CONXCMD_SHOWTOL}
        $m add command -label "Step size for drawing circles" -underline 0 \
            -command {tconx_get_viz_input CONXCMD_GETTSTEP CONXCMD_SHOWTSTEP}
        $m add command -label "Debug logging level" -underline 0 \
            -command {tconx_get_viz_input CONXCMD_GETLOGLEVEL CONXCMD_SHOWLOGLEVEL}
    }
    # DLC find a use for that widget at least!
}

proc tconx_init_choose_click_action_menu { m } {
    tconx_new_nameless_menu $m
    
    # Update the viz during mouse motion, not just when the mouse button is
    # released, if the following is checked:
    global tconx_globls
    if ![info exists tconx_globls(update_during_mouse_motion)] {
        set tconx_globls(update_during_mouse_motion) 1; # DLC config file entry
        tconx_set_mouse_motion_matters $tconx_globls(update_during_mouse_motion)
        tconx_cca CONXCMD_GETF1; # DLC config file entry
    }
    $m add checkbutton -label "Update during mouse motion" \
        -offvalue 0 -onvalue 1 \
        -variable tconx_globls(update_during_mouse_motion) \
        -command {tconx_set_mouse_motion_matters $tconx_globls(update_during_mouse_motion)} \
        -underline 0

    $m add separator
    $m add command -label "Get Foci" \
        -command {tconx_cca CONXCMD_MGETFS} -underline 4
    $m add command -label "Get Focus 1" \
        -command {tconx_cca CONXCMD_MGETF1} -underline 5
    $m add command -label "Get Focus 2" \
        -command {tconx_cca CONXCMD_MGETF2} -underline 6
    $m add command -label "Get viewing rectangle" \
        -command {tconx_cca CONXCMD_MZOOM} -underline 4
    $m add command -label "Get magic distance" \
        -command {tconx_cca CONXCMD_MGETCD} -underline 4
    $m add command -label "Get alternate line" \
        -command {tconx_cca CONXCMD_MGETALT} -underline 4
}

proc tconx_init_draw_menu { m } {
# m is a pathname.
    tconx_new_nameless_menu $m
    tconx_init_submenu $m .c Conics 0 tconx_init_conics_menu
    tconx_init_submenu $m .d Draw 0 tconx_init_mp_draw_menu
    tconx_init_submenu $m .t Toggle 0 tconx_init_toggles_menu
    tconx_init_submenu $m .sc {SLOW (yet reliable!) conics} 0 \
        tconx_init_slowconics_menu
    $m add separator
    $m add command -command {tconx_draw CONXCMD_OUTPUT} \
        -label {Output to console} -underline 0
    $m add command -command {tconx_draw CONXCMD_RESETTHISONE} \
        -label {Clear windows, retain changes} -underline 1
    $m add command -command {tconx_draw CONXCMD_CLEAR} -label {Reset program} \
        -underline 0
}

proc tconx_init_toggles_menu { m } {
    tconx_new_nameless_menu $m
    $m add command -label {Garnish} -command {tconx_draw CONXCMD_GARNISHON} \
        -underline 0
    $m add command -label {Automatic circular borders} \
        -command {tconx_draw CONXCMD_BORDERSON} -underline 0
    $m add command -label {Automatic Foci Drawing} \
        -command {tconx_draw CONXCMD_AUTOFOCION} -underline 1
}

proc tconx_init_slowconics_menu { m } {
    tconx_new_nameless_menu $m
    $m add command -label {Circle} -command {tconx_draw CONXCMD_CIRCLE} \
        -underline 0
    $m add command -label {Ellipse/Hyperbola} \
        -command {tconx_draw CONXCMD_ELLIPSE} -underline 0
    $m add command -label {Parabola} -command {tconx_draw CONXCMD_PARABOLA} \
        -underline 0
    $m add command -label {Equidistant curve} \
        -command {tconx_draw CONXCMD_EQDIST} -underline 0
}

proc tconx_init_conics_menu { m } {
    tconx_new_nameless_menu $m
    $m add command -label {Circle} -command {tconx_draw CONXCMD_BCIRCLE} \
        -underline 0
    $m add command -label {Ellipse/Hyperbola} \
        -command {tconx_draw CONXCMD_BELLIPSE} -underline 0
    $m add command -label {Parabola} -command {tconx_draw CONXCMD_BPARABOLA} \
        -underline 0
    $m add command -label {Equidistant curve} \
        -command {tconx_draw CONXCMD_BEQDIST} -underline 0
}

proc tconx_init_mp_draw_menu { m } {
    tconx_new_nameless_menu $m
    $m add command -label {Garnish} -command {tconx_draw CONXCMD_GARNISHON} \
        -underline 0

    $m add command -label "Line thru foci" \
        -command {tconx_draw CONXCMD_LINEF} -underline 0
    $m add command -label "Alternate line (e.g., defined by a & r)" \
        -command {tconx_draw CONXCMD_ALTLINE} -underline 0
    $m add command -label "Focus 1" -command {tconx_draw CONXCMD_FOCUS} \
        -underline 0
    $m add command -label "Foci 1 & 2" -command {tconx_draw CONXCMD_FOCI} \
        -underline 1
    $m add command -label "Segment between foci" \
        -command {tconx_draw CONXCMD_SEGMENT} -underline 0
}

