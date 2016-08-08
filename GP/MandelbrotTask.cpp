// Mandelbrot Explorer - Josh Hale - 1402439
// MandelbrotTask.cpp
// Class which evaluates the equation for a row of pixels in the mandelbrot set

#include "MandelbrotTask.h"
#include <complex>

using std::complex;

// constructor - handed row number
MandelbrotTask::MandelbrotTask(int y)
{
	row = y;
}

MandelbrotTask::~MandelbrotTask()
{
}

// sets the limiting parameters for the current viewport
void MandelbrotTask::SetZoom(ZoomData *zoom_level_)
{
	zoom = zoom_level_->GetValues();
}

// performs calculation and sets colour data for each pixel in a row
RowInfo* MandelbrotTask::run()
{
	// create a new row object and assign which row it is
	this_row = new RowInfo;
	this_row->id = row;

	// for every pixel in the row
	for (int x = 0; x < WIDTH; ++x)
	{
		// Wwrk out the point in the complex plane that
		// corresponds to this pixel in the output image.
		complex<double> c(zoom.left + (x * (zoom.right - zoom.left) / WIDTH),
			zoom.top + (row * (zoom.bottom - zoom.top) / HEIGHT));

		// start off z at (0, 0).
		complex<double> z(0.0, 0.0);

		// pixels start at white
		uint32_t colour = BLACK;

		// iterate z = z^2 + c until z moves more than 2 units
		// away from (0, 0), or we've iterated too many times.
		int iterations = 0;
		while (abs(z) < 2.0 && iterations < zoom.max_iterations)
		{
			z = (z * z) + c;
			++iterations;
			// add a small amount of red to the pixel
			// creates a gradient depending on number of iterations
		//	colour += RED;
			colour = ColourCorrection(colour);
		}
		if (iterations == zoom.max_iterations)
		{
			// z didn't escape from the circle.
			// this point is in the Mandelbrot set.
			this_row->row_data[x] = BLACK;
		}
		else
		{
			// this point is not in the mandelbrot set
			// give the pixel colour based on the iteration it exited on
			this_row->row_data[x] = colour;
		}
	}
	// return the pixel information for the row
	return this_row;
}

uint32_t MandelbrotTask::ColourCorrection(uint32_t current)
{
	if (current < 0x0000FF)
	{
		current += RED;
	}
	else if (current < 0x00FF00)
	{
		current -= RED;
		current += GREEN;
	}
	else if (current < 0xFF0000)
	{
		current -= GREEN;
		current += BLUE;
	}
	else if (current < 0xFFFFFF)
	{
		current += RED;
	}
	else
	{
		current = BLACK;
	}

	return current;
}

