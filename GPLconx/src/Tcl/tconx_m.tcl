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


# This code is used by `tconx_l.tcl' to create menus.

# We have both named and nameless menus.  A named menu hides it nameless self,
# i.e. its pathname, behind an alias.  This makes changing the location of
# a menu simpler, and it makes duplicating a menu simpler.

proc tconx_define_named_menu { name pathname } {
# Associate a name with a pathname.
    global tconx_globls
    set tconx_globls(Menus,$name) [tconx_base]$pathname
}

proc tconx_menuname2path { menu_name } {
# $menu_name is the name of a named menu.  This returns the associated
# pathname, which is set by tconx_define_named_menu
    global tconx_globls
    # pass in file, our ID for the menu, get back ".file_mb.m", the pathname
    return $tconx_globls(Menus,$menu_name)
}

proc tconx_menu_add_submenu {menu_name submenu_name labeltext underline_char} {
# Add one of our menus, known by its name.
    tconx_menu_add_nameless_submenu [tconx_menuname2path $menu_name] \
        [tconx_menuname2path $submenu_name] $labeltext $underline_char
}

proc tconx_menu_add_nameless_submenu {parent_menu_pathname submenu_pathname
                                      labeltext underlined} {
# Add a submenu to 
    if ![tconx_is_int $underlined] {
        set underline_command ""
    } else {
        set underline_command " -underline $underlined "
    }
    eval "$parent_menu_pathname add cascade -label [list $labeltext] \
             -menu [list $submenu_pathname] $underline_command"
}

proc tconx_init_submenu { parent submenu_path label underline init_func } {
# Create a submenu labeled $label off of $parent.  Underline the
# ${underline}th character in $label.  After creation, run $init_func on
# the submenu, which will be at $parent$submenu_path
    set child $parent$submenu_path
    tconx_menu_add_nameless_submenu $parent $child $label $underline
    eval [list $init_func $child]
}

proc tconx_new_menu { menu_name } {
# Create a menu based on its name instead of its pathname.
    tconx_new_nameless_menu [tconx_menuname2path $menu_name]
}

proc tconx_new_nameless_menu { pathname } {
# Create a menu without a name, usually of a child of a named menu created
# by tconx_new_menu
# So that `sync' works, we re-create the menu.
    destroy $pathname
    menu $pathname
    $pathname delete 0 end
}

