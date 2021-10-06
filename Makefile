#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32
CLEAN=rm hw5.exe
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=2
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f hw5-Dumitrescu *.o *.a
endif

#  Compile and link
hw5:hw5-Dumitrescu.c
	gcc $(CFLG) -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
