#include"player.h"
#include"game.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

constexpr float gravity = 0.7f;
constexpr float air_resistance = 0.005f;
constexpr float ver_res = 2.0f;
constexpr float friction = 0.01f;
constexpr float jump_force = 10.0f;
constexpr float speed = 0.2;
bool stop = false;

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

Tmpl8::vec2 Ball::findContact(Tmpl8::vec2 diff)
{
	Tmpl8::vec2 obj_cord = coordinates - diff;
	Tmpl8::vec2 cord0 = coordinates - obj_cord;
	Tmpl8::vec2 pcord0 = pcord - obj_cord;
	Tmpl8::vec2 d = { cord0.x - pcord0.x, cord0.y - pcord0.y };
	float det = pcord0.x * cord0.y - cord0.x * pcord0.y;
	float sqr_len = d.length() * d.length();
	if ((r * r) * sqr_len - (det * det) < 0)
	{
		printf("no intersection!");
		return cord0;
	}
	float aux = sqrtf((r * r) *  sqr_len - (det * det));
	Tmpl8::vec2 new_cord1 = { (det * d.y + (d.y / abs(d.y)) * d.x * aux) / sqr_len,(-det * d.x + abs(d.y) * aux) / sqr_len };
	Tmpl8::vec2 new_cord2 = { (det * d.y - (d.y / abs(d.y)) * d.x * aux) / sqr_len, (-det * d.x - abs(d.y) * aux) / sqr_len };
	float dist1 = pow(new_cord1.x - pcord0.x, 2) + pow(new_cord1.y - pcord0.y, 2);
	float dist2 = pow(new_cord2.x - pcord0.x, 2) + pow(new_cord2.y - pcord0.y, 2);
	Tmpl8::vec2 new_cord = (dist1 > dist2) ? new_cord2 : new_cord1;

	//calculating nre veloctiy
	Tmpl8::vec2 norm = new_cord;
	aux = (2 * norm.dot(velocity)) / pow(norm.length(),2);
	Tmpl8::vec2 new_vel = (norm * aux) - velocity;
	new_cord += obj_cord;
	pcord = (new_cord) - new_vel;
	return new_cord;
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
	if (GetAsyncKeyState(VK_LEFT))
	{
		newPos.x -= speed;
		if (checkPosX(map , newPos))
			return;
		if (pcord.x < coordinates.x)
			pcord.x += speed * 2;
		else
			pcord.x += speed;
		dir.x = (pcord.x > coordinates.x) ? -1 : dir.x;

	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		newPos.x += speed;
		if (checkPosX(map, newPos))
			return;
		if (pcord.x > coordinates.x)
			pcord.x -= speed * 2;
		else
			pcord.x -= speed;
		dir.x = (pcord.x < coordinates.x) ? 1 : dir.x;
	}
	
	
}

void Ball::verlet(TileMaps &map)
{
	if (stop)
		return;
	if (pcord.y > coordinates.y)
		dir.y = -1;
	if (pcord.y < coordinates.y)
		dir.y = 1;

	velocity.x = Tmpl8::Clamp(coordinates.x - pcord.x,-(18.0f),18.0f) ,velocity.y = Tmpl8::Clamp(coordinates.y - pcord.y,-(18.0f), 18.0f);
	
	//velocity += acceleration;
	// store previous position.
	// Verlet integration
	// checking if the ball is heading towards the left edge of the map or the right edge of the map, if so than canceling map scrolling
	bool edgeX = ((map.getXoffSet() == (map.getWidth()/3.0f - 25.0f) && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));
	bool edgeY = ((map.getYoffSet() == (map.getHeight() -  16.0f) && velocity.y > 0) || (map.getYoffSet() == 0.0f && velocity.y < 0));
	if (coordinates.x != ScreenWidth / 2 && !edgeX)
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
	{
		if (edgeX)
		{
			pcord.x = coordinates.x;
			coordinates.x += velocity.x;
		}
		else
		{
			map.setXoffSet(map.getXoffSet() + velocity.x / 32.0f);
		}
	}
	if ((coordinates.y != (32.0f + r) && coordinates.y != ScreenHeight - (32.0f + r))  && !edgeY)
	{
		if (((coordinates.y > ScreenHeight - (32.0f + r)) && (pcord.y <= ScreenHeight - (32.0f + r))) && velocity.y > 0)
		{ //if the ball has passed the center of the screen than I place him in the center and add the remaining velocity to the horizontal offset
			map.setYoffSet(map.getYoffSet() + ((ScreenHeight - (32.0f + r)) - coordinates.y) / 32.0f);
			coordinates.y = ScreenHeight - (32.0f + r);
			pcord.y = coordinates.y - velocity.y;
		}
		else if (((coordinates.y < (32.0f + r)) && (pcord.y >= (32.0f + r))) && velocity.y < 0)
		{
			map.setYoffSet(map.getYoffSet() - (32.0f - coordinates.y) / 32.0f);
			coordinates.y = (32.0f + r);
			pcord.y = coordinates.y - velocity.y;
		}
		else
		{
			pcord.y = coordinates.y;
			coordinates.y += velocity.y;
		}
	}
	else
	{
		if (edgeY || (coordinates.y == (32.0f + r) && velocity.y > 0) || (coordinates.y == ScreenHeight - (32.0f + r) && velocity.y < 0))
		{
			pcord.y = coordinates.y;
			coordinates.y += velocity.y;
		}
		else
		{
			map.setYoffSet(map.getYoffSet() + velocity.y / 32.0f);
		}
	}
	//adding friction to the resistance force if the ball is touching any object
	float resistance = (collision) ? air_resistance + (friction * abs(velocity.x)): air_resistance;
	if (gravity_switch)// adding gravity to the vertical velocity
		if (edgeY || (coordinates.y > (32.0f + r) && coordinates.y < ScreenHeight - (32.0f + r)))
			coordinates.y += gravity;
		else
			map.setYoffSet(map.getYoffSet() + gravity / 32.0f);
	else // adding resistance to the vertical velocity
	{
		if (velocity.y > 0)
			pcord.y += ver_res;
		else if (velocity.y < 0)
			pcord.y -= ver_res;
		//recalculating the vertical velocity
		velocity.y = Tmpl8::Clamp(coordinates.y - pcord.y, -(18.0f), 18.0f);
	}

	gravity_switch = true;

	//adding friction and air resistance to the horizontal velocity 
	if (pcord.x < coordinates.x)
		pcord.x += resistance;
	if (pcord.x > coordinates.x)
		pcord.x -= resistance;

	//recalculating the horizontal velocity
	velocity.x = Tmpl8::Clamp(coordinates.x - pcord.x, -(18.0f), 18.0f);

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
	Tmpl8::vec2 new_coor = coordinates;
	std::vector<Tmpl8::vec4> objects[2] = {map.getColliders(), map.getJumpers() };
	Tmpl8::vec2 newCord;
	bool edgeX = ((map.getXoffSet() == (map.getWidth() / 3.0f - 25.0f) && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));
	bool edgeY = ((map.getYoffSet() == (map.getHeight() - 16.0f) && velocity.y > 0) || (map.getYoffSet() == 0.0f && velocity.y < 0));

	char object_type[2] = { 'C','J' };
	for (int i = 0; i < sizeof(object_type); i++)
	{
		for (auto a : objects[i])
		{
			Tmpl8::vec2 pos = { new_coor.x + map.getXoffSet() * 32.0f , new_coor.y + map.getYoffSet() * 32.0f };
			Tmpl8::vec2 diff = checkCollision(a, pos);
			if (abs(diff.x) < 0.001)
				diff.x = 0;
			if (abs(diff.y) < 0.001)
				diff.y = 0;
			if (diff.length() > r)
			{
				continue;
			}
			switch (object_type[i])
			{
			case 'C':
				if (diff.y > 0)
					dir.y = 1, velocity.y *= -1;
				if (diff.y < 0)
					dir.y = -1, velocity.y *= -1;
				if (diff.x < 0)
					dir.x = -1, velocity.x *= -1;
				if (diff.x > 0)
					dir.x = 1, velocity.x *= -1;

				if (diff.x != 0 && diff.y != 0)
				{
					newCord = findContact(diff);
					coordinates = newCord;
					//coordinates = { coordinates.x + abs(diff.x) * dir.x,coordinates.y + abs(diff.y) * dir.y };
					//pcord = { coordinates.x + velocity.x * dir.x, coordinates.y + velocity.y * dir.y };
				}
				else
				{
					newCord = linearFunc(diff);
					printf("new coordinate: (%f, %f)\n", newCord.x, newCord.y);
					Tmpl8::vec2 dist(coordinates - newCord);
					//if the resistnace is greater than the velocity then the velocity is equivelant to zero
					if (edgeX || coordinates.x != ScreenWidth/2)
						coordinates.x = newCord.x + (abs(dist.x) * dir.x);
					else
						map.setXoffSet(map.getXoffSet() + (abs(dist.x) * dir.x) / 32.0f);
					if (abs(velocity.y) <= ver_res)
					{
						coordinates.y = newCord.y;
						velocity.y = 0;
					}
					else if (edgeY || (coordinates.y > (32.0f + r) && coordinates.y < ScreenHeight - (32.0f + r)))
						coordinates.y = newCord.y + (abs(dist.y) * dir.y);
					else
						map.setYoffSet(map.getYoffSet() + (abs(dist.y) * dir.y) / 32.0f);
					pcord = { coordinates.x - (abs(velocity.x) * dir.x) ,coordinates.y - velocity.y };
					printf("coordinates: (%f, %f), Pcord: (%f, %f)\n", coordinates.x, coordinates.y, pcord.x, pcord.y);
				}
				collision = true;
				// checking if there was a bottom collision so can cancel the gravity with normal force
				if (diff.y != 0 && dir.y == -1)
					gravity_switch = false;
				break;
			case 'J':
				if (diff.x == 0)
					pcord.y += jump_force;
				break;
			default:
				break;
			}
		}
	}
}