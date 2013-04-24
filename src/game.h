#ifndef game_h
#define game_h 

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include "Math/Maths.h"
#include "Camera.h"
#include "Map/Map.h"
#include "Systems/water.h"
#include "Systems/birds.h"
#include "Units/unit.h"

// Mouse struct for position and buttons states
struct Mouse{
	vector3f pos;
	bool left, middle, right;
};

class Game{
private:
	bool Running, active;
	SDL_Surface* Surf_Display;

	int winWidth;
	int winHeight;

	bool moving, disable;
	vector3f destination;

	bool* keyStates;

	Mouse mouse;
	int freelook_dx, freelook_dy;
	Camera camera;
	vector3f light;

	float dt;

	std::vector<tile> move_area, move_path;
	bool picked;
	tile picked_tile, current, target;
	int pickx, picky, selected_unit, target_unit, active_team;

	Map world;
	std::vector<Unit> units;
	Water water_system;
	Birds bird_system[2];

	GLuint tex[7];
	GLuint help;
	bool help_mode;

	bool Init();
	void Events(SDL_Event* Event);
	void Loop();
	void Render();
	void Cleanup();
	void drawScene();
	void draw_picking_scene();
	void update();
	void process_pick();
	int unit_ontile(tile);
	void fliptiles(int);
	void draw_hud();
	void draw_skybox();

public:
	Game();
	~Game(){};

	int Execute();
};

#endif