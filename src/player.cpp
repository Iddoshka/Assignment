#include"player.h"
#include"game.h"

constexpr float gravity = 0.7f; // gravity acceleration const var
constexpr float air_resistance = 0.005f; // the air resistance const var 
constexpr float ver_res = 2.0f; // const var that slowls the ball on a vertical collsion
constexpr float friction = 0.01f; // friction const var on the horizontal axis
constexpr float jump_force = 7.0f; // the const var that is added to velocity on a jumping tile
constexpr float slow_force = 4.0f; // the const var that is subtracted from the velocity on a slowing tile
constexpr float speed = 0.3; // const var that is added to the horizontal velocity on key press
constexpr float MAX_SPEED_Y = 18.0f; // the const speed amount that the velocity Y is clamped to
constexpr float MAX_SPEED_X = 15.0f; // the const speed amount that the velocity X is clamped to
bool gravity_switch = true;

// initialzing the player with its sprite and the given coordinates and radius
Ball::Ball(float xIn, float yIn, int rIn) 
	:	ball_sprite(new Tmpl8::Surface("assets/ball.png"),4)
{
	coordinates.x = xIn, coordinates.y = yIn;
	r = rIn;
	pcord.x = xIn, pcord.y = yIn;
	collision = false;
}

// printing the ball on the screen
void Ball::printBall(Tmpl8::Surface* screen) 
{
	ball_sprite.SetFrame(roundf((float)((int)this->ball_sprite.GetSurface()->GetAngle() % 360) / 90.0f));
	ball_sprite.DrawScaled((int)coordinates.x - r, (int)coordinates.y - r, r * 2, r * 2, screen);
	for (int i = 0; i < 64; i++)
	{
		float r1 = (float)i * Tmpl8::PI / TileLength, r2 = (float)(i + 1) * Tmpl8::PI / TileLength;
		screen->Line(coordinates.x - r * sinf(r1), coordinates.y - r * cosf(r1),
			coordinates.x - r * sinf(r2), coordinates.y - r * cosf(r2), 0x84898b); // silver color
	}// printing circumference of the ball
}

// checking collision between the ball and an AABB object and returns the distance between the center of the ball to the edge of the object
// taken from a tutorial on collision between a ball and an AABB object https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-resolution
Tmpl8::vec2 Ball::checkCollision(Tmpl8::vec4 coll_obj, Tmpl8::vec2 coor)
{
	Tmpl8::vec2 closest; // Tmpl8::vec2 var that will hold the closest coordinate on the object to the ball 

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
}

// function to find the exact first contact point between the ball and an AABB object on a one axis collision
// receives the distance from the ball to the edge of the object of collision and retruns the corrected position
Tmpl8::vec2 Ball::linearFunc(Tmpl8::vec2 diff)
{	
	Tmpl8::vec2 fix = { fmodf((float)r - abs(diff.x),(float)r), fmodf((float)r - abs(diff.y),(float)r) }; // the distance on each axis to move the ball so it will not collide with the object
	if (fix.y > r) {
		printf("stop");
	}
	if (diff.x == 0)
	{ // if the ball collision is only verticly
		Tmpl8::vec2 newCord = { coordinates.x, coordinates.y + fix.y * dir.y };
		return newCord; 
	}
	if (diff.y == 0)
	{ // if the ball collision is only horizontaly
		Tmpl8::vec2 newCord = { coordinates.x + fix.x * dir.x, coordinates.y };
		return newCord;
	}
	float m = (coordinates.y - pcord.y) / (coordinates.x - pcord.x); // calculating the incline of the linear function
	float b = pcord.y - m * pcord.x;
	Tmpl8::vec2 newCord = { ((coordinates.y + (fix.y * dir.y) - b) / m) + (fix.x * dir.x), ((coordinates.x + (fix.x * dir.x)) * m + b) + (fix.y * dir.y)};
	if (abs(coordinates.y - newCord.y) > 80) {
		printf("stop");
	}
	return newCord; // using linear function between the pcord and the coordinates I find the point on the function when add the fix vector to the coordinates
}
// function to find the exact first contact point between the ball and an AABB object on a two axis collision
// receives the distance from the ball to the edge of the object of collision and retruns the corrected position
Tmpl8::vec2 Ball::findContact(Tmpl8::vec2 diff)
{	
	
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
		pcord = (abs(velocity.y) <= ver_res) ? Tmpl8::vec2(velocity.x,0) : velocity;
		return coordinates + fix;
	}
	float aux = sqrtf((r * r) *  sqr_len - (det * det));
	Tmpl8::vec2 new_cord1 = { (det * d.y + (d.y / abs(d.y)) * d.x * aux) / sqr_len,(-det * d.x + abs(d.y) * aux) / sqr_len };
	Tmpl8::vec2 new_cord2 = { (det * d.y - (d.y / abs(d.y)) * d.x * aux) / sqr_len, (-det * d.x - abs(d.y) * aux) / sqr_len };
	float dist1 = (float)(pow(new_cord1.x - pcord0.x, 2) + pow(new_cord1.y - pcord0.y, 2));
	float dist2 = (float)(pow(new_cord2.x - pcord0.x, 2) + pow(new_cord2.y - pcord0.y, 2));
	Tmpl8::vec2 new_cord = (dist1 > dist2) ? new_cord2 : new_cord1;

	/*
		*	calculating new veloctiy using reflection along the normal vector to the corner of the collision 
		*/
	Tmpl8::vec2 norm = new_cord;
	aux = (2 * norm.dot(velocity)) / (float)pow(norm.length(),2);
	Tmpl8::vec2 new_vel = (norm * aux) - velocity;
	new_vel.y = (abs(new_vel.y) <= ver_res) ? 0 : new_vel.y;
	new_cord += obj_cord;
	pcord = { abs(new_vel.x) * dir.x, abs(new_vel.y) * dir.y };
	

	return new_cord;
}

// function that checks collision on the horizontal axis for the drive function
// receives the tilemap calss object and coordinates of the ball as parameters and returns true if finds collision
bool Ball::checkCollisionX(TileMaps map, Tmpl8::vec2 coor)
{ // adding the offset of each axis so it will check collision on the real map coordiantes
	coor = { coor.x + map.getXoffSet() * TileLength, coor.y + map.getYoffSet() *TileLength};
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

// function that controls the X movement of the ball through user input
void Ball::Drive(TileMaps &map)
{ 

	/*
	* the function accepts right or left key input from the user then 
	* checks if teh ball doesnt collide with any object in that direction
	* and if not it adds to the velocity of the ball the const speed
	*/
	Tmpl8::vec2 newPos = coordinates;
	if (GetAsyncKeyState(VK_LEFT))
	{
		newPos.x -= speed;
		if (checkCollisionX(map , newPos))
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
		if (checkCollisionX(map, newPos))
			return;
		if (pcord.x > coordinates.x)
			pcord.x -= speed * 2;
		else
			pcord.x -= speed;
		dir.x = (pcord.x < coordinates.x) ? 1 : dir.x;
	}
	
	
}

void Ball::positioningX(TileMaps &map)
{
	/*
	* if the ball is placed in the middle of the screen and not heading towards the edge the function scrolls
	* otherwise doesn't scroll and add the velocity straight to the current coordinates
	*/
	// checking if the ball is heading towards the left edge of the map or the right edge of the map
	bool edgeX = ((map.getXoffSet() == (map.getWidth() / 3.0f - ScreenWidth	/ TileLength) && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));

	if (coordinates.x != ScreenWidth / 2 && !edgeX)
	{
		if (((coordinates.x > ScreenWidth / 2) && (pcord.x <= ScreenWidth / 2)) || ((coordinates.x < ScreenWidth / 2) && (pcord.x >= ScreenWidth / 2)))
		{ //if the ball has passed the center of the screen than I place him in the center and add the remaining velocity to the horizontal offset
			map.setXoffSet(map.getXoffSet() + ((ScreenWidth / 2 - coordinates.x) / TileLength) * dir.x);
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
			map.setXoffSet(map.getXoffSet() + velocity.x / TileLength);
		}
	}
};

void Ball::positioningY(TileMaps& map, bool edgeY)
{
	/*
	* if the ball is one tile length from the bottom of the screen and is moving down I scroll the map
	* if the ball is one tile length from the top of the screen and is moving up I scroll the map
	* otherwise I dont scroll and add the velocity straight to the current coordinates
	*/
	if ((coordinates.y != (TileLength + r) && coordinates.y != ScreenHeight - (TileLength + r)) && !edgeY)
	{
		if (((coordinates.y > ScreenHeight - (TileLength + r)) && (pcord.y < ScreenHeight - (TileLength + r))) && velocity.y > 0)
		{ //if the ball has passed the thresholds of the screen than he is placed in the correct position and add the remaining velocity to the vertical offset
			map.setYoffSet(map.getYoffSet() + ((ScreenHeight - (TileLength + r)) - coordinates.y) / (float)TileLength);
			coordinates.y = ScreenHeight - ((float)TileLength + r);
			pcord.y = coordinates.y - velocity.y;
		}
		else if (((coordinates.y < (TileLength + r)) && (pcord.y > (TileLength + r))) && velocity.y < 0)
		{
			map.setYoffSet(map.getYoffSet() - (TileLength - coordinates.y) / TileLength);
			coordinates.y = (float)(TileLength + r);
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
		if (edgeY || (coordinates.y == (TileLength + r) && velocity.y > 0) || (coordinates.y == ScreenHeight - (TileLength + r) && velocity.y < 0))
		{
			pcord.y = coordinates.y;
			coordinates.y += velocity.y;
		}
		else
		{
			map.setYoffSet(map.getYoffSet() + velocity.y / TileLength);
		}
	}
};

//function takes the corrected coordinate and depending on position and new position changes coordinate or offset
void Ball::fixCollision(TileMaps& map, Tmpl8::vec2 newCord)
{ // need to fix collision when the fix is over the scroll line
	bool edgeX = ((map.getXoffSet() == (map.getWidth() / 3.0f - ScreenWidth / TileLength) && velocity.x > 0) || (map.getXoffSet() == 0.0f && velocity.x < 0));
	bool edgeY = ((map.getYoffSet() == (map.getHeight() - ScreenHeight / TileLength) && velocity.y > 0) || (map.getYoffSet() == 0.0f && velocity.y < 0));
	Tmpl8::vec2 dist(coordinates - newCord);
	if(!edgeX && coordinates.x != ScreenWidth / 2)
	{
		if (coordinates.x < ScreenWidth / 2 && newCord.x > ScreenWidth / 2 ||
			coordinates.x > ScreenWidth / 2 && newCord.x < ScreenWidth / 2)
		{
			dist.x -= ScreenWidth / 2 - coordinates.x;
			coordinates.x = ScreenWidth / 2.0f;
			map.setXoffSet(map.getXoffSet() + (dist.x * dir.x) / TileLength);
		}
	}
	if (edgeX || coordinates.x != ScreenWidth / 2)
		coordinates.x = newCord.x;
	else
		map.setXoffSet(map.getXoffSet() + (abs(dist.x) * dir.x) / TileLength);
	if (!edgeY && (coordinates.y != ScreenHeight - TileLength || coordinates.y != TileLength))
	{
		if (coordinates.y < ScreenHeight - TileLength && newCord.y > ScreenHeight - TileLength)
		{
			dist.y -= ScreenHeight - TileLength - coordinates.y;
			coordinates.y = ScreenHeight - TileLength;
			map.setYoffSet(map.getYoffSet() + (dist.y * dir.y) / TileLength);
		}
		else if (coordinates.y > TileLength && newCord.y < TileLength)
		{
			dist.y -= TileLength - coordinates.y;
			coordinates.y = TileLength;
			map.setYoffSet(map.getYoffSet() + (dist.y * dir.y) / TileLength);
		}
	}
	if (edgeY || (coordinates.y != (TileLength + r) && coordinates.y != ScreenHeight - (TileLength + r)))
		coordinates.y = newCord.y;
	else
		map.setYoffSet(map.getYoffSet() + (abs(dist.y) * dir.y) / TileLength);
}
void Ball::velocityCalc()
{
	velocity.x = Tmpl8::Clamp(coordinates.x - pcord.x, -(MAX_SPEED_X), MAX_SPEED_X);
	velocity.y = Tmpl8::Clamp(coordinates.y - pcord.y, -(MAX_SPEED_Y), MAX_SPEED_Y);
}
// Verlet integration
void Ball::verlet(TileMaps &map)
{
	if (dead || won) // doesnt move the ball if the game is over or the player is dead
		return;

	/*
	* the function uses the previous position and the current position
	* using the difference of the two position it calculates the velocity.
	* I Clamp it to the Max speed const and using the two positions calculate the direction of the ball
	* and depending if the ball is placed in the center of the map or is heading towards the edge I add
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

	velocityCalc();

	// checking if the ball is heading towards the upper edge of the map or the bottom edge of the map
	bool edgeY = ((map.getYoffSet() == (map.getHeight() - 16.0f) && velocity.y >= 0) || (map.getYoffSet() == 0.0f && velocity.y <= 0));

	positioningX(map);
	positioningY(map, edgeY);

	//adding friction to the resistance force if the ball is touching any object
	float resistance = (collision) ? air_resistance + (friction * abs(velocity.x)): air_resistance;
	if (gravity_switch)// adding gravity to the vertical velocity
		if (edgeY || (coordinates.y != (TileLength + r) && coordinates.y != ScreenHeight - (TileLength + r)))
			coordinates.y += gravity;
		else
			map.setYoffSet(map.getYoffSet() + gravity / TileLength);
	else // adding resistance to the vertical velocity
	{
		if (velocity.y > 0)
			pcord.y += ver_res;
		else if (velocity.y < 0)
			pcord.y -= ver_res;
	}

	gravity_switch = true;

	//adding friction and air resistance to the horizontal velocity 
	if (pcord.x < coordinates.x)
		pcord.x += resistance;
	if (pcord.x > coordinates.x)
		pcord.x -= resistance;

	//recalculating the velocity
	velocityCalc();

	//if the resistnace is greater than the velocity then the velocity is equivelant to zero
	if(abs(velocity.x) < resistance)
		pcord.x = coordinates.x , velocity.x = 0;

	//reseting the collision bool
	collision = false;

	if (coordinates.y > ScreenHeight - (r + 1)) // checking for bottom of the screen collision
	{
		pcord.y = coordinates.y, coordinates.y = (float)(ScreenHeight - (r + 1));
		dir.y = -1;
	}

	if (coordinates.x > ScreenWidth - (r + 1)) // checking for right side of the screen collision
	{
		coordinates.x = (float)(ScreenWidth - (r + 1)), pcord.x = coordinates.x + velocity.x;
		dir.x = -1;
	}

	if (coordinates.x < (r + 1)) // checking for left side of the screen collision
	{
		coordinates.x = (float)(r + 1), pcord.x = coordinates.x + velocity.x;
		dir.x = 1;
	}

	if (velocity.x != 0) // depending on the velocity of the ball on the X axis I reload the ball image in and angle

	{
		double safe_angle = ball_sprite.GetSurface()->GetAngle() + 20 * (velocity.x / MAX_SPEED_X);
		safe_angle = (safe_angle < 0) ? safe_angle + 360.0 : safe_angle;
		ball_sprite.GetSurface()->SetAngle(safe_angle);
	}

}

// function that resolves different object to ball interactions
void Ball::mapReact(Tmpl8::Surface* screen, TileMaps& map)
{
	/*
	* using the checkCollision function defined earlier this function checks for interseciton 
	* between the player and collision type tile or jumping tiles etc...
	* and it resolves the interactions by moving the ball using function defined prior
	* or either adding or subtracting from the velocity through the pcord vector
	*/

	float holding_vel = velocity.length();
	Tmpl8::vec2 new_coor = coordinates;
	const std::vector<Tmpl8::vec4> objects[5] = { map.getWin(), map.getDeaths(), map.getColliders(), map.getSlowers(), map.getJumpers()};
	Tmpl8::vec2 newCord;
	
	const char object_type[5] = { 'W', 'D', 'C', 'S', 'J'};
	for (int i = 0; i < sizeof(object_type); i++) // running through all the tiles on the map
	{
		for (const auto& a : objects[i])
		{
			Tmpl8::vec2 pos = { new_coor.x + map.getXoffSet() * TileLength , new_coor.y + map.getYoffSet() * TileLength }; // adding the offset of each axis so it will check collision on the real map coordiantes
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
					fixCollision(map, newCord);
					pcord = coordinates - pcord;
				}
				else
				{ // resolving non-corner collisions
					newCord = linearFunc(diff);
					fixCollision(map, newCord);
					//if the resistnace is greater than the velocity then the velocity is equivelant to zero
					if (abs(velocity.y) <= ver_res)
						velocity.y = 0;
					pcord = { coordinates.x - velocity.x ,coordinates.y - velocity.y };
				}
				collision = true;
				// checking if there was a bottom collision so can cancel the gravity with normal force
				if (diff.y != 0 && dir.y == -1)
					gravity_switch = false;
				break;
			case 'J':
				if ((diff.y < 0) && dir.y == -1) // resolving jump tile interactions
					pcord.y += jump_force;
				break;
			case 'S': // resolving slow tile interactions
				// checks the direction and type of collsion to slow the velocity in the right direction
				// doesn't slow top corner collisions
				velocityCalc();
				if (diff.y != 0 && (diff.x == 0) || (diff.x != 0 && !diff.y < 0))
					pcord.y = (dir.y == 1) ? Tmpl8::Min(coordinates.y - slow_force /2.0f, pcord.y + slow_force) 
					: Tmpl8::Max(coordinates.y, pcord.y - slow_force);
				if (diff.x != 0 && (diff.y == 0) || !((a.y - map.getYoffSet() * 32.0f) > coordinates.y))
					pcord.x = (dir.x == 1) ? Tmpl8::Min(coordinates.x - velocity.x/4.0f, pcord.x + slow_force) 
					: Tmpl8::Max(coordinates.x + velocity.x / 4.0f, pcord.x - slow_force);
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
