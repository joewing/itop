jtop for IRIX
=============
Joe Wingbermuehle
http://joewing.net

Background
----------
This is a program that closely resembles the "top" program that comes
with many UNIX variants (IRIX included). I made this program to become
more familiar with the system calls needed to read process information
and other statistics. I think the end product is useful and certainly
the source code is useful.

Platforms
---------
This program was developed on a single-processor Octane R10000-250
running IRIX 6.5.17m.  It should work on any IRIX 6.5 system.

Compilation
-----------
Edit the "Makefile" and run "make".

Usage
-----
jtop displays a few lines of information at the top of the display and
then gives a list of processes. Press 'h' or '?' for help while jtop
is running. The process list displays the following:
 PID       the process identification number
 PGRP      the process group
 USERNAME  the owner of the process
 PRI       the priority of the process (a number between 0 and 255)
 SIZE      the process size
 RES       the resident process size
 STATE     the process state
 TIME      the time the process has been running
 TRS       the number of threads the process has
 RCPU%     the relative percent of CPU the process is using
 COMMAND   the command used to start the process

History
-------
The first (and latest) version of this was created 20030417.
