#include "game.h"

//Set light position and properties
void setLights(vector3f light, float intensity){
	GLfloat sun[]  = { 1, 1, 1, 1 };
	GLfloat ambient[]  = { 1, 1, 1, 1 };
	GLfloat diffuse[]  = { 0.4, 0.4, 0.4, 1 };
	GLfloat specular[] = { 1, 1, 1, 1 };
	float light_position[] = {light.x, light.y, light.z, 0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, diffuse);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//Draw target reticle
void draw_target(vector3f p){
	static GLuint index;
	if( !index ){
		index = glGenLists(1);
		glNewList(index,GL_COMPILE);
		glPushMatrix();
		for (int i = 1; i <= 6; ++i)
		{
			glRotatef(60,0,1,0);
			glBegin(GL_TRIANGLES);
			glVertex3f(0,0.03f,0.23f);
			glVertex3f(-0.06,0.03f,0.33f);
			glVertex3f(0.06,0.03f,0.33f);
			glEnd();
		}
		glPopMatrix();
		glEndList();
	}

	glPushMatrix();
	glTranslatef(p.x,p.y + 0.01,p.z);
	glCallList(index);
	glPopMatrix();
}

//Draw tile highlight
void draw_hilite(vector3f p){
	static GLuint index;
	if( !index ){
		index = glGenLists(1);
		glNewList(index,GL_COMPILE);
			int d = 360;
			float r = 0.3;
			glNormal3f(0,1,0);
			glBegin(GL_QUAD_STRIP);
			for (int i = 0; i < 7; ++i)
			{
				glVertex3f(r * 0.9 * cos(to_rad(d)), 0, r * 0.9 * sin(to_rad(d)));
				glVertex3f(r * cos(to_rad(d)), 0, r * sin(to_rad(d)));
				
				d -= 60;
			}
			glEnd();
		glEndList();
	}

	glPushMatrix();
	glTranslatef(p.x,p.y + 0.01,p.z);
	glCallList(index);
	glPopMatrix();
}

void draw_still_water(){
	static GLuint index;
	if(!index){
		index = glGenLists(1);
		glNewList(index,GL_COMPILE);
		int rate = 10, r = 80;
		glNormal3f(0,1,0);
		glColor4ub(137,205,252,200);
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(0,-1,0);
		for( int i = 0; i >= -360; i -= rate ){
			vector3f p(r * cos(to_rad(i)),-1, r * sin(to_rad(i)));
			glVertex3f(p.x,p.y,p.z);
		}
		glEnd();
		glEndList();
	}

	glDisable(GL_FOG);
	glCallList(index);
	glEnable(GL_FOG);
}

// Draw the HUD
void Game::draw_hud(){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winWidth,winHeight,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if( active_team == 0 )
		glColor3f(0.3,0.3,0.3);
	else
		glColor3f(0.9,0.9,0.9);
	glBegin(GL_QUADS); 
	glVertex2f(winWidth-40,10); 
	glVertex2f(winWidth-40,40); 
	glVertex2f(winWidth-10,40); 
	glVertex2f(winWidth-10,10); 

	if( selected_unit > -1 ){
		int health = units[selected_unit].get_health();
		glColor3f(1,0,0);
		glVertex2f(10,10);
		glVertex2f(10,20);
		glVertex2f(80,20);
		glVertex2f(80,10);

		glColor3f(0,1,0);
		glVertex2f(10,10);
		glVertex2f(10,20);
		glVertex2f(10 + health * 7,20);
		glVertex2f(10 + health * 7,10);
	}

	if( target_unit > -1 ){
		int health = units[target_unit].get_health();
		glColor3f(1,0,0);
		glVertex2f(10,30);
		glVertex2f(10,40);
		glVertex2f(80,40);
		glVertex2f(80,30);

		glColor3f(0,1,0);
		glVertex2f(10,30);
		glVertex2f(10,40);
		glVertex2f(10 + health * 7,40);
		glVertex2f(10 + health * 7,30);
	}
	glEnd();

	if( help_mode ){
		
		int size,offsetx,offsety;
		if( winWidth > winHeight ){
			size = winHeight * 0.8f;
			offsetx = (winWidth - size)/2.0f;
			offsety = (winHeight - size)/2.0f;
		}
		else{
			size = winWidth * 0.8f;
			offsetx = (winWidth - size)/2.0f;
			offsety = (winHeight - size)/2.0f;
		}
		glColor3f(1,1,1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, help);
		glBegin(GL_QUADS);
			glTexCoord2d(0.0,0.0); glVertex2f(offsetx,offsety);
			glTexCoord2d(0.0,1.0); glVertex2f(offsetx,offsety+size);
			glTexCoord2d(1.0,1.0); glVertex2f(offsetx+size,offsety+size);
			glTexCoord2d(1.0,0.0); glVertex2f(offsetx+size,offsety);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void Game::draw_skybox(){
	static GLuint index;
	if( !index ){
		int s = 100;
		float offset = s/2.0f;

		index = glGenLists(1);
		glNewList(index,GL_COMPILE);
			glRotatef(90,0,1,0);
			glColor3f(1,1,1);
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, tex[0]);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0); glVertex3f(-s,s,s);
				glTexCoord2d(0.0,1.0); glVertex3f(-s,s,-s);
				glTexCoord2d(1.0,1.0); glVertex3f(s,s,-s);
				glTexCoord2d(1.0,0.0); glVertex3f(s,s,s);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, tex[1]);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0); glVertex3f(-s,s,s);
				glTexCoord2d(0.0,1.0); glVertex3f(-s,-s,s);
				glTexCoord2d(1.0,1.0); glVertex3f(-s,-s,-s);
				glTexCoord2d(1.0,0.0); glVertex3f(-s,s,-s);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, tex[2]);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0); glVertex3f(-s,s,-s);
				glTexCoord2d(0.0,1.0); glVertex3f(-s,-s,-s);
				glTexCoord2d(1.0,1.0); glVertex3f(s,-s,-s);
				glTexCoord2d(1.0,0.0); glVertex3f(s,s,-s);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, tex[3]);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0); glVertex3f(s,s,-s);
				glTexCoord2d(0.0,1.0); glVertex3f(s,-s,-s);
				glTexCoord2d(1.0,1.0); glVertex3f(s,-s,s);
				glTexCoord2d(1.0,0.0); glVertex3f(s,s,s);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, tex[4]);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0); glVertex3f(s,s,s);
				glTexCoord2d(0.0,1.0); glVertex3f(s,-s,s);
				glTexCoord2d(1.0,1.0); glVertex3f(-s,-s,s);
				glTexCoord2d(1.0,0.0); glVertex3f(-s,s,s);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, tex[5]);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0); glVertex3f(-s,-s,-s);
				glTexCoord2d(0.0,1.0); glVertex3f(-s,-s,s);
				glTexCoord2d(1.0,1.0); glVertex3f(s,-s,s);
				glTexCoord2d(1.0,0.0); glVertex3f(s,-s,-s);
			glEnd();

			glColor4f(1,1,1,0.8);
			glBindTexture(GL_TEXTURE_2D, tex[6]);
			glBegin(GL_QUADS);
				int rate = 10;
				int r = 80;
				for( int i = 0; i < 360; i += rate ){
					vector3f p(r * cos(to_rad(i)), 22, r * sin(to_rad(i)));
					glTexCoord2d(0.0,0.0); glVertex3f(p.x,p.y,p.z);
					glTexCoord2d(0.0,1.0); glVertex3f(p.x,p.y-30,p.z);
					p.set(r * cos(to_rad(i+rate)), 22, r * sin(to_rad(i+rate)));
					glTexCoord2d(1.0,1.0); glVertex3f(p.x,p.y-30,p.z);
					glTexCoord2d(1.0,0.0); glVertex3f(p.x,p.y,p.z);
				}
			glEnd();

			glDisable(GL_TEXTURE_2D);	
		glEndList();
	}

	glDisable(GL_FOG);
	glPushMatrix();
	vector3f p = camera.get_pos();
	glTranslatef(p.x,-10,p.z);
	glCallList(index);
	glPopMatrix();
	glEnable(GL_FOG);
}

//Draw the world and all objects
void Game::drawScene(){
	static float hlt_rate = 0.01;		// opacity rate for highlights
	static float hlt =  0.4;			// opacity for highlights
	world.display(camera.get_pos());	// Draw the map

	// Draw fish in the water
	glColor3f(0,0.1,0.2);
	glPushMatrix();
		glTranslatef(0,-1,0);
		bird_system[0].display(dt, camera.get_pos());
	glPopMatrix();

	// Draw birds in the sky
	glColor3f(1,1,1);
	bird_system[1].display(dt, camera.get_pos());

	// Draw water
	glColor4ub(89,173,237,200);
	water_system.display(dt);
	draw_still_water();
	
	glDisable(GL_FOG);
	
	glEnable(GL_FOG);

	glDisable(GL_LIGHTING);

	// Draw the highlight around the selected unit
	glPointSize(4);
	if( current.is_traversable() ){
		vector3f p = current.get_wpos();
		glColor4f(0,0,1,hlt);
		draw_hilite(p);
	}

	// Draw the target around the target tile or unit
	if( target.is_traversable() ){
		vector3f p = target.get_wpos();
		glColor4f(1,0,0,hlt);
		draw_target(p);
	}

	// Draw the highlights for the possible movement area
	if( move_area.size() > 0 ){
		glColor4ub(255,0,93,255*hlt);
		for (int i = 0; i < move_area.size(); ++i){
			vector3f p = move_area[i].get_wpos();
			draw_hilite(p);
		}
	}

	// Draw the hilights for the movement path
	if( move_path.size() > 0 ){
		glColor4f(0,0,1,hlt);
		for (int i = 1; i < move_path.size(); ++i){
			vector3f p = move_path[i].get_wpos();
			draw_hilite(p);
		}
	}

	hlt += hlt_rate;
	if( hlt < 0.4 || hlt > 0.75)
		hlt_rate *= -1;

	draw_skybox();

	glEnable(GL_LIGHTING);
	
	// Draw all the units
	for (int i = 0; i < units.size(); ++i){
		if( units[i].get_team() == 0 )
			glColor3f(0.3,0.3,0.3);
		else
			glColor3f(0.7,0.7,0.7);
		units[i].display(dt,camera.get_pos());	
	}

	draw_hud();
}

// Draw the tiles with their unique colours for picking
void Game::draw_picking_scene(){
	glDisable(GL_DITHER);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);

	world.display_pick();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winWidth,winHeight,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0,0,1);
	glBegin(GL_QUADS); 
	glVertex2f(winWidth-40,10); 
	glVertex2f(winWidth-40,40); 
	glVertex2f(winWidth-10,40); 
	glVertex2f(winWidth-10,10); 
	glEnd();

	glEnable(GL_FOG);
	glEnable(GL_LIGHTING);
	glEnable(GL_DITHER);
}

// Setup the environment for drawing
void Game::Render(){
	if( active ){
		//glClearColor(0, 0, 0, 0);
		glClearColor(79/255.0, 173/255.0, 240/255.0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(camera.getProjectionMatrix());

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(camera.getViewMatrix());

		setLights(light, 0.5);

		// If in picking mode, draw the picking scene, else draw the normal scene
		if( picked ){
			draw_picking_scene();
			process_pick();
		}
		else{
			drawScene();
			SDL_GL_SwapBuffers();
		}
	}
}