#include "../Math/Maths.h"
#include <SDL/SDL_opengl.h>

#define MG 2000
#define MW 20

class Grass
{
private:
	struct blade
	{
		vector3f t,m,b;
	};

	struct wind
	{
		vector3f p,f;
	};

	vector3f center;
	int ng, nw;
	float rad, force,height;
	blade *G;
	wind *W;

	void draw_grass(blade);

public:
	Grass();
	~Grass();

	void init(vector3f,float,float,float,int);
	void update();
	void display(vector3f,vector3f);	
	void cleanup();
};