#include"template.h"
#include"surface.h"
#include<iostream>
#include"tilemap.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma once

//this class will be for the player ball
class Ball
{
public:
	Ball(float xIn, float yIn, int rIn);
	void setX(float xIn) { coordinates.x = xIn; } // to change the x coordinate
	void setY(float yIn) { coordinates.y = yIn; } // to change the y coordinate
	void setPX(float pxIn) { pcord.x = pxIn; } // to save the last x coordinate
	void setPY(float pyIn) { pcord.y = pyIn; } // to save the last y coordinate
	void SetAngle(double new_angle) { angle = new_angle, ball_sprite.GetSurface()->SetAngle(new_angle); } // changing ball angle function
	double GetAngle() { return angle; }
	int getR() { return r; }
	void Drive(TileMaps &map); // the moving through user input function
	void mapReact(Tmpl8::Surface* screen, TileMaps &map); // for any reaction with the tilemap tiles
	void printBall(Tmpl8::Surface* screen); // to print the body of the ball and if its able to jump the a green outline
	void verlet(TileMaps &map); //the verlet integration for the ball
	void died() { dead = true; }
	void revive() { dead = false; }
	bool isdead() { return dead; }
	bool victory() { return won; }
	
	void positioningX(TileMaps &map);
	void positioningY(TileMaps& map, bool edgeY);
	void fixCollision(TileMaps& map, Tmpl8::vec2 new_cord);
	void velocityCalc(); // calculates the velocity
	Tmpl8::vec2 checkCollision(Tmpl8::vec4 coll_obj, Tmpl8::vec2 coor); // collision function between player and any AABB objects
	Tmpl8::vec2 getCrd() { return coordinates; }
private:
	Tmpl8::Sprite ball_sprite; //the sprite will be of the ping ball from the assets folder
	Tmpl8::vec2 coordinates; //for the players coordinates
	Tmpl8::vec2 dir = 1; //players direction, 1 is towards the max width or height, -1 is towards the zero value of the axis
	Tmpl8::vec2 pcord; //here will be saved the previous coordinates(px and py)
	Tmpl8::vec2 velocity = 0.0f;
	int r; // the balls radius
	double angle = 0; // ball angle
	bool won = false; // true if the ball hits a win tile
	bool collision; // collision var if the ball collides with any object
	bool dead = false; // true if the ball hits a death tile

	bool checkCollisionX(TileMaps map, Tmpl8::vec2 coor); // checking horizontal collision for the drive function
	Tmpl8::vec2 linearFunc(Tmpl8::vec2 diff); // correction function for non-corner collisions
	Tmpl8::vec2 findContact(Tmpl8::vec2 diff);// correction function for corner collisions
};