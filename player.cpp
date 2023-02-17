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
	velocity.x = coordinates.x - pcord.x , velocity.y = coordinates.y - pcord.y ;
	//velocity += acceleration;
	// store previous position.
	pcord.x = coordinates.x, pcord.y = coordinates.y;
	// Verlet integration
	coordinates.x += velocity.x;
	coordinates.y += velocity.y;

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
		diff = -closest + coordinates;
		//printf("diff.length: %f\n", tileCenter.y);
		if (diff.length() <= r)
		{
			Tmpl8::vec2 dir;
			Tmpl8::vec2 fix(r - abs(Tmpl8::Min(diff.x,0.0f)),r - abs(diff.y));
			if (closest.y > coordinates.y)
				dir.y = 1;
			else
				dir.y = -1;
			coordinates.x -= fmodf(fix.x, r);
			coordinates.y -= fmodf((fix.y * dir.y), r);
			pcord = coordinates;
			printf("%f\n", fmodf(fix.y, r));
			pcord.x -= diff.x + velocity.x;
			pcord.y -= Tmpl8::Clamp(diff.y,-velocity.y * dir.y,velocity.y * dir.y);
		}
	}
	//constraints
	if (coordinates.y > ScreenHeight - (r + 1)) // checking for bottom of the screen collision
	{
		pcord.y = coordinates.y, coordinates.y = ScreenHeight - (r + 1);
	}
	if (coordinates.x > ScreenWidth - (r + 1)) // checking for right side of the screen collision
	{
		coordinates.x = ScreenWidth - (r + 1), pcord.x = coordinates.x + velocity.x;
	}
	if (coordinates.x < (r + 1)) // checking for left side of the screen collision
	{
		coordinates.x = (r + 1), pcord.x = coordinates.x + velocity.x;
	}
}