// Mandelbrot Explorer - Josh Hale - 1402439
// Farm.h
// Class which manages the threads and their functions

#ifndef FARM_H
#define FARM_H

#include <windows.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <fstream>
#include <iostream>
#include <gl/glu.h>
#include <chrono>
#include <atomic>
#include "MandelbrotTask.h"
#include "Input.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::condition_variable;
using std::ofstream;
using std::queue;
using std::mutex;
using std::thread;
using std::vector;
using std::unique_lock;
using std::mem_fun;
using std::cout;
using std::endl;

typedef std::chrono::steady_clock the_clock;

// number of threads the program will run
// 1 input thread, the rest computing threads
#define THREAD_NUM 8

// structure for holding the complete image pixel data
struct ImageInfo
{
	uint32_t dat[HEIGHT][WIDTH];
};

class Farm
{
public:
	/// PUBLIC FUNCTIONS ///
	void Initialise(Input* in_, ZoomData* zoom_level_, ImageInfo *image_data_);
	void CleanUp();

	/// PUBLIC VARIABLES ///
	int line_count;
	bool image_ready;
	bool texture_drawn;

private:
	/// PRIVATE FUNCTIONS ///
	void AddTask(MandelbrotTask* task_);
	void Worker();
	void GetInput();
	void RemoveOldTasks();

	/// PRIVATE VARIABLES ///
	the_clock::time_point start;
	the_clock::time_point end;
	vector<thread*> threads_;
	queue<MandelbrotTask*> task_queue_;
	queue<MandelbrotTask*> old_queue_;
	Input* input_;
	ImageInfo* image_struct_;
	ZoomData* current_zoom_;
	mutex task_queue_mutex;
	mutex line_count_mutex;
	mutex ready_for_input_mutex;
	condition_variable ready_for_input_cv;
	bool ready_for_input;
	std::atomic<bool> end_threads;
	ofstream timing_output;
	int calculation;
};

#endif