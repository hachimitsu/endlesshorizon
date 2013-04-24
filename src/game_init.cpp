#include "game.h"
#include "time.h"

void load_texture(GLuint *i, char *path){
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

bool Game::Init()
{
	putenv("SDL_VIDEO_CENTERED=1"); 
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
        return false;
    }

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    if((Surf_Display = SDL_SetVideoMode(winWidth, winHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL | SDL_RESIZABLE)) == NULL)
    {
        return false;
    }

    SDL_WM_SetCaption( "Endless Horizon - Press H for help", NULL );

    glViewport(0, 0, winWidth, winHeight);
    
    glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	glEnable( GL_POINT_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	GLfloat fogcolor[4] = {1,1,1,1};
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glFogfv(GL_FOG_COLOR,fogcolor);
	glFogf(GL_FOG_DENSITY,0.02);
	glFogf(GL_FOG_START,0.1f);
	glFogf(GL_FOG_END,15.0f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);

	for (int i = 0; i < 256; ++i)
		keyStates[i] = false;

	// Initialize camera
	camera.init((float)winWidth/(float)winHeight);
	camera.setPos(0,2,3);
	camera.lookat(0, 0, 1.5);
	camera.setSpeed(0.025f);

	// Initialize lights
	light.set(-0.5,2,-2.7);
	float global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Initialize the map
	world.init();
	picked = false;	

	// Set active team and initialize all units
	active_team = 0;
	selected_unit = target_unit = -2;
	// Team 0
	units.push_back(Unit(world.get_tile(15,12),45,45,0));
	units.push_back(Unit(world.get_tile(16,12),45,45,0));
	units.push_back(Unit(world.get_tile(16,13),45,45,0));
	units.push_back(Unit(world.get_tile(19,6),45,45,0));
	units.push_back(Unit(world.get_tile(20,5),45,45,0));
	units.push_back(Unit(world.get_tile(20,6),45,45,0));

	// Team 1
	units.push_back(Unit(world.get_tile(9,1),-135,-135,1));
	units.push_back(Unit(world.get_tile(10,1),-135,-135,1));
	units.push_back(Unit(world.get_tile(9,2),-135,-135,1));
	units.push_back(Unit(world.get_tile(5,7),-135,-135,1));
	units.push_back(Unit(world.get_tile(5,6),-135,-135,1));
	units.push_back(Unit(world.get_tile(4,7),-135,-135,1));

	load_texture(&tex[0],"assets/top.png");
	load_texture(&tex[1],"assets/1.png");
	load_texture(&tex[2],"assets/2.png");
	load_texture(&tex[3],"assets/3.png");
	load_texture(&tex[4],"assets/4.png");
	load_texture(&tex[5],"assets/bottom.png");
	load_texture(&tex[6],"assets/border.png");
	load_texture(&help,"assets/help.png");

	return true;
}
