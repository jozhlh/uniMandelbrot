// Mandelbrot Explorer - Josh Hale - 1402439
// Scene3D.cpp
// Class which handles OpenGL scene and graphical capabilities

#include "Scene3D.h"
#include <iostream>

bool Scene3D::CreatePixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd = {0}; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);	// Set the size of the structure
    pfd.nVersion = 1;							// Always set this to 1
	// Pass in the appropriate OpenGL flags
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;			// standard mask (this is ignored anyway)
    pfd.iPixelType = PFD_TYPE_RGBA;				// RGB and Alpha pixel type
    pfd.cColorBits = COLOUR_DEPTH;				// Here we use our #define for the color bits
    pfd.cDepthBits = COLOUR_DEPTH;				// Z buffer precision for depth, (z-fighting)
    pfd.cAccumBits = 0;							// nothing for accumulation
    pfd.cStencilBits = COLOUR_DEPTH;			// Used for stencil buffer (COLOUR_DEPTH for on, 0 for off)
 
	//Gets a best match on the pixel format as passed in from device
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == false ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return false; 
    } 
 
	//sets the pixel format if its ok. 
    if (SetPixelFormat(hdc, pixelformat, &pfd) == false) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return false; 
    } 
 
    return true;
}

void Scene3D::ResizeGLWindow(int width, int height)// Initialize The GL Window
{
	if (height==0)// Prevent A Divide By Zero error
	{
		height=1;// Make the Height Equal One
	}

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//calculate aspect ratio
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 1 ,150.0f);

	glMatrixMode(GL_MODELVIEW);// Select The Modelview Matrix
	glLoadIdentity();// Reset The Modelview Matrix
}

void Scene3D::InitializeOpenGL(int width, int height) 
{  
    hdc = GetDC(*hwnd);//  sets  global HDC

    if (!CreatePixelFormat(hdc))//  sets  pixel format
        PostQuitMessage (0);

    hrc = wglCreateContext(hdc);	//  creates  rendering context from  hdc
    wglMakeCurrent(hdc, hrc);		//	Use this HRC.

	ResizeGLWindow(width, height);	// Setup the Screen
}

// initialise program functionality and graphical output
void Scene3D::Init(HWND* wnd, Input* in)
{
	hwnd = wnd;
	input_ = in;

	GetClientRect(*hwnd, &screenRect);	//get rect into our handy global rect
	InitializeOpenGL(screenRect.right, screenRect.bottom); // initialise openGL

	//OpenGL settings
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	//glClearColor(0.39f, 0.58f, 93.0f, 1.0f);			// Cornflour Blue Background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glClearStencil(0);									// How to clear the stencil buffer (added in for on, comment out for off)
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	//Also, do any other setting variables here for your app if you wish. 
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// create new objects
	zoom_level_ = new ZoomData;
	farm_ = new Farm;
	image_struct_ = new ImageInfo;

	// initialise viewport parameters
	zoom_level_->Initialise();

	// start threads
	farm_->Initialise(in, zoom_level_, image_struct_);
}

// every frame see if theres anything new to render, and render everything
void Scene3D::Update(float dt)
{
	// if the pixel data has been created for the entire image
	if (farm_->image_ready & !farm_->texture_drawn)
	{
		// create a texture from the pixel data
		GenerateTexture();

		// we already have a texture
		farm_->image_ready = false;
		farm_->texture_drawn = true;
	}

	// how many lines have been calculated by the worker threads
	progress = farm_->line_count;

	// render scene once all updating has been complete
	Render(dt);
}

void Scene3D::Render(float dt) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear The Screen And The Depth Buffer
	glLoadIdentity();// load Identity Matrix

	// set camera looking down the -z axis, 6 units away from the center
	gluLookAt(0, 0, 6, 0, 0, 0, 0, 1, 0); //Where we are, What we look at, and which way is up

	// draw surface which mandelbrot texture is applied to
	DrawSurface();

	// draw loading progress bar
	DrawLoading();

	// finished rendering
	SwapBuffers(hdc);// Swap the frame buffers.
}		

// draw loading progress bar
void Scene3D::DrawLoading()
{
	float x = 1.0f;
	float y = 1.0f;

	// what percentage of total prgress are we at
	float done = progress / 480.0f;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glBegin(GL_QUADS);	// begin drawing state
	glColor3f(0.0f, 1.0f, 0.0f); // set colour to 1.0 green
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f((x*-4.2f), ((y*-2.4f) + y*4.8f*done), 0.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f((x*-4.2f), (y*-2.4f), 0.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f((x*-3.2f), (y*-2.4f), 0.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f((x*-3.2f), ((y*-2.4f) + y*4.8f*done), 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f); // set colour to 1.0 white
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

// draw surface which mandelbrot texture is applied to
void Scene3D::DrawSurface()
{
	float x = 1.0f;
	float y = 1.0f;

	glBindTexture(GL_TEXTURE_2D, mandelbrot_texture);	// tells opengl which texture to use
	glBegin(GL_QUADS);	// begin drawing state

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f((x*-3.2f), (y*2.4f), 0.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f((x*-3.2f), (y*-2.4f), 0.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f((x*3.2f), (y*-2.4f), 0.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f((x*3.2f), (y*2.4f), 0.0f);
	glEnd();		// end drawing
}

void Scene3D::Resize()
{
	if(hwnd == NULL)
		return;

	GetClientRect(*hwnd, &screenRect);	
	ResizeGLWindow(screenRect.right, screenRect.bottom);	
}

// free up memory
void Scene3D::CleanUp()
{
	farm_->CleanUp();
	
	delete zoom_level_;
	zoom_level_ = NULL;

	delete farm_;
	farm_ = NULL;

	delete image_struct_;
	image_struct_ = NULL;
}

// generate texture from pixel data
void Scene3D::GenerateTexture()
{
	glGenTextures(1, &mandelbrot_texture);
	glBindTexture(GL_TEXTURE_2D, mandelbrot_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, WIDTH, HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, image_struct_->dat);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	farm_->image_ready = false;
	farm_->texture_drawn = true;
}