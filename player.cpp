#include"player.h"
#include"game.h"

constexpr float gravity = 0.5f;

Ball::Ball(float xIn, float yIn, int rIn) // initialzing the player with its sprite and the given coordinates and radius
	:	ball_sprite(new Tmpl8::Surface("assets/ball.png"),1)
{
	coordinates.x = xIn, coordinates.y = yIn;
	r = rIn;
	pcord.x = xIn, pcord.y = yIn;
	jumpAble = false;
	collision = false;
}
void Ball::printBall(Tmpl8::Surface* screen)
{
	ball_sprite.DrawScaled(coordinates.x - r, coordinates.y - r, r * 2, r * 2, screen);
	if (!jumpAble)
	{
		for (int i = 0; i < 64; i++)
		{
			float r1 = (float)i * Tmpl8::PI / 32, r2 = (float)(i + 1) * Tmpl8::PI / 32;
			screen->Line(coordinates.x - r * sinf(r1), coordinates.y - r * cosf(r1),
				coordinates.x - r * sinf(r2), coordinates.y - r * cosf(r2), 0x00ff00);
		}
	}
}

void Ball::verlet(TileMaps map)
{
	velocity.x = coordinates.x - pcord.x, velocity.y = coordinates.y - pcord.y;
	// store previous position.
	pcord.x = coordinates.x, pcord.y = coordinates.y;
	// Verlet integration
	coordinates.x += velocity.x;
	coordinates.y += velocity.y;
	if(!collision) 
		coordinates.y += gravity;
	//constraints
	if (coordinates.y > ScreenHeight - r + 1) // checking for bottom of the screen collision
	{
		pcord.y = coordinates.y, coordinates.y = ScreenHeight - r + 1;
	}
}

void Ball::mapReact(Tmpl8::Surface* screen, TileMaps map)
{
	for (int i = 0; i < map.getHeight(); i++)
	{
		for (int j = 0; j < map.getWidth() / 3; j++)
		{
			if (map.getMap()[i][(j * 3) + 2] == 'x')
			{
				Tmpl8::vec2 tileCenter((j) * 32 + 32 / 2, (i) * 32 + 32 / 2);
				Tmpl8::vec2 diff = coordinates - tileCenter;
				Tmpl8::vec2 clamped;
				clamped.x = Tmpl8::Clamp(diff.x, -16.0f, 16.0f);
				clamped.y = Tmpl8::Clamp(diff.y, -16.0f, 16.0f);
				Tmpl8::vec2 closest = tileCenter + clamped;
				diff = closest - coordinates;
				//printf("diff.length: %f\n", tileCenter.y);
				if (diff.length() < r)
				{
					float fix = r - diff.length();
					float fraction = fix / diff.length();
					//pcord.x = coordinates.x;
					coordinates.x -= fix * diff.x;
					//pcord.y = coordinates.y;
					coordinates.y -= fix * diff.y;
				}
			}
		}
	}
}