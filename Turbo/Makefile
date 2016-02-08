#
# Makefile for CG exercises for Linux and MacOS X
# On Windows Visual Studio can be used (you will have to build 
# a project file yourself).
#
CC     = cc
CPP    = c++
CPPFLAGS = -I../glfw/include -I../glloadgen -I../glm/glm -pthread -O2 -Wall

GROUP = game

#
# OS detection
#
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
EXTRA_DEFINES  = -DLINUX
LIBS         = ../glfw/src/libglfw3.a
LFLAGS       = $(LIB_LINUX) -lXrandr -lGL -lXi -pthread -lm -lX11 -lXxf86vm
endif
ifeq ($(UNAME), Darwin)
EXTRA_DEFINES  = -DLION
LIBS         = ../glfw/src/libglfw3.a
LFLAGS       = $(LIB_MAC) -lGLEW -framework GLUT -framework Cocoa -framework OpenGL -framework IOKit
endif

PROJECTFILES	= lodepng.o heightmapper.o camera.o shaderprogram.o obj_loader.o mesh.o
OBJECTFILES      = turbo.o ../glloadgen/gl_core_32.o ../glloadgen/KHR_debug_emulator.o $(PROJECTFILES)


# all: 
	# g++ Turbo.cpp 

game: $(OBJECTFILES) $(LIBS)
	$(CPP) $(OBJECTFILES) $(LIBS) $(LFLAGS) -o $(GROUP)

turbo.o: turbo.cpp
	$(CPP) $(CPPFLAGS) $(EXTRA_DEFINES) -c turbo.cpp -o turbo.o

../glloadgen/gl_core_32.o: ../glloadgen/gl_core_32.cc
	$(CPP) $(CPPFLAGS) -Wno-write-strings -c ../glloadgen/gl_core_32.cc -o ../glloadgen/gl_core_32.o

../glloadgen/KHR_debug_emulator.o: ../glloadgen/KHR_debug_emulator.cc
	$(CPP) $(CPPFLAGS) -Wno-write-strings -c ../glloadgen/KHR_debug_emulator.cc -o ../glloadgen/KHR_debug_emulator.o

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $<

../glfw/src/libglfw3.a:
	( cd ../glfw && cmake CMakeLists.txt && make )

lodepng.o: lodepng.cpp
	$(CPP) $(CPPFLAGS) -Wno-write-strings -c lodepng.cpp -o lodepng.o

heightmapper.o: HeightMapper.cpp
	$(CPP) $(CPPFLAGS) -Wno-write-strings -c HeightMapper.cpp -o heightmapper.o

camera.o: Camera.cpp
	$(CPP) $(CPPFLAGS) -Wno-write-strings -c Camera.cpp -o camera.o

shaderprogram.o: ShaderProgram.cpp
	$(CPP) $(CPPFLAGS) -Wno-write-strings -c ShaderProgram.cpp -o shaderprogram.o

obj_loader.o: obj_loader.cpp
	$(CPP) $(CPPFLAGS) -Wno-write-strings -c obj_loader.cpp -o obj_loader.o

mesh.o: Mesh.cpp
	$(CPP) $(CPPFLAGS) -Wno-write-strings -c Mesh.cpp -o mesh.o

clean:
	rm -f *.o
	rm -f $(GROUP)

mrproper:
	rm -f *.o
	rm -f $(GROUP)
	rm -f ../shared/tools.o
	rm -f ../glloadgen/gl_core_32.o
	rm -f ../glfw/lib/cocoa/libglfw.a
	rm -f ../glfw/lib/x11/libglfw.a



