// Mandelbrot Explorer - Josh Hale - 1402439
// Scene3D.h
// Class which handles OpenGL scene and graphical capabilities

#ifndef SCENE3D_H
#define SCENE3D_H

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <SOIL.h>
#include "Farm.h"

#define _USE_MATH_DEFINES
#define COLOUR_DEPTH 16	// colour depth

class Scene3D
{
public:
	/// PUBLIC FUNCTIONS ///
	void Init(HWND* hwnd, Input* in);
	void Update(float dt);
	void Render(float dt);	
	void Resize();
	void CleanUp();

private:
	/// PRIVATE FUNCTIONS ///
	bool CreatePixelFormat(HDC hdc);
	void ResizeGLWindow(int width, int height);	
	void InitializeOpenGL(int width, int height); 
	void GenerateTexture();
	void DrawSurface();
	void DrawLoading();
	
	/// PRIVATE VARIABLES ///
	HWND* hwnd;
	Input* input_;
	Farm* farm_;
	ZoomData* zoom_level_;
	ImageInfo* image_struct_;
	RECT screenRect;
	HDC	hdc;
	HGLRC hrc;
	GLuint mandelbrot_texture;
	float progress;
};

#endif