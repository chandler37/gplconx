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


# This file contains commands facilitating the Smalltalkish language
# user interface.

# Shortcuts so that we can refer to the models' windows easily:


proc p { a } {
    return [tconx_parse_smalltalkish $a]
}

proc tconx_parse_smalltalkish { a } {
    # Any togl widget will do -- this should be a built-in command but
    # I'm lazy.
    Debuglocup {tconx_parse_smalltalkish "`$a'"}
    return [[puhp].togl_wig yap "$a\n"]
}

proc Render { } {
    tconx_parse_smalltalkish {pdc sync. kdc sync. uhpc sync}
    [pdt] render; [kdt] render; [puhpt] render
}

proc tconx_pew { } {
# Returns the path of the parser's entry widget (pew)
    return [tconx_base].entry\#3
}

proc tconx_prw { } {
# Returns the path of the parser's result widget (pew)
    return [tconx_base].text\#1
}

proc tconx_init_pew { } {
    [tconx_prw] configure -state disabled
    bind [tconx_pew] <Key-Return> tconx_handle_pew
}

proc tconx_set_prw_wrapping { e } {
    if {"$e" != "word" && "$e" != "char"} {
        set e none
    }
    [tconx_prw] configure -wrap $e
}

proc tconx_prw_retains_old_results { } {
    global tconx_globls
    if ![info exists tconx_globls(prw_retains_old_results)] {
        set tconx_globls(prw_retains_old_results) 1
    }
    return $tconx_globls(prw_retains_old_results)
}

proc tconx_handle_pew { } {
    set cmdtxt [[tconx_pew] get]
    if {[string length $cmdtxt] > 0} {
        # DLC could throw an error:
        tconx_handle_st_cmd $cmdtxt
        [tconx_pew] selection range 0 end
    }
# DLC we don't get an empty result when we should thanks to a lazy mparse.hh
}

proc tconx_prw_prompt { } {
    return "\n\$ "; # DLC config file entry
}

proc tconx_prw_result_marker { } {
    return "\n>>> "; # DLC config file entry
}

proc tconx_prw_result_end { } {
    return ""; # DLC config file entry
}

proc tconx_color_prw { } {
    return 1; # DLC config file entry
}

proc tconx_handle_st_cmd { cmd } {
    set r [tconx_parse_smalltalkish $cmd]
    [tconx_prw] configure -state normal
    if [tconx_prw_retains_old_results] {
        global tconx_globls
        if [tconx_color_prw] {
            if ![info exists tconx_globls(prwtagnum)] {
                set tconx_globls(prwtagnum) 1
            }
            set c1 [[tconx_prw] index end]
        }
        [tconx_prw] insert end "[tconx_prw_prompt]$cmd"
        if [tconx_color_prw] {
            set c2 [[tconx_prw] index end]
        }
        [tconx_prw] insert end "[tconx_prw_result_marker]$r[tconx_prw_result_end]"
        if [tconx_color_prw] {
            set c3 [[tconx_prw] index end]
            [tconx_prw] tag add "tag$tconx_globls(prwtagnum)" $c1 $c2
            [tconx_prw] tag configure "tag$tconx_globls(prwtagnum)" -foreground purple -background orange
            incr tconx_globls(prwtagnum)
            [tconx_prw] tag add "tag$tconx_globls(prwtagnum)" $c2 $c3
            [tconx_prw] tag configure "tag$tconx_globls(prwtagnum)" -foreground black -background white
            incr tconx_globls(prwtagnum)
        }
    } else {
        [tconx_prw] delete 1.0 end
        [tconx_prw] insert 1.0 $r
    }
    [tconx_prw] see end
    [tconx_prw] configure -state disabled
}

proc tconx_read_in_st_commands_ui { } {
    set fname [tk_getOpenFile -filetypes {{{Text files} {.txt}} \
                                              {{All files} *   }} ]
    tconx_read_in_st_commands $fname
}

proc tconx_log_is_repeated_in_prw { } {
    return 1; # DLC config file entry at the very least.
}

# DLC save commands processed by tconx_parse_smalltalkish, up to a limit,
# and let the user save them in a text file.

# DLC allow reading in a file of stish commands at startup
# DLC allow clearing the results and limiting the number of words, chars, or
# lines in prw

proc tconx_read_in_st_commands { f } {
# Reads a file named f of Smalltalk-ish commands and executes them.
    if {[file exists $f] && [file isfile $f]} {
        set handle [open $f r]; # read-only
        set contents [read -nonewline $handle]
        set lines [split $contents "\n"]
        unset contents
        if [tconx_log_is_repeated_in_prw] {
            foreach line $lines {
                tconx_handle_st_cmd $line
            }
        } else {
            foreach line $lines {
                tconx_parse_smalltalkish $line
            }
        }
        close $handle
    }
    # DLC bgerror or an exception...
}

proc tconx_we_are_cxxconx { } {
# If this returns true, then cxxconx is running, not tconx.
    global tconx_globls
    return [info exists tconx_globls(CXXCONX)]
}

# We give a few geometric objects special names so that we can then
# use the mouse to alter them.
proc tconx_circle1_name { } {
# DLC isReservedWord test
    return "circle";
}

proc tconx_focus1_name { } {
    return "f1"; # DLC config file entry
}

proc tconx_focus2_name { } {
    return "f2"; # DLC config file entry
}

proc tconx_line1_name { } {
    return "l"; # DLC config file entry
}

proc tconx_parabola1_name { } {
    return "pa1"; # DLC config file entry
}

proc tconx_conicdistance_name { } {
# The radius of a circle, the scalar that defines an ellipse or hyperbola
 # or equidistant curve.
    return "r"; # DLC config file entry
}

proc tconx_widget_to_model { W } {
    case $W {
        *kd* { return kd }
        *pd* { return pd }
        *puhp* { return uhp }
        * {
            puts "that should not happen";
            exit 4;
        }
    }
}

proc tconx_widget_to_stcanvas { W } {
# {tconx_parse_smalltalkish "kdc sync"} is facilitated by this.  Pass in
# a widget name, [pd], e.g., to get the associated Smalltalkish canvas.
    case $W {
        *kd* { return kdc }
        *pd* { return pdc }
        *puhp* { return uhpc }
        * {
            puts "that should not happen either";
            exit 4;
        }
    }
}

proc tconx_change_focus1 { x y W } {
    Debuglocup {tconx_change_focus1 $x $y $W}
    tconx_change_point [tconx_focus1_name] $x $y $W
    tconx_finalize_mouse_action $W
}

proc tconx_set_conicdistance_press { x y W } {
# Change this and tconx_set_conicdistance* all at once or not at all
    Debuglocup {tconx_set_conicdistance_press $x $y $W}
    global tconx_globls
    set tconx_globls(cd_press_param) [list $x $y $W]
}

proc tconx_set_conicdistance_motion { x y W } {
# Change this and tconx_set_conicdistance* all at once or not at all

# This is CPU-intensive.
    global tconx_globls
    set saved $tconx_globls(cd_press_param)
    tconx_set_conicdistance_release $x $y $W
    set tconx_globls(cd_press_param) $saved
}

proc tconx_set_conicdistance_release { x y W } {
# Change this and tconx_set_conicdistance* all at once or not at all
    Debuglocup {tconx_set_conicdistance_release $x $y $W}
    global tconx_globls
    Debug_assert { [info exists tconx_globls(cd_press_param)] }
    set s $tconx_globls(cd_press_param)
    Debug_assert { [lindex $s 2] == "$W" }
    set model [tconx_widget_to_model $W]
    if {[tconx_parse_smalltalkish "[tconx_conicdistance_name] isFloat"] == "false"} {
        tconx_parse_smalltalkish "[tconx_conicdistance_name] := 1.01";
        # DLC This fiddling has ruined our chance of changing the circle
        # by changing the radius.
    }
    tconx_parse_smalltalkish \
        "[tconx_conicdistance_name] set: \
                     ((Point x: $x y: $y model: $model)\
                        distanceFromPoint: (Point x: [lindex $s 0]\
                                            y: [lindex $s 1] model: $model))"
    unset tconx_globls(cd_press_param)
    tconx_finalize_mouse_action $W
}

proc tconx_sync_immediately { } {
    return 1; # DLC config file entry
}

proc tconx_we_use_late_binding { } {
    # The default init uses late binding in the Klein disk but not in the
    # Poincare Upper Half Plane.  If you use late binding anywhere, then
    # this should return 1.  The default mouse actions take advantage of
    # late binding.
    return 1; # DLC config file entry
}

# DLC perhaps we should forget the magic variable names in favor
# of changing the window's associated canvas's Nth entry.  Then N is the
# magic number, which can be implemented easily in C++ to be a read-only
# array position.

proc tconx_finalize_mouse_action { W } {
    if {[tconx_sync_immediately]} {
        if {[tconx_we_use_late_binding]} {
            set l [list [pdt] [kdt] [puhpt]]
        } else {
            set l [list $W]
        }
        foreach w $l {
            tconx_parse_smalltalkish "[tconx_widget_to_stcanvas $w] sync"
            if [tconx_auto_update] {
                $w render
            }
        }
    }
}

proc tconx_change_focus2 { x y W } {
    Debuglocup {tconx_change_focus2 $x $y $W}
    tconx_change_point [tconx_focus2_name] $x $y $W
    tconx_finalize_mouse_action $W
}

proc tconx_change_point { varname x y W } {
    Debuglocup {tconx_change_point $varname $x $y $W}
    set model [tconx_widget_to_model $W]
    if {[tconx_parse_smalltalkish "$varname isPoint"] == "false"} {
        puts "You have been fiddling with $varname, which is treated\
              specially by the UI";
        # DLC
        tconx_parse_smalltalkish "$varname := Point x: $x y: $y model: $model"
    } else {
        # This is more efficient since the GC does not get involved.
        tconx_parse_smalltalkish \
            "$varname model: $model ...\
             $varname x set: $x ...\
             $varname y set: $y"
    }
}

proc tconx_debug_button_action { x y W } {
# DLC no longer needed, right?
    puts "model coords are $x $y"
}

proc cxxconx_setup_button_action { button state action } {
    global tconx_globls
    set tconx_globls(cxxconx_${button}_${state}_action) $action
}

proc cxxconx_setup_mouse_semantics { m } {
    set ps [cxxconx_mouse_semantics $m press]
    set ms [cxxconx_mouse_semantics $m motion]
    set rs [cxxconx_mouse_semantics $m release]
    if {"" != "$ps"} {
        cxxconx_setup_button_action b1 press $ps
        foreach W [tconx_open_windows] {
            bind $W.togl_wig <ButtonPress-1> {cxxconx_b1_press %x %y %W}
        }
    } else {
        foreach W [tconx_open_windows] {
            bind $W.togl_wig <ButtonPress-1> ""
        }
    }
    if {"" != "$ms"} {
        cxxconx_setup_button_action b1 motion $ms
        foreach W [tconx_open_windows] {
            bind $W.togl_wig <B1-Motion> {cxxconx_b1_motion %x %y %W}
        }
    } else {
        foreach W [tconx_open_windows] {
            bind $W.togl_wig <B1-Motion> ""
        }
    }
    if {"" != "$rs"} {
        cxxconx_setup_button_action b1 release $rs
        foreach W [tconx_open_windows] {
            bind $W.togl_wig <ButtonRelease-1> {cxxconx_b1_release %x %y %W}
        }
    } else {
        foreach W [tconx_open_windows] {
            bind $W.togl_wig <ButtonRelease-1> ""
        }
    }
}

proc cxxconx_mouse_semantics { m state } {
    global tconx_globls
    case $m {
        *GETCD {
            if {$state == "press"} {
                return tconx_set_conicdistance_press
            }
            if {$state == "release"} {
                return tconx_set_conicdistance_release
            }
            if {[tconx_mouse_motion_matters] && ($state == "motion")} {
                return tconx_set_conicdistance_motion
            }
        }
        *GETF1 {
            if {$state == "press"} {
                return tconx_change_focus1
            }
            if {[tconx_mouse_motion_matters] && ($state == "motion")} {
                return tconx_change_focus1
            }
        }
        *GETF2 {
            if {$state == "press"} {
                return tconx_change_focus2
            }
            if {[tconx_mouse_motion_matters] && ($state == "motion")} {
                return tconx_change_focus2
            }
        }
    }
    return "";
}

# DLC st color by name

proc tconx_mouse_motion_matters { } {
    global tconx_globls
    if ![info exists tconx_globls(mouse_motion_matters)] {
        return 0
        # this is not the real init, that's in `tconx_l.tcl'
    }
    return $tconx_globls(mouse_motion_matters)
}

proc tconx_set_mouse_motion_matters { itDoes } {
    global tconx_globls
    if {"$itDoes" == "0"} {
        set tconx_globls(mouse_motion_matters) 0
    } else {
        set tconx_globls(mouse_motion_matters) 1
    }
}

proc tconx_st_init { } {
# DLC Config file entry
    set pa1 [tconx_parabola1_name]
    set f1 [tconx_focus1_name]
    set f2 [tconx_focus2_name]
    set circle [tconx_circle1_name]
    set r [tconx_conicdistance_name]
    set line1 [tconx_line1_name]

    # Replace these with "clone" or "cloneDeep" if you don't want all
    # models to start out alike.
    set pdcClone ""
    set uhpcClone ""

    tconx_parse_smalltalkish "$f1 := Point x: .5 y: .75 model: uhp ...\
                              $f2 := Point x: .2 y: .75 model: uhp ...\
                              $f1 color R: 1 G: 0 B: 0 ...\
                              $f2 color R: .7 G: .4 B: 0 ...\
                              $line1 := Line A: $f1 B: $f2 isSegment: false ...\
                              $line1 color H: 200 S: .5 V: .4 ...\
                              pointThickness := 6.0 ...\
                              $f1 thickness: pointThickness ...\
                              $f2 thickness: pointThickness ...\
                              $r := 1.0 ...\
                              $circle := Circle center: $f1 radius: $r ...\
                              $circle color H: 50 S: .65 V: .9 ...\
                              $pa1 := Parabola focus: $f1 line: Line random ...\
                              $pa1 color H: 260 S: .45 V: .67 ...\
                              $pa1 line color: $line1 color ...\
                              C := Circle ...\
                              P := Point ...\
                              L := Line ...\
                              PA := Parabola ...\
                              pdc addFirst: (kdc addFirst: $f1) $pdcClone ...\
                              pdc addLast: (kdc addLast: $f2) $pdcClone ...\
                              pdc addLast: (kdc addLast: $circle) $pdcClone ...\
                              pdc addLast: (kdc addLast: $line1) $pdcClone ...\
                              pdc addLast: (kdc addLast: $pa1) $pdcClone ...\
                              pdc addLast: (kdc addLast: $pa1 line) $pdcClone ...\
                              kdc sync ...\
                              pdc sync ...\
                              uhpc addFirst: $f1 $uhpcClone ...\
                              uhpc addLast: $f2 $uhpcClone ...\
                              uhpc addLast: $circle $uhpcClone ...\
                              uhpc addLast: $line1 $uhpcClone ...\
                              uhpc addLast: $pa1 $uhpcClone ...\
                              uhpc addLast: $pa1 line $uhpcClone ...\
                              uhpc sync ...\
"
}

proc tconx_DLC_new_parabola { } {
    p "pa1 := Parabola focus: [tconx_focus1_name] line: Line random"
}

proc tconx_syncall { } {
    tconx_parse_smalltalkish "pdc sync. kdc sync. uhpc sync."
    tconx_update_canvases_button_command [tconx_mother_widget_pathname]
}
