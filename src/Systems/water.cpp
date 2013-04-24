#include "water.h"

// Initializes the water
Water::Water(){
	D = new double[SAMPLES];
	H = new double*[MS];
	N = new vector3f*[MS];
	for (int i = 0; i < MS; i++){
		H[i] = new double[MS];
		N[i] = new vector3f[MS];
	}

	amplitude = 0.07;
	amplituderatio = .5;
	wavelength = 0.15;
	wavelengthratio = 0.56241;
	time = 0;

	for (int n = 0; n < SAMPLES; n++)
		D[n] = 2 * M_PI * real_rand();
}

Water::~Water(){
	for (int i = 0; i < MS; i++){
		delete [] H[i];
		delete [] N[i];
	}
	delete [] H;
	delete [] N;
	delete [] D;
}

void Water::clean(){
	for (int i = 0; i < MS; i++){
		delete [] H[i];
		delete [] N[i];
	}
	delete [] H;
	delete [] N;
	delete [] D;
}

// Updates the water using complex waves
// Done by overlaying cos/sin waves
void Water::update(){
	for (int i = 0; i < MS; i++)
		for (int j = 0; j < MS; j++)
			H[i][j] = -0.2;
	double vamp = amplitude;
	double vlen = wavelength;
	double vel = 0.1 * sqrt(wavelength);
	for (int n = 0; n < SAMPLES; n++){
		for (int i = 0; i < MS; i++)
		{
			double x = float((2.0 * M_PI * i) / MS);
			for (int j = 0; j < MS; j++)
			{
				double y = float((2.0 * M_PI * j) / MS);
				H[i][j] += vamp * cos((x * cos(D[n]) + y * sin(D[n]) - vel * time) / vlen);
			}
		}
		
		vamp *= amplituderatio;
		vlen *= wavelengthratio;
		vel = 0.1 * sqrt(wavelength);
	}
	generate_normals();
	time += 0.1;
}

// Draws the water
void Water::display(float dt){
	GLfloat material_diffuse[] = {0.4,0.4,0.4,1};
	GLfloat material_specular[] = {0.8,0.8,0.8,1};
	GLfloat material_shininess[] = { 120 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glBegin(GL_TRIANGLES);

    const float offset = MS/400.0f;
	for (int i = 0; i < MS-1; i++){
		for (int j = 0; j < MS-1; j++){
			float x = i * offset - MS/8.0f;
			float y = j * offset - MS/8.0f;

			vector3f normal = ((N[i][j] + N[i+1][j] + N[i][j+1] + N[i+1][j+1])*(0.3333333f)).unit();

			glNormal3f(N[i][j].x,N[i][j].y,N[i][j].z);
			glVertex3f(x,H[i][j],y);

			glNormal3f(N[i][j+1].x,N[i][j+1].y,N[i][j+1].z);
			glVertex3f(x,H[i][j+1],y+offset);

			glNormal3f(N[i+1][j].x,N[i+1][j].y,N[i+1][j].z);
			glVertex3f(x+offset,H[i+1][j],y);

			glNormal3f(N[i+1][j].x,N[i+1][j].y,N[i+1][j].z);
			glVertex3f(x+offset,H[i+1][j],y);

			glNormal3f(N[i][j+1].x,N[i][j+1].y,N[i][j+1].z);
			glVertex3f(x,H[i][j+1],y+offset);

			glNormal3f(N[i+1][j+1].x,N[i+1][j+1].y,N[i+1][j+1].z);
			glVertex3f(x+offset,H[i+1][j+1],y+offset);
		}
	}
	glEnd();
}

// Generates the normals for the water
void Water::generate_normals(){
	vector3f p1,p2,p3,u,v;
	p1 = vector3f(0,H[0][0],0);
	p2 = vector3f(1,H[1][0],0);
	p3 = vector3f(0,H[0][1],1);
	u = p1 - p2;
	v = p3 - p1;
	N[0][0] = crossProduct(u,v).unit();

	p1 = vector3f(MS-1,H[MS-1][0],0);
	p2 = vector3f(MS-2,H[MS-2][0],0);
	p3 = vector3f(MS-1,H[MS-1][1],1);
	u = p2 - p1;
	v = p3 - p1;
	N[MS-1][0] = crossProduct(u,v).unit();

	p1 = vector3f(MS-1,H[MS-1][MS-1],MS-1);
	p2 = vector3f(MS-2,H[MS-2][MS-1],MS-1);
	p3 = vector3f(MS-1,H[MS-1][MS-2],MS-2);
	u = p1 - p2;
	v = p3 - p1;
	N[MS-1][MS-1] = crossProduct(u,v).unit();

	p1 = vector3f(0,H[0][MS-1],MS-1);
	p2 = vector3f(1,H[1][MS-1],MS-1);
	p3 = vector3f(0,H[0][MS-2],MS-2);
	u = p2 - p1;
	v = p3 - p1;
	N[0][MS-1] = crossProduct(u,v).unit();

	for (int i = 1; i < MS-1; ++i){
		p1 = vector3f(i,H[i][0],0);
		p2 = vector3f(i+1,H[i+1][0],0);
		p3 = vector3f(i,H[i][1],1);
		u = p1 - p2;
		v = p3 - p1;
		N[i][0] = crossProduct(u,v).unit();

		p1 = vector3f(i,H[i][MS-1],MS-1);
		p2 = vector3f(i+1,H[i+1][MS-1],MS-1);
		p3 = vector3f(i,H[i][MS-2],MS-2);
		u = p2 - p1;
		v = p3 - p1;
		N[i][MS-1] = crossProduct(u,v).unit();

		p1 = vector3f(0,H[0][i],i);
		p2 = vector3f(0,H[0][i+1],i+1);
		p3 = vector3f(1,H[1][i],i);
		u = p2 - p1;
		v = p3 - p1;
		N[0][i] = crossProduct(u,v).unit();
		
		p1 = vector3f(MS-1,H[MS-1][i],i);
		p2 = vector3f(i+1,H[MS-1][i+1],MS-1);
		p3 = vector3f(i,H[MS-2][i],MS-2);
		u = p1 - p2;
		v = p3 - p1;
		N[MS-1][i] = crossProduct(u,v).unit();

		for (int j = 1; j < MS-1; ++j){
			vector3f p1(i,H[i][j],j), p2(i+1,H[i+1][j],j), p3(i,H[i][j+1],j+1);
			vector3f u = p1 - p2;
			vector3f v = p3 - p1;

			N[i][j] = crossProduct(u,v).unit();
		}
	}
}