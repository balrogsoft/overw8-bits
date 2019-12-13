Overw8-bits 4k intro for MorphOS
BalrogSoft / AmigaSkool.net

Music by Ryg, taked from this video:
https://www.youtube.com/watch?v=tCRPUv8V22o

History of the 4k intro that becomes 6k intro and 
later it went to the dietist to be a 4k intro again!
A touching story of overcoming...

This intro was developed some time ago, I have 
decided to publish it because I will not present it 
to a demoscene competition.

At the time to publish this intro, some changes on 
TinyGL made my intro left working properly, so to get
it working again, I had to use exit() function from 
stdlib, so it increased the size to 6k, and it was 
published with 6k.

After some discussion on morphzone.info about the 
latest changes on TinyGL, Piru pointed me that the
only solution to have a proper 4k version was use the 
windowed mode until TinyGL gets updated to response 
properly to CTRL+C signals.

With MorphOS 3.6 released recently, TinyGL has been
fixed to accept CTRL+C signals, and now this intro
works as expected with 4k.

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

