#include "IGaphicsWndCreate.h"

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#define POINTER_WARP_COUNTDOWN	1

IGraphicsWnd* graphicsWnd = NULL;
int		X_width = 0;
int		X_height = 0;

// Fake mouse handling.
// This cannot work properly w/o DGA.
// Needs an invisible mouse cursor at least.
boolean		grabMouse = 0;
int		doPointerWarp = POINTER_WARP_COUNTDOWN;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply = 1;

//
//  Translates the key currently in X_event
//

HRESULT xlatekey(IGraphicsWndInputEvent* inputEvt, const char** errStr, int* rc)
{
	HRESULT hr = S_OK;
	enum GraphicsWndInputEventKey key = GraphicsWndInputEventKey_None;

	hr = inputEvt->vtable->GetKey(inputEvt, &key);
	if (FAILED(hr)) {
		*errStr = "inputEvt.GetKey failed in xlatekey";
		return hr;
	}

	switch (key) {
	case GraphicsWndInputEventKey_Left: *rc = KEY_LEFTARROW; break;
	case GraphicsWndInputEventKey_Right: *rc = KEY_RIGHTARROW; break;
	case GraphicsWndInputEventKey_Down: *rc = KEY_DOWNARROW; break;
	case GraphicsWndInputEventKey_Up: *rc = KEY_UPARROW; break;
	case GraphicsWndInputEventKey_Escape: *rc = KEY_ESCAPE; break;
	case GraphicsWndInputEventKey_Return: *rc = KEY_ENTER; break;
	case GraphicsWndInputEventKey_Tab: *rc = KEY_TAB; break;
	case GraphicsWndInputEventKey_F1: *rc = KEY_F1; break;
	case GraphicsWndInputEventKey_F2: *rc = KEY_F2; break;
	case GraphicsWndInputEventKey_F3: *rc = KEY_F3; break;
	case GraphicsWndInputEventKey_F4: *rc = KEY_F4; break;
	case GraphicsWndInputEventKey_F5: *rc = KEY_F5; break;
	case GraphicsWndInputEventKey_F6: *rc = KEY_F6; break;
	case GraphicsWndInputEventKey_F7: *rc = KEY_F7; break;
	case GraphicsWndInputEventKey_F8: *rc = KEY_F8; break;
	case GraphicsWndInputEventKey_F9: *rc = KEY_F9; break;
	case GraphicsWndInputEventKey_F10: *rc = KEY_F10; break;
	case GraphicsWndInputEventKey_F11: *rc = KEY_F11; break;
	case GraphicsWndInputEventKey_F12: *rc = KEY_F12; break;
	case GraphicsWndInputEventKey_BackSpace:
	case GraphicsWndInputEventKey_Delete: *rc = KEY_BACKSPACE; break;
	case GraphicsWndInputEventKey_Pause: *rc = KEY_PAUSE; break;
	case GraphicsWndInputEventKey_Equal: *rc = KEY_EQUALS; break;
	case GraphicsWndInputEventKey_Subtract: *rc = KEY_MINUS; break;
	case GraphicsWndInputEventKey_ShiftLeft:
	case GraphicsWndInputEventKey_ShiftRight: *rc = KEY_RSHIFT; break;
	case GraphicsWndInputEventKey_CtrlLeft:
	case GraphicsWndInputEventKey_CtrlRight: *rc = KEY_RCTRL; break;
	case GraphicsWndInputEventKey_AltLeft:
	case GraphicsWndInputEventKey_AltRight: *rc = KEY_RALT; break;
	case GraphicsWndInputEventKey_Char:
		hr = inputEvt->vtable->GetKeyChar(inputEvt, rc);
		if (FAILED(hr)) {
			*errStr = "inputEvt.GetKeyChar failed in xlatekey";
			return hr;
		}
		break;
	}

	return S_OK;
}

void I_ShutdownGraphics(void)
{
	//// Detach from X server
	//if (!XShmDetach(X_display, &X_shminfo))
	//	I_Error("XShmDetach() failed in I_ShutdownGraphics()");

	//// Release shared memory.
	//shmdt(X_shminfo.shmaddr);
	//shmctl(X_shminfo.shmid, IPC_RMID, 0);

	//// Paranoia.
	//image->data = NULL;

	if (graphicsWnd) {
		HRESULT hr = S_OK;

		hr = graphicsWnd->vtable->Release(graphicsWnd);
		if (FAILED(hr)) {
			I_Error("graphicsWnd.Release failed");
		}
		graphicsWnd = NULL;
	}
}



//
// I_StartFrame
//
void I_StartFrame(void)
{
	// er?

}

static int	lastmousex = 0;
static int	lastmousey = 0;
boolean		mousemoved = false;

HRESULT I_GetEvent_impl(IGraphicsWndInputEvent** inputEvt, const char** errStr, boolean* res) {
	event_t event;
	HRESULT hr = S_OK;
	enum GraphicsWndInputEventType evtType = GraphicsWndInputEventType_None;
	enum GraphicsWndInputEventMouseButton mouseButton = GraphicsWndInputEventMouseButton_None;
	int mouseX = 0;
	int mouseY = 0;

	assert(graphicsWnd);

	hr = graphicsWnd->vtable->TryGetNextInputEvent(graphicsWnd, inputEvt);
	if (FAILED(hr)) {
		*errStr = "graphicsWnd.TryGetNextInputEvent failed";
		return hr;
	}

	if (*inputEvt == NULL) {
		// no events for now
		*res = false;
		return S_OK;
	}

	hr = (*inputEvt)->vtable->GetEventType(*inputEvt, &evtType);
	if (FAILED(hr)) {
		*errStr = "inputEvt.GetEventType failed";
		return hr;
	}

	switch (evtType) {
	case GraphicsWndInputEventType_None:
		break;
	case GraphicsWndInputEventType_KeyPress:
		event.type = ev_keydown;
		hr = xlatekey(*inputEvt, errStr, &event.data1);
		if (FAILED(hr)) {
			return hr;
		}
		D_PostEvent(&event);
		// fprintf(stderr, "k");
		break;
	case GraphicsWndInputEventType_KeyRelease:
		event.type = ev_keyup;
		hr = xlatekey(*inputEvt, errStr, &event.data1);
		if (FAILED(hr)) {
			return hr;
		}
		D_PostEvent(&event);
		// fprintf(stderr, "ku");
		break;
	case GraphicsWndInputEventType_MousePress:
		event.type = ev_mouse;
		hr = (*inputEvt)->vtable->GetMouseButton(*inputEvt, &mouseButton);
		if (FAILED(hr)) {
			*errStr = "inputEvt.GetMouseButton press failed";
			return hr;
		}
		event.data1 =
			(mouseButton == GraphicsWndInputEventMouseButton_Left ? 1 : 0)
			| (mouseButton == GraphicsWndInputEventMouseButton_Right ? 2 : 0)
			| (mouseButton == GraphicsWndInputEventMouseButton_Middle ? 4 : 0);
		event.data2 = event.data3 = 0;
		D_PostEvent(&event);
		// fprintf(stderr, "b");
		break;
	case GraphicsWndInputEventType_MouseRelease:
		event.type = ev_mouse;
		hr = (*inputEvt)->vtable->GetMouseButton(*inputEvt, &mouseButton);
		if (FAILED(hr)) {
			*errStr = "inputEvt.GetMouseButton release failed";
			return hr;
		}
		event.data1 =
			(mouseButton == GraphicsWndInputEventMouseButton_Left ? 1 : 0)
			| (mouseButton == GraphicsWndInputEventMouseButton_Right ? 2 : 0)
			| (mouseButton == GraphicsWndInputEventMouseButton_Middle ? 4 : 0);
		event.data2 = event.data3 = 0;
		D_PostEvent(&event);
		// fprintf(stderr, "bu");
		break;
	case GraphicsWndInputEventType_MouseMove:
		event.type = ev_mouse;
		hr = (*inputEvt)->vtable->GetMouseButton(*inputEvt, &mouseButton);
		if (FAILED(hr)) {
			*errStr = "inputEvt.GetMouseButton move failed";
			return hr;
		}
		hr = (*inputEvt)->vtable->GetMousePosition(*inputEvt, &mouseX, &mouseY);
		if (FAILED(hr)) {
			*errStr = "inputEvt.GetMousePosition move failed";
			return hr;
		}
		event.data1 =
			(mouseButton == GraphicsWndInputEventMouseButton_Left ? 1 : 0)
			| (mouseButton == GraphicsWndInputEventMouseButton_Right ? 2 : 0)
			| (mouseButton == GraphicsWndInputEventMouseButton_Middle ? 4 : 0);
		event.data2 = (mouseX - lastmousex) << 2;
		event.data3 = (lastmousey - mouseY) << 2;

		if (event.data2 || event.data3)
		{
			lastmousex = mouseX;
			lastmousey = mouseY;
			if (mouseX != X_width / 2 &&
				mouseY != X_height / 2)
			{
				D_PostEvent(&event);
				// fprintf(stderr, "m");
				mousemoved = false;
			}
			else
			{
				mousemoved = true;
			}
		}
		break;
	default:
		// not handled event
		assert(false);
		break;
	}

	*res = true;

	return S_OK;
}

boolean I_GetEvent(void)
{
	HRESULT hr = S_OK;
	const char* errStr = NULL;
	IGraphicsWndInputEvent* inputEvt = NULL;
	boolean res = false;

	hr = I_GetEvent_impl(&inputEvt, &errStr, &res);

	if (inputEvt) {
		inputEvt->vtable->Release(inputEvt);
	}

	if (FAILED(hr)) {
		I_Error(errStr);
	}

	return res;
}

//
// I_StartTic
//
void I_StartTic(void)
{
	while (I_GetEvent());

	//if (!X_display)
	//	return;

	//while (XPending(X_display))
	//	I_GetEvent();

	//// Warp the pointer back to the middle of the window
	////  or it will wander off - that is, the game will
	////  loose input focus within X11.
	//if (grabMouse)
	//{
	//	if (!--doPointerWarp)
	//	{
	//		XWarpPointer(X_display,
	//			None,
	//			X_mainWindow,
	//			0, 0,
	//			0, 0,
	//			X_width / 2, X_height / 2);

	//		doPointerWarp = POINTER_WARP_COUNTDOWN;
	//	}
	//}

	//mousemoved = false;

}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit(void)
{
	// what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate(void)
{

	static int	lasttic;
	int		tics;
	int		i;
	byte* imageData = NULL;
	HRESULT hr = S_OK;
	// UNUSED static unsigned char *bigscreen=0;

	// draws little dots on the bottom of the screen
	if (devparm)
	{

		i = I_GetTime();
		tics = i - lasttic;
		lasttic = i;
		if (tics > 20) tics = 20;

		for (i = 0; i < tics * 2; i += 2)
			screens[0][(SCREENHEIGHT - 1) * SCREENWIDTH + i] = 0xff;
		for (; i < 20 * 2; i += 2)
			screens[0][(SCREENHEIGHT - 1) * SCREENWIDTH + i] = 0x0;

	}

	if (multiply != 1) {
		hr = graphicsWnd->vtable->GetCPUBackBuffer(graphicsWnd, (void**)&imageData);
		if (FAILED(hr)) {
			I_Error("Failed to GetCPUBackBuffer");
		}
	}

	// scales the screen size before blitting it
	if (multiply == 2)
	{
		unsigned int* olineptrs[2];
		unsigned int* ilineptr;
		int x, y, i;
		unsigned int twoopixels;
		unsigned int twomoreopixels;
		unsigned int fouripixels;

		ilineptr = (unsigned int*)(screens[0]);
		for (i = 0; i < 2; i++)
			olineptrs[i] = (unsigned int*)&imageData[i * X_width];

		y = SCREENHEIGHT;
		while (y--)
		{
			x = SCREENWIDTH;
			do
			{
				fouripixels = *ilineptr++;
				twoopixels = (fouripixels & 0xff000000)
					| ((fouripixels >> 8) & 0xffff00)
					| ((fouripixels >> 16) & 0xff);
				twomoreopixels = ((fouripixels << 16) & 0xff000000)
					| ((fouripixels << 8) & 0xffff00)
					| (fouripixels & 0xff);
#ifdef __BIG_ENDIAN__
				* olineptrs[0]++ = twoopixels;
				*olineptrs[1]++ = twoopixels;
				*olineptrs[0]++ = twomoreopixels;
				*olineptrs[1]++ = twomoreopixels;
#else
				* olineptrs[0]++ = twomoreopixels;
				*olineptrs[1]++ = twomoreopixels;
				*olineptrs[0]++ = twoopixels;
				*olineptrs[1]++ = twoopixels;
#endif
			} while (x -= 4);
			olineptrs[0] += X_width / 4;
			olineptrs[1] += X_width / 4;
		}

	}
	else if (multiply == 3)
	{
		unsigned int* olineptrs[3];
		unsigned int* ilineptr;
		int x, y, i;
		unsigned int fouropixels[3];
		unsigned int fouripixels;

		ilineptr = (unsigned int*)(screens[0]);
		for (i = 0; i < 3; i++)
			olineptrs[i] = (unsigned int*)&imageData[i * X_width];

		y = SCREENHEIGHT;
		while (y--)
		{
			x = SCREENWIDTH;
			do
			{
				fouripixels = *ilineptr++;
				fouropixels[0] = (fouripixels & 0xff000000)
					| ((fouripixels >> 8) & 0xff0000)
					| ((fouripixels >> 16) & 0xffff);
				fouropixels[1] = ((fouripixels << 8) & 0xff000000)
					| (fouripixels & 0xffff00)
					| ((fouripixels >> 8) & 0xff);
				fouropixels[2] = ((fouripixels << 16) & 0xffff0000)
					| ((fouripixels << 8) & 0xff00)
					| (fouripixels & 0xff);
#ifdef __BIG_ENDIAN__
				* olineptrs[0]++ = fouropixels[0];
				*olineptrs[1]++ = fouropixels[0];
				*olineptrs[2]++ = fouropixels[0];
				*olineptrs[0]++ = fouropixels[1];
				*olineptrs[1]++ = fouropixels[1];
				*olineptrs[2]++ = fouropixels[1];
				*olineptrs[0]++ = fouropixels[2];
				*olineptrs[1]++ = fouropixels[2];
				*olineptrs[2]++ = fouropixels[2];
#else
				* olineptrs[0]++ = fouropixels[2];
				*olineptrs[1]++ = fouropixels[2];
				*olineptrs[2]++ = fouropixels[2];
				*olineptrs[0]++ = fouropixels[1];
				*olineptrs[1]++ = fouropixels[1];
				*olineptrs[2]++ = fouropixels[1];
				*olineptrs[0]++ = fouropixels[0];
				*olineptrs[1]++ = fouropixels[0];
				*olineptrs[2]++ = fouropixels[0];
#endif
			} while (x -= 4);
			olineptrs[0] += 2 * X_width / 4;
			olineptrs[1] += 2 * X_width / 4;
			olineptrs[2] += 2 * X_width / 4;
		}

	}
	else if (multiply == 4)
	{
		// Broken. Gotta fix this some day.
		void Expand4(unsigned*, double*);
		Expand4((unsigned*)(screens[0]), (double*)(imageData));
	}

	hr = graphicsWnd->vtable->FinishScreenUpdate(graphicsWnd);
	if (FAILED(hr)) {
		I_Error("Failed to FinishScreenUpdate");
	}

	//if (doShm)
	//{

	//	if (!XShmPutImage(X_display,
	//		X_mainWindow,
	//		X_gc,
	//		image,
	//		0, 0,
	//		0, 0,
	//		X_width, X_height,
	//		True))
	//		I_Error("XShmPutImage() failed\n");

	//	// wait for it to finish and processes all input events
	//	shmFinished = false;
	//	do
	//	{
	//		I_GetEvent();
	//	} while (!shmFinished);

	//}
	//else
	//{

	//	// draw the image
	//	XPutImage(X_display,
	//		X_mainWindow,
	//		X_gc,
	//		image,
	//		0, 0,
	//		0, 0,
	//		X_width, X_height);

	//	// sync up with server
	//	XSync(X_display, False);

	//}

}


//
// I_ReadScreen
//
void I_ReadScreen(byte* scr)
{
	memcpy(scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}


//
// Palette stuff.
//
//static XColor	colors[256];

//void UploadNewPalette(Colormap cmap, byte* palette)
//{
//
//	register int	i;
//	register int	c;
//	static boolean	firstcall = true;
//
//#ifdef __cplusplus
//	if (X_visualinfo.c_class == PseudoColor && X_visualinfo.depth == 8)
//#else
//	if (X_visualinfo.class == PseudoColor && X_visualinfo.depth == 8)
//#endif
//	{
//		// initialize the colormap
//		if (firstcall)
//		{
//			firstcall = false;
//			for (i = 0; i < 256; i++)
//			{
//				colors[i].pixel = i;
//				colors[i].flags = DoRed | DoGreen | DoBlue;
//			}
//		}
//
//		// set the X colormap entries
//		for (i = 0; i < 256; i++)
//		{
//			c = gammatable[usegamma][*palette++];
//			colors[i].red = (c << 8) + c;
//			c = gammatable[usegamma][*palette++];
//			colors[i].green = (c << 8) + c;
//			c = gammatable[usegamma][*palette++];
//			colors[i].blue = (c << 8) + c;
//		}
//
//		// store the colors to the current colormap
//		XStoreColors(X_display, cmap, colors, 256);
//
//	}
//}

//
// I_SetPalette
//
void I_SetPalette(byte* palette)
{
	assert(false);
	//UploadNewPalette(X_cmap, palette);
}


//
// This function is probably redundant,
//  if XShmDetach works properly.
// ddt never detached the XShm memory,
//  thus there might have been stale
//  handles accumulating.
//
//void grabsharedmemory(int size)
//{
//
//	int			key = ('d' << 24) | ('o' << 16) | ('o' << 8) | 'm';
//	struct shmid_ds	shminfo;
//	int			minsize = 320 * 200;
//	int			id;
//	int			rc;
//	// UNUSED int done=0;
//	int			pollution = 5;
//
//	// try to use what was here before
//	do
//	{
//		id = shmget((key_t)key, minsize, 0777); // just get the id
//		if (id != -1)
//		{
//			rc = shmctl(id, IPC_STAT, &shminfo); // get stats on it
//			if (!rc)
//			{
//				if (shminfo.shm_nattch)
//				{
//					fprintf(stderr, "User %d appears to be running "
//						"DOOM.  Is that wise?\n", shminfo.shm_cpid);
//					key++;
//				}
//				else
//				{
//					if (getuid() == shminfo.shm_perm.cuid)
//					{
//						rc = shmctl(id, IPC_RMID, 0);
//						if (!rc)
//							fprintf(stderr,
//								"Was able to kill my old shared memory\n");
//						else
//							I_Error("Was NOT able to kill my old shared memory");
//
//						id = shmget((key_t)key, size, IPC_CREAT | 0777);
//						if (id == -1)
//							I_Error("Could not get shared memory");
//
//						rc = shmctl(id, IPC_STAT, &shminfo);
//
//						break;
//
//					}
//					if (size >= shminfo.shm_segsz)
//					{
//						fprintf(stderr,
//							"will use %d's stale shared memory\n",
//							shminfo.shm_cpid);
//						break;
//					}
//					else
//					{
//						fprintf(stderr,
//							"warning: can't use stale "
//							"shared memory belonging to id %d, "
//							"key=0x%x\n",
//							shminfo.shm_cpid, key);
//						key++;
//					}
//				}
//			}
//			else
//			{
//				I_Error("could not get stats on key=%d", key);
//			}
//		}
//		else
//		{
//			id = shmget((key_t)key, size, IPC_CREAT | 0777);
//			if (id == -1)
//			{
//				extern int errno;
//				fprintf(stderr, "errno=%d\n", errno);
//				I_Error("Could not get any shared memory");
//			}
//			break;
//		}
//	} while (--pollution);
//
//	if (!pollution)
//	{
//		I_Error("Sorry, system too polluted with stale "
//			"shared memory segments.\n");
//	}
//
//	X_shminfo.shmid = id;
//
//	// attach to the shared memory segment
//	image->data = X_shminfo.shmaddr = shmat(id, 0, 0);
//
//	fprintf(stderr, "shared memory id=%d, addr=0x%x\n", id,
//		(int)(image->data));
//}

void I_InitGraphics(void)
{
	HRESULT hr = S_OK;
	int bitDepth = 8;
	//char* displayname = 0;
	char* graphicsImplName = 0;
	char* d = 0;
	//int			n = 0;
	int			pnum = 0;
	//int			x = 0;
	//int			y = 0;

	// warning: char format, different type arg
	//char		xsign = ' ';
	//char		ysign = ' ';

	int			oktodraw = 0;
	unsigned long	attribmask = 0;
	int			valuemask = 0;
	static int		firsttime = 1;

	if (!firsttime)
		return;
	firsttime = 0;

	signal(SIGINT, (void (*)(int)) I_Quit);

	if ((pnum = M_CheckParm("-graphicsImplName"))) // get name to create graphics implementation
	{
		graphicsImplName = myargv[pnum + 1];
		hr = CreateIGraphicsWnd(graphicsImplName, &graphicsWnd);

		if (FAILED(hr)) {
			I_Error("Failed to create graphics implementation [%s]", graphicsImplName);
		}
	}
	else
	{
		I_Error("no -graphicsImplName parameter. Must set -graphicsImplName.");
	}

	if (M_CheckParm("-2"))
		multiply = 2;

	if (M_CheckParm("-3"))
		multiply = 3;

	if (M_CheckParm("-4"))
		multiply = 4;

	X_width = SCREENWIDTH * multiply;
	X_height = SCREENHEIGHT * multiply;

	// check if the user wants to grab the mouse (quite unnice)
	grabMouse = !!M_CheckParm("-grabmouse");

	hr = graphicsWnd->vtable->InitializeScreen(graphicsWnd,
		X_width,
		X_height,
		bitDepth
	);
	if (FAILED(hr)) {
		I_Error("Failed to initialize graphics with width=[%d] height=[%d] bitDepth=[%d]", X_width, X_height, bitDepth);
	}

	if (multiply == 1) {
		hr = graphicsWnd->vtable->GetCPUBackBuffer(graphicsWnd, (void**)&screens[0]);
		if (FAILED(hr)) {
			I_Error("Failed to GetCPUBackBuffer");
		}
	}
	else {
		screens[0] = (unsigned char*)malloc(SCREENWIDTH * SCREENHEIGHT);
	}
}


unsigned	exptable[256];

void InitExpand(void)
{
	int		i;

	for (i = 0; i < 256; i++)
		exptable[i] = i | (i << 8) | (i << 16) | (i << 24);
}

double		exptable2[256 * 256];

void InitExpand2(void)
{
	int		i;
	int		j;
	// UNUSED unsigned	iexp, jexp;
	double* exp;
	union
	{
		double 		d;
		unsigned	u[2];
	} pixel;

	printf("building exptable2...\n");
	exp = exptable2;
	for (i = 0; i < 256; i++)
	{
		pixel.u[0] = i | (i << 8) | (i << 16) | (i << 24);
		for (j = 0; j < 256; j++)
		{
			pixel.u[1] = j | (j << 8) | (j << 16) | (j << 24);
			*exp++ = pixel.d;
		}
	}
	printf("done.\n");
}

int	inited;

void
Expand4
(unsigned* lineptr,
	double* xline)
{
	double	dpixel;
	unsigned	x;
	unsigned 	y;
	unsigned	fourpixels;
	unsigned	step;
	double* exp;

	exp = exptable2;
	if (!inited)
	{
		inited = 1;
		InitExpand2();
	}


	step = 3 * SCREENWIDTH / 2;

	y = SCREENHEIGHT - 1;
	do
	{
		x = SCREENWIDTH;

		do
		{
			fourpixels = lineptr[0];

			dpixel = *(double*)((int)exp + ((fourpixels & 0xffff0000) >> 13));
			xline[0] = dpixel;
			xline[160] = dpixel;
			xline[320] = dpixel;
			xline[480] = dpixel;

			dpixel = *(double*)((int)exp + ((fourpixels & 0xffff) << 3));
			xline[1] = dpixel;
			xline[161] = dpixel;
			xline[321] = dpixel;
			xline[481] = dpixel;

			fourpixels = lineptr[1];

			dpixel = *(double*)((int)exp + ((fourpixels & 0xffff0000) >> 13));
			xline[2] = dpixel;
			xline[162] = dpixel;
			xline[322] = dpixel;
			xline[482] = dpixel;

			dpixel = *(double*)((int)exp + ((fourpixels & 0xffff) << 3));
			xline[3] = dpixel;
			xline[163] = dpixel;
			xline[323] = dpixel;
			xline[483] = dpixel;

			fourpixels = lineptr[2];

			dpixel = *(double*)((int)exp + ((fourpixels & 0xffff0000) >> 13));
			xline[4] = dpixel;
			xline[164] = dpixel;
			xline[324] = dpixel;
			xline[484] = dpixel;

			dpixel = *(double*)((int)exp + ((fourpixels & 0xffff) << 3));
			xline[5] = dpixel;
			xline[165] = dpixel;
			xline[325] = dpixel;
			xline[485] = dpixel;

			fourpixels = lineptr[3];

			dpixel = *(double*)((int)exp + ((fourpixels & 0xffff0000) >> 13));
			xline[6] = dpixel;
			xline[166] = dpixel;
			xline[326] = dpixel;
			xline[486] = dpixel;

			dpixel = *(double*)((int)exp + ((fourpixels & 0xffff) << 3));
			xline[7] = dpixel;
			xline[167] = dpixel;
			xline[327] = dpixel;
			xline[487] = dpixel;

			lineptr += 4;
			xline += 8;
		} while (x -= 16);
		xline += step;
	} while (y--);
}