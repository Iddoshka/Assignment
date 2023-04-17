#pragma once
#include"gun.h"

namespace Tmpl8
{
	class Bullet : public Sprite
	{
	public:

		Bullet(vec2 In_coordinates, unsigned int num_frame);

		bool fly(Ball& player);

		vec2 coordinates;

		~Bullet();
	private:
		unsigned int angle;
	};
}