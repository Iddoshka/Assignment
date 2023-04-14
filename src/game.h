#pragma once
#include"tilemap.h"
#include"player.h"
#include"SDL.h"
namespace Tmpl8 {

enum  state
{ // creating an enum var for the state machine
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
	void winingLine();
	void reset(Ball &player);
	void Shutdown();
	void Tick( float deltaTime );
	void printScreen(TileMaps map);
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { /* implement if you want to detect mouse movement */ }
	void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown( int key ) { /* implement if you want to handle keys */ }
	void switch_state();
private:
	Surface* screen;
	state state_machine = menu; // the var with the current state
};

}; // namespace Tmpl8