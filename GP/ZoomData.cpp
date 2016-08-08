// Mandelbrot Explorer - Josh Hale - 1402439
// ZoomData.cpp
// Class which stores viewport parameter values and performs
// viewport transformations

#include "ZoomData.h"

// sets parameters to initial values
void ZoomData::Initialise()
{
	zoom.left = START_LEFT;
	zoom.right = START_RIGHT;;
	zoom.top = START_TOP;
	zoom.bottom = START_BOTTOM;
	zoom.max_iterations = START_MAX_ITERATIONS;
	magnification = 1;
}

// getter for parameters
ZoomValues ZoomData::GetValues()
{
	return zoom;
}

// sets the scaling multiplier for changing the viewport parameters
void ZoomData::Magnify(directions direction)
{
	// set parameters
	if (direction == m_up)
	{
		magnification = ZOOM_IN;

		// increase iterations per zoom
		zoom.max_iterations = zoom.max_iterations * ITERATION_MULTIPLIER;
	}
	else
	{
		magnification = ZOOM_OUT;


		// increase iterations per zoom
		zoom.max_iterations = zoom.max_iterations / ITERATION_MULTIPLIER;
	}
	
	// perform transformation of viewport
	Transform();
}

// transforms viewport
void ZoomData::Transform()
{
	// get centre of viewport
	double tempX = (zoom.left + zoom.right) / 2;
	double tempY = (zoom.bottom + zoom.top) / 2;

	// transform to origin
	intermediate.right = zoom.right - tempX;
	intermediate.left = zoom.left - tempX;
	intermediate.top = zoom.top - tempY;
	intermediate.bottom = zoom.bottom - tempY;

	// scale according to zoom multiplier
	intermediate.left = intermediate.left * magnification;
	intermediate.right = intermediate.right * magnification;
	intermediate.top = intermediate.top * magnification;
	intermediate.bottom = intermediate.bottom * magnification;


	// transform back to centre of viewport
	zoom.left = tempX + intermediate.left;
	zoom.right = tempX + intermediate.right;
	zoom.top = tempY + intermediate.top;
	zoom.bottom = tempY + intermediate.bottom;
}

// shifts the viewport in the xy plane
void ZoomData::Move(directions direction)
{
	switch (direction)
	{
	case m_left:
		zoom.left -= (magnification * HORIZONTAL_SPEED);
		zoom.right -= (magnification * HORIZONTAL_SPEED);
		break;
	case m_right:
		zoom.left += (magnification * HORIZONTAL_SPEED);
		zoom.right += (magnification * HORIZONTAL_SPEED);
		break;
	case m_up:
		zoom.top += (magnification * VERTICAL_SPEED);
		zoom.bottom += (magnification * VERTICAL_SPEED);
		break;
	case m_down:
		zoom.top -= (magnification * VERTICAL_SPEED);
		zoom.bottom -= (magnification * VERTICAL_SPEED);
		break;
	}
}