#pragma once
#include "../Math/Maths.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

#define TRAILSIZE 20

class fireball
{
private:
	struct smoke
	{
		colour c;
		vector3f p;
		vector3f f;
		float l, dr, size;
		int texid;
	};
	smoke F[TRAILSIZE];
	bool active;

	void load_texture(GLuint *i, char *path);
	void load_textures();
public:
	fireball();
	fireball(vector3f,float,float);
	~fireball();

	void init(vector3f,float,float);
	void display(float,vector3f);
	bool update();
};