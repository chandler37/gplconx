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


# This file contains utility functions.

proc tconx_max { args } {
# Returns the maximum of the numeric arguments.
    set L [llength $args]
    if {$L == 0} { return {} }
    set max [lindex $args 0]
    for {set i 1} {$i < $L} {incr i 1} {
        if {[lindex $args $i] > $max} {
            set max [lindex $args $i]
        }
    }
    return $max
}

proc tconx_min { args } {
# Returns the minimum of the numeric arguments.
    set L [llength $args]
    if {$L == 0} { return {} }
    set min [lindex $args 0]
    for {set i 1} {$i < $L} {incr i 1} {
        if {[lindex $args $i] < $min} {
            set min [lindex $args $i]
        }
    }
    return $min
}

proc tconx_lapply_after { cmd l args } {
# Apply the command $cmd to each of the items in $l, with the arguments
# in $args following.  Returns a list of the results.
    set r {}
    foreach item $l {
        lappend r [eval "[list $cmd $item] $args"]
    }
    return $r
}

proc tconx_catch_error { cmd } {
    if [catch {uplevel "$cmd"} errmsg] {
        bgerror $errmsg
        return $errmsg
    }
    return $errmsg
}

proc tconx_is_int { a } {
# Returns 1 if a is an integer, 0 otherwise

    # We could do a scanf test, but we want to make sure that
    # this is something we can do math with.  So do math with it and
    # see what happens.
    return [expr ![catch {expr $a % 2}]]
}

proc tconx_is_real { a } {
# Returns 1 if a is a real number, 0 otherwise
    return [expr ![catch {expr $a > 1.0}]]
}

