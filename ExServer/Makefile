ifeq ($(shell uname),Linux)
	BIN = /var/www/authoring/converter/bin/linux64/ExServer
else
	BIN = /Users/toshi/SDK/Communicator/HOOPS_Communicator_2023_U1/authoring/converter/bin/macos/ExServer
endif

OBJS = main.cpp utilities.cpp ExProcess.cpp AddAttributesVisitor.cpp ClassifyEntityVisitor.cpp FeatureRecognitionVisitor.cpp ./visitor/BrepTraverse.cpp ./visitor/CascadedAttributeConnector.cpp ./visitor/MarkupTessConnector.cpp ./visitor/MarkupTraverse.cpp ./visitor/Matrix.cpp ./visitor/TessConnector.cpp ./visitor/TransfoConnector.cpp ./visitor/TreeTraverse.cpp ./visitor/ViewTraverse.cpp ./visitor/VisitorCascadedAttribute.cpp ./visitor/VisitorContainer.cpp ./visitor/VisitorTessellation.cpp ./visitor/VisitorTransfo.cpp ./visitor/VisitorTree.cpp
CC = g++ -std=c++11

ifeq ($(shell uname),Linux)
	PATH_TO_INCLUDES = -I /usr/local/include \
					-I /var/hoops/HOOPS_Exchange_Publish_2023_U1/include \
					-I /var/www/authoring/libconverter/include

	PATH_TO_LIBCONVERTER = /var/www/authoring/converter/bin/linux64
else
	PATH_TO_INCLUDES = -I /usr/local/include \
			-I /Users/toshi/SDK/Exchange/HOOPS_Exchange_2023_U1/include \
			-I /Users/toshi/SDK/Communicator/HOOPS_Communicator_2023_U1/authoring/libconverter/include

	PATH_TO_LIBCONVERTER = /Users/toshi/SDK/Communicator/HOOPS_Communicator_2023_U1/authoring/converter/bin/macos
endif

PATH_TO_LIBMHD_LIBS = /usr/local/lib

LDLIBS = -ldl -lstdc++ \
	 -L$(PATH_TO_LIBMHD_LIBS) \
	 -lmicrohttpd \
	 -L$(PATH_TO_LIBCONVERTER) \
	 -lconverter \
	 -lstdc++fs

$(BIN): $(OBJS)
	$(CC) $(PATH_TO_INCLUDES) $(OBJS) -o $(BIN) $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(BIN)