#Makefile - Trent Ellingsen
CC=g++
CFLAGS=-Wall -lglut -lGL -lGLU -g -O4
#CFLAGS=-framework OpenGL -framework GLUT
LDFLAGS=

ALL=part

all: $(ALL)

part: lib/*.o main.o camera.o obj.o obj2d.o part.o ground.o model.o tex.o sprite.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: src/main.cpp src/*.h
	$(CC) $(CFLAGS) -c $<

model.o: src/model.cpp src/model.h
	$(CC) $(CFLAGS) -c $<

obj.o: src/obj.cpp src/obj.h
	$(CC) $(CFLAGS) -c $<

obj2d.o: src/obj2d.cpp src/obj2d.h
	$(CC) $(CFLAGS) -c $<

part.o: src/part.cpp src/part.h
	$(CC) $(CFLAGS) -c $<

camera.o: src/camera.cpp src/camera.h
	$(CC) $(CFLAGS) -c $<

tex.o: src/tex.cpp src/tex.h src/SOIL.h
	$(CC) $(CFLAGS) -c $<

sprite.o: src/sprite.cpp src/sprite.h
	$(CC) $(CFLAGS) -c $<

ground.o: src/ground.cpp src/ground.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf core* *.o *.gch src/*.gch $(ALL)
