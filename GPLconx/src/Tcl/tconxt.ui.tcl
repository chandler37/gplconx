#! /bin/sh
# the next line restarts using wish \
exec wish "$0" "$@"

# interface generated by SpecTcl version 1.1 from /home/davidc/Conx/conxv/src/tconxt.ui
#   root     is the parent window for this user interface

proc tconxt_ui {root args} {

	# this treats "." as a special case

	if {$root == "."} {
	    set base ""
	} else {
	    set base $root
	}
    
	frame $base.frame#5 \
		-borderwidth 2 \
		-relief ridge

	frame $base.mainframe \
		-borderwidth 2 \
		-relief sunken

	frame $base.frame#1

	menubutton $base.file_mb \
		-menu "$base.file_mb.m" \
		-text File \
		-underline 0

	menubutton $base.mouse_mb \
		-menu "$base.mouse_mb.m" \
		-text Mouse \
		-underline 0

	menubutton $base.draw_mb \
		-menu "$base.draw_mb.m" \
		-text Draw \
		-underline 0

	menubutton $base.input_mb \
		-menu "$base.input_mb.m" \
		-text Input \
		-underline 0

	menubutton $base.help_mb \
		-menu "$base.help_mb.m" \
		-text Help \
		-underline 0

	checkbutton $base.checkbutton#1 \
		-command {tconx_mother_check puhp} \
		-text {Poincare Upper Half Plane} \
		-underline 9 \
		-variable tconx_globls(check_puhp)
	catch {
		$base.checkbutton#1 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-200-*-*-*-*-*-*
	}

	checkbutton $base.checkbutton#2 \
		-command {tconx_mother_check kd} \
		-text {Beltrami-Klein Disk} \
		-underline 9 \
		-variable tconx_globls(check_kd)
	catch {
		$base.checkbutton#2 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-200-*-*-*-*-*-*
	}

	checkbutton $base.checkbutton#3 \
		-command {tconx_mother_check pd} \
		-text {Poincare Disk} \
		-underline 0 \
		-variable tconx_globls(check_pd)
	catch {
		$base.checkbutton#3 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-200-*-*-*-*-*-*
	}

	entry $base.bottom_entry \
		-relief flat \
		-selectborderwidth 0 \
		-textvariable tconx_globls(bottom_entry) \
		-width 1
	catch {
		$base.bottom_entry configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-120-*-*-*-*-*-*
	}


	# Geometry management

	grid $base.frame#5 -in $root	-row 3 -column 1  \
		-sticky nesw
	grid $base.mainframe -in $root	-row 2 -column 1  \
		-sticky nesw
	grid $base.frame#1 -in $root	-row 1 -column 1  \
		-sticky new
	grid $base.file_mb -in $base.frame#1	-row 1 -column 1 
	grid $base.mouse_mb -in $base.frame#1	-row 1 -column 2 
	grid $base.draw_mb -in $base.frame#1	-row 1 -column 3 
	grid $base.input_mb -in $base.frame#1	-row 1 -column 4 
	grid $base.help_mb -in $base.frame#1	-row 1 -column 6 
	grid $base.checkbutton#1 -in $base.mainframe	-row 1 -column 1 
	grid $base.checkbutton#2 -in $base.mainframe	-row 2 -column 1 
	grid $base.checkbutton#3 -in $base.mainframe	-row 3 -column 1 
	grid $base.bottom_entry -in $base.frame#5	-row 1 -column 1  \
		-sticky ew

	# Resize behavior management

	grid rowconfigure $base.mainframe 1 -weight 1 -minsize 12
	grid rowconfigure $base.mainframe 2 -weight 1 -minsize 30
	grid rowconfigure $base.mainframe 3 -weight 1 -minsize 30
	grid columnconfigure $base.mainframe 1 -weight 1 -minsize 27

	grid rowconfigure $root 1 -weight 0 -minsize 30
	grid rowconfigure $root 2 -weight 1 -minsize 28
	grid rowconfigure $root 3 -weight 0 -minsize 7
	grid columnconfigure $root 1 -weight 1 -minsize 125

	grid rowconfigure $base.frame#5 1 -weight 0 -minsize 6
	grid columnconfigure $base.frame#5 1 -weight 1 -minsize 30

	grid rowconfigure $base.frame#1 1 -weight 0 -minsize 13
	grid columnconfigure $base.frame#1 1 -weight 0 -minsize 26
	grid columnconfigure $base.frame#1 2 -weight 0 -minsize 26
	grid columnconfigure $base.frame#1 3 -weight 0 -minsize 10
	grid columnconfigure $base.frame#1 4 -weight 0 -minsize 30
	grid columnconfigure $base.frame#1 5 -weight 1 -minsize 30
	grid columnconfigure $base.frame#1 6 -weight 0 -minsize 30
# additional interface code
tconx_mother_widget_init $root

# end additional interface code

}


# Allow interface to be run "stand-alone" for testing

catch {
    if [info exists embed_args] {
	# we are running in the plugin
	tconxt_ui .
    } else {
	# we are running in stand-alone mode
	if {$argv0 == [info script]} {
	    wm title . "Testing tconxt_ui"
	    tconxt_ui .
	}
    }
}
