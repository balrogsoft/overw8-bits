CC 		= gcc
LD 		= ld
STRIP 	= strip
CFLAGS 	= -O1 -s -fomit-frame-pointer -noixemul  -ISDK:tinygl-sdk/include

NOSTARTUPFILES  = 1
REMOVE 			= 1
#2D_PROJECTION	= 1

ifdef NOSTARTUPFILES
	CFLAGS+= -DNOSTARTUPFILES -nostartfiles
endif

ifdef REMOVE
	CFLAGS+= -DREMOVE
endif

ifdef 2D_PROJECTION
	CFLAGS+= -DP2D
endif

INTRO_OBJS = intro.o

all: intro

intro: $(INTRO_OBJS)
	$(CC) $(CFLAGS) -o $@ $(INTRO_OBJS)
	$(STRIP) -s -R .comment -R .gnu.version -R .gnu.version_d -R .gnu.version_r $@
	chmod u+x $@

intro_map:
	$(CC) $(CFLAGS) intro.c -Wl,-Map=foobar.map,--traditional-format

intro_asm:
	$(CC) $(CFLAGS) -save-temps -o $@ $(INTRO_OBJS)
	
clean:
	rm -f *.o intro Overw8-bits
