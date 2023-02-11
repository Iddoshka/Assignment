#include "game.h"
#include "surface.h"
#include <cstdio> //printf
#include"player.h"
constexpr float gravity = 0.7f;
namespace Tmpl8
{
	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
	}
	
	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{
	}

	static Sprite rotatingGun(new Surface("assets/aagun.tga"), 36);
	static int frame = 0;
	Ball player(ScreenWidth / 2.0f, 0, 18);
	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{
		// clear the graphics window
		screen->Clear(0);
		player.printBall(screen);
		player.verlet();
		player.addY(gravity);
		_sleep(25);
	}
};