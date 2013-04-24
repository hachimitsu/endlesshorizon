#include "fireball.h"
#include <cmath>

static GLuint tex[7];
static bool textures_loaded = false;

fireball::~fireball(){}

fireball::fireball(){}

fireball::fireball(vector3f o, float force, float size){
	init(o,force,size);
}

// Initializes the fireball to start at given position, force and size
void fireball::init(vector3f o, float force, float size){
	float theta = to_rad(rand()%360), phi = to_rad(rand()%360);
	vector3f d(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
	d = d.unit() * force;
	for (int i = 0; i < TRAILSIZE; ++i){
		set_colour(F[i].c,100,100,100,150);
		F[i].p = o;
		F[i].f = d;
		F[i].l = 10 + i * 0.6;
		F[i].dr = 0.6;
		F[i].size = size + size * pow(i*0.05,2);
		F[i].texid = rand() % 6;
	}
	set_colour(F[0].c,255,132,0,150);
	set_colour(F[1].c,255,132,0,150);
	active = true;

	load_textures();
}

// Updates the fireball
bool fireball::update(){
	if( active ){
		bool alive = false;
		for (int i = 0; i < TRAILSIZE; ++i){
			if( F[i].l <= 10 ){
				F[i].p += F[i].f;
				F[i].f.y -= 0.001f;
				F[i].c.o = 150 * F[i].l/10;
			}
			if( F[i].c.o > 0 )
				alive = true;
			F[i].l -= F[i].dr;
		}
		if( !alive ){
			active = false;
		}
	}
	return active;
}

// Draws the fireball
void fireball::display(float dt, vector3f camera){
	if( active ){
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		
		for (int i = 0; i < TRAILSIZE; ++i){
			if( F[i].l <= 10 ){
				float point_size = F[i].size * 1.0f/(F[i].p - camera).magnitude();
				glBindTexture(GL_TEXTURE_2D, tex[F[i].texid]);
				glColor4ub(F[i].c.r,F[i].c.g,F[i].c.b,F[TRAILSIZE-1].c.o);
				glPointSize(point_size);
				glBegin(GL_POINTS);
				glVertex3f(F[i].p.x, F[i].p.y, F[i].p.z);
				glEnd();
			}
		}

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_POINT_SPRITE);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}	
}

// Loads a texture with given path
void fireball::load_texture(GLuint *i, char *path){
	SDL_Surface *image = IMG_Load(path);
	glGenTextures(1,i);
	glBindTexture(GL_TEXTURE_2D,*i);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	SDL_FreeSurface(image);
}

// Loads the textures for the fireball
void fireball::load_textures(){
	if( !textures_loaded ){
		textures_loaded = true;
		load_texture(&tex[0],"assets/smoke1.png");
		load_texture(&tex[1],"assets/smoke2.png");
		load_texture(&tex[2],"assets/smoke3.png");
		load_texture(&tex[3],"assets/smoke4.png");
		load_texture(&tex[4],"assets/smoke5.png");
		load_texture(&tex[5],"assets/smoke6.png");
		load_texture(&tex[6],"assets/smoke7.png");
	}
}