#include"player.h"
#include"game.h"


constexpr float gravity = 0.7f;
constexpr float air_resistance = 0.005f;
constexpr float ver_res = 2.0f;
constexpr float friction = 0.01f;
constexpr float jump_force = 7.0f;
constexpr float slow_force = 4.0f;
constexpr float speed = 0.3;
constexpr float MAX_SPEED = 18.0f;


Ball::Ball(float xIn, float yIn, int rIn) // initialzing the player with its sprite and the given coordinates and radius
	:	ball_sprite(new Tmpl8::Surface("assets/ball.png"),1)
{
	coordinates.x = xIn, coordinates.y = yIn;
	r = rIn;
	pcord.x = xIn, pcord.y = yIn;
	collision = false;
}
void Ball::printBall(Tmpl8::Surface* screen)
{
	ball_sprite.DrawScaled(coordinates.x - r, coordinates.y - r, r * 2, r * 2, screen,true);
		for (int i = 0; i < 64; i++)
		{
			float r1 = (float)i * Tmpl8::PI / 32, r2 = (float)(i + 1) * Tmpl8::PI / 32;
			screen->Line(coordinates.x - r * sinf(r1), coordinates.y - r * cosf(r1),
				coordinates.x - r * sinf(r2), coordinates.y - r * cosf(r2), 0x84898b);
		}// printing circumfrance of the ball
}

Tmpl8::vec2 Ball::checkCollision(Tmpl8::vec4 coll_obj, Tmpl8::vec2 coor)
{
	Tmpl8::vec2 closest;

	for (int i = 0; i < 2; i++)
	{
		Tmpl8::vec2 tileCenter((coll_obj.x) + coll_obj.z / 2, (coll_obj.y) + coll_obj.w / 2);
		Tmpl8::vec2 diff = coor - tileCenter;
		Tmpl8::vec2 clamped;
		clamped.x = Tmpl8::Clamp(diff.x, -coll_obj.z / 2.0f, coll_obj.z / 2.0f);
		clamped.y = Tmpl8::Clamp(diff.y, -coll_obj.w / 2.0f, coll_obj.w / 2.0f);
		closest = tileCenter + clamped;
		
		if (coor.x == closest.x && coor.y == closest.y)
		{
			float scale = r / velocity.length();
			Tmpl8::vec2 comp = velocity * -scale;
			coor += comp;
		}
		else
			break;
	}
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
	if ((r * r) * sqr_len - (det * det) < 0 || d.x == 0 || d.y ==0)
	{
		Tmpl8::vec2 fix = { 0, ((r + 1) - abs(diff.y)) * dir.y };
		pcord = velocity;
		return coordinates + fix;
	}
	float aux = sqrtf((r * r) *  sqr_len - (det * det));
	Tmpl8::vec2 new_cord1 = { (det * d.y + (d.y / abs(d.y)) * d.x * aux) / sqr_len,(-det * d.x + abs(d.y) * aux) / sqr_len };
	Tmpl8::vec2 new_cord2 = { (det * d.y - (d.y / abs(d.y)) * d.x * aux) / sqr_len, (-det * d.x - abs(d.y) * aux) / sqr_len };
	float dist1 = pow(new_cord1.x - pcord0.x, 2) + pow(new_cord1.y - pcord0.y, 2);
	float dist2 = pow(new_cord2.x - pcord0.x, 2) + pow(new_cord2.y - pcord0.y, 2);
	Tmpl8::vec2 new_cord = (dist1 > dist2) ? new_cord2 : new_cord1;

	//calculating new veloctiy
	Tmpl8::vec2 norm = new_cord;
	aux = (2 * norm.dot(velocity)) / pow(norm.length(),2);
	Tmpl8::vec2 new_vel = (norm * aux) - velocity;
	new_cord += obj_cord;
	pcord = { abs(new_vel.x) * dir.x, abs(new_vel.y) * dir.y };
	

	return new_cord;
}

bool Ball::checkPosX(TileMaps map, Tmpl8::vec2 coor)
{
	coor = { coor.x + map.getXoffSet() * 32.0f, coor.y + map.getYoffSet() *32.0f};
	for (auto a : map.getColliders())
	{
		Tmpl8::vec2 diff = checkCollision(a, coor);
		if (abs(diff.x) < 0.001)
			diff.x = 0;
		if (diff.x != 0 && abs(diff.x) < r && abs(diff.y) < 0.01)
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
	if (dead || won)
		return;
	if (pcord.y > coordinates.y)
		dir.y = -1;
	if (pcord.y < coordinates.y)
		dir.y = 1;

	velocity.x = Tmpl8::Clamp(coordinates.x - pcord.x,-(MAX_SPEED),MAX_SPEED) ,velocity.y = Tmpl8::Clamp(coordinates.y - pcord.y,-(MAX_SPEED), MAX_SPEED);
	
	// store previous position.
	// Verlet integration
	// checking if the ball is heading towards the left edge of the map or the right edge of the map, if so than canceling map scrolling
	bool edgeX = ((map.getXoffSet() == (map.getWidth()/3.0f - 25.0f) && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));
	bool edgeY = ((map.getYoffSet() == (map.getHeight() -  16.0f) && velocity.y >= 0) || (map.getYoffSet() == 0.0f && velocity.y <= 0));
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
		if (((coordinates.y > ScreenHeight - (32.0f + r)) && (pcord.y < ScreenHeight - (32.0f + r))) && velocity.y > 0)
		{ //if the ball has passed the center of the screen than I place him in the center and add the remaining velocity to the horizontal offset
			map.setYoffSet(map.getYoffSet() + ((ScreenHeight - (32.0f + r)) - coordinates.y) / 32.0f);
			coordinates.y = ScreenHeight - (32.0f + r);
			pcord.y = coordinates.y - velocity.y;
		}
		else if (((coordinates.y < (32.0f + r)) && (pcord.y > (32.0f + r))) && velocity.y < 0)
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
		if (edgeY || (coordinates.y != (32.0f + r) && coordinates.y != ScreenHeight - (32.0f + r)))
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
	collision = true;

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

	if (velocity.x != 0)
		ball_sprite.GetSurface()->SetAngle(ball_sprite.GetSurface()->GetAngle() - 20 * (velocity.x / MAX_SPEED));

}

void Ball::mapReact(Tmpl8::Surface* screen, TileMaps& map)
{
	float holding_vel = velocity.length();
	Tmpl8::vec2 new_coor = coordinates;
	std::vector<Tmpl8::vec4> objects[5] = { map.getWin(), map.getDeaths(), map.getColliders(), map.getSlowers(), map.getJumpers()};
	Tmpl8::vec2 newCord;
	bool edgeX = ((map.getXoffSet() == (map.getWidth() / 3.0f - 25.0f) && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));
	bool edgeY = ((map.getYoffSet() == (map.getHeight() - 16.0f) && velocity.y > 0) || (map.getYoffSet() == 0.0f && velocity.y < 0));

	char object_type[5] = { 'W', 'D', 'C', 'S', 'J'};
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
				if (diff.y > 0 && dir.y != 1)
					dir.y = 1, velocity.y *= -1;
				if (diff.y < 0 && dir.y != -1)
					dir.y = -1, velocity.y *= -1;
				if (diff.x < 0 && dir.x != -1)
					dir.x = -1, velocity.x *= -1;
				if (diff.x > 0 && dir.x != 1)
					dir.x = 1, velocity.x *= -1;

				if (diff.x != 0 && diff.y != 0)
				{
					newCord = findContact(diff);
					Tmpl8::vec2 dist(coordinates - newCord);
					if(edgeX || coordinates.x != ScreenWidth/2 )
						coordinates.x = newCord.x;
					else
						map.setXoffSet(map.getXoffSet() + (abs(dist.x) * dir.x) / 32.0f);
					if(edgeY || (coordinates.y > (32.0f + r) && coordinates.y < ScreenHeight - (32.0f + r)))
						coordinates.y = newCord.y;
					else
						map.setYoffSet(map.getYoffSet() + (abs(dist.y) * dir.y) / 32.0f);
					pcord = coordinates - pcord;
				}
				else
				{
					newCord = linearFunc(diff);
					Tmpl8::vec2 dist(coordinates - newCord);
					//if the resistnace is greater than the velocity then the velocity is equivelant to zero
					if (edgeX || coordinates.x != ScreenWidth/2)
						coordinates.x = newCord.x + (abs(dist.x) * dir.x);
					else
						map.setXoffSet(map.getXoffSet() + (abs(dist.x) * dir.x) / 32.0f);
					if (abs(velocity.y) <= ver_res)
						velocity.y = 0;
					if (edgeY || (coordinates.y != (32.0f + r) && coordinates.y != ScreenHeight - (32.0f + r)))
						coordinates.y = newCord.y + (abs(dist.y) * dir.y);
					else
						map.setYoffSet(map.getYoffSet() + (abs(dist.y) * dir.y) / 32.0f);
					pcord = { coordinates.x - velocity.x ,coordinates.y - velocity.y };
					
				}
				collision = true;
				// checking if there was a bottom collision so can cancel the gravity with normal force
				if (diff.y != 0 && dir.y == -1)
					gravity_switch = false;
				break;
			case 'J':
				if (diff.y < 0)
					pcord.y += jump_force;
				break;
			case 'S':
				if (diff.y != 0 && diff.x == 0)
					pcord.y += slow_force *  -dir.y;
				if (diff.x != 0 && diff.y == 0)
					pcord.x += slow_force * -dir.x;
				break;
			case 'D':
				dead = true;
				break;
			case 'W':
				won = true;
				break;
			default:
				break;
			}
		}
	}
	
}