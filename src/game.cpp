#include "game.h"

Game::Game(){
	Surf_Display  = NULL;
	Running       = true;
	winWidth      = 800;
	winHeight     = 600;
	keyStates     = new bool[256];
	moving        = false;
	active        = true;
	mouse.left    = mouse.middle = mouse.right = false;
	selected_unit = -1;
	disable       = false;
	freelook_dx   = freelook_dy = 0;
	help_mode 	  = false;
}

int Game::Execute(){
	if(Init() == false){
		Cleanup();
		return -1;
	}

	SDL_Event event;

	while(Running){
		while(SDL_PollEvent(&event))
			Events(&event);
		Loop();
		Render();
		//usleep(10000);
		// if( !active )
		// 	Sleep(10);
	}

	Cleanup();
	return 1;
}

int main(int argc, char* argv[]){
	#ifdef _WIN32 
	FreeConsole();
	#endif
 	Game theGame;
    return theGame.Execute();
}