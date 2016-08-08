#ifndef INPUT_H
#define INPUT_H
// Input class


#include <Windows.h>

#define W_KEY 0x57
#define A_KEY 0x41
#define S_KEY 0x53
#define D_KEY 0x44
#define KEY_1 0x31
#define KEY_2 0x32

class Input
{
	typedef struct Mouse
	{
		int x,y;
		bool left, right;
	};

public:
	void SetKeyDown(WPARAM);
	void SetKeyUp(WPARAM);

	bool isKeyDown(int);
	void setMouseX(int);
	void setMouseY(int);
	int getMouseX();
	int getMouseY();
	// set mouse botton down (left, right, middle)
	// set mouse botton up

private:
	bool keys[256];
	Mouse mouse;

};

#endif