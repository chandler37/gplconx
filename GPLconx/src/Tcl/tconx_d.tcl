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


# Some Tcl code to aid in debugging Tcl programs.  These are inspired
# by Brent Welch's book, but many levels of debugging messages are allowed.
# In addition, there are procedures here that evaluate their argument only
# if the argument will be printed, which is a speed increase.

# The global variable tconx_globls is used.

proc Debuglevel { } {
    global tconx_globls
    if [info exists tconx_globls(Debug,on)] {
        if [info exists tconx_globls(Debug,level)] {
            return $tconx_globls(Debug,level);
        }
    } else {
        return none
    }
}

proc Debugloc { msg args } {
# Location debugging lets you zero in on the problem code.
    Debugn 1 {tconx loc:} $msg $args
}

proc Debugn_no_eval { level cmd } {
# Optionally print out a string that requires optional evaluation.  If you
# try `Debugn 0 [print_a_lot]' you will slow your program
# down.  Using `Debugneval 0 {[print_a_lot]}' will not
# actually run the proc print_a_lot unless it will
# be printed out.  Of course, `Debugneval 0 [print_a_lot]' is perfectly
# useless, the same as Debugn.

# Example:
# DebugOn 0
# proc p { } {
#     puts "proc p has executed, slowing your program down";
#     return "This is a debugging message";
# }
# Debugneval 1 {[p]} ;# This prints out nothing.
# Debugneval 0 {[p]} ;# This prints out two lines, one a by-product.
    global tconx_globls
    if {[info exists tconx_globls(Debug,on)]
        && [info exists tconx_globls(Debug,level)]
        && ($level <= $tconx_globls(Debug,level))} {
        uplevel "Debugn [list $level] $cmd"
    }
}

proc Debugn { level args } {
#Debugn 30 "abc" will cause `abc' to be printed only if the debug level is >= 30.
    global tconx_globls
    Debug_assert {[llength $args] >= 1}
    if {[info exists tconx_globls(Debug,on)]
        && [info exists tconx_globls(Debug,level)]
        && ($level <= $tconx_globls(Debug,level))} {
        puts $tconx_globls(Debug,file) [join $args " "]
    }
}

proc Debug { args } {
# Print the concatenation of the arguments out if DebugOn has been called.
    global tconx_globls
    if {[info exists tconx_globls(Debug,on)]} {
        puts $tconx_globls(Debug,file) [join $args " "]
    }
}

proc DebugOn {level {logfile {}}} {
# Enable Debug, Debugn, Debugn_no_eval, and Debugloc.
    global tconx_globls
    set tconx_globls(Debug,on) 1
    if {[string length $logfile] == 0} {
        set tconx_globls(Debug,file) stderr
    } else {
        if [catch {open $logfile w} file_handle] {
            puts stderr "Cannot open $logfile: $file_handle"
            set tconx_globls(Debug,file) stderr
        } else {
            puts stderr "Debug info to $logfile"
            set tconx_globls(Debug,file) $file_handle
        }
    }
    set tconx_globls(Debug,level) $level
    return
}

proc DebugOff {} {
# reverse the effects of DebugOn.
    global tconx_globls
    if [info exists tconx_globls(Debug,on)] {
        unset tconx_globls(Debug,level)
        unset tconx_globls(Debug,on)
        flush $tconx_globls(Debug,file)
        if {($tconx_globls(Debug,file) != "stderr")
            && ($tconx_globls(Debug,file) != "stdout")} {
            close $tconx_globls(Debug,file)
            unset $tconx_globls(Debug,file)
        }
    }
}

proc Debug_assert { cmd } {
# $cmd had better evalute to true, or the program will exit.  Like
# C's assert.h's assert() macro.
    if {[Debug_asserts_enabled]} {
        if {[set m [uplevel [list expr $cmd]]] == 0} {
            # DLC log to DebugOn log file also???
            puts stderr "An assertion failed.\nThe following expression is\
                         false: $cmd\n"
            exit 1
        }
    }
}

proc Debug_asserts_flip { yesno } {
# Enable or disable asserts, $yesno is "no" to disable them.
# Enabling asserts slows the program down a bit.
    global tconx_globls

    if {$yesno == "no"} {
        set tconx_globls(Debug_asserts_enabled_var) 0
    } else {
        set tconx_globls(Debug_asserts_enabled_var) 1
    }
}

proc Debug_asserts_enabled { } {
# Returns 1 if asserts are enabled, 0 if they are not.
    global tconx_globls
    if ![info exists tconx_globls(Debug_asserts_enabled_var)] {
        # They are on unless you turn them off.
        set tconx_globls(Debug_asserts_enabled_var) 1
    }
    return $tconx_globls(Debug_asserts_enabled_var)
}

