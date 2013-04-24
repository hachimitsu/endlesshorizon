#include "unit.h"

using namespace std;

static GLuint beam;
fireball ball[10];
explosion boom;

Unit::~Unit(){

}

Unit::Unit(){
	mode 		 = -1;
	visible 	 = true;
	moved        = false;
	attacked	 = false;
	health       = 10;
	attack       = 5;
	move_range   = 3;
	attack_range = 2;
	team         = -1;
	distance     = 0;
	tilt 		 = 0;
	heading      = cannon_heading = 0;
	target_angle = cannon_heading_o = 0;
	dir.set(0,0,0);
}

Unit::Unit(tile t,float h,float ch,int p){
	pos 		 = t.get_pos();
	wpos 		 = t.get_wpos();
	heading 	 = h;
	cannon_heading = ch;
	target_angle = cannon_heading_o = 0;
	mode 		 = -1;
	visible 	 = true;
	moved        = false;
	attacked	 = false;
	health       = 10;
	attack       = 5;
	move_range   = 3;
	attack_range = 2;
	distance     = 0;
	tilt 		 = 0;
	team 		 = p;
	dir.set(0,0,0);
	init(t,h,ch,p);
}

void Unit::init(tile t,float h,float ch,int p){
	pos = t.get_pos();
	wpos = t.get_wpos();
	heading = h;
	cannon_heading = ch;
	team = p;
}

// Draws the helix around the laser
void draw_laser_spiral(float radius, float length, float sections){
	float dr = 360.0f/sections, d = 0;
	glLineWidth(4);
	radius *= 1.4;
	glBegin(GL_LINE_STRIP);
		for (float i = 0; i < length; i += 0.005){
			vector3f p;
			p.set( radius * cos(to_rad(d)), radius * sin(to_rad(d)), -i );
			d += dr;
			glVertex3f(p.x,p.y,p.z);
		}
	glEnd();
}

// Draws the ships firing laser
void draw_laser(float radius, float length, float sections){
	float dr = 360.0f/sections, d = 0;
	glBegin(GL_QUADS);
	for (int i = 0; i < sections; ++i){
		vector3f p1,p2,p3,p4,n1,n2;

		p1.set( radius * cos(to_rad(d)), radius * sin(to_rad(d)), -length );
		p2.set( p1.x, p1.y, 0);
		d += dr;
		p3.set( radius * cos(to_rad(d)), radius * sin(to_rad(d)), 0 );
		p4.set( p3.x, p3.y, -length);

		n1 = p2.unit();
		n2 = p3.unit();

		glNormal3f(n1.x,n1.y,n2.z);
		glVertex3f(p1.x,p1.y,p1.z);
		glVertex3f(p2.x,p2.y,p2.z);
		glNormal3f(n2.x,n2.y,n2.z);
		glVertex3f(p3.x,p3.y,p3.z);
		glVertex3f(p4.x,p4.y,p4.z);
	}
	glEnd();
}

// Draw the unit if it is visible
void Unit::display(float dt, vector3f camera){
	if( visible ){
		// Interpolates the heading and position of the unit
		float a = cannon_heading + (target_angle - cannon_heading_o) * 0.04 * dt;
		glPushMatrix();
		vector3f p = wpos + dir * dt;
		glTranslatef(p.x, p.y, p.z);
		glRotatef(tilt,1,0,0);
		draw_ship(a,heading);
		glPopMatrix();

		// Rotates the cannon to point at the target and fires the laser
		if( mode == 2){
			glDisable(GL_LIGHTING);
			glPushMatrix();
				p += target_heading * 0.06f;
				glTranslatef(p.x,p.y+0.185,p.z);
				glRotatef(a,0,1,0);
				glColor4f(1,0,0,laser.t);
				draw_laser(0.005 * laser.t ,distance,12);
				glColor4f(1,0,0,laser.t/3.0f);
				draw_laser(0.007 * laser.t ,distance,12);
				glColor4f(1,0,0,laser.t/6.0f);
				draw_laser(0.009 * laser.t ,distance,12);
				glColor4f(1,1,1,laser.t/2.0f);
				draw_laser_spiral(0.01 * laser.t ,distance,12);
			glPopMatrix();
			glLineWidth(3);
			glBegin(GL_LINES);
			for (int i = 0; i < 50; ++i)
			{
				vector3f h = sparks[i].h, t = h + (sparks[i].t-h).unit() * 0.07f;
				glColor4f(1,0,0,sparks[i].life);
				glVertex3f(h.x,h.y,h.z);
				glVertex3f(t.x,t.y,t.z);
			}
			glEnd();
			glEnable(GL_LIGHTING);
		}

		// Draws the explosion for when the unit is destroyed
		if( mode == 3){
			for (int i = 0; i < 10; ++i)
			{
				ball[i].display(dt,camera);
			}
			boom.display(dt,camera);
		}
	}
}

// Updates the unit depending on its current state
int Unit::update(){
	switch( mode ){
		// Update the position/heading of the unit
		case 0:{
			distance += 0.06;
			float temp_length = length_table.at(length_table.size()-1);;

			if( distance >= temp_length ){
				wpos = move_points[move_points.size()-1];
				move_points.clear();
				length_table.clear();
				mode = -1;
				dir.set(0,0,0);
				return 1;
			}

			float d = distance/temp_length;
			float t = d * d * (3 - 2 * d);
			d = temp_length * t;

			for (int i = 1; i < length_table.size(); ++i){
				if( d < length_table.at(i) ){ 
					vector3f new_pos = move_points[i-1] + ((move_points[i]-move_points[i-1]) * ((d-length_table[i-1])/(length_table[i] - length_table[i-1])));
					dir = new_pos - wpos;
					wpos = new_pos;

					float heading_t = to_deg(acos(dotProduct(vector3f(0,0,-1),dir)/dir.magnitude()));
					if( dir.x > 0 )
						heading_t = 360-heading_t;

					cannon_heading += heading_t - heading;
					heading = heading_t;
					break;
				}
			}

			break;
		}
		// Updates the cannon's heading
		case 1:{
			if( t <= 1 ){
				t += 0.04;
				cannon_heading = cannon_heading_o + (target_angle - cannon_heading_o) * t;
			}
			else if( t > 1 ){
				mode = 2;
				target_angle = 0;
				cannon_heading_o = 0;
			}

			break;
		}
		// Updates the laser and sparks it creates
		case 2:{
			if( laser.cl <= laser.l ){
				laser.cl += laser.dr;
				laser.t = -(laser.cl*2) * (laser.cl*2) + 1;
			}

			bool alive = false;
			for (int i = 0; i < 50; ++i){
				if( sparks[i].life <= 1 ){
					sparks[i].t = sparks[i].h;
					sparks[i].h += sparks[i].f;
					sparks[i].f.y -= 0.0003f;
				}
				sparks[i].life -= sparks[i].decay;
				if( sparks[i].life > 0 )
					alive = true;
			}

			if( !alive ){
				mode = -1;
			}

			if( laser.cl > laser.l && laser.dr > 0 ){
				laser.dr = -1;
				return 2;
			}
			break;
		}
		// Updates the position/tilt and explosion for when the unit dies
		case 3:{
			wpos.y -= 0.01f;
			tilt += 5;
			for (int i = 0; i < 10; ++i)
			{
				ball[i].update();
			}
			bool done = boom.update();
			if( wpos.y < -0.3 && !done ){
				visible = false;
				mode = -1;
				return 3;
			}
			break;
		}
	}

	return -1;
}

// Sets up the unit to fire at specified target
void Unit::fire(vector3f t){
	attacked = true;
	mode = 1;
	target = t;
	Unit::t = 0;
	target_heading = (t - wpos);
	distance = target_heading.magnitude();
	target_angle = to_deg(acos(dotProduct(vector3f(0,0,-1),target_heading)/target_heading.magnitude()));

	if( target_heading.x > 0 )
		target_angle = 360-target_angle;

	if( target_angle - cannon_heading > 180 )
		cannon_heading += 360;

	cannon_heading_o = cannon_heading;

	target_heading.normalize();
	laser.cl = -0.5;
	laser.l = 0.5;
	laser.dr = 0.05;
	laser.t = 0;
	distance -= 0.06;

	for (int i = 0; i < 50; ++i){
		//srand(time(NULL));
		sparks[i].t = sparks[i].h = target + vector3f(0,0.185,0);
		sparks[i].f = vector3f(target_heading.x + real_rand() * 0.4,
			target_heading.y + real_rand() * 0.4,
			target_heading.z + real_rand() * 0.4).unit() * 0.02f;
		sparks[i].life = 1 + real_rand() * 0.6;
		sparks[i].decay = 0.02 + real_rand() * 0.1;
	}
}

// Sets up the unit to move given a movement path
// Creates a hermite curve and length table for consistent movement
void Unit::move(std::vector<tile> move_path){
	mode = 0;
	moved = true;
	pos = move_path[move_path.size()-1].get_pos();
	distance = 0;

	for (int i = 0; i < move_path.size()-1; ++i){
		vector3f v1 = move_path[i].get_wpos();
		vector3f v2 = move_path[i+1].get_wpos();
		vector3f t1 = 0.9f * (v2-v1);
		vector3f t2;
		
		if( move_path.size()-2 == i )
			t2 = 0.9f * (move_path[i+1].get_wpos() - move_path[i+1].get_wpos());
		else
			t2 = 0.9f * (move_path[i+2].get_wpos() - v2);
		for (int j = 0; j <= 10; ++j){
			float t = (float)j / 10.0f;
			float h1 = 2*t*t*t - 3*t*t +1;
			float h2 = -2*t*t*t + 3*t*t;
			float h3 = t*t*t - 2*t*t + t;
			float h4 = t*t*t - t*t;
			move_points.push_back(v1*h1 + v2*h2 + t1*h3 + t2*h4);
		}
	}

	float temp_length = 0;
	length_table.push_back(0.0f);
	for (int i = 1; i < move_points.size(); ++i){
		temp_length += (move_points[i] - move_points[i-1]).magnitude();
		length_table.push_back(temp_length);
	}
}

// Reduces the units life and sets up the animation for the units death if it goes below 0
void Unit::receive_damage(int d){
	health -= d;
	if( health < 1 ){
		for (int i = 0; i < 10; ++i)
			ball[i].init(wpos,0.03,30);

		boom.init(wpos,0.01,80);
		mode = 3;
	}
}

// Set the tile it is in
void Unit::set_tile(tile u){
	pos = u.get_pos();
	wpos = u.get_wpos();
}

// Draws the units circular body
void Unit::draw_mass(){
	std::vector<vector3f> v;
	int intersections = 14;
	float d = 360;
	float rate = 360.0f/intersections;
	float r = 1;

	for (int i = 0; i < intersections+1; ++i)
	{
		v.push_back(vector3f(r * 0.5 * cos(to_rad(d)),0,r * 0.5 *sin(to_rad(d))));
		v.push_back(vector3f(r * cos(to_rad(d)),0.2,r*sin(to_rad(d))));
		d -= rate;
	}

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	v.clear();
	d = 360;
	rate = 360.0f/intersections;
	r = 1;
	for (int i = 0; i < intersections+1; ++i)
	{
		v.push_back(vector3f(r * cos(to_rad(d)),0.2,r*sin(to_rad(d))));
		v.push_back(vector3f(r * 1.2 * cos(to_rad(d)),0.1,r * 1.2 *sin(to_rad(d))));
		d -= rate;
	}

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	v.clear();
	rate = 360.0f/intersections;
	d = 360 - rate * 9;
	r = 1.2;
	for (int i = 0; i < intersections-3; ++i)
	{
		v.push_back(vector3f(r * cos(to_rad(d)),0.1,r*sin(to_rad(d))));
		v.push_back(vector3f(r * 1.6 * cos(to_rad(d)),-0.1,r * 1.6 *sin(to_rad(d))));
		d -= rate;
	}

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	v.clear();
	rate = 360.0f/intersections;
	d = 360 - rate * 9;
	r = 1.2;
	for (int i = 0; i < intersections-3; ++i)
	{
		v.push_back(vector3f(r * 1.6 * cos(to_rad(d)),-0.1,r * 1.6 *sin(to_rad(d))));
		v.push_back(vector3f(r * cos(to_rad(d)),-0.3,r*sin(to_rad(d))));
		d -= rate;
	}

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	d = 0;
	rate = 360.0f/intersections;
	r = 1.2;
	glNormal3f(0,-1,0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < intersections+1; ++i)
		{
			glVertex3f(r * cos(to_rad(d)),-0.3,r*sin(to_rad(d)));
			d += rate;
		}
	glEnd();

	d = 360;
	rate = 360.0f/intersections;
	r = 0.5;
	glNormal3f(0,1,0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < intersections+1; ++i)
		{
			glVertex3f(r * cos(to_rad(d)),0,r*sin(to_rad(d)));
			d -= rate;
		}
	glEnd();
}

// Draws the two fins at the front
void Unit::draw_nose(){
	std::vector<vector3f> v;
	int intersections = 14;
	float rate = 360.0f/intersections;
	float d = 360 - rate * 8;
	float r = 1.2;

	v.push_back(vector3f( r * cos(to_rad(d)), 0.2, r * sin(to_rad(d)) ));
	v.push_back(vector3f( r * cos(to_rad(d))-2, 0, r * sin(to_rad(d)) ));
	d -= rate;
	r = 1.2;
	v.push_back(vector3f( r * cos(to_rad(d)), 0.2, r * sin(to_rad(d)) ));
	r = 1.92;
	v.push_back(vector3f( r * cos(to_rad(d)), 0, r * sin(to_rad(d)) ));

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
			vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
			vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	v.clear();
	d = 360 - rate * 9;
	r = 1.92;

	v.push_back(vector3f( r * cos(to_rad(d)), 0, r * sin(to_rad(d)) ));
	d += rate;
	r = 1.2;
	v.push_back(vector3f( r * cos(to_rad(d))-2, 0, r * sin(to_rad(d)) ));
	d -= rate;
	v.push_back(vector3f( r * cos(to_rad(d)), -0.2, r * sin(to_rad(d)) ));
	d += rate;
	v.push_back(vector3f( r * cos(to_rad(d)), -0.2, r * sin(to_rad(d)) ));

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
			vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
			vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	v.clear();
	d = 360 - rate * 7;
	r = 1.2;

	v.push_back(vector3f( r * cos(to_rad(d)), 0.2, r * sin(to_rad(d)) ));
	v.push_back(vector3f( r * cos(to_rad(d)), -0.2, r * sin(to_rad(d)) ));
	d-=rate;
	v.push_back(vector3f( r * cos(to_rad(d)), 0.2, r * sin(to_rad(d)) ));
	v.push_back(vector3f( r * cos(to_rad(d)), -0.2, r * sin(to_rad(d)) ));
	v.push_back(vector3f( r * cos(to_rad(d))-2, 0, r * sin(to_rad(d)) ));

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < v.size()-3; i+=2)
	{
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
	}
	// vector3f p = v[v.size()-1];
	// glNormal3f(0,0,-1);
	// glVertex3f(p.x,p.y,p.z);
	glEnd();

	glBegin(GL_TRIANGLES);
		vector3f p1 = v[2], p2 = v[3], p3 = v[4];
		glNormal3f(0,0,-1);
		glVertex3f(p1.x,p1.y,p1.z);
		glVertex3f(p2.x,p2.y,p2.z);
		glVertex3f(p3.x,p3.y,p3.z);
	glEnd();
}

// Draws the entire body
void Unit::draw_body(){
	static GLuint index;

	if( !index ){
		index = glGenLists(1);
		glNewList(index,GL_COMPILE);
		glPushMatrix();
			glRotatef(-90,0,1,0);
			glTranslatef(0,0.1,0);
			glScalef(0.08,0.08,0.06);
			glTranslatef(0,0.65,0);
			glPushMatrix();
				glRotatef(180,1,0,0);
				draw_nose();
			glPopMatrix();
			draw_nose();	

			glTranslatef(0,0.1,0);
			draw_mass();
		glPopMatrix();
		glEndList();
	}

	glCallList(index);
}

// Draws the cannon top
void Unit::draw_cannon_top(){
	std::vector<vector3f> v;
	int intersections = 14;
	float d = 360;
	float rate = 360.0f/intersections;
	float r = 0.8;

	for (int i = 0; i < intersections+1; ++i)
	{
		v.push_back(vector3f(r * cos(to_rad(d)),0.4,r*sin(to_rad(d))));
		v.push_back(vector3f(r * 0.5 * cos(to_rad(d)),0.2,r * 0.5 *sin(to_rad(d))));
		d -= rate;
	}

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	d = 360;
	for (int i = 0; i < intersections+1; ++i)
	{
		v.push_back(vector3f(r * 0.9 * cos(to_rad(d)),0.5,r * 0.9 *sin(to_rad(d))));
		v.push_back(vector3f(r * cos(to_rad(d)),0.4,r*sin(to_rad(d))));
		d -= rate;
	}

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	d = 360;
	for (int i = 0; i < intersections+1; ++i)
	{
		v.push_back(vector3f(r * 0.7 * cos(to_rad(d)),0.6,r * 0.7 * sin(to_rad(d))));
		v.push_back(vector3f(r * 0.9 * cos(to_rad(d)),0.5,r * 0.9 * sin(to_rad(d))));
		d -= rate;
	}

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	d = 360;
	for (int i = 0; i < intersections+1; ++i)
	{
		v.push_back(vector3f(r * 0.3 * cos(to_rad(d)),0.7,r * 0.3 * sin(to_rad(d))));
		v.push_back(vector3f(r * 0.7 * cos(to_rad(d)),0.6,r * 0.7 * sin(to_rad(d))));
		d -= rate;
	}

	for (int i = 0; i < v.size()-2; i+=2)
	{
		glBegin(GL_TRIANGLE_STRIP);
		vector3f p1 = v[i], p2 = v[i+1], p3 = v[i+2], p4 = v[i+3];
		vector3f n = crossProduct(p2-p1,p3-p1).unit();
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		glEnd();
	}

	d = 360;
	rate = 360.0f/intersections;
	r = 0.3;
	glNormal3f(0,1,0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < intersections+1; ++i)
		{
			glVertex3f(r * cos(to_rad(d)),0.7,r*sin(to_rad(d)));
			d -= rate;
		}
	glEnd();

	d = 0;
	rate = 360.0f/intersections;
	r = 0.4;
	glNormal3f(0,-1,0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < intersections+1; ++i)
		{
			glVertex3f(r * cos(to_rad(d)),0.2,r*sin(to_rad(d)));
			d += rate;
		}
	glEnd();
}

// Draws a cube with given size
void draw_cube(float size){
	float v = size/2.0f;
	glBegin(GL_QUADS);
		glNormal3f(0,0,1);
		glVertex3f(-v,v,v);
		glVertex3f(-v,-v,v);
		glVertex3f(v,-v,v);
		glVertex3f(v,v,v);

		glNormal3f(0,0,-1);
		glVertex3f(v,v,-v);
		glVertex3f(v,-v,-v);
		glVertex3f(-v,-v,-v);
		glVertex3f(-v,v,-v);

		glNormal3f(1,0,0);
		glVertex3f(v,v,v);
		glVertex3f(v,-v,v);
		glVertex3f(v,-v,-v);
		glVertex3f(v,v,-v);

		glNormal3f(-1,0,0);
		glVertex3f(-v,v,-v);
		glVertex3f(-v,-v,-v);
		glVertex3f(-v,-v,v);
		glVertex3f(-v,v,v);

		glNormal3f(0,1,0);
		glVertex3f(-v,v,-v);
		glVertex3f(-v,v,v);
		glVertex3f(v,v,v);
		glVertex3f(v,v,-v);

		glNormal3f(0,-1,0);
		glVertex3f(-v,-v,v);
		glVertex3f(-v,-v,-v);
		glVertex3f(v,-v,-v);
		glVertex3f(v,-v,v);
	glEnd();
}

// Draws the railgun
void Unit::draw_railgun(){
	glPushMatrix();
		glTranslatef(-0.6,0.5,0);
		glPushMatrix();
			glTranslatef(0,0,-0.1);
			glScalef(9,0.7,0.5);
			glTranslatef(-0.1,0,-0.1);
			draw_cube(0.2);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,0,+0.1);
			glScalef(9,0.7,0.5);
			glTranslatef(-0.1,0,+0.1);
			draw_cube(0.2);
		glPopMatrix();
		glScalef(3,1,3);
		draw_cube(0.2);
	glPopMatrix();
}

// Draws the ship's upper half
void Unit::draw_cannon(){
	static GLuint index;
	if( !index ){
		index = glGenLists(1);

		glNewList(index,GL_COMPILE);
			glPushMatrix();
				glRotatef(-90,0,1,0);
				glTranslatef(0,0.1,0);
				glScalef(0.08,0.08,0.06);
				glTranslatef(0,0.55,0);
				draw_cannon_top();
				draw_railgun();
			glPopMatrix();
		glEndList();
	}

	glCallList(index);
}

// Draws the entire ship with given headings for the ship and cannon
void Unit::draw_ship(float deg, float deg2){
	GLfloat material_diffuse[] = {0.2,0.2,0.2,1};
	GLfloat material_specular[] = {1,1,1,1};
	GLfloat material_shininess[] = { 120 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	glEnable(GL_NORMALIZE);
	glPushMatrix();
		glRotatef(deg,0,1,0);
		draw_cannon();
	glPopMatrix();

	glPushMatrix();
		glRotatef(deg2,0,1,0);
		draw_body();
	glPopMatrix();
	
	glDisable(GL_NORMALIZE);
}