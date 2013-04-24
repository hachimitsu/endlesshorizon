#pragma once
#include "../Math/Maths.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

#define VOLUME 50

class explosion
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
	smoke F[VOLUME];
	bool active;

	void load_texture(GLuint *i, char *path);
	void load_textures();
public:
	explosion();
	explosion(vector3f,float,float);
	~explosion();

	void init(vector3f,float,float);
	void display(float,vector3f);
	bool update();
};