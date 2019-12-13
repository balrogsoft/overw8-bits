# overw8-bits
4kb intro for MorphOS

Overw8-bits 4k intro for MorphOS
BalrogSoft / AmigaSkool.net

Music by Ryg, taked from this video:
https://www.youtube.com/watch?v=tCRPUv8V22o

Some details to build this intro.

There are three scripts to build, clean and crunch the
intro, be sure you have lzma cruncher on this directory.

A makefile is used to build the intro, you can take a 
look to the makefile build options:

NOSTARTUPFILES
This build a version with -nostartupfiles option of gcc.
If you want the 4k version, this option should be active.
In this mode, Overw8-bits runs in windowed mode (see
history of this intro)

REMOVE
For code to be removed that apparently are not really
needed.

2D_PROJECTION
Use a 2D projection instead 3D, not used on this intro.


You are free to use this code as you want.
Make 4k intros for our favourite system!
