#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite()
{
}

AnimatedSprite::~AnimatedSprite()
{
}

void AnimatedSprite::init()
{
	i = 0;
	j = 0;
}

bool AnimatedSprite::Animate(int rows, int columns)
{
	SpriteRows = rows;
	SpriteColumns = columns;
	
	float uv[4];

	uv[0] = 0.25;
	uv[1] = 0.50;
	uv[2] = 0.75;
	uv[3] = 1;


	this->set_uv_position(gef::Vector2(uv[i] - 0.25, uv[j] - 0.25));

	if (i > SpriteRows)
	{
		i = -1;
		j++;
	}

	if (j > SpriteColumns)
		return false;

	i++;

    this->set_uv_width(0.25);
	this->set_uv_height(0.25);
	
	
	return true;
}

void AnimatedSprite::fade()
{
	
}
