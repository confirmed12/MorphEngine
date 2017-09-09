#pragma once

class CCInput
{
public:
	CCInput() {};

	void GetClicked();
	bool Holding( int, int, int, int );
	bool Hovering( int, int, int, int );
	bool Clicked( int, int, int, int );
private:
	bool bClicked, bMouseD;
}; extern CCInput* Inputs;