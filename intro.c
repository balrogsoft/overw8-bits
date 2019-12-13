/*
 *	4Kb Intro by Pedro Gil (Balrog Soft)
 *	        www.amigaskool.net
 *
 *  Overw8-bits, 4k intro that becomes a 6k intro and 
 *  later it went to the dietist to be a 4k intro again!
 *  A touching story of overcoming...
 *
 *  Music by Ryg, taked from this video:
 *  https://www.youtube.com/watch?v=tCRPUv8V22o
 *
 *  Special thanks to Piru for his small opts,
 *  and Morphzone.org people!
 *
 */

#include <devices/ahi.h>

#include <intuition/intuition.h>

#include <intuition/extensions.h>

#include <proto/intuition.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/tinygl.h>
#include <proto/ahi.h>

#include <stdio.h>

#include <limits.h>

#define ubyte   unsigned char
#define uint    unsigned int

#define byte    signed char

#ifndef M_PI
#define M_PI  3.14159265
#endif
#define M_PI2 6.283185

#define TABLE_DEGREES	720
#define DEGREE_STEP     M_PI2 / TABLE_DEGREES

#define FREQUENCY  		44100
#define TYPE       		AHIST_M8S
#define BUFFERSIZE 		24000
#define CHANELS			2


#define TONEFREQ   		440.0f
#define TONEBASE   		1.059463094359f

static void	keys(unsigned char c, int x, int y);
static void display(void);
static void blurLine(ubyte width, ubyte rad);
static void synth4k(void);

struct ExecBase      *SysBase;
struct IntuitionBase *IntuitionBase;
struct Library 	   	 *TinyGLBase;

GLContext *__tglContext;

static struct MsgPort    *AHImp     = NULL;
static struct AHIRequest *AHIios1	= NULL;
static struct AHIRequest *AHIios2	= NULL;
static struct AHIRequest *link 		= NULL;

static struct Message *wbstartupmsg = NULL;
static struct Process *self = NULL;
	
static void  *tmp;

static BOOL  init = TRUE, mirror = FALSE;

static ubyte cr, cg, cb, ca, scene;
static byte  AHIDevice, ch, bit, frame, copy;

static byte  buffer1[BUFFERSIZE], buffer2[BUFFERSIZE];
static byte  *p1=buffer1, *p2=buffer2;
static short color;
static uint  signals;
static int   timer, t=0,tt, i, j, cx, cy,
			 n, m, rotz, xbr, ybr, rd, spm, cp, parms;

static GLfloat vertex[6], coords[6],  sinus[TABLE_DEGREES], cosinus[TABLE_DEGREES],
			   a = 0, b = 1, c, d = DEGREE_STEP;

static unsigned int  EmptyPointer[] = {0, 0, 0, 0};

						
static short sprites[] = {NULL,
						0b0000100000100000,
						0b0000010001000000,
						0b0000111111100000,
						0b0001101110110000,
						0b0011111111111000,
						0b0010111111101000,
						0b0010100000101000,
						0b0000011011000000,

						0b0000100000100000,
						0b0010010001001000,
						0b0010111111101000,
						0b0011101110111000,
						0b0011111111111000,
						0b0000111111100000,
						0b0000100000100000,
						0b0001000000010000,

						0b0000111111100000,
						0b0011111111111000,
						0b0111111111111100,
						0b0111111111111100,
						0b0111111111111100,
						0b0111110001111100,
						0b0111100000111100,
						0b0000000000000000,

                        0b0000000000000000,
						0b0000000100000000,
						0b0000001110000000,
						0b0000001110000000,
						0b0001111111110000,
						0b0011111111111000,
						0b0011111111111000,
						0b0000000000000000,

						0b0000011111100000,
						0b0000111111000000,
						0b0001111100000000,
						0b0001111000000000,
						0b0001111100000000,
						0b0000111111000000,
						0b0000011111100000,
						0b0000000000000000,

						0b0000011111100000,
						0b0000111111110000,
						0b0001111111100000,
						0b0001111100000000,
						0b0001111111100000,
						0b0000111111110000,
						0b0000011111100000,
                        0b0000000000000000,

						0b0000011111100000,
						0b0000111111110000,
						0b0001111111111000,
						0b0001111111111000,
						0b0001111111111000,
						0b0000111111110000,
						0b0000011111100000,
						0b0000000000000000,

						0b0000011111100000,
						0b0000111111110000,
						0b0001110011001000,
						0b0001110111011000,
						0b0011111111111100,
						0b0011111111111100,
						0b0011111111111100,
						0b0001100110011000,

						0b0010011111100100,
						0b0110100110010110,
						0b0011111111111100,
						0b0000111111110000,
						0b0011111111111100,
						0b0010111111110100,
						0b0110011111100110,
						0b0010000000000100,

						0b1110111011111100,
						0b1111111111110010,
						0b1111110011100001,
						0b1111111111101101,
						0b1100111111101101,
						0b1111111101100001,
						0b1111111111110010,
						0b1101111101101100,

						0b0011101111101111,
						0b0111111111111111,
						0b1111100011111111,
						0b1111111111110001,
						0b1100111111111111,
						0b1111111100011111,
						0b0111111111111111,
						0b0011101111011111,
                        };

static ushort color_table[]	= {0x0f0, 0xfff, 0xff0, 0x00e, 0xd00};

static short scene_index[3][2] = {{ 0,12},
								  {12,28},
								  {24,48}};

static void* scene_defs[] = {(void*)((1<<16) | 0), (void*)0, (void*)-25, (void*)1+(3<<16),
							 (void*)((1<<16) |  1), (void*)-32, (void*)32, (void*)2+(2<<8)+(128<<16),//0x10202,
							 (void*)((1<<16) |  0), (void*)0, (void*)-10, (void*)1+(2<<16),//0x201,
					
							 (void*)((5<<16) |  2), (void*)-48, (void*)0, (void*)3+(128<<16)+(1<<24),//0x1010003,
							 (void*)((5<<16) |  3), (void*)-16, (void*)0, (void*)1+(131<<16)+(1<<24),//0x1010001,
							 (void*)((0<<16) | /*-60*/ 0xffc4), (void*)60, (void*)4, (void*)0xff20,

							 (void*)((0<<16) |  /*-60*/ 0xffc4), (void*)60, (void*)-11, (void*)0xff20, // SHARED WITH FROGGER

							 (void*)((0<<16) |  /*-60*/ 0xffc4), (void*)60, (void*)34, (void*)0xff0020,
							 (void*)((9<<16) |  0), (void*)0, (void*)-15, (void*)1,
							 (void*)((5<<16) |  2), (void*)-32, (void*)0, (void*)1+(2<<8)+(130<<16)+(0b10000<<24),//0x10010201,
							 (void*)((9<<16) |  4), (void*)-32+8, (void*)20, (void*)1+(2<<8)+(129<<16)+(0b10000<<24),//0x10010201,
							 (void*)((9<<16) |  4), (void*)-48+8, (void*)20, (void*)1+(2<<8)+(130<<16)+(0b10000<<24),//0x10010201
							 };

#define __TEXTSECTION__ __attribute__((section(".text")))

#ifdef NOSTARTUPFILES
int entry(void)
#else
int main(void)
#endif
{
  SysBase    = *(struct ExecBase **) 4;
  
  wbstartupmsg = 0;
  self = (struct Process *) FindTask(0);
  if (self->pr_CLI == 0)
  {
	WaitPort(&self->pr_MsgPort);
	wbstartupmsg = GetMsg(&self->pr_MsgPort);
  }
  
  IntuitionBase = OpenLibrary("intuition.library", 30);

  TinyGLBase = OpenLibrary("tinygl.library",   50);

  if (TinyGLBase && IntuitionBase)
  {
	
    // Initialize AHI device

	AHImp=CreateMsgPort();
	#ifndef REMOVE
	if(AHImp != NULL)
	{
	#endif
		AHIios1 = (struct AHIRequest *) CreateIORequest(AHImp, sizeof(struct AHIRequest));
		//AHIios1->ahir_Version = 4;
		AHIDevice = OpenDevice(AHINAME, 0, (struct IORequest *)AHIios1, 0);
	#ifndef REMOVE
    }
	#endif

	// Make a copy of the request (for double buffering)

	AHIios2 = AllocVec(sizeof(struct AHIRequest), MEMF_ANY);
	CopyMem(AHIios1, AHIios2, sizeof(struct AHIRequest));

	// Generate sinus and cosinus tables
	i = TABLE_DEGREES;
	while (i) 
	{
		c = a + b * d;
		b = b - c * d;
		sinus  [TABLE_DEGREES-i] = c;
		cosinus[TABLE_DEGREES-i] = b;
		a = c;
		i--;
	}
	c = 0;

	__tglContext = GLInit();
	if (__tglContext)
	{
		// Initialize TinyGL
		
        glutInit(NULL,NULL);

		#ifndef REMOVE
		glutInitWindowSize(640, 480);
		#endif

		glutFullScreen();
	
		glutCreateWindow(NULL);
		
		SetPointer((struct Window*)glutGetWindowHandle(), EmptyPointer, 1L, 1L, 0L, 0L);

		#ifndef REMOVE
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		#endif
		
		// OpenGL settings

		glEnable(GL_BLEND);
		
		glEnable(GL_DEPTH_TEST);
        
		glDisable(GL_CULL_FACE);
		
        glEnableClientState(GL_VERTEX_ARRAY);

		glMatrixMode(GL_PROJECTION);
		
		#ifdef P2D
		glOrtho(0.0f, 640.0f,
			  480.0f, 0.0f,
			   -1.0f, 1.0f);
		#else
		gluPerspective(80, 1.333333f,  0.01, 200.0f );
		#endif

		glMatrixMode(GL_MODELVIEW);
		
        glutIdleFunc(display);
        glutKeyboardFunc(keys);
		
		glutMainLoop();
		
		// Abort any pending iorequests
		AbortIO((struct IORequest *) AHIios1);
		WaitIO((struct IORequest *) AHIios1);

		if(link)  // Only if the second request was started
		{
			AbortIO((struct IORequest *) AHIios2);
			WaitIO((struct IORequest *) AHIios2);
		} 
		
		if(!AHIDevice)
			CloseDevice((struct IORequest *)AHIios1);
		
		DeleteIORequest((struct IORequest *)AHIios1);
		FreeVec(AHIios2);
		
		GLClose(__tglContext);
	}
	CloseLibrary(TinyGLBase);
	CloseLibrary((struct Library*)IntuitionBase);	
  }
  if (wbstartupmsg)
  {
    Forbid();
    ReplyMsg(wbstartupmsg);
  }
  return 0;
}


static void	keys(unsigned char c, int x, int y)
{
	if (c == 0x1b)
	{
		SetSignal(SIGBREAKF_CTRL_C, SIGBREAKF_CTRL_C);
	}
}

static void display(void)
{	 

	// Send io request to the device
	if ((signals & (1L << AHImp->mp_SigBit) || init))
	{
		if (init && link)
			init = FALSE;
		if (!link)
			synth4k();
			
	//	  AHIios1->ahir_Std.io_Message.mn_Node.ln_Pri = 0;
		AHIios1->ahir_Std.io_Command  = CMD_WRITE;
		AHIios1->ahir_Std.io_Data     = p1;
		AHIios1->ahir_Std.io_Length   = BUFFERSIZE;
	//	  AHIios1->ahir_Std.io_Offset   = 0;
		AHIios1->ahir_Frequency       = FREQUENCY;
		AHIios1->ahir_Type            = TYPE;
		AHIios1->ahir_Volume          = 0x10000;          // Full volume
		AHIios1->ahir_Position        = 0x8000;           // Centered
		AHIios1->ahir_Link            = link;
		SendIO((struct IORequest *) AHIios1);
	}

	// Check if there are any signals from ahi device
	if(link)
	{
		// Send a fake signal to not stop the loop
		SetSignal(SIGBREAKF_CTRL_D, SIGBREAKF_CTRL_D);
        signals = Wait(SIGBREAKF_CTRL_D | (1L << AHImp->mp_SigBit));
	}
	
	link = AHIios1;
   
	// Swap ahi buffers
	if ((signals & (1L << AHImp->mp_SigBit)) || init)
    {
		if (!init)
			WaitIO((struct IORequest *) AHIios2);
		
		// Swap buffer and request pointers and fill buffer
		tmp    = p1;
		p1     = p2;
		p2     = tmp;
		
		synth4k();
		  	
		tmp    = AHIios1;
		AHIios1 = AHIios2;
		AHIios2 = tmp;
	}


	// TinyGL stuff goes here

	tt = glutGet(GLUT_ELAPSED_TIME);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	  
	glLoadIdentity();

	gluLookAt(40.0f + sinus[rotz]*80, 0, 90.0f+cosinus[rotz]*40, 0, 1, 0, -sinus[rotz]*30, cosinus[(rotz*2)%TABLE_DEGREES]*60, 0);

	spm = (rotz & 127) >> 4;

    mirror = FALSE;

	if (spm < 5) mirror = FALSE;
	else mirror = TRUE;


	for (scene = scene_index[(timer>>14)%3][0]; scene < scene_index[(timer>>14)%3][1]; scene += 4)
	{
		parms = (int*)scene_defs[scene+3];
		if (!(((int)scene_defs[scene])>>16))
		{
			coords[0] = ((int)scene_defs[scene])&0xffff + 120;
			coords[3] = (int)scene_defs[scene+1] - 140;
			coords[1] = coords[4] = (int)scene_defs[scene+2];
			
			cr = (parms >> 24);
			cg = (parms >> 16);
			cb = (parms >>  8);
			ca = parms;

			blurLine(4, 2);
		}
		else
		{
			frame = parms;
			i = (parms >> 23)&1;
			j = (parms >> 28);
            copy = (parms >> 8);
			frame = ((parms>>16)&0x7f) + ((rotz>>4) % frame);

			color = color_table[((int)scene_defs[scene])&0xffff];
			
            if (((parms >> 24) & 0xf) == 0)
				mirror = FALSE;

			for (cp = -1; cp < copy; cp++)
			{

				if (j & (cp&1))
					parms = (spm < 4 ? 3-spm : spm-4);
				else
					parms = (spm < 4 ? spm : 8-spm);
				
                parms*=16;

				cx = (int)scene_defs[scene+1] + ((copy?cp:copy) * 32) + (i?parms:0);
				cy = (int)scene_defs[scene+2] + (j?(cp&1)*10:0);

				for (m = 0; m < 8; m ++)
				{
					b = cy + (m - 8);

					for (n = 0; n < 16; n ++)
					{
						a = cx + (n - 4);

						bit = (sprites[ (((((int)scene_defs[scene])>>16)-1)*8)+ 8 - m + frame * 8] >> (mirror?n:15-n)) & 1;
						if (bit)
						{
							bit = (bit - 1) * 3;

							cr = (color>>4)&0xf0; cg = color&0xf0; cb = (color<<4)&0xf0; ca = 12;
							coords[0] = a;
							coords[1] = coords[4] = b;

							coords[3] = a + 1;
							blurLine(4, 3);

							ca = 28;
							blurLine(3, 2);

							cr = cg = cb = 255;
							ca = 120;
							blurLine(2, 1);
						}
					}
				}
			}
		}
	}
	
	glutSwapBuffers();

	c += 30.0f * (tt-timer) / 1000;
	rotz = c;
	rotz %= TABLE_DEGREES;
	timer = tt;
	
}

static void synth4k(void)
{
	n = BUFFERSIZE;
	while (n)
	{
		p1[BUFFERSIZE-n] = ((t*("363664689"[t>>13&7]&15))/12&128)+
				(((((t>>12)^(t>>12)-2)%11*t)/4|
				t>>13)&127);
		t++;
		n--;
	}
	
}

static void blurLine(ubyte width, ubyte rad)
{
    rd = rad >> 1;
	glLineWidth(width);

	for (i = 0; i < rad; i++)
    {
		xbr	= (i - rd) ;
		for (j = 0; j < rad; j++)
		{
			ybr = (j - rd);

			d = xbr + ybr;
			if (d < 1) d = 1;
			if (ca/d < 10) continue;
			glColor4ub(cr, cg, cb, ca/d);
				
			vertex[0] = coords[0] + xbr;
			vertex[1] = coords[1] + ybr;
			vertex[3] = coords[3] + xbr;
			vertex[4] = coords[4] + ybr;

			glVertexPointer(3, GL_FLOAT, 0, vertex);


			glDrawArrays(GL_LINE_LOOP, 0, 2);
        }
    }
}

#ifdef NOSTARTUPFILES
/* __abox__ symbol is required or else the binary is loaded as PowerUP app */
const int __abox__ __TEXTSECTION__ = 1;
#endif
