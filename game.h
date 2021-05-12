#pragma once

namespace Tmpl8 {

class Surface;

class Game
{
public:
	int mousePressed;
	int mousex, mousey;
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();
	void Tick( float deltaTime );
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown(int button) { mousePressed = button; }
	void MouseMove(int x, int y) { mousex = x, mousey = y; }
	void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown( int key ) { /* implement if you want to handle keys */ }

private:
	Surface* screen;
};


}; // namespace Tmpl8