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

Tmpl8::vec2 Ball::checkCollision(Tmpl8::vec4 coll_obj)
{
	Tmpl8::vec2 tileCenter((coll_obj.x) + coll_obj.z / 2, (coll_obj.y) + coll_obj.w / 2);
	Tmpl8::vec2 diff = coordinates - tileCenter;
	Tmpl8::vec2 clamped;
	clamped.x = Tmpl8::Clamp(diff.x, -coll_obj.z / 2.0f, coll_obj.z / 2.0f);
	clamped.y = Tmpl8::Clamp(diff.y, -coll_obj.w / 2.0f, coll_obj.w / 2.0f);
	Tmpl8::vec2 closest = tileCenter + clamped;
	if (closest.x == coordinates.x && closest.y == coordinates.y)
	{
		closest = Tmpl8::vectorRectangleIntersection(pcord, coordinates, coll_obj);
	}
	return (coordinates - closest);
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
		printf("before: %f\n\n", velocity.length());
		diff.x = diff.x < 0.0001f && diff.x > -0.0001f ? 0 : diff.x;
		diff.y = diff.y < 0.0001f && diff.y > -0.0001f ? 0 : diff.y;
		Tmpl8::vec2 fix(0);
		if (diff.y < 0)
			dir.y = 1;
		else if(diff.y > 0)
			dir.y = -1;
		if (diff.x > 0)
			dir.x = 1;
		else if(diff.x < 0)
			dir.x = -1;
		//fix.y += diff.x != 0? r - abs(Tmpl8::Min(diff.y, 0.0f)) : r - abs(diff.y);
		fix.y += r - abs(diff.y);
		fix.x += r - abs(diff.x);
		if (diff.x != 0 && diff.y > 0)
		{
			printf("hi");
		}
		if (diff.y == 0)
		{
			diff.y = (velocity.y / velocity.length()) * diff.length();
		}
		if (diff.x == 0)
		{
			diff.x = (velocity.x / velocity.length()) * diff.length();
		}
		Tmpl8::vec2 newVel;
		newVel.x = velocity.length() * (diff.x / diff.length());
		newVel.y = velocity.length() * (diff.y / diff.length());
		fix.x = (fmodf(fix.x, r) * abs(velocity.x) / velocity.length());
		fix.y = (fmodf(fix.y, r) * abs(velocity.y) / velocity.length());
		coordinates.x -= 2 * dir.x * (fix.x * (abs(newVel.x) / newVel.length()));// new to change so it will keep the fix on the same vector as the newVel vector
		coordinates.y -= 2 * dir.y * (fix.y * (abs(newVel.y) / newVel.length()));
		pcord = coordinates;
		pcord.y -= newVel.y;//the direction of the fix and the direction of the bounce velocity do not match causing the wierd staggering movement
		pcord.x -= newVel.x;
		printf("after: %f\n\n", newVel.length());
	}
	//constraints
	
}