# Written by Lee Mracek
CC = gcc
CXX = g++
LD = g++
RM = rm
CP = cp
MV = mv
MKDIR = mkdir
TOUCH = touch
SED = sed
MAKEDEPEND = makedepend

LIB = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect
CPPSRCS = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
SRCS = $(CPPSRCS) $(CSRCS)

CFLAGS_DEBUG = -g -D_LAZER_DEBUG
OBJS_DEBUG = $(patsubst src/%.cpp,obj/%-dbg.o, $(SRCS))
CFLAGS_RELEASE = -O3 -Wno-unused-value
OBJS_RELEASE = $(patsubst src/%.cpp,obj/%.o,$(SRCS))

CFLAGS = -Iinclude -static-libgcc -Wall -fno-use-linker-plugin -fno-exceptions -shared -fPIC
CXXFLAGS = -static-libstdc++ -std=c++11 -Wall -Wextra -fexceptions

deploy: .build_dir build/lazer-vision.so

debug: .build_dir build/lazer-vision-dbg.so

build/lazer-vision.so: $(OBJS_RELEASE)
	$(LD) $(CFLAGS_RELEASE) $(CFLAGS) $(CXXFLAGS) -o $@ $^ $(LIB)

build/lazer-vision-dbg.so: $(OBJS_DEBUG)
	$(LD) $(CFLAGS_DEBUG) $(CFLAGS) $(CXXFLAGS) -o $@ $^ $(LIB)

clean:
	$(RM) -rf build/ obj/ .build_dir

### build objects ###
obj: $(OBJ_RELEASE)

obj-debug: $(OBJ_DEBUG)

obj/%-dbg.o: src/%.cpp
	$(CXX) $(CFLAGS_DEBUG) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<

obj/%.o: src/%.cpp
	$(CXX) $(CFLAGS_RELEASE) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<

### create file tree ###
.build_dir:
	$(MKDIR) obj obj/input obj/filters obj/logging obj/utils \
		build
	$(TOUCH) .build_dir
