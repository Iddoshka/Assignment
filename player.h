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
	Tmpl8::Sprite ball_sprite; //the sprite will be of the ping ball from the assets folder
	Tmpl8::vec2 coordinates; //for the players coordinates
	Tmpl8::vec2 dir = 1; //players direction, 1 is towards the max width or height, -1 is towards the zero value of the axis
	bool jumpAble; //an indicator for if the player can jump at a certian point or not
	Tmpl8::vec2 pcord; //here will be saved the previous coordinates(px and py)
	Tmpl8::vec2 velocity = 0.0f;
	int r; // the balls radius
	bool collision;
	bool gravity_switch = true;
	double angle = 0;
	bool dead = false;

	bool checkPosX(TileMaps map, Tmpl8::vec2 coor);
	Tmpl8::vec2 checkCollision(Tmpl8::vec4 coll_obj, Tmpl8::vec2 coor);
	Tmpl8::vec2 linearFunc(Tmpl8::vec2 diff);
	Tmpl8::vec2 findContact(Tmpl8::vec2 diff);
public:
	Ball(float xIn, float yIn, int rIn);
	void setX(float xIn) { coordinates.x = xIn; } // to change the x coordinate
	void setY(float yIn) { coordinates.y = yIn; } // to change the y coordinate
	void setPX(float pxIn) { pcord.x = pxIn; } // to save the last x coordinate
	void setPY(float pyIn) { pcord.y = pyIn; } // to save the last y coordinate
	void SetAngle(double new_angle) { angle = new_angle, ball_sprite.GetSurface()->SetAngle(new_angle); }
	void enableJump() { jumpAble = true; } // to enable the jump bool
	void diableJump() { jumpAble = false; } // to disable the jump bool
	bool getJump() { return jumpAble; }
	double GetAngle() { return angle; }
	Tmpl8::vec2 getCrd() { return coordinates; } // returns the coordinate vector
	Tmpl8::vec2 getPcord() { return pcord; } // returns the velocity vector
	Tmpl8::vec2 getVel() { return velocity; }
	void Drive(TileMaps &map);
	int getRadius() { return r; } // return the radius variable
	void mapReact(Tmpl8::Surface* screen, TileMaps &map); // for any reaction with the tilemap tiles
	void printBall(Tmpl8::Surface* screen); // to print the body of the ball and if its able to jump the a green outline
	void verlet(TileMaps &map); //the verlet integration for the ball
	void died() { dead = true; }
	bool isdead() { return dead; }
};