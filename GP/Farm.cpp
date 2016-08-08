// Mandelbrot Explorer - Josh Hale - 1402439
// Farm.cpp
// Class which manages the threads and their functions

#include "Farm.h"

// sets up the thread manager
void Farm::Initialise(Input* in_, ZoomData* zoom_level_, ImageInfo *image_data_)
{
	// allows benchmarking data to be added to file
	timing_output.open("timings.txt");
	timing_output << endl << "Test 1" << endl;
	// receives pointers to input manger, viewport parameters and pixel data
	input_ = in_;
	current_zoom_ = zoom_level_;
	image_struct_ = image_data_;
	calculation = 0;

	// creates (cpu's - 1) threads for computing the mandelbrot set
	for (int i = 0; i < (THREAD_NUM - 1); i++)
	{
		threads_.push_back(new thread(mem_fun(&Farm::Worker), this));
	}

	// creates a thread for responding to input
	threads_.push_back(new thread(mem_fun(&Farm::GetInput), this));

	// signalling booleans initialised
	image_ready = false;
	end_threads = false;
	ready_for_input = false;

	// Start timing
	timing_output << "Image " << calculation << " with " << THREAD_NUM << " threads ";
	start = the_clock::now();

	// add first image to task list row by row
	for (int i = 0; i < HEIGHT; i++)
	{
		AddTask(new MandelbrotTask(i));
	}
}

// closes output file and deletes all threads
void Farm::CleanUp()
{
	timing_output.close();

	end_threads = true;

	ready_for_input_cv.notify_one();

	for (int i = 0; i < THREAD_NUM; i++)
	{
	//std::terminate(threads_.at(i));
		threads_.at(i)->join();
		delete threads_.at(i);
		threads_.at(i) = NULL;
	}

	threads_.clear();
}

// adds a row to the queue for computation
void Farm::AddTask(MandelbrotTask* task_)
{
	task_queue_mutex.lock();
	task_queue_.push(task_);
	task_queue_mutex.unlock();
}

// removes task objects which have been calculated
void Farm::RemoveOldTasks()
{
	while (true)
	{
		if (!old_queue_.empty())
		{
			MandelbrotTask* current_task_ = old_queue_.front();
			old_queue_.pop();
			delete current_task_;
			current_task_ = NULL;
		}
		else
		{
			break;
		}
	}
}

// thread which runs continously computing any tasks which are available
void Farm::Worker()
{
	// used to detect whether this was the last thread to finish
	bool fin = false;

	// while the program is running, continuously check for tasks
	while (!end_threads)
	{
		// lock mutex to access shared task list
		task_queue_mutex.lock();
		if (!task_queue_.empty())
		{
			// still computing an image
			fin = true;

			// get next task, remove from active list, add to finished list
			MandelbrotTask* current_task_ = task_queue_.front();
			task_queue_.pop();
			old_queue_.push(current_task_);

			// finished with shared task list, allow other threads to continue
			task_queue_mutex.unlock();

			// set the parameters for the calculation based on current viewport
			current_task_->SetZoom(current_zoom_);

			// recieve a pointer to the pixel data for the row this task calculated
			RowInfo* temp_row_ = current_task_->run();

			// for the whole row add pixel data to the complete image container
			for (int i = 0; i < WIDTH; i++)
			{
				image_struct_->dat[temp_row_->id][i] = temp_row_->row_data[i];
			}

			// delete the data in the row this pointed to
			delete temp_row_;

			// lock mutex to shared progress tracker
			line_count_mutex.lock();

			// update progress
			line_count++;

			// finished with shared resource, allow other threads to continue
			line_count_mutex.unlock();
		}
		else
		{
			// finished with shared resource, allow other threads to continue
			task_queue_mutex.unlock();

			// lock mutex to shared progress tracker
			line_count_mutex.lock();

			// if every row in the image has been calculated
			if (!(line_count < HEIGHT))
			{
				// the pixel data is ready for texture generation
				image_ready = true;

				// if this was the final thread to finish
				if (fin)
				{
					// Stop timing
					end = the_clock::now();

					// Compute the difference between the two times in milliseconds
					auto time_taken = duration_cast<milliseconds>(end - start).count();
					
					timing_output << "and " << current_zoom_->GetValues().max_iterations << " iterations took " << time_taken << " ms" << endl;

					// delete old task list from memory
					RemoveOldTasks();
					
					// image complete
					line_count = 0;

					// waiting to generate the texture
					texture_drawn = false;

					// awaiting new input
					ready_for_input = true;

					// notify input thread it can recieve input
					ready_for_input_cv.notify_one();
					fin = false;
				}
			}
			// finished with shared resource, allow other threads to continue
			line_count_mutex.unlock();
		}
	}
	return;
}

// receives input
void Farm::GetInput()
{
	// whether we receive input in this cycle
	bool new_input;
	
	// while the program is running
	while (!end_threads)
	{
		// unique lock for condition variable
		unique_lock<mutex> lock(ready_for_input_mutex);

		// exit if program has requested shut down
		if (end_threads)
		{
			break;
		}

		// while we are not awaiting input
		while (!ready_for_input)
		{
			// exit if program has requested shut down
			if (end_threads)
			{
				break;
			}
			// await notification
			ready_for_input_cv.wait(lock);
		}
		// turns false if no input is received this cycle
		new_input = true;

		// if the relevant key is pressed, recalculate viewport parameters
		if (input_->isKeyDown(W_KEY))
		{
			// move up
			input_->SetKeyUp(W_KEY);
			current_zoom_->Move(m_up);
		}
		else if (input_->isKeyDown(S_KEY))
		{
			// move down
			input_->SetKeyUp(S_KEY);
			current_zoom_->Move(m_down);
		}
		else if (input_->isKeyDown(A_KEY))
		{
			// move left
			input_->SetKeyUp(A_KEY);
			current_zoom_->Move(m_left);
		}
		else if (input_->isKeyDown(D_KEY))
		{
			// move right
			input_->SetKeyUp(D_KEY);
			current_zoom_->Move(m_right);
		}
		else if (input_->isKeyDown(VK_UP))
		{
			// increase zoom
			input_->SetKeyUp(VK_UP);
			current_zoom_->Magnify(m_up);
		}
		else if (input_->isKeyDown(VK_DOWN))
		{
			// decrease zoom
			input_->SetKeyUp(VK_DOWN);
			current_zoom_->Magnify(m_down);
		}
		else
		{
			// no input this cycle
			new_input = false;
		}

		// if we received input
		if (new_input)
		{
			// calculating new pixel data
			image_ready = false;
			ready_for_input = false;
			calculation++;

			timing_output << "Image " << calculation << " with " << THREAD_NUM << " threads ";
			start = the_clock::now();

			// add a new image to be calculated
			for (int i = 0; i < HEIGHT; i++)
			{
				AddTask(new MandelbrotTask(i));
			}
		}
	}	
}
