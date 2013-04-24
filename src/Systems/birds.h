#include <SDL/SDL_opengl.h>
#include "../Math/Maths.h"

#define MB 100

class Birds
{
	struct bird
	{
		vector3f pos;
		vector3f heading;
	};

	bird *B;

public:
	Birds();
	~Birds();

	void update();
	void display(float, vector3f);
};