#include "surface.h"
#include "template.h"
class TileMaps
{
	float XoffSet;
	float YoffSet;
	int width;
	int height;
	char** Map;
	Tmpl8::Surface* Tiles;

public:
	TileMaps(char* TilesIn);
	TileMaps(char* TilesIn, int heightIn, int widthtIn);
	void mapScroll(Tmpl8::Surface* screen, float posY, float posX);
	char** getMap() { return Map; }
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
	void setTile(char** mapAdd, int sizeI, int sizeJ, int startX, int startY);
	void setTile(char* mapAdd, int sizeI, int startX, int startY);
	void printMap();
	void DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y);
	void DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y, float precY, int startY, float precX, int startX);
};