# Path to v-rep installation directory
VREP_FOLDER?=/opt/V-REP_PRO_EDU_V3_1_0_64_Linux/

P=$(VREP_FOLDER)/programming

#CXX=g++
#CC=gcc
CFLAGS = -Wall -g3 -O0 -I$(P)/remoteApi -I$(P)/include -Iinclude -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -DUSE_OPENCV
LDFLAGS = -lpthread -lopencv_highgui -lopencv_core -lopencv_imgproc

OBJS_SERVER = server.o
OBJS_SERVER_VREP = $(OBJS_SERVER) $(P)/remoteApi/extApi.o $(P)/remoteApi/extApiPlatform.o src/robot-vrep.o
OBJS_SERVER_VBOT = server.o src/robot-vbot.o
OBJS_CLIENT = client.o
OBJS_CLIENT_JOYSTICK = $(OBJS_CLIENT) src/client-joystick.o
OBJS_CLIENT_KEYBOARD = $(OBJS_CLIENT) src/client-keyboard.o

OS = $(shell uname -s)
ECHO = @

ifeq ($(OS), Linux)
	CFLAGS += -D__linux
else
	CFLAGS += -D__APPLE__
endif

all: vrep-server client
	
vrep-server: $(OBJS_SERVER_VREP)
		@echo "Linking $(OBJS_SERVER_VREP) to server-vrep"
		$(ECHO)$(CXX) $(CFLAGS) $(OBJS_SERVER_VREP) -o server-vrep $(LDFLAGS)

vbot-server: $(OBJS_SERVER_VBOT)
		@echo "Linking $(OBJS_SERVER_VBOT) to server-vbot"
		$(ECHO)$(CXX) $(CFLAGS) $(OBJS_SERVER_VBOT) -o server-vbot $(LDFLAGS) libVMC.a

client: $(OBJS_CLIENT_JOYSTICK) $(OBJS_CLIENT_KEYBOARD)
		@echo "Linking $(OBJS_CLIENT_KEYBOARD) to client-keyboard"
		$(ECHO)$(CXX) $(CFLAGS) $(OBJS_CLIENT_KEYBOARD) -o client-keyboard $(LDFLAGS)
		@echo "Linking $(OBJS_CLIENT_JOYSTICK) to client-joystick"
		$(ECHO)$(CXX) $(CFLAGS) $(OBJS_CLIENT_JOYSTICK) -o client-joystick $(LDFLAGS)

%.o: %.cpp
		@echo "Compiling $< to $@"
		$(ECHO)$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.c
		@echo "Compiling $< to $@" 
		$(ECHO)$(CC) $(CFLAGS) -c $< -o $@

%32.o: %.c
		@echo "Compiling $< to $@" 
		$(ECHO)$(CC) -m32 $(CFLAGS) -c $< -o $@

%32.o: %.cpp
		@echo "Compiling $< to $@"
		$(ECHO)$(CXX) -m32 $(CFLAGS) -c $< -o $@

clean:
		@rm -f server-vbot server-vrep client-keyboard client-joystick \
		$(OBJS_CLIENT_JOYSTICK) \
		$(OBJS_CLIENT_KEYBOARD) \
		$(OBJS_SERVER_VBOT) \
		$(OBJS_SERVER_VREP)

project=$(shell pwd)
startsim:
		cd $(VREP_FOLDER) && \
		./vrep.sh $(project)/bubblerob.ttt -s
