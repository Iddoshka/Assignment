#pragma once
#include"tilemap.h"
#include"player.h"
#include"SDL.h"
namespace Tmpl8 {

enum states
{ // creating an enum definition for the state machine
	menu,
	play,
	stop
};

class Surface;
class Game
{
public:
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void winningLine();/* prints a line upon winning */
	void reset(Ball &player);
	void Shutdown();
	void Tick( float deltaTime );
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { /* implement if you want to detect mouse movement */ }
	void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown( int key ) { /* implement if you want to handle keys */ }
	void switchState();
private:
	Surface* screen;
	states state_machine = menu; // holds the current state
};

}; // namespace Tmpl8