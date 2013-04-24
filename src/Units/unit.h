#include "../Math/Maths.h"
#include "../Map//tile.h"
#include <SDL/SDL_opengl.h>
#include <GL/glut.h>
#include "../Systems/fireball.h"
#include "../Systems/explosion.h"

class Unit
{
private:
	struct Beam
	{
		float cl;
		float l;
		float dr;
		float t;
	};

	struct Sparks
	{
		vector3f h,t,f;
		float life;
		float decay;
	};

	int mode;
	bool visible;
	vector3f pos,wpos,dir,target,target_heading;
	float heading,cannon_heading,target_angle, cannon_heading_o, t,tilt;
	std::vector<vector3f> move_points;
	std::vector<float> length_table;
	Sparks sparks[50];
	Beam laser;
	float distance;

	int team;
	int health;
	int attack;
	int move_range;
	int attack_range;
	bool attacked,moved;

	void draw_mass();
	void draw_nose();
	void draw_body();
	void draw_cannon_top();
	void draw_railgun();
	void draw_cannon();
	void draw_ship(float, float);

public:
	Unit();
	Unit(tile,float,float,int);
	~Unit();

	void display(float,vector3f);
	int update();
	void init(tile,float,float,int);
	void set_tile(tile);
	void move(std::vector<tile>);
	void fire(vector3f);
	void receive_damage(int);

	vector3f get_pos(){ return pos; };
	int get_team(){ return team; };
	int get_move_range(){ return move_range; };
	int get_attack_range(){ return attack_range; };
	int get_attack(){ return attack; };
	int get_health(){ return health; };
	bool movedq(){ return moved; };
	bool attackedq(){ return attacked; };
	bool visibleq(){ return visible; };

	void flip_attacked(){ attacked = !attacked; };
	void flip_moved(){ moved = !moved; };
	void reset(){ attacked = moved = false; };
};