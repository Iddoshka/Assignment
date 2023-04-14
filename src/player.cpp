#include"player.h"
#include"game.h"


constexpr float gravity = 0.7f; // gravity acceleration const var
constexpr float air_resistance = 0.005f; // the air resistance const var 
constexpr float ver_res = 2.0f; // const var that slowls the ball on a vertical collsion
constexpr float friction = 0.01f; // friction const var on the horizontal axis
constexpr float jump_force = 7.0f; // the const var that is added to velocity on a jumping tile
constexpr float slow_force = 4.0f; // the const var that is subtracted from the velocity on a slowing tile
constexpr float speed = 0.3; // const var that is added to the horizontal velocity on key press
constexpr float MAX_SPEED = 18.0f; // the const speed amount that the velocity is clamped to
bool gravity_switch = true;


Ball::Ball(float xIn, float yIn, int rIn) // initialzing the player with its sprite and the given coordinates and radius
	:	ball_sprite(new Tmpl8::Surface("assets/ball.png"),1)
{
	coordinates.x = xIn, coordinates.y = yIn;
	r = rIn;
	pcord.x = xIn, pcord.y = yIn;
	collision = false;
}
void Ball::printBall(Tmpl8::Surface* screen) // printing the ball on the screen
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
{// checking collision between the ball and an AABB object
	Tmpl8::vec2 closest; // vec2 var that will hold the closest coordinate on the object to the ball 

	for (int i = 0; i < 2; i++)
	{
		Tmpl8::vec2 tileCenter((coll_obj.x) + coll_obj.z / 2, (coll_obj.y) + coll_obj.w / 2); // center of the AABB object
		Tmpl8::vec2 diff = coor - tileCenter; // the distance between the center of the object to the center of the ball
		Tmpl8::vec2 clamped;
		clamped.x = Tmpl8::Clamp(diff.x, -coll_obj.z / 2.0f, coll_obj.z / 2.0f);
		clamped.y = Tmpl8::Clamp(diff.y, -coll_obj.w / 2.0f, coll_obj.w / 2.0f);
		closest = tileCenter + clamped; // clamping the the distance between the center of the ball and the object center on the edge of the object

		/* 
		 * if the center of the ball is inside the object the closest point will be on the ball's center
		 * So we move the ball along the velocity vector by ball's radius so the ball's center
		 * will be outside the object's bounderies
		 */
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
}// taken from a tutorial on collision between a ball and an AABB object 
	//https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-resolution

Tmpl8::vec2 Ball::linearFunc(Tmpl8::vec2 diff)
{	// function to find the exact first contact point between the ball and an AABB object on a one axis collision
	Tmpl8::vec2 fix = { fmodf(r - abs(diff.x),r), fmodf(r - abs(diff.y),r) }; // the distance on each axis to move the ball so it will not collide with the object
	if (coordinates.x == pcord.x)
	{ // if the ball collision is only verticly
		Tmpl8::vec2 newCord = { coordinates.x, coordinates.y + fix.y * dir.y };
		return newCord; 
	}
	if (coordinates.y == pcord.y)
	{ // if the ball collision is only horizontaly
		Tmpl8::vec2 newCord = { coordinates.x + fix.y * dir.x, coordinates.y };
		return newCord;
	}
	float m = (coordinates.y - pcord.y) / (coordinates.x - pcord.x); // calculating the incline of the linear function
	float b = pcord.y - m * pcord.x;
	Tmpl8::vec2 newCord = { ((coordinates.y + (fix.y * dir.y) - b) / m) + (fix.x * dir.x), ((coordinates.x + (fix.x * dir.x)) * m + b) + (fix.y * dir.y)};
	return newCord; // using linear function between the pcord and the coordinates I find the point on the function when add the fix vector to the coordinates
}

Tmpl8::vec2 Ball::findContact(Tmpl8::vec2 diff)
{	// function to find the exact first contact point between the ball and an AABB object on a two axis collision
	
	/*
	 *	the function creates a circle with the same radius as the ball around the closest point of imapct
	 *	on the AABB object and and the closest intersection point to pcord between the ball and the velocity vector
	 *	is the coordinate where the ball first collides with the object
	 *  math taken from here https://mathworld.wolfram.com/Circle-LineIntersection.html#:~:text=In%20geometry%2C%20a%20line%20meeting,429).&text=therefore%20determines%20the%20incidence%20of,summarized%20in%20the%20following%20table
	 */
	Tmpl8::vec2 obj_cord = coordinates - diff;
	Tmpl8::vec2 cord0 = coordinates - obj_cord;
	Tmpl8::vec2 pcord0 = pcord - obj_cord;
	Tmpl8::vec2 d = { cord0.x - pcord0.x, cord0.y - pcord0.y };
	float det = pcord0.x * cord0.y - cord0.x * pcord0.y;
	float sqr_len = d.length() * d.length();
	if ((r * r) * sqr_len - (det * det) < 0 || d.x == 0 || d.y ==0)
	{ // the velocity of the ball in one or both axis is zero
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

	/*
	 *	calculating new veloctiy using reflection along the normal vector to the corner of the collision 
	 */
	Tmpl8::vec2 norm = new_cord;
	aux = (2 * norm.dot(velocity)) / pow(norm.length(),2);
	Tmpl8::vec2 new_vel = (norm * aux) - velocity;
	new_cord += obj_cord;
	pcord = { abs(new_vel.x) * dir.x, abs(new_vel.y) * dir.y };
	

	return new_cord;
}

bool Ball::checkPosX(TileMaps map, Tmpl8::vec2 coor)
{ // function that checks collision on the horizontal axis for the drive function
	coor = { coor.x + map.getXoffSet() * 32.0f, coor.y + map.getYoffSet() *32.0f};// adding the offset of each axis so it will check collision on the real map coordiantes
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
{ // function that controls the X movement of the ball through user input

	/*
	* the function accepts right or left key input from the user then 
	* checks if teh ball doesnt collide with any object in that direction
	* and if not it adds to the velocity of the ball the const speed
	*/
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
{// Verlet integration
	if (dead || won) // doesnt move the ball if the game is over or the player is dead
		return;

	/*
	* the function uses the previous position and the current position
	* using the differance of the two position it calculates the velocity.
	* I clamp it to the max speed const and using the two position calculate the direction of the ball
	* and depending if the ball is placed on the center of the map or is heading towards the edge i add
	* the newly calculated velocity to the coordinates and place the now previous coordinates at the pcord var
	*/
	if (pcord.y > coordinates.y)
		dir.y = -1;
	if (pcord.y < coordinates.y)
		dir.y = 1;
	if (pcord.x > coordinates.x) 
		dir.x = -1;
	if (pcord.x < coordinates.x)
		dir.x = 1;

	velocity.x = Tmpl8::Clamp(coordinates.x - pcord.x, -(MAX_SPEED), MAX_SPEED);
	velocity.y = Tmpl8::Clamp(coordinates.y - pcord.y,-(MAX_SPEED), MAX_SPEED);
	
	// checking if the ball is heading towards the left edge of the map or the right edge of the map
	bool edgeX = ((map.getXoffSet() == (map.getWidth()/3.0f - 25.0f) && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));
	// checking if the ball is heading towards the upper edge of the map or the bottom edge of the map
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
		{
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

	/*
	* if the ball is one tile length from the bottom of the screen and is moving down the I scroll the map
	* if the ball is one tile length from the top of the screen and is moving up I scroll the map
	* otherwise I dont scroll and add the velocity straight to the current coordinates
	*/
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

	if (velocity.x != 0) // depending on the velocity of the ball on the X axis I reload the ball image in and angle
		ball_sprite.GetSurface()->SetAngle(ball_sprite.GetSurface()->GetAngle() - 20 * (velocity.x / MAX_SPEED));

}

void Ball::mapReact(Tmpl8::Surface* screen, TileMaps& map)
{// function that resolves differant object to ball interactions

	/*
	* using the checkCollision function defined earlier this function checks for interseciton 
	* between the player and collision type tile or jumping tiles etc...
	* and it resolves the interactions by moving the ball using function I defined prior
	* or either adding or subtracting from the velocity through the pcord vector
	*/

	float holding_vel = velocity.length();
	Tmpl8::vec2 new_coor = coordinates;
	std::vector<Tmpl8::vec4> objects[5] = { map.getWin(), map.getDeaths(), map.getColliders(), map.getSlowers(), map.getJumpers()};
	Tmpl8::vec2 newCord;
	bool edgeX = ((map.getXoffSet() == (map.getWidth() / 3.0f - 25.0f) && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));
	bool edgeY = ((map.getYoffSet() == (map.getHeight() - 16.0f) && velocity.y > 0) || (map.getYoffSet() == 0.0f && velocity.y < 0));

	char object_type[5] = { 'W', 'D', 'C', 'S', 'J'};
	for (int i = 0; i < sizeof(object_type); i++) // running through all the tiles on the map
	{
		for (auto a : objects[i])
		{
			Tmpl8::vec2 pos = { new_coor.x + map.getXoffSet() * 32.0f , new_coor.y + map.getYoffSet() * 32.0f }; // adding the offset of each axis so it will check collision on the real map coordiantes
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
				if (diff.y > 0 && dir.y != 1) // changing direction and velocity direction depending on the collision
					dir.y = 1, velocity.y *= -1;
				if (diff.y < 0 && dir.y != -1)
					dir.y = -1, velocity.y *= -1;
				if (diff.x < 0 && dir.x != -1)
					dir.x = -1, velocity.x *= -1;
				if (diff.x > 0 && dir.x != 1)
					dir.x = 1, velocity.x *= -1;

				if (diff.x != 0 && diff.y != 0)
				{ // resolving corner collision
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
				{ // resolving non-corner collisions
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
				if (diff.y < 0) // resolving jump tile interactions
					pcord.y += jump_force;
				break;
			case 'S': // resolving slow tile interactions
				// checks the direction and type of collsion to slow the velocity in the right direction
				// doesn't slow top corner collisions
				if ((diff.y != 0 && diff.x == 0) || (diff.x != 0 && diff.y > 0))
					pcord.y = (dir.y == 1) ? Tmpl8::Min(coordinates.y - slow_force, pcord.y + slow_force) 
					: Tmpl8::Max(coordinates.y, pcord.y - slow_force);
				if ((diff.x != 0 && diff.y == 0) || (diff.x != 0 && diff.y > 0))
					pcord.x = (dir.x == 1) ? Tmpl8::Min(coordinates.x - 0.2f, pcord.x + slow_force) 
					: Tmpl8::Max(coordinates.x + 0.2f, pcord.x - slow_force);
				break;
			case 'D': // resolving death tile interactions
				dead = true;
				break;
			case 'W': // resolving win tile interactions
				won = true;
				break;
			default:
				break;
			}
		}
	}
	
}