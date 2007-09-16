#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <set>

#include "adt.h"
#include "mpq_libmpq.h"

#define MODEL_VISUALIZATION
#ifdef MODEL_VISUALIZATION
#include <gl/GL.h>
#include <gl/GLU.h>

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}


int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	return TRUE;										// Initialization Went OK
}


/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
						 UINT	uMsg,			// Message For This Window
						 WPARAM	wParam,			// Additional Message Information
						 LPARAM	lParam)			// Additional Message Information
{
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}
#endif

model::~model()
{
	printf("Unloading model %s\n", file2);
}
bool model::Load(const char * filename)
{
	/* fix filename - change .mdx -> .m2 */
	char * temp_filename = strdup(filename);
	char * p = strrchr(temp_filename, '.');
	file2 = strdup(filename);
	*(p+1) = 'm';
	*(p+2) = '2';
	*(p+3) = '\0';
	printf("Loading model: %s\n", temp_filename);

	MPQFile f(temp_filename);
	if(f.isEof())
	{
		free(temp_filename);
		printf("Error! Could not open %s\n", temp_filename);
		return false;
	}
	free(temp_filename);
	
	f.read(&header, sizeof(ModelHeader));
	
	if(!header.nBoundingVertices || !header.nBoundingTriangles)
	{
		printf("File %s has no bounding volume!\n", filename);
		f.close();
		return false;
	}

	Vector * bounds = new Vector[header.nBoundingVertices];
	uint16 * boundTris = new uint16[header.nBoundingTriangles];
	float min_x=0, max_x=0, min_y=0, max_y=0, min_z=0, max_z=0;
	
	bounds = new Vector[header.nBoundingVertices];
	//Vector *b = (Vector*)(f.getBuffer() + header.ofsBoundingVertices);
	for (size_t i=0; i<header.nBoundingVertices; i++) {
		//memcpy(&bounds[i], (b+sizeof(Vector)*i), sizeof(Vector) );
		f.seek(header.ofsBoundingVertices + 12 * i);
		f.read(&bounds[i], 12);
		bounds[i].Fix();
	}

	boundTris = new uint16[header.nBoundingTriangles];
	//memcpy(boundTris, f.getBuffer() + header.ofsBoundingTriangles, header.nBoundingTriangles*sizeof(uint16));
	f.seek(header.ofsBoundingTriangles);
	f.read(boundTris, header.nBoundingTriangles*sizeof(uint16));

	for(int i = 0; i < header.nBoundingTriangles; ++i)
	{
		size_t v = boundTris[i];
		if(v < header.nBoundingTriangles)
		{
			/* set of 3 vectors, each representing a point in the triangle */
			Vector * vc = &bounds[v];

			/* only use the base of the model */
			/* TODO: Replace with AABB collision between sphere (grid square) and triangle (model) */
			if(bounds[v].y >= 5.0f)
				continue;
			
			/* shit, this is messy :P */
			int j=0;
			if(vc[j].x < min_x)
				min_x = vc[j].x;
			if(vc[j].y < min_y)
				min_y = vc[j].y;
			if(vc[j].z < min_z)
				min_z = vc[j].z;

			if(vc[j].x > max_x)
				max_x = vc[j].x;
			if(vc[j].y > max_y)
				max_y = vc[j].y;
			if(vc[j].z > max_z)
				max_z = vc[j].z;
		}
	}

	/* calculate the size of the bounding volume */
	/* let's remember wow's coordinate system is fucked, z = y */
	sizex = fabs(max_x - min_x);
	sizey = fabs(max_z - min_z);

	printf("Bounding box calculated: \n   %f-%f/%f-%f/%f/%f\n", min_x, max_x, min_z, max_z, sizex, sizey);

#ifdef MODEL_VISUALIZATION
	CreateGLWindow("BoundingPreview", 1100, 600, 32, false);

	MSG msg;
	GLfloat rrt=38;

	while(true)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(10.0f,-10.0f,-100.0f);						// Move Left 1.5 Units And Into The Screen 6.0
	//glTranslatef(0.0f,0.0f,-6.0f);
	glRotatef(rrt, 1.0f, 0.0f, 0.0f);
	//glRotatef(rrt, 0.0f, 0.0f, 1.0f);
	//glRotatef(rrt, 1.0f, 0.0f, 1.0f);
	rrt += 0.6f;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	for (size_t i=0; i<header.nBoundingTriangles; i++) {
		size_t v = boundTris[i];
		if (v < header.nBoundingVertices)
		{
			//if(bounds[v].y < 5.0f)
                glVertex3f(bounds[v].x, bounds[v].y, bounds[v].z);
			//else
				//glVertex3f(0,0,0);
		}
		else 
			glVertex3f(0,0,0);
	}
	glEnd();

	glLoadIdentity();
	glTranslatef(-15.0f, -10.0f, -100.0f);
	
	glRotatef(rrt, 1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	
	for (size_t i=0; i<header.nBoundingTriangles; i++) {
		size_t v = boundTris[i];
		if (v < header.nBoundingVertices)
		{
            Vector * StoreVector = &bounds[v];
            if (StoreVector->y > 10.0f)
            {
                // if we are here we need to split up the vector into 2 vectors
                // and offcourse draw them
                Vector vectorsplit[2]; // we split it into 2 vectors
                vectorsplit[1].x = StoreVector->x;
                vectorsplit[1].y = 10.0f;
                vectorsplit[1].z = 10.0f;

                vectorsplit[2].x = StoreVector->x;
                vectorsplit[2].y = 10.0f;
                vectorsplit[2].z = StoreVector->z;

                glVertex3f(vectorsplit[1].x, vectorsplit[1].y, vectorsplit[1].z);
                glVertex3f(vectorsplit[2].x, vectorsplit[2].y, vectorsplit[2].z);

            }
            else
            {
                glVertex3f(StoreVector->x, StoreVector->y, StoreVector->z);
            }
                //if(bounds[v].y < 5.0f)
                //glVertex3f(bounds[v].x, 5.0f, bounds[v].z);
                //else
				//glVertex3f(0,0,0);
		}
		else 
			glVertex3f(0,0,0);
	}
	glEnd();

	/* make a box from the maxx/maxz */

	glLoadIdentity();
	glTranslatef(-45.0f, -10.0f, -100.0f);

	glRotatef(rrt, 1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	// top
	glVertex3f(min_x, 5.0f, max_z);
	glVertex3f(min_x, 5.0f, min_z);
	glVertex3f(max_x, 5.0f, min_z);
	glVertex3f(max_x, 5.0f, max_z);
	glEnd();

	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		SwapBuffers(hDC);
	Sleep(100);
	}
#endif

	delete [] bounds;
	delete [] boundTris;
	return true;
}