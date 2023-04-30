#pragma once
#include"gun.h"
#include"SDL.h"

//#define HITBOX

namespace Tmpl8
{
	const int a_Width = 87; // gun sprite width
	const int a_Height = 78; // gun sprite height
	const int b_Width = 60; // bullet sprite width
	const int b_Height = 60; // bullet sprite height

	const unsigned int bullet_speed = 10;

	// static func that gets gun angle and returns the corrosponding bullet sprite's frame number
	static unsigned int angle_to_frame(unsigned int angle)
	{
		unsigned int frame = (unsigned int)roundf((angle % 360) / 22.5f);
		return frame;
	}

	// static func that gets frame of the bullet and returns the right angle for that frame
	static unsigned int frame_to_angle(unsigned int num_frame)
	{
		unsigned int angle = (int)(num_frame * (292.5f / 26.0f));
		return angle;
	}

	static bool elapesd_time(uint32_t str_time, double lapse)
	{
		if (lapse == 0) return true;
		uint32_t currTime = (uint32_t)SDL_GetPerformanceCounter();
		double elapsedTime = static_cast<double>(
			(currTime - str_time) / static_cast<double>(SDL_GetPerformanceFrequency())
			);
		if (fmod(elapsedTime, lapse) < 0.1)
			return true;
		else
			return false;
	}

	// function that takes a given sprite's metrics and coordinates and returns true if it is currently on screen
	static bool on_screen(TileMaps map, vec2 coordinates, int width, int height)
	{
		bool x = false;
		bool y = false;
		if ((map.getXoffSet() * TileLength) < (coordinates.x + width) && (map.getXoffSet() * TileLength + ScreenWidth) > coordinates.x)
			x = true;

		if ((map.getYoffSet() * TileLength) < (coordinates.y + height) && (map.getYoffSet() * TileLength + ScreenHeight) > coordinates.y)
			y = true;

		return (x && y);
	}

	// a static func that gets a bulltes frame and returns a vector with all the hitboxes of the bullet
	static std::vector<vec4> hitBox(unsigned int frame, vec2 coordinates)
	{
		std::vector<vec4> hit_box;

		switch (frame)
		{
		case 0: // frame towards north
			hit_box.push_back(vec4(8.45, 2.0f, 3.1f, 16.0f));
			break;
		case 1:
			hit_box.push_back(vec4(10.0f, 1.0f, 4.0f, 4.0f));
			hit_box.push_back(vec4(8.5f, 5.0f, 3.5f, 6.0f));
			hit_box.push_back(vec4(7.0f, 11.0f, 2.0f, 4.0f));
			hit_box.push_back(vec4(6.0f, 15.0f, 2.0f, 3.0f));
			break;
		case 2: // frame towards north east
			hit_box.push_back(vec4(12.0f, 3.0f, 3.0f, 4.5f));
			hit_box.push_back(vec4(9.5f, 6.0f, 2.5f, 3.0f));
			hit_box.push_back(vec4(7.5f, 9.0f, 2.0f, 3.0f));
			hit_box.push_back(vec4(5.0f, 12.0f, 2.5f, 3.0f));
			break;
		case 3:
			hit_box.push_back(vec4(13.0f, 6.5f, 4.5f, 3.0f));
			hit_box.push_back(vec4(10.0f, 8.5f, 3.0f, 2.5f));
			hit_box.push_back(vec4(9.0f, 11.0f, 3.0f, 1.0f));
			hit_box.push_back(vec4(3.5f, 12.0f, 3.5f, 1.0f));
			break;
		case 4: // frame towards east
			hit_box.push_back(vec4(2.5f, 8.8f, 15.5f, 2.95f));
			break;
		case 5:
			hit_box.push_back(vec4(3.0f, 7.5f, 2.0f, 1.0f));
			hit_box.push_back(vec4(5.0f, 8.5f, 4.0f, 1.0f));
			hit_box.push_back(vec4(9.5f, 9.5f, 3.5f, 2.5f));
			hit_box.push_back(vec4(13.0f, 10.5f, 4.0f, 3.5f));
			break;
		case 6: // frame towards south east
			hit_box.push_back(vec4(4.5f, 4.5f, 2.0f, 2.5f));
			hit_box.push_back(vec4(6.5f, 7.0f, 3.0f, 3.0f));
			hit_box.push_back(vec4(9.5f, 10.0f, 3.0f, 2.5f));
			hit_box.push_back(vec4(12.5f, 12.5f, 3.5f, 3.0f));
			break;
		case 7: 
			hit_box.push_back(vec4(6.5f, 3.0f, 1.5f, 2.0f));
			hit_box.push_back(vec4(7.5f, 5.0f, 1.5f, 4.0f));
			hit_box.push_back(vec4(9.0f, 9.0f, 3.0f, 4.0f));
			hit_box.push_back(vec4(10.5f, 13.0f, 3.5f, 5.5f));
			break;
		case 8: // frame towards south
			hit_box.push_back(vec4(8.45, 2.0f, 3.1f, 16.0f));
			break;
		case 9:
			hit_box.push_back(vec4(11.5f, 3.0f, 2.0f, 2.0f));
			hit_box.push_back(vec4(9.5f, 5.0f, 2.0f, 3.0f));
			hit_box.push_back(vec4(7.5f, 8.0f, 4.0f, 3.0f));
			hit_box.push_back(vec4(6.0f, 11.0f, 5.0f, 3.0f));
			hit_box.push_back(vec4(5.0f, 14.0f, 4.0f, 4.0f));
			break;
		case 10: // frame towards south west
			hit_box.push_back(vec4(12.0f, 4.5f, 2.5f, 2.5f));
			hit_box.push_back(vec4(9.5f, 7.0f, 2.5f, 3.0f));
			hit_box.push_back(vec4(6.5f, 10.0f, 3.0f, 2.5f));
			hit_box.push_back(vec4(3.5f, 12.5f, 3.0f, 3.5f));
			break;
		case 11:
			hit_box.push_back(vec4(14.5f, 12.5f, 2.0f, 1.0f));
			hit_box.push_back(vec4(10.5f, 8.5f, 4.0f, 1.0f));
			hit_box.push_back(vec4(7.0f, 9.5f, 3.5f, 2.5f));
			hit_box.push_back(vec4(2.5f, 10.5f, 4.5f, 3.5f));
			break;
		case 12: // frame towards west
			hit_box.push_back(vec4(1.5f, 8.8f, 15.5f, 2.95f));
			break;
		case 13:
			hit_box.push_back(vec4(2.0f, 6.0f, 4.0f, 3.0f));
			hit_box.push_back(vec4(6.0f, 8.0f, 3.0f, 2.5f));
			hit_box.push_back(vec4(9.0f, 9.0f, 3.0f, 2.5f));
			hit_box.push_back(vec4(12.0f, 11.5f, 3.5f, 1.0f));
			break;
		case 14:// frame towards north west
			hit_box.push_back(vec4(3.0f, 3.0f, 4.0f, 3.5f));
			hit_box.push_back(vec4(7.0f, 6.5f, 3.5f, 3.5f));
			hit_box.push_back(vec4(10.5f, 10.0f, 2.0f, 2.0f));
			hit_box.push_back(vec4(12.5f, 12.0f, 2.0f, 2.0f));
			break;
		case 15:
			hit_box.push_back(vec4(9.0f, 2.0f, 4.0f, 4.0f));
			hit_box.push_back(vec4(10.5f, 6.0f, 5.0f, 3.0f));
			hit_box.push_back(vec4(12.5f, 9.0f, 4.0f, 3.0f));
			hit_box.push_back(vec4(14.0f, 12.0f, 2.0f, 3.0f));
			hit_box.push_back(vec4(15.0f, 15.0f, 3.0f, 3.0f));
			break;
		default:
			break;
		}
		//moving the hitboxes to the bullets location
		for (vec4& a : hit_box)
		{ 
			a *= (b_Height / 20.0f);
			a.x += coordinates.x;
			a.y += coordinates.y;
		}

		return hit_box;
	}

	Gun::Gun(vec2 In_coordinates, unsigned int num_frame, unsigned int lapse_in) :
		Sprite(new Surface("assets/minigun.tga"), 32)
	{
		lapse = lapse_in;
		frame = num_frame;
		SetFrame(num_frame);
		coordinates = In_coordinates;
	}

	//overloads the DrawScaled function of the parent class(Sprite) so it will draw it with its current frame
	void Gun::DrawScaled(Surface* screen, TileMaps map)
	{
		int posX = int(coordinates.x - map.getXoffSet() * (float)TileLength);
		int posY = int(coordinates.y - map.getYoffSet() * (float)TileLength);
		for (int x = 0; x < a_Width; x++) for (int y = 0; y < a_Height; y++)
		{
			int u = (int)((float)x * ((float)GetWidth() / (float)a_Width));
			int v = (int)((float)y * ((float)GetHeight() / (float)a_Height));
			Pixel color = GetBuffer()[(u + frame * GetWidth()) + v * GetSurface()->GetPitch()];
			if ((posY + y) < ScreenHeight && (posX + x) < ScreenWidth && (posX + x) > 0 && (posY + y) > 0)
				if (color & 0xffffff) screen->GetBuffer()[(int)(posX + x + ((posY + y) * screen->GetPitch()))] = color;
		}
	}

	// checks if the gun is on screen, if it is it will draw itself and start trying shooting bullets
	void Gun::render(Surface* screen, TileMaps map, Ball& player, uint32_t str_time)
	{
		if (!on_screen(map, coordinates, a_Width, a_Height))
			return;
		
		shoot(player, screen, map, str_time);
		DrawScaled(screen, map);
	}
	
	/*
	* if the gun fired a bullet already it will calculate its next trajectory and if after calculations
	* its outside of its bounds it will destroy itself.
	* if no bullet was fired it will check if the lapse time is correct to the gun object and if so it will
	* create a bullet object
	*/
	void Gun::shoot(Ball& player, Surface* screen, TileMaps map, uint32_t str_time)
	{
		if (fired)
		{
			fired = casing->fly(player, screen, map);
			if (!fired)
			{
				delete casing;
				casing = NULL;
			}
		}
		else
		{
			if (elapesd_time(str_time, lapse))
			{
				casing = new Bullet(vec2(coordinates.x + a_Width/2.0f - b_Width/2.0f, coordinates.y), frame);
				fired = true;
			}
		}
	}

	//destroys the bullet object of the gun object and resets the fired mode to false
	void Gun::reset()
	{
		if (fired)
		{
			delete casing;
			casing = NULL;
			fired = false;
		}
	}

	Bullet::Bullet(vec2 In_coordinates, unsigned int num_frame) :
		Sprite(new Surface("assets/phaser.tga"), 16)
	{
		angle = frame_to_angle(num_frame);
		coordinates += { In_coordinates.x + b_Height * sin((angle * PI) / 180.0f), In_coordinates.y - b_Height * cosf((angle * PI) / 180.0f) };
		str_cord = coordinates;
		frame = angle_to_frame(angle);
		SetFrame(frame);
		hit_boxes = hitBox(frame, coordinates);
	}

	/*
	* function that calculates the balls trajectory by the const bulle_speed,
	* it checks that the bullet is on screen before making any calculations,
	* it will return false if the bullet is outside of its bounds or if it has hit the player
	*/
	bool Bullet::fly(Ball& player, Surface* screen, TileMaps map)
	{
		if(!on_screen(map,coordinates,b_Width,b_Height))
			return false;
		vec2 vel = { sin((angle * PI) / 180.0f) * bullet_speed , cosf((angle * PI) / 180.0f) * bullet_speed };
		vel.x = (abs(vel.x) < 0.01) ? 0 : vel.x;
		vel.y = (abs(vel.y) < 0.01) ? 0 : -vel.y;
		coordinates += vel;
		if ((hit_boxes.back().y + hit_boxes.back().w) > ScreenHeight * (int(str_cord.y / ScreenHeight) + 1) || coordinates.y < (0 + ScreenHeight * (int(str_cord.y / ScreenHeight))) ||
			coordinates.x < 0 || coordinates.x + b_Width >(map.getWidth() / 3.0f) * 32.0f)
			return false;
		DrawScaled(screen, map);
		for (vec4& a : hit_boxes)
		{
			a.x += vel.x;
			a.y += vel.y;

			if (player.checkCollision(a, player.getCrd() + vec2(map.getXoffSet() * 32.0f, map.getYoffSet() * 32.0f)).length() < player.getR())
			{
				player.died();
				return false;
			}
		}

#ifdef HITBOX
		for (const vec4& a : hit_boxes) //hit box printer
			screen->Box((int)(a.x - map.getXoffSet() * TileLength), 
						(int)(a.y - map.getYoffSet() * TileLength), 
						(int)((a.x - map.getXoffSet() * TileLength) + a.z),
						(int)((a.y - map.getYoffSet() * TileLength) + a.w),
						GreenMask);
#endif // HITBOX

		return true;
	}

	//overloads the DrawScaled function of the parent class(Sprite) so it will draw it with its current frame
	void Bullet::DrawScaled(Surface* screen, TileMaps map)
	{
		int posX = int(coordinates.x - map.getXoffSet() * (float)TileLength);
		int posY = int(coordinates.y - map.getYoffSet() * (float)TileLength);
		for (int x = 0; x < b_Width; x++) for (int y = 0; y < b_Height; y++)
		{
			int u = (int)((float)x * ((float)GetWidth() / (float)b_Width));
			int v = (int)((float)y * ((float)GetHeight() / (float)b_Height));
			Pixel color = GetBuffer()[(u + frame * GetWidth()) + v * GetSurface()->GetPitch()];
			if ((posY + y) < ScreenHeight && (posX + x) < ScreenWidth && (posX + x) > 0 && (posY + y) > 0)
				if (color & 0xffffff) screen->GetBuffer()[(int)(posX + x + ((posY + y) * screen->GetPitch()))] = color;
		}
	}
}

