#include"player.h"
#include"game.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

constexpr float gravity = 0.5f;
constexpr float air_resistance = 0.005f;
constexpr float ver_res = 1.0f;
constexpr float friction = 0.02f;


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

Tmpl8::vec2 Ball::checkCollision(Tmpl8::vec4 coll_obj, Tmpl8::vec2 coor)
{
	Tmpl8::vec2 tileCenter((coll_obj.x) + coll_obj.z / 2, (coll_obj.y) + coll_obj.w / 2);
	Tmpl8::vec2 diff = coor - tileCenter;
	Tmpl8::vec2 clamped;
	clamped.x = Tmpl8::Clamp(diff.x, -coll_obj.z / 2.0f, coll_obj.z / 2.0f);
	clamped.y = Tmpl8::Clamp(diff.y, -coll_obj.w / 2.0f, coll_obj.w / 2.0f);
	Tmpl8::vec2 closest = tileCenter + clamped;
	/*if (closest.x == coor.x && closest.y == coor.y)
	{
		closest = Tmpl8::vectorRectangleIntersection(pcord, coor, coll_obj);
	}*/
	return (coor - closest);
}

Tmpl8::vec2 Ball::linearFunc(Tmpl8::vec2 diff)
{
	
	Tmpl8::vec2 fix = { fmodf(r - abs(diff.x),r), fmodf(r - abs(diff.y),r) };
	if (coordinates.x == pcord.x)
	{
		Tmpl8::vec2 newCord = { coordinates.x, coordinates.y + fix.y * dir.y };
		return newCord;
	}
	if (coordinates.y == pcord.y)
	{
		Tmpl8::vec2 newCord = { coordinates.x + fix.y * dir.x, coordinates.y };
		return newCord;
	}
	float m = (coordinates.y - pcord.y) / (coordinates.x - pcord.x);
	float b = pcord.y - m * pcord.x;
	Tmpl8::vec2 newCord = { ((coordinates.y + (fix.y * dir.y) - b) / m) + (fix.x * dir.x), ((coordinates.x + (fix.x * dir.x)) * m + b) + (fix.y * dir.y)};
	return newCord;
}

bool Ball::checkPosX(TileMaps map, Tmpl8::vec2 coor)
{
	for (auto a : map.getColliders())
	{
		Tmpl8::vec2 diff = checkCollision(a, coor);
		if (abs(diff.x) < 0.001)
			diff.x = 0;
		if (diff.x != 0 && abs(diff.x) < r)
		{
			return true;
		}
	}
	return false;
}
void Ball::Drive(TileMaps &map)
{
	Tmpl8::vec2 newPos = coordinates;
	float speed = 0.2;
	if (GetAsyncKeyState(VK_LEFT))
	{
		newPos.x -= speed;
		if (checkPosX(map , newPos))
			return;
		pcord.x += speed;
		dir.x = (pcord.x > coordinates.x) ? -1 : dir.x;

	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		newPos.x += speed;
		if (checkPosX(map, newPos))
			return;
		pcord.x -= speed;
		dir.x = (pcord.x < coordinates.x) ? 1 : dir.x;
	}
	
	
}

void Ball::verlet(TileMaps &map)
{
	if (pcord.y > coordinates.y)
		dir.y = -1;
	if (pcord.y < coordinates.y)
		dir.y = 1;

	velocity.x = Tmpl8::Clamp(coordinates.x - pcord.x,-(32.0f + r),32.0f + r) ,velocity.y = Tmpl8::Clamp(coordinates.y - pcord.y,-(32.0f + r), 32.0f + r);
	
	//velocity += acceleration;
	// store previous position.
	pcord.y = coordinates.y;
	// Verlet integration
	// checking if the ball is heading towards the left edge of the map or the right edge of the map, if so than canceling map scrolling
	bool edge = ((map.getXoffSet() == 25.0f && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));
	if (coordinates.x != ScreenWidth / 2 && !edge)
	{
		if (((coordinates.x > ScreenWidth / 2) && (pcord.x <= ScreenWidth / 2)) || ((coordinates.x < ScreenWidth / 2) && (pcord.x >= ScreenWidth / 2)))
		{ //if the ball has passed the center of the screen than I place him in the center and add the remaining velocity to the horizontal offset
			map.setXoffSet(map.getXoffSet() + ((ScreenWidth / 2 - coordinates.x) / 32.0f) * dir.x);
			coordinates.x = ScreenWidth / 2;
			pcord.x = coordinates.x - velocity.x;
		}
		else
		{ // if
			pcord.x = coordinates.x;
			coordinates.x += velocity.x;
		}
	}
	else
		if (edge)
		{
			pcord.x = coordinates.x;
			coordinates.x += velocity.x;
		}
		else
		{
			map.setXoffSet(map.getXoffSet() + velocity.x / 32.0f);
		}
	coordinates.y += velocity.y;
	//adding friction to the resistance force if the ball is touching any object
	float resistance = (collision) ? air_resistance + (friction * abs(velocity.x)): air_resistance;
	if (gravity_switch)
		coordinates.y += gravity; // adding gravity to the vertical velocity
	else // adding resistance to the vertical velocity
	{
		if (pcord.y < coordinates.y)
			pcord.y += ver_res;
		if (pcord.y > coordinates.y)
			pcord.y -= ver_res;
		//recalculating the vertical velocity
		velocity.y = Tmpl8::Clamp(coordinates.y - pcord.y, -(32.0f + r), 32.0f + r);
	}

	gravity_switch = true;

	//adding friction and air resistance to the horizontal velocity 
	if (pcord.x < coordinates.x)
		pcord.x += resistance;
	if (pcord.x > coordinates.x)
		pcord.x -= resistance;

	//recalculating the horizontal velocity
	velocity.x = Tmpl8::Clamp(coordinates.x - pcord.x, -(32.0f + r), 32.0f + r);

	//if the resistnace is greater than the velocity then the velocity is equivelant to zero
	if(abs(velocity.x) < resistance)
		pcord.x = coordinates.x , velocity.x = 0;

	//reseting the collision bool
	collision = false;

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

void Ball::mapReact(Tmpl8::Surface* screen, TileMaps& map)
{
	for (auto a : map.getColliders())
	{
		Tmpl8::vec2 pos = { coordinates.x + map.getXoffSet() * 32.0f , coordinates.y };
		Tmpl8::vec2 diff = checkCollision(a, pos);
		if (abs(diff.x) < 0.001)
			diff.x = 0;
		if (abs(diff.y) < 0.001)
			diff.y = 0;
		if (diff.length() > r)
		{
			continue;
		}
		bool edge = ((map.getXoffSet() == 25.0f && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0)) || coordinates.x != ScreenWidth / 2;
		if (diff.y > 0)
			dir.y = 1, velocity.y *= -1;
		if (diff.y < 0)
			dir.y = -1, velocity.y *= -1;
		if (diff.x < 0)
			dir.x = -1, velocity.x *= -1;
		if (diff.x > 0)
			dir.x = 1, velocity.x *= -1;
		Tmpl8::vec2 newCord;
		if (diff.x != 0 && diff.y != 0)
		{
			coordinates = { coordinates.x + abs(diff.y) * dir.x,coordinates.y + abs(diff.x) * dir.y };
			pcord = { coordinates.x + velocity.y * dir.x, coordinates.y + velocity.y * dir.y };
		}
		else
		{
			newCord = linearFunc(diff);
			printf("new coordinate: (%f, %f)\n", newCord.x, newCord.y);
			Tmpl8::vec2 dist(coordinates - newCord);
			//if the resistnace is greater than the velocity then the velocity is equivelant to zero
			if (abs(velocity.y) <= ver_res)
			{
				coordinates.y = newCord.y;
				velocity.y = 0;
			}
			else
				coordinates.y = newCord.y + (abs(dist.y) * dir.y);
			if (edge)
				coordinates.x = newCord.x + (abs(dist.x) * dir.x);
			else
				map.setXoffSet(map.getXoffSet() + (abs(dist.x) * dir.x)/ 32.0f);
			pcord = { coordinates.x - (abs(velocity.x) * dir.x) ,coordinates.y - velocity.y };
			printf("coordinates: (%f, %f), Pcord: (%f, %f)\n", coordinates.x, coordinates.y ,pcord.x,pcord.y);
		}
		collision = true;
		// checking if there was a bottom collision so can cancel the gravity with normal force
		if (diff.y != 0 && dir.y == -1)
			gravity_switch = false;
	}
}