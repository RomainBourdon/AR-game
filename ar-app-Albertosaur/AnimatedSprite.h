#pragma once
#include <graphics/sprite.h>


class AnimatedSprite : public gef::Sprite
{
public:
	AnimatedSprite();
	~AnimatedSprite();

	void init();
	bool Animate(int rows, int columns);

	void fade();

	int SpriteRows;
	int SpriteColumns;
	
	bool exploded;

	int i, j;
	float uv_width, uv_height;
};
