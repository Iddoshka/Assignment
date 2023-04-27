#pragma once
#include"gun.h"

namespace Tmpl8
{
	class Bullet : public Sprite
	{
	public:

		Bullet(vec2 In_coordinates, unsigned int num_frame);

		bool fly(Ball& player, Surface* screen, TileMaps map);

		vec2 coordinates = { 0,0 };
		vec2 str_cord = coordinates;

	private:
		unsigned int angle = 0;
		unsigned int frame = 0;

		std::vector<vec4> hit_boxes;

		void DrawScaled(Surface* screen, TileMaps map);
	};
}