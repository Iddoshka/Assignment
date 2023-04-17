#include"gun.h"
#include"SDL.h"
namespace Tmpl8
{
	const int a_Width = 58;
	const int a_Height = 52;
	const int b_Width = 32;
	const int b_Height = 40;

	const unsigned int bullet_speed = 10;

	static unsigned int angle_to_frame(unsigned int angle) 
	{
		unsigned int frame = (angle % 360) / 18 ;
		return frame;
	}

	static unsigned int frame_to_angle(unsigned int num_frame)
	{
		unsigned int angle = (num_frame * (360.0f / 32.0f));
		return angle;
	}

	static bool elapesd_time(uint32_t str_time, unsigned int lapse)
	{
		if(lapse == 0) return true;
		uint32_t currTime = SDL_GetPerformanceCounter();
		int elapsedTime = static_cast<int>(
			(currTime - str_time) / static_cast<int>(SDL_GetPerformanceFrequency())
			);
		if (elapsedTime % lapse == 0)
			return true;
		else
			return false;
	}

	static bool on_screen(TileMaps map, vec2 coordinates, int width, int height)
	{
		if ((map.getXoffSet() * TileLength) > (coordinates.x + width) || (map.getXoffSet() * TileLength + ScreenWidth) < coordinates.x)
			return false;

		if ((map.getYoffSet() * TileLength) > (coordinates.y + height) || (map.getYoffSet() * TileLength + ScreenHeight) < coordinates.y)
			return false;
	}

	Gun::Gun(vec2 In_coordinates, unsigned int num_frame ,unsigned int lapse_in) :
		Sprite(new Surface("assets/minigun.tga"), 32)
	{
		lapse = lapse_in;
		frame = num_frame;
		SetFrame(num_frame);
		coordinates = In_coordinates;
	}

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

	void Gun::render(Surface* screen, TileMaps map , Ball& player, uint32_t str_time)
	{
		shoot(player, screen, map, str_time);
		if (!on_screen(map, coordinates, a_Width, a_Height))
			return;
		/*if ((map.getXoffSet() * TileLength) > (coordinates.x + a_Width) || (map.getXoffSet() * TileLength + ScreenWidth) < coordinates.x)
			return;

		if ((map.getYoffSet() * TileLength) > (coordinates.y + a_Height) || (map.getYoffSet() * TileLength + ScreenHeight) < coordinates.y)
			return;*/

		DrawScaled(screen, map);
	}

	void Gun::shoot(Ball& player, Surface* screen, TileMaps map, uint32_t str_time)
	{
		if (fired)
		{
			fired = (*casing).fly(player, screen, map);
			if(!fired)
				(*casing).~Bullet();
		}
		else
		{
			if (elapesd_time(str_time , lapse))
			{
				casing = new Bullet(vec2(coordinates.x + a_Width / 2.0f - 4.0f, coordinates.y + a_Height), frame);
				fired = true;
			}
		}
	}

	void Gun::reset()
	{
		if (fired)
		{
			(*casing).~Bullet();
			fired = false;
		}
	}

	Bullet::Bullet(vec2 In_coordinates, unsigned int num_frame) :
		Sprite(new Surface("assets/phaser.tga"), 20)
	{
		coordinates = { In_coordinates.x - b_Width/2.0f , In_coordinates.y - b_Height/2.0f };
		angle = frame_to_angle(num_frame);
		frame = angle_to_frame(angle);
		SetFrame(frame);
		
	}

	bool Bullet::fly(Ball& player , Surface* screen, TileMaps map)
	{
		vec2 vel = { sin((angle * PI) /180.0f ) * bullet_speed , cosf((angle * PI) / 180.0f) * bullet_speed };
		vel.x = (abs(vel.x) < 0.01) ? 0 : vel.x;
		vel.y = (abs(vel.y) < 0.01) ? 0 : -vel.y;
		coordinates += vel;
		if (player.checkCollision(vec4(coordinates.x + 10.0f , coordinates.y , b_Width - 10.0f, b_Height), player.getCrd() + vec2(map.getXoffSet() * 32.0f, map.getYoffSet() * 32.0f)).length() < player.getR())
		{
			player.died();
			return false;
		}
		else if (coordinates.y + b_Height > ScreenHeight || coordinates.y < 0 || coordinates.x < 0 || coordinates.x + b_Width > ScreenWidth)
		{
			coordinates = str_cord;
			return false;
		}
		DrawScaled(screen, map);
		return true;
	}

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