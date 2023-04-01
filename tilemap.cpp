#include"tilemap.h"
#include<iostream>

TileMaps::TileMaps(char* TilesIn, int heightIn, int widthIn)
	:	Tiles(new Tmpl8::Surface(TilesIn))
{
	height = heightIn;
	width = widthIn;
	Map = new char* [heightIn];
	for (int i = 0; i < heightIn; i++)
	{
		Map[i] = new char[widthIn];
		for (int j = 0; j < widthIn; j += 3)
		{
			Map[i][j] = 'm';
			Map[i][j + 1] = 'a';
			Map[i][j + 2] = ' ';
		}
	}
}

TileMaps::TileMaps(char* TilesIn)
	: Tiles(new Tmpl8::Surface(TilesIn))
{
	XoffSet = 0;
	YoffSet = 0;
	height = ScreenHeight / 32;
	width = ScreenWidth / 32;
	Map = new char* [height];
	for (int i = 0; i < height; i++)
	{
		Map[i] = new char[width];
		for (int j = 0; j < width; j += 3)
		{
			Map[i][j] = 'm';
			Map[i][j + 1] = 'a';
			Map[i][j + 2] = ' ';
		}
	}
}
void TileMaps::setTile(char** mapAdd, int sizeI, int sizeJ, int startX, int startY,char sign)
{
	startX *= 3;
	for (int i = startY; i < sizeI + startY; i++)
	{
		for (int j = startX; j < sizeJ + startX; j++)
		{
			Map[i][j] = mapAdd[i - startY][j - startX];
		}
	}
	setColliders(startX/3 * 32, startY * 32, (sizeJ / 3) * 32, sizeI * 32, sign);
}
void TileMaps::setTile(char* mapAdd, int sizeI, int startX, int startY, char sign)
{
	startX *= 3;
	for (int j = startX; j < sizeI + startX; j++)
	{
		Map[startY][j] = mapAdd[(j - startX) % sizeof(&mapAdd)];
	}
	setColliders(startX/3 * 32, startY * 32, sizeI / 3 * 32, 32, sign);
}

void TileMaps::setColliders(int strtX, int strtY, int length, int height, char sign)
{
	switch (sign)
	{
	case 'x':
		colliders.push_back(Tmpl8::vec4(strtX, strtY, length, height));
		break;
	case 'j':
		jumpers.push_back(Tmpl8::vec4(strtX, strtY, length, height));
		break;
	default:
		break;
	}
}
void TileMaps::printMap()
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j += 3)
		{
			std::cout << Map[i][j] << Map[i][j + 1] << Map[i][j + 2];
		}
		std::cout << "\n";
	}
}

void TileMaps::DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y)
{
	Tmpl8::Pixel* src = Tiles->GetBuffer() + 1 + tx * 33 + (1 + ty * 33) * 595;
	Tmpl8::Pixel* dst = screen->GetBuffer() + x + y * ScreenWidth;
	for (int i = 0; i < 32; i++, src += 595, dst += ScreenWidth)
		for (int j = 0; j < 32; j++) {
			dst[j] = src[j];
		}
}

void TileMaps::DrawTile(int tx, int ty, Tmpl8::Surface* screen, int x, int y, float precY, int startY, float precX, int startX)
{
	Tmpl8::Pixel* src = Tiles->GetBuffer() + 1 + tx * 33 + (1 + ty * 33) * 595;
	Tmpl8::Pixel* dst = screen->GetBuffer() + x + y * 800;
	for (int i = 0; i < 32 * precY; i++, src += 595, dst += 800)
		for (int j = 0; j < 32 * precX; j++)
			dst[j] = src[(j + startX) + startY * 595];
}
void TileMaps::mapScroll(Tmpl8::Surface* screen)
{
	for (int i = 0; i < ScreenHeight/32 + int(fmodf(2 - fmodf(YoffSet, 1), 2)); i++)
	{
		for (int j = 0; j < ScreenWidth/32 + int(fmodf(2 - fmodf(XoffSet, 1), 2)); j++)
		{
			int tx = Map[int(i + YoffSet)][int(j + XoffSet) * 3] - 'a';
			int ty = Map[int(i + YoffSet)][int(j + XoffSet) * 3 + 1] - 'a';
			float precX = 1, precY = 1;
			int startX = 0, startY = 0;
			int posX = (j * 32) - int(fmodf(XoffSet, 1) * 32), posY = (i * 32) - int(fmodf(YoffSet, 1) * 32);
			if (j == 0)
				startX = int(fmodf(XoffSet, 1) * 32),posX = 0,precX = 1.0f - fmodf(XoffSet,1);
			if (j == (ScreenWidth / 32 + int(fmodf(2 - fmodf(XoffSet, 1), 2))) - 1)
				precX = (fmodf(XoffSet,1) == 0.0f) ? 1.0f : fmodf(XoffSet, 1);
			if (i == 0)
				startY = int(fmodf(YoffSet, 1) * 32), posY = 0, precY = 1.0f - fmodf(YoffSet, 1);
			if (i == (ScreenHeight / 32 + int(fmodf(2 - fmodf(YoffSet, 1), 2))) - 1)
				precY = (fmodf(YoffSet, 1) == 0.0f) ? 1.0f : fmodf(YoffSet, 1);

			DrawTile(tx, ty, screen, posX, posY,precY,startY,precX,startX);
		}
	}
}
