// Mandelbrot Explorer - Josh Hale - 1402439
// MandelbrotTask.h
// Class which evaluates the equation for a row of pixels in the mandelbrot set

#ifndef MANDELBROTTASK_H
#define MANDELBROTTASK_H

#include <cstdint>
#include "ZoomData.h"

// structure which holds the row number and the pixel data for the row
struct RowInfo
{
	int id;
	uint32_t row_data[WIDTH];
};

class MandelbrotTask
{
public:
	/// PUBLIC FUNCTIONS ///
	MandelbrotTask(int y);
	~MandelbrotTask();
	RowInfo* run();
	void SetZoom(ZoomData *zoom_level_);

private:
	/// PRIVATE FUNCTIONS ///
	uint32_t ColourCorrection(uint32_t current);

	/// PRIVATE VARIABLES ///
	int row;
	RowInfo* this_row;
	ZoomValues zoom;
};


#endif