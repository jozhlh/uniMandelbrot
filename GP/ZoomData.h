// Mandelbrot Explorer - Josh Hale - 1402439
// ZoomData.h
// Class which stores viewport parameter values and performs
// viewport transformations

#ifndef _ZOOMDATA_H
#define _ZOOMDATA_H

#include <math.h>

// pixel dimensions of the generated image
#define WIDTH 640
#define HEIGHT 480	

// colour values
#define WHITE	0xFFFFFF
#define BLACK	0x000000
#define RED		0x000002
#define GREEN	0x000200
#define BLUE	0x020000

// initial viewport parameters
#define START_MAX_ITERATIONS	500
#define START_LEFT		-2.0
#define START_RIGHT		 1.0
#define START_TOP		 1.125
#define START_BOTTOM	-1.125

// panning modifier (amount moved each key-press)
#define HORIZONTAL_SPEED 0.6
#define VERTICAL_SPEED	 0.6

// zooming modifier (amount zoomed each key-press)
#define ZOOM_OUT	2.0
#define ZOOM_IN		0.5
#define ITERATION_MULTIPLIER 1.4

enum directions { m_left, m_right, m_up, m_down };

// struct for holding and passing data
struct ZoomValues
{
	double left;
	double right;
	double top;
	double bottom;

	int max_iterations;
};

class ZoomData
{
public:
	/// PUBLIC FUNCTIONS ///
	void Initialise();
	ZoomValues GetValues();
	void Magnify(directions direction);
	void Move(directions direction);

private:
	/// PRIVATE FUNCTIONS ///
	void Transform();

	/// PRIVATE VARIABLES ///
	double magnification;
	ZoomValues intermediate;
	ZoomValues zoom;
	
};

#endif