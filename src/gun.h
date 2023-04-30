#pragma once
#include"template.h"
#include"surface.h"
#include"tilemap.h"
#include"player.h"
#include"bullet.h"

namespace Tmpl8
{

	class Gun : public Sprite
	{
	public:
		Gun(vec2 In_coordinates, unsigned int num_frame , unsigned int lapse_in = 0);
		
		void reset();
		void render(Surface* screen, TileMaps map, Ball& player, uint32_t str_time);
	private:

		vec2 coordinates;
		unsigned int frame;
		double lapse; // the minimum time period between each bullet
		Bullet* casing; // the bullet object of the gun
		bool fired = false; // bool variable that tells if a bullet was fired or not
		void shoot(Ball& player, Surface* screen, TileMaps map, uint32_t str_time);
		void DrawScaled(Surface* screen, TileMaps map);
	};


	
}