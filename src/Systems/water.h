#include "../Math/Maths.h"
#include <SDL/SDL_opengl.h>
#define MS 100
#define SAMPLES 5

class Water
{
private:
	double **H, *D;
	vector3f **N;
	float amplitude, amplituderatio, wavelength, wavelengthratio;
	double time;

	void generate_normals();
public:
	Water();
	~Water();

	void update();
	void display(float);
	void clean();
};