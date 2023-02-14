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
	velocity += acceleration;
	// store previous position.
	pcord.x = coordinates.x, pcord.y = coordinates.y;
	// Verlet integration
	coordinates.x += velocity.x;
	coordinates.y += velocity.y;
	if(!collision) 
		coordinates.y += gravity;
}

void Ball::mapReact(Tmpl8::Surface* screen, TileMaps map)
{
	for (auto a : map.getColliders())
	{
		Tmpl8::vec2 tileCenter((a.x) + a.z/ 2, (a.y) + a.w / 2);
		Tmpl8::vec2 diff = coordinates - tileCenter;
		Tmpl8::vec2 clamped;
		clamped.x = Tmpl8::Clamp(diff.x, -a.z / 2.0f, a.z / 2.0f);
		clamped.y = Tmpl8::Clamp(diff.y, -a.w / 2.0f, a.w / 2.0f);
		Tmpl8::vec2 closest = tileCenter + clamped;
		diff = closest - coordinates;
		//printf("diff.length: %f\n", tileCenter.y);
		if (diff.length() <= r)
		{
			Tmpl8::vec2 fix = Tmpl8::vec2(r) - diff;
			coordinates.x -= fmod(fix.x, r);
			coordinates.y -= fmod(fix.y, r);
			pcord = coordinates + diff;
		}
	}
	//constraints
	if (coordinates.y > ScreenHeight - (r + 1)) // checking for bottom of the screen collision
	{
		pcord.y = coordinates.y, coordinates.y = ScreenHeight - (r + 1);
	}
}