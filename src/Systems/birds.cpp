#include "birds.h"

// Generates birds and places them at a random position with random heading
Birds::Birds(){
	B = new bird[MB];
	for (int i = 0; i < MB; i++){
		float radius = 5 * real_rand();
		float rad = to_rad(rand()%360);
		vector3f h(cos(to_rad(rand()%360)),0,sin(to_rad(rand()%360)));
		h = h.unit() * 3.0f;

		B[i].pos.set(radius * cos(rad), real_rand() * 0.3f + 0.3f, radius * sin(rad));
		B[i].heading.set(h.x,h.y,h.z);
	}
}

Birds::~Birds(){
	delete [] B;
}

// Updates the birds using flocking rules
void Birds::update(){
	const int border = 6;

	for (int i = 0; i < MB; ++i){
		vector3f seperation, alignment, cohesion, bounds;
		int counts = 0, counta = 0;
		for (int j = 0; j < MB; ++j){
			if( j != i ){
				vector3f v = B[i].pos - B[j].pos;
				float d = v.magnitude();
				if( d < 0.25f ){
					seperation += v / d;
					counts++;
				}
				if( d < 0.4f ){
					alignment += B[j].heading;
					cohesion += B[j].pos;
					counta++;
				}
			}
		}
		if( counts > 0 )
			seperation = seperation / float(counts);
		if( counta > 0 ){
			alignment = alignment / float(counta);
			cohesion = cohesion / float(counta);
			cohesion = cohesion - B[i].pos;
		}

		if( B[i].pos.x < -border )
			bounds.x = 1;
		else if( B[i].pos.x > border )
			bounds.x = -1;
		if( B[i].pos.z < -border )
			bounds.z = 1;
		else if( B[i].pos.z > border )
			bounds.z = -1;
		if( B[i].pos.y < 0.3f)
			bounds.y = 1.5;
		else if ( B[i].pos.y > 0.6f )
			bounds.y = -1.5;		

		B[i].heading += (seperation*2.0f + alignment * 0.6f + cohesion * 0.6f + bounds) * 0.2f;

		if( B[i].heading.magnitude() > 3)
			B[i].heading = B[i].heading.unit() * 3.0f;

		B[i].pos += B[i].heading * 0.01f;
	}	
}

// Draws the bird
void Birds::display(float dt, vector3f camera){

	glDisable(GL_LIGHTING);
	//glColor3f(0,0.1,0.2);
	for (int i = 0; i < MB; ++i){
		vector3f pos = B[i].pos + B[i].heading * 0.01f * dt;
		float point_size = 15 * 1.0f/(camera - pos).magnitude();
		glPointSize(point_size);
		glBegin(GL_POINTS);
			glVertex3f(pos.x, pos.y, pos.z);
		glEnd();
	}
	glEnable(GL_LIGHTING);
}