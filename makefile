DIR=src
ODIR=obj
CC = g++
PFLAGS = -O3 -ffast-math -g -c -w
CFLAGS = -lmingw32 -lSDLmain -lSDL SDL_image.lib -lopengl32 -lglu32 -static-libgcc -static-libstdc++

OBJ = $(ODIR)/game.o $(ODIR)/game_cleanup.o $(ODIR)/game_events.o $(ODIR)/game_init.o $(ODIR)/game_loop.o $(ODIR)/game_render.o $(ODIR)/vector3f.o $(ODIR)/Maths.o $(ODIR)/Camera.o $(ODIR)/Map.o $(ODIR)/tile.o $(ODIR)/water.o $(ODIR)/birds.o $(ODIR)/node.o $(ODIR)/grass.o $(ODIR)/unit.o $(ODIR)/game_handler.o $(ODIR)/fireball.o $(ODIR)/explosion.o

main.exe: $(OBJ)
	$(CC) $(ODIR)/*.o -o main.exe $(CFLAGS)

$(ODIR)/game.o: $(DIR)/game.cpp $(DIR)/game.h
	$(CC) $(PFLAGS) $(DIR)/game.cpp -o $(ODIR)/game.o

$(ODIR)/game_cleanup.o: $(DIR)/game_cleanup.cpp $(DIR)/game.h
	$(CC) $(PFLAGS) $(DIR)/game_cleanup.cpp -o $(ODIR)/game_cleanup.o

$(ODIR)/game_handler.o: $(DIR)/game_handler.cpp $(DIR)/game.h
	$(CC) $(PFLAGS) $(DIR)/game_handler.cpp -o $(ODIR)/game_handler.o

$(ODIR)/game_events.o: $(DIR)/game_events.cpp $(DIR)/game.h
	$(CC) $(PFLAGS) $(DIR)/game_events.cpp -o $(ODIR)/game_events.o

$(ODIR)/game_init.o: $(DIR)/game_init.cpp $(DIR)/game.h
	$(CC) $(PFLAGS) $(DIR)/game_init.cpp -o $(ODIR)/game_init.o

$(ODIR)/game_loop.o: $(DIR)/game_loop.cpp $(DIR)/game.h
	$(CC) $(PFLAGS) $(DIR)/game_loop.cpp -o $(ODIR)/game_loop.o

$(ODIR)/game_render.o: $(DIR)/game_render.cpp $(DIR)/game.h
	$(CC) $(PFLAGS) $(DIR)/game_render.cpp -o $(ODIR)/game_render.o

$(ODIR)/Camera.o: $(DIR)/Camera.h $(DIR)/Camera.cpp
	$(CC) $(PFLAGS) $(DIR)/Camera.cpp -o $(ODIR)/Camera.o

$(ODIR)/vector3f.o: $(DIR)/Math/vector3f.h $(DIR)/Math/vector3f.cpp
	$(CC) $(PFLAGS) $(DIR)/Math/vector3f.cpp -o $(ODIR)/vector3f.o

$(ODIR)/Maths.o: $(DIR)/Math/Maths.h $(DIR)/Math/Maths.cpp
	$(CC) $(PFLAGS) $(DIR)/Math/Maths.cpp -o $(ODIR)/Maths.o

$(ODIR)/tile.o: $(DIR)/Map/tile.h $(DIR)/Map/tile.cpp
	$(CC) $(PFLAGS) $(DIR)/Map/tile.cpp -o $(ODIR)/tile.o

$(ODIR)/Map.o: $(DIR)/Map/Map.h $(DIR)/Map/Map.cpp
	$(CC) $(PFLAGS) $(DIR)/Map/Map.cpp -o $(ODIR)/Map.o

$(ODIR)/node.o: $(DIR)/Map/node.h $(DIR)/Map/node.cpp
	$(CC) $(PFLAGS) $(DIR)/Map/node.cpp -o $(ODIR)/node.o

$(ODIR)/sand.o: $(DIR)/Systems/sand.h $(DIR)/Systems/sand.cpp
	$(CC) $(PFLAGS) $(DIR)/Systems/sand.cpp -o $(ODIR)/sand.o

$(ODIR)/water.o: $(DIR)/Systems/water.h $(DIR)/Systems/water.cpp
	$(CC) $(PFLAGS) $(DIR)/Systems/water.cpp -o $(ODIR)/water.o

$(ODIR)/birds.o: $(DIR)/Systems/birds.h $(DIR)/Systems/birds.cpp
	$(CC) $(PFLAGS) $(DIR)/Systems/birds.cpp -o $(ODIR)/birds.o

$(ODIR)/grass.o: $(DIR)/Systems/grass.h $(DIR)/Systems/grass.cpp
	$(CC) $(PFLAGS) $(DIR)/Systems/grass.cpp -o $(ODIR)/grass.o

$(ODIR)/fireball.o: $(DIR)/Systems/fireball.h $(DIR)/Systems/fireball.cpp
	$(CC) $(PFLAGS) $(DIR)/Systems/fireball.cpp -o $(ODIR)/fireball.o

$(ODIR)/explosion.o: $(DIR)/Systems/explosion.h $(DIR)/Systems/explosion.cpp
	$(CC) $(PFLAGS) $(DIR)/Systems/explosion.cpp -o $(ODIR)/explosion.o

$(ODIR)/unit.o: $(DIR)/Units/unit.h $(DIR)/Units/unit.cpp
	$(CC) $(PFLAGS) $(DIR)/Units/unit.cpp -o $(ODIR)/unit.o

clean:
	rm $(ODIR)/*.o

cleanall:
	rm $(ODIR)/*.o main.exe