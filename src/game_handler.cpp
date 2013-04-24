#include "game.h"

// Handles mouse colour picking
void Game::process_pick(){
	// Gets what colour was picked at position mouse was clicked
	GLubyte pixel[3];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(pickx, winHeight-picky, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
	colour picked_colour;
	set_colour(picked_colour,pixel[0],pixel[1],pixel[2],255);

	// Find the tile that was clicked
	picked_tile = world.get_tile(picked_colour);
	bool reset = false;
	int picked_unit;
	
	if( picked && picked_colour.r == 0 && picked_colour.g == 0 && picked_colour.b == 255 ){
		active_team = ( active_team == 0 ) ? 1:0;
		move_path.clear();
		move_area.clear();
		disable  = false;
		current  = tile();
		target   = tile();
		target_unit = -2;
		selected_unit = -1;
		for (int i = 0; i < units.size(); ++i)
			units[i].reset();
	}
	// If the tile is traversable, make some decisions
	else if( picked && picked_tile.is_traversable() ){
		// Checks if there is an active unit on the tile
		if( (picked_unit = unit_ontile(picked_tile)) > -1 && units[picked_unit].visibleq() ){
			// If it is a unit that has already been selected, deselect it
			if( picked_unit == selected_unit ){
				reset = true;
				selected_unit = -1;
			}
			// If it is a unit on your team and it can still act, select it
			else if( units[picked_unit].get_team() == active_team && (!units[picked_unit].movedq() || !units[picked_unit].attackedq()) ){
				// If it can still move, generate the area that it can move in
				if( !units[picked_unit].movedq() ){
					fliptiles(active_team);
					move_area = world.generate_area(picked_tile, 3);
					fliptiles(active_team);
				}
				move_path.clear();
				selected_unit = picked_unit;
				current = picked_tile;
				target_unit = -2;
				target = tile();
			}
			// If it is a unit on the other team, target it for attacking if able to attack and the enemy unit is in range
			else if( units[picked_unit].get_team() != active_team && selected_unit > -1 && !units[selected_unit].attackedq() && picked_tile.distance(units[selected_unit].get_pos()) <= units[selected_unit].get_attack_range() ){
				if( picked_unit == target_unit ){
					units[selected_unit].fire(picked_tile.get_wpos());
					disable = true;
				}
				else{
					move_path.clear();
					target = picked_tile;
					target_unit = picked_unit;
				}
			}
			else{
				target = picked_tile;
				target_unit = picked_unit;
			}
		}
		// If there is an active selected unit, move it to the selected tile if it is in range or deselect it if it is out of range.
		else if( selected_unit > -1 ){
			if( picked_tile == target ){
				units[selected_unit].move(move_path);
				disable = true;
			}
			else{
				bool in_area = false;
				for (int i = 0; i < move_area.size(); ++i){
					if( move_area[i] == picked_tile ){
						fliptiles(active_team);
						move_path = world.generate_path(move_area[i], current, 10);
						fliptiles(active_team);
						target = move_area[i];
						in_area = true;
						target_unit = -2;
						break;
					}
				}
				if( !in_area ){
					reset = true;
					selected_unit = -1;
				}
			}
		}
		else{
			reset = true;
			selected_unit = -1;
		}
	}
	else{
		reset = true;
		selected_unit = -1;
	}

	if( reset ){
		move_path.clear();
		move_area.clear();
		disable  = false;
		current  = tile();
		target   = tile();
		target_unit = -2;
	}

	picked = false;
}

// Checks if a unit in on a tile
int Game::unit_ontile(tile t){
	for (int i = 0; i < units.size(); ++i){
		if( units[i].get_pos() == t.get_pos() )
			return i;
	}
	return -1;
}

// Flips the traversable property of tiles where enemy tiles reside on to so that they count as obstacles for path finding.
void Game::fliptiles(int t){
	for (int i = 0; i < units.size(); ++i){
		if( units[i].get_team() != t && units[i].visibleq() )
			world.flip_traversable(units[i].get_pos());
	}
}