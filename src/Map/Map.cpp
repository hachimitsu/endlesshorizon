#include "Map.h"

int map_layout[][23] = {
		{0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,5,5,5,2,2,0,1,1,1,1,1,0,0,0},
		{0,0,0,2,2,2,2,2,0,2,5,2,2,2,2,2,2,3,1,1,1,1,0},
		{0,2,2,2,2,4,4,4,2,2,2,2,2,2,2,2,2,3,3,3,1,1,0},
		{0,2,2,2,4,4,4,4,4,4,2,0,2,2,2,2,2,2,3,3,1,1,0},
		{0,2,2,2,4,4,4,4,4,4,4,0,0,0,0,0,2,2,3,3,1,0,0},
		{2,3,3,3,2,2,4,2,4,2,0,0,0,0,0,0,2,2,3,3,1,1,0},
		{1,3,3,3,3,3,2,2,2,2,1,1,0,0,0,1,1,3,2,3,3,1,0},
		{1,3,3,3,3,3,2,2,2,2,3,3,1,1,1,1,1,3,2,2,1,2,1},
		{1,1,3,3,3,3,2,0,2,2,2,3,3,3,1,3,3,2,2,4,2,4,2},
		{1,1,1,1,3,1,1,0,0,0,2,2,3,3,3,3,3,2,2,4,4,4,4},
		{0,0,1,1,1,0,0,0,0,0,0,0,2,2,2,2,2,0,2,0,4,4,4},
		{0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,5,5,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,2,5,5,5,5,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0}
	};	

Map::~Map(){ 
	for (int i = 0; i < 12; i++){
		delete [] layout[i];
	}
	delete [] layout;

	grass_system.cleanup();
}

Map::Map(){
	grass_mode = true;
}

// Initializes the map with given layout
void Map::init(){
	width = 23;
	length = 15;

	layout = new tile*[length];
	for (int i = 0; i < length; ++i){
		layout[i] = new tile[width];
	}
	const float radius = 0.3;
	const float offsety = sqrt(radius*radius - radius/2*radius/2);
	const float offsetx = (radius * 2) * 0.75;

	vector3f wpos(-(width * radius * 2 * 0.75 + radius * 2 * 0.25)/2.0f, 0, -(length * offsetx * 2 + offsety)/2.0f);
	vector3f pos(0,0,0);

	colour c[6], p;

	set_colour(c[0],0,0,0,255);
	set_colour(c[1],255,246,215,255);
	set_colour(c[2],130,212,76,255);
	set_colour(c[3],228,238,162,255);
	set_colour(c[4],86,178,68,255);
	set_colour(c[5],212,200,181,255);
	set_colour(p,0,0,0,255);

	for (int i = 0; i < length; i++){
		wpos.z = -(length * offsety * 2 + offsety)/2.0f + i*offsety*2;
		wpos.x = -(width * radius * 2 * 0.75 + radius * 2 * 0.25)/2.0f;
		pos.x = 0;
		pos.y = i;
		pos.z = -i;
		for (int j = 0; j < width; j++){
			if( map_layout[i][j] != 0 ){
				layout[i][j] = tile(pos, wpos, c[map_layout[i][j]], p, true, true, radius);
			}
			else
				layout[i][j] = tile(pos, wpos, c[map_layout[i][j]], p, false, false, radius);

			if( p.r < 255 )
				p.r += 1;
			else if( p.g < 255 )
				p.g += 1;
			else
				p.b += 1;

			pos.x++;
			if( j%2 == 1 ){
				pos.y--;
				wpos.z -= offsety;
			}
			else
				wpos.z += offsety;
			pos.z = -(pos.x + pos.y);
			wpos.x += offsetx;
		}
	}

	grass_system.init(vector3f(0,0,0),0.28f,0.05f,0.008f,150); 
}

// Updates the systems within the map
void Map::update(){
	if( grass_mode)
		grass_system.update();
}

// Draws the map and its systems
void Map::display(vector3f camera){
	static GLuint index;

	if( !index ){
		index = glGenLists(1);
		glNewList(index,GL_COMPILE);
		for (int i = 0; i < length; ++i){
			for (int j = 0; j < width; ++j){
				if( layout[i][j].is_visible() )
					layout[i][j].display();
			}
		}
		glEndList();
	}

	GLfloat material_diffuse[] = {0.5,0.5,0.5,1};
	GLfloat material_specular[] = {0.0,0.0,0.0,1};
	GLfloat material_shininess[] = { 20 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	glCallList(index);

	GLuint grass = glGenLists(1);
	glNewList(grass,GL_COMPILE);
	grass_system.display(vector3f(0,0,0),vector3f(1,1,1));
	glEndList();
	if( grass_mode ){
		for (int i = 0; i < length; ++i){
			for (int j = 0; j < width; ++j){
				if( map_layout[i][j] == 3 ){
					vector3f p = layout[i][j].get_wpos();
					// float line_width = 2 * 1.0f/(camera - p).magnitude();
					// glLineWidth(line_width);
					glPushMatrix();
						glTranslatef(p.x,p.y,p.z);
						glCallList(grass);
					glPopMatrix();
				}
			}
		}
	}
}

// Draws only the maps tiles with their picking colour
void Map::display_pick(){
	static GLuint index;

	if( !index ){
		index = glGenLists(1);
		glNewList(index,GL_COMPILE);
		for (int i = 0; i < length; ++i){
			for (int j = 0; j < width; ++j){
				if( layout[i][j].is_visible() )
					layout[i][j].display_pick();
			}
		}
		glEndList();
	}

	glCallList(index);
}

// Flips the traversable property for a given position on the map
void Map::flip_traversable(vector3f v){
	int x = v.x;
	int y = v.y + floor(x/2.0f);
	layout[y][x].flip_traversable();
}

// Gets the tile with given picking colour
tile Map::get_tile(colour c){
	for (int i = 0; i < length; ++i){
		for (int j = 0; j < width; ++j){
			colour t = layout[i][j].get_pick_colour();
			if( c.r == t.r && c.g == t.g && c.b == t.b )
				return layout[i][j];
		}
	}
	return tile();
}

// Gets the tile at given position
tile Map::get_tile(vector3f v){
	int x = v.x;
	int y = v.y + floor(x/2.0f);

	return layout[y][x];
}

// Gets the tile at given array position
tile Map::get_tile(int x, int y){
	return layout[y][x];
}

// Generates the movement area with given radius
std::vector<tile> Map::generate_area(tile t, int radius){
	std::vector<tile> v;
	for (int i = 0; i < length; i++){
		for (int j = 0; j < width; j++){
			if( layout[i][j] != t && t.distance(layout[i][j]) <= radius && traversable(t, layout[i][j], radius) )
				v.push_back(layout[i][j]);
		}
	}

	return v;
}

// Checks if it's possible to move from one tile to the other in given steps
bool Map::traversable(tile o, tile t, int depth){
	std::vector<tile> path = generate_path(o,t,depth);

	if( path.size() > 0 )
		return true;

	return false;
}

// Finds the path from one tile to another given a certain number of steps
// Uses A* search
std::vector<tile> Map::generate_path(tile o, tile t, int depth){
	std::vector<Node*> open,closed,children;
	std::vector<tile> path;
	bool found = false;
	Node *n = new Node;
	n->t = o;
	n->c = t.distance(o);
	n->n = NULL;
	n->s = 0;
	open.push_back(n);

	while( open.size() > 0 ){
		n = open[0];

		if( n->c == 0 ){
			found = true;
			break;
		}
				
		if( n->s < depth )		
			children = expand(n,t);
		closed.push_back(n);
		open.erase(open.begin());

		for (int i = 0; i < children.size(); ++i){
			if( exists( closed, children[i]) )
				continue;
			if( !exists( open, children[i]) )
				open.push_back( children[i] );
		}

		sort(open);
		children.clear();
	}

	if( found ){
		while( n ){
			path.push_back(n->t);
			n = n->n;
		}
	}

	return path;
}

// Expands the given node and returns a list of its children
// Assigned a heuristic value based on the distance from the goal
std::vector<Node*> Map::expand(Node *t, tile g){
	std::vector<Node*> v;
	int x = t->t.get_pos().x;
	int y = t->t.get_pos().y + floor(x/2.0f);

	if( y-1 > -1 && layout[y-1][x].is_traversable() ){
		Node *n = new Node;
		n->t = layout[y-1][x];
		n->c = g.distance(layout[y-1][x]);
		n->s = t->s + 1;
		n->n = t;
		v.push_back( n );
	}
	if( y+1 < length && layout[y+1][x].is_traversable() ){
		Node *n = new Node;
		n->t = layout[y+1][x];
		n->c = g.distance(layout[y+1][x]);
		n->s = t->s + 1;
		n->n = t;
		v.push_back( n );
	}
	if( x-1 > -1 && layout[y][x-1].is_traversable() ){
		Node *n = new Node;
		n->t = layout[y][x-1];
		n->c = g.distance(layout[y][x-1]);
		n->s = t->s + 1;
		n->n = t;
		v.push_back( n );
	}
	if( x+1 < width && layout[y][x+1].is_traversable() ){
		Node *n = new Node;
		n->t = layout[y][x+1];
		n->c = g.distance(layout[y][x+1]);
		n->s = t->s + 1;
		n->n = t;
		v.push_back( n );
	}

	if( x%2 == 0 ){
		if( x-1 > -1 && y-1 > -1 && layout[y-1][x-1].is_traversable() ){
			Node *n = new Node;
			n->t = layout[y-1][x-1];
			n->c = g.distance(layout[y-1][x-1]);
			n->s = t->s + 1;
			n->n = t;
			v.push_back( n );
		}
		if( x+1 < width && y-1 > -1 && layout[y-1][x+1].is_traversable() ){
			Node *n = new Node;
			n->t = layout[y-1][x+1];
			n->c = g.distance(layout[y-1][x+1]);
			n->s = t->s + 1;
			n->n = t;
			v.push_back( n );
		}
	}
	else{
		if( x-1 > -1 && y+1 < length && layout[y+1][x-1].is_traversable() ){
			Node *n = new Node;
			n->t = layout[y+1][x-1];
			n->c = g.distance(layout[y+1][x-1]);
			n->s = t->s + 1;
			n->n = t;
			v.push_back( n );
		}
		if( x+1 < width && y+1 < length && layout[y+1][x+1].is_traversable() ){
			Node *n = new Node;
			n->t = layout[y+1][x+1];
			n->c = g.distance(layout[y+1][x+1]);
			n->s = t->s + 1;
			n->n = t;
			v.push_back( n );
		}
	}

	return v;
}

// Checks if a node already exists in the list
bool Map::exists(std::vector<Node*> l, Node *n){
	for (int i = 0; i < l.size(); ++i){
		if ( l[i]->t == n->t ){
			return true;
		}
	}
	return false;
}

// Sorts the node list for faster A* searching
// Uses insertion sort
void Map::sort(std::vector<Node*> &t){
	int size = t.size();

	for (int i = 1; i < size; ++i){
		int pos = i;
		Node *val = t[i];
		while( pos > 0 && val->c < t[pos-1]->c ){
			t[pos] = t[pos-1];
			pos--;
		}
		t[pos] = val;
	}
}