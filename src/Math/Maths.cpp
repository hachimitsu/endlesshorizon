#include "Maths.h"

// Produces a random real number from 0 - 1
float real_rand(){
	return (float)rand()/(float)RAND_MAX;
}

// Calculates the dot product
float dotProduct(vector3f u, vector3f v)
{
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

// Finds the angle between two vectors
float getAngle(vector3f u, vector3f v)
{
	return 180.0f/M_PI * acos(dotProduct(u,v)/(u.magnitude() * v.magnitude()));
}

// Normalize a vector
vector3f normalize(vector3f u)
{
	float mag = u.magnitude();

	return vector3f(u.x/mag, u.y/mag, u.z/mag);
}

// Calcultes the cross product
vector3f crossProduct(vector3f u, vector3f v)
{
	return vector3f(u.y*v.z - u.z*v.y,
		u.z*v.x - u.x*v.z,
		u.x*v.y - u.y*v.x);
}

// Finds the normal given three points
vector3f normal(vector3f i, vector3f j, vector3f k)
{
	vector3f v1 = normalize(j-i);
	vector3f v2 = normalize(k-i);

	return crossProduct(v1,v2);
}

// Convert degrees to radians
float to_rad(float d){
	return d * M_PI1;
}

// Convert radians to degrees
float to_deg(float r){
	return r * M_PI2;
}

// Sets the colour for colour type
void set_colour(colour &c, int r, int g, int b, int o){
	c.r = r;
	c.g = g;
	c.b = b;
	c.o = o;
}