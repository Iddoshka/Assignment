#include"player.h"
#include"game.h"

constexpr float gravity = 0.0f;//0.5f;

Ball::Ball(float xIn, float yIn, int rIn) // initialzing the player with its sprite and the given coordinates and radius
	:	ball_sprite(new Tmpl8::Surface("assets/ball.png"),1)
{
	coordinates.x = xIn, coordinates.y = yIn;
	r = rIn;
	pcord.x = xIn - 4, pcord.y = yIn - 14;
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

Tmpl8::vec2 Ball::checkCollision(Tmpl8::vec4 coll_obj)
{
	Tmpl8::vec2 tileCenter((coll_obj.x) + coll_obj.z / 2, (coll_obj.y) + coll_obj.w / 2);
	Tmpl8::vec2 diff = coordinates - tileCenter;
	Tmpl8::vec2 clamped;
	clamped.x = Tmpl8::Clamp(diff.x, -coll_obj.z / 2.0f, coll_obj.z / 2.0f);
	clamped.y = Tmpl8::Clamp(diff.y, -coll_obj.w / 2.0f, coll_obj.w / 2.0f);
	Tmpl8::vec2 closest = tileCenter + clamped;
	/*if (closest.x == coordinates.x && closest.y == coordinates.y)
	{
		closest = Tmpl8::vectorRectangleIntersection(pcord, coordinates, coll_obj);
	}*/
	return (coordinates - closest);
}

Tmpl8::vec2 Ball::linearFunc(Tmpl8::vec2 diff)
{
	Tmpl8::vec2 fix = { fmodf(r - abs(diff.x),r), fmodf(r - abs(diff.y),r) };
	float m = (coordinates.y - pcord.y) / (coordinates.x - pcord.x);
	float b = pcord.y - m * pcord.x;
	Tmpl8::vec2 newCord = { ((coordinates.y + (fix.y * dir.y) - b) / m) + (fix.x * dir.x), ((coordinates.x + (fix.x * dir.x)) * m + b) + (fix.y * dir.y)};
	return newCord;
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
	if (coordinates.y > ScreenHeight - (r + 1)) // checking for bottom of the screen collision
	{
		pcord.y = coordinates.y, coordinates.y = ScreenHeight - (r + 1);
		dir.y = -1;
	}
	if (coordinates.x > ScreenWidth - (r + 1)) // checking for right side of the screen collision
	{
		coordinates.x = ScreenWidth - (r + 1), pcord.x = coordinates.x + velocity.x;
		dir.x = -1;
	}
	if (coordinates.x < (r + 1)) // checking for left side of the screen collision
	{
		coordinates.x = (r + 1), pcord.x = coordinates.x + velocity.x;
		dir.x = 1;
	}
}

void Ball::mapReact(Tmpl8::Surface* screen, TileMaps map)
{
	for (auto a : map.getColliders())
	{
		Tmpl8::vec2 diff = checkCollision(a);
		if (diff.length() > r)
		{
			continue;
		}
		if (diff.y > 0)
			dir.y = 1, velocity.y *= -1;
		if (diff.y < 0)
			dir.y = -1, velocity.y *= -1;
		if (diff.x < 0)
			dir.x = -1,velocity.x *= -1;
		if (diff.x > 0)
			dir.x = 1, velocity.x *= -1;
		if (diff.x < 0)
			printf("stop!");
		Tmpl8::vec2 newCord(linearFunc(diff));
		printf("new coordinate: (%f, %f)\n", newCord.x, newCord.y);
		Tmpl8::vec2 dist(coordinates - newCord);
		coordinates = { newCord.x + (abs(dist.x) * dir.x), newCord.y + (abs(dist.y) * dir.y)};
		pcord = { coordinates.x - velocity.x,coordinates.y - velocity.y };
		printf("distance to fix: %f\n", dist.length());
		//pcord = coordinates;
	}
	//constraints
	
}