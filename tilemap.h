#include "surface.h"
#include "template.h"
#include<vector>

#pragma once

class TileMaps
{
	float XoffSet;
	float YoffSet;
	int width;
	int height;
	char** Map;
	std::vector<Tmpl8::vec4> colliders;
	std::vector<Tmpl8::vec4> jumpers;
	std::vector<Tmpl8::vec4> deaths;
	std::vector<Tmpl8::vec4> win;
	Tmpl8::Surface* Tiles;

	

public:
	TileMaps(char* TilesIn);
	TileMaps(char* TilesIn, int heightIn, int widthtIn);
	void mapScroll(Tmpl8::Surface* screen);
	char** getMap() { return Map; }
	std::vector<Tmpl8::vec4> getColliders() { return colliders; }
	std::vector<Tmpl8::vec4> getJumpers() { return jumpers; }
	std::vector<Tmpl8::vec4> getDeaths() { return deaths; }
	std::vector<Tmpl8::vec4> getWin() { return win; }
	int getWidth() { return width; }
	float getXoffSet() { return XoffSet; }
	void setXoffSet(float xoff) {
		XoffSet = xoff;
		XoffSet = Tmpl8::Clamp(XoffSet, 0.0f, width / 3.0f - 25.0f);
	}
	float getYoffSet() { return YoffSet; }
	void setYoffSet(float yoff) {
		YoffSet = yoff;
		YoffSet = Tmpl8::Clamp(YoffSet, 0.0f, height - 16.0f);
	}
	int getHeight() { return height; }
	void setTile(char** mapAdd, int sizeI, int sizeJ, int startX, int startY, char sign);
	void setTile(char* mapAdd, int sizeI, int startX, int startY, char sign);
	void setTile(char** mapAdd, int sizeI, int sizeJ, int startX, int startY);
	void setMap(char** newMap) { Map = newMap; }
	void setColliders(int strtX, int strtY, int length, int height, char sign);
	void printMap();
	void DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y);
	void DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y, float precY, int startY, float precX, int startX);
};