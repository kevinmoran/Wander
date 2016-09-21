BIN = wander
CC = g++
FLAGS = -Wall -pedantic -g -mmacosx-version-min=10.5 -arch x86_64 -fmessage-length=0 -UGLFW_CDECL -fprofile-arcs -ftest-coverage
INC = -I common/include -I/sw/include -I/usr/local/include
LIB_PATH = common/osx_64/
LOC_LIB = $(LIB_PATH)libGLEW.a $(LIB_PATH)libglfw3.a
FRAMEWORKS = -framework Cocoa -framework OpenGL -framework IOKit
SRC = main.cpp

all:
	rm -f *.gcda
	${CC} ${FLAGS} ${FRAMEWORKS} -o ${BIN} ${SRC} ${INC} ${LOC_LIB}
