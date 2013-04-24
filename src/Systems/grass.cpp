#include "grass.h"

Grass::~Grass(){
	// if( G )
	// 	delete [] G;
	// if( W )
	// 	delete [] W;
}

Grass::Grass(){
	
}

void Grass::cleanup(){
	if( G )
		delete [] G;
	if( W )
		delete [] W;
}

// Initializes the grass to start at a given position with certain radius, height, wind force and amount of grass blades
void Grass::init(vector3f pos, float radius, float h, float f, int num){
	force = f;
	center = pos;
	ng = num;
	nw = MW;
	rad = radius;
	height = h;

	G = new blade[ng];
	W = new wind[nw];

	for (int i = 0; i < ng; i++){
		float r = real_rand() * radius * 0.2 + real_rand() * radius * 0.8;
		float rad = to_rad(rand()%360);
		G[i].b = vector3f(pos.x + r * cos(rad), pos.y, pos.z + r * sin(rad));
		G[i].m = G[i].b + vector3f(0,height,0);
		G[i].t = G[i].m + vector3f(0,height,0);
	}

	for (int i = 0; i < nw; i++){
		W[i].p = vector3f(center.x + rad,center.y + 0.1, center.z + real_rand()*rad*1.3-rad*1.3/2);
		W[i].f = vector3f(-0.002f-real_rand()*force,0,0);
	}
}

// Updates the grass system
// Wind particles affect only local grass and bends it depending on how strong it is and how far away it is from the blade of grass
// Uses elasticity to bend the grass blades
void Grass::update(){
	for (int i = 0; i < ng; ++i){
		vector3f w;
		for (int j = 0; j < nw; ++j){
			float distance = (W[j].p - G[i].t).magnitude();
			if( distance < rad * 0.4f )
				w += W[j].f * (1.0f/distance*distance);
		}

		G[i].m += w * 0.4f;
		vector3f F = ((G[i].b + vector3f(0,0.1,0)) - G[i].m) * 0.2f;
		G[i].m += F;
		G[i].m = G[i].b + normalize(G[i].m - G[i].b)*height;

		G[i].t += w * 0.75f;
		F = ((G[i].m + vector3f(0,0.1,0)) - G[i].t) * 0.2f;
		G[i].t += F;
		G[i].t = G[i].m + normalize(G[i].t - G[i].m)*height;
	}

	for (int i = 0; i < nw; ++i){
		W[i].p += W[i].f;
		if( W[i].p.x < center.x-rad ){
			W[i].p = vector3f(center.x + rad,center.y + 0.1, center.z + real_rand()*rad*1.3-rad*1.3/2);
			W[i].f = vector3f(-0.002f-real_rand()*force,0,0);
		}
	}
}

// Draws the grass
void Grass::display(vector3f pos, vector3f camera){
	vector3f n = vector3f(0.4,1,-0.5).unit();
	glNormal3f(n.x,n.y,n.z);
	for (int i = 0; i < ng; ++i)
	{
		float line_width = 2 * 1.0f/(camera - pos).magnitude();
		glLineWidth(line_width);
		draw_grass(G[i]);
	}
}

// Draws a blade of grass
void Grass::draw_grass(blade g){
	glBegin(GL_LINE_STRIP);
		glColor3ub(124,186,30);
		glVertex3f(g.b.x, g.b.y, g.b.z);
		glVertex3f(g.m.x, g.m.y, g.m.z);
		glColor3ub(150,222,42);
		glVertex3f(g.t.x, g.t.y, g.t.z);
	glEnd();
}