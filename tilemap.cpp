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
		Map[startY][j] = mapAdd[j - startX];
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
	for (int i = startY; i < 32 * precY; i++, src += 595, dst += 800)
		for (int j = 0; j < 32 * precX; j++)
			dst[j] = src[(j + startX) + startY * 595];
}
void TileMaps::mapScroll(Tmpl8::Surface* screen, float posY, float posX)
{
	for (int i = 0; i < ScreenHeight/32; i++)
	{
		for (int j = 0; j < ScreenWidth/32; j++)
		{
			int tx = Map[int(i + YoffSet)][int(j + XoffSet) * 3] - 'a';
			int ty = Map[int(i + YoffSet)][int(j + XoffSet) * 3 + 1] - 'a';

		}
	}
	/*int YmapStart = (int)posY;
	int XmapStart = (int)posX;
	float Yprec = Tmpl8::Clamp(fmodf(posY, 1), 0.0f, (float)(height - 16.0f));
	float Xprec = Tmpl8::Clamp(fmodf(posX, 1), 0.0f, (float)(width / 3.0f - 25.0f));
	for (int y = YmapStart; y <	Tmpl8::Clamp(YmapStart + 16, 16, height); y++)
		for (int x = XmapStart; x < Tmpl8::Clamp(XmapStart + 25, 25,width/3); x++)
		{
			int tx = Map[y][(x) * 3] - 'a';
			int ty = Map[y][(x) * 3 + 1] - 'a';
			if (y == YmapStart)
			{
				if (x == XmapStart)
				{
					DrawTile(tx, ty, screen, 0, 0, 1, 32 * fmodf(Yprec, 1), 1, 32 * fmodf(Xprec, 1));
					continue;
				}
				else if (x == XmapStart + 24) {
					DrawTile(tx, ty, screen, ((x - XmapStart) * 32) - (32 * fmodf(Xprec, 1)), (y - YmapStart) * 32, 1, 32 * fmodf(Yprec, 1), 1, 0);
					tx = Map[y][((x) % (width - 1)) * 3] - 'a';
					ty = Map[y][((x) % (width - 1)) * 3 + 1] - 'a';
					DrawTile(tx, ty, screen, ((x - XmapStart + 1) * 32) - (32 * fmodf(Xprec, 1)), (y - YmapStart) * 32, 1, 32 * fmodf(Yprec, 1), fmodf(Xprec, 1), 0);

					continue;
				}
				else {
					DrawTile(tx, ty, screen, ((x - XmapStart) * 32) - (32 * fmodf(Xprec, 1)), (y - YmapStart) * 32, 1, 32 * fmodf(Yprec, 1), 1, 0);
					continue;
				}
			}
			else if (x == XmapStart)
			{
				if (y == YmapStart + 15) {
					DrawTile(tx, ty, screen, ((x - XmapStart) * 32) - (32 * fmodf(Xprec, 1)), (y - YmapStart) * 32 - (32 * fmodf(Yprec, 1)));
					tx = Map[(y) % (height - 1)][(x) * 3] - 'a';
					ty = Map[(y) % (height - 1)][(x) * 3 + 1] - 'a';
					DrawTile(tx, ty, screen, ((x - XmapStart) * 32), (y - YmapStart + 1) * 32 - (32 * fmodf(Yprec, 1)), fmodf(Yprec, 1), 0, 1, 32 * fmodf(Xprec, 1));
					continue;
				}
				else {
					DrawTile(tx, ty, screen, (x - XmapStart) * 32, (y - YmapStart) * 32 - (32 * fmodf(Yprec, 1)), 1, 0, 1, 32 * fmodf(Xprec, 1));
					continue;
				}
			}
			DrawTile(tx, ty, screen, ((x - XmapStart) * 32) - (32 * fmodf(Xprec, 1)), (y - YmapStart) * 32 - (32 * fmodf(Yprec, 1)));
			if (y == YmapStart + 15)
			{
				if (x == XmapStart + 24)
				{
					tx = Map[(y) % (height - 1)][((x + 1) % (width - 1)) * 3] - 'a';
					ty = Map[(y) % (height - 1)][((x + 1) % (width - 1)) * 3 + 1] - 'a';
					DrawTile(tx, ty, screen, ((x - XmapStart + 1) * 32) - (32 * fmodf(Xprec, 1)), ((y - YmapStart + 1) * 32) - (32 * fmodf(Yprec, 1)), fmodf(Yprec, 1), 0, fmodf(Xprec, 1), 0);
					tx = Map[y][((x) % (width - 1)) * 3] - 'a';
					ty = Map[y][((x) % (width - 1)) * 3 + 1] - 'a';
					DrawTile(tx, ty, screen, (x - XmapStart + 1) * 32 - (32 * fmodf(Xprec, 1)), ((y - YmapStart) * 32) - (32 * fmodf(Yprec, 1)), 1, 0, fmodf(Xprec, 1), 0);
					tx = Map[(y) % (height - 1)][((x)) * 3] - 'a';
					ty = Map[(y) % (height - 1)][((x)) * 3 + 1] - 'a';
					DrawTile(tx, ty, screen, (x - XmapStart) * 32 - (32 * fmodf(Xprec, 1)), ((y - YmapStart + 1) * 32) - (32 * fmodf(Yprec, 1)), fmodf(Yprec, 1), 0, 1, 0);

					continue;
				}
				else
				{
					tx = Map[(y) % (height - 1)][(x) * 3] - 'a';
					ty = Map[(y) % (height - 1)][(x) * 3 + 1] - 'a';
					DrawTile(tx, ty, screen, (x - XmapStart) * 32 - (32 * fmodf(Xprec, 1)), ((y - YmapStart + 1) * 32) - (32 * fmodf(Yprec, 1)), fmodf(Yprec, 1), 0, 1, 0);
				}
			}
			else if (x == XmapStart + 24)
			{
				tx = Map[y][((x + 1) % (width - 1)) * 3] - 'a';
				ty = Map[y][((x + 1) % (width - 1)) * 3 + 1] - 'a';
				DrawTile(tx, ty, screen, ((x - XmapStart + 1) * 32) - (32 * fmodf(Xprec, 1)), (y - YmapStart) * 32 - (32 * fmodf(Yprec, 1)), 1, 0, fmodf(Xprec, 1), 0);
			}

		}*/
}
