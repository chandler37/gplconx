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


# This file is the first Tcl source file processed.

# We must catch any errors in any of the Tcl source files here:
if [catch {

proc tconx_is_debug_mode { } {
    global tconx_globls
    return $tconx_globls(DebuggingOutput)
}

proc tconx_source { filename } {
# DLC `Tcl/' is a kludge.
    uplevel "tconx_source_i [list Tcl/$filename] 1 1"
}

proc tconx_source_absolute { filename } {
# DLC `Tcl/' is a kludge.
    uplevel "tconx_source_i [list Tcl/$filename] 2 0"
}

proc tconx_source_i { filename allow_dbg in_tcl_source_dir } {
    if {![string compare $in_tcl_source_dir 0]} {
        set base_directory ""
    } else {
        set base_directory "./"
    }
    if ![string compare $allow_dbg 1] {
        Debugloc "beginning sourcing $base_directory$filename"
    }
    if [catch {
        if [file exists $base_directory$filename] {
            uplevel "source $base_directory$filename"
        } else {
            set rv "File $base_directory$filename does not exist"
            puts $rv
            return -code error $rv
        }
    } err] {
        set rv "Could not source $base_directory$filename: $err"
        puts $rv
        return -code error $rv
    }
    if ![string compare $allow_dbg 1] {
        Debugloc "   ending sourcing $base_directory$filename"
    }
    return ""
}

# DLC allow --Tcl-code-is-in=/usr/local/GPLconx/mojo
tconx_source_absolute tconx_d.tcl
if {[tconx_is_debug_mode]} {
    DebugOn 1
}
Debugloc "Beginning tconx's Tcl Initialization"

wm withdraw .

tconx_source tconx_a.tcl
tconx_do_most_sourcing
if {[catch {tconxt_ui .} eek]} {
    puts "Problem in tconx_a.tcl:\n$eek\nEND PROBLEM IN\
                                   tconx_a.tcl\n"
    return -code error "Problem in tconx_a.tcl:\n$eek\nEND PROBLEM IN\
                                   tconx_a.tcl\n"
}

tconx_menus_init
unset eek; # Clean up global namespace.

tconx_finally_show_main_window .

global auto_noexec
set auto_noexec \
    "don't allow shell-like behavior unless the user knows what she is doing"

# Matches the big `if [catch]':
} toplevel_error] {
    puts "Error in tconx.tcl;  tconx init is flawed.\n$toplevel_error.\n"
    tconx_verbosity [tconx_verbosity_level LOGG_OFF]
    exit 2
}
unset toplevel_error; # Clean up global namespace.
Debugloc "\n\n\nI made it out!!!\n\n"
