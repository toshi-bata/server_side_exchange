ifeq ($(shell uname),Linux)
	BIN = /var/www/authoring/converter/bin/linux64/ProcessServer
else
	BIN = /Users/toshi/SDK/Communicator/HOOPS_Communicator_2023_U1/authoring/converter/bin/macos/ProcessServer
endif

OBJS = main.cpp utilities.cpp ProcessManager.cpp
CC = g++ -std=c++11

PATH_TO_INCLUDES = -I /usr/local/include \

PATH_TO_LIBMHD_LIBS = /usr/local/lib

LDLIBS = -ldl -lstdc++ \
	 -L$(PATH_TO_LIBMHD_LIBS) \
	 -lmicrohttpd

$(BIN): $(OBJS)
	$(CC) $(PATH_TO_INCLUDES) $(OBJS) -o $(BIN) $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(BIN)