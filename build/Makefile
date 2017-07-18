CC = g++

CFLAGS = -Wall -std=c++11 -fopenmp
#DEBUG:
#CFLAGS += -g
# RELEASE:
CFLAGS += -O3

EXEC_NAME = graphAnalyzer
INCLUDES =
LIBS = -fopenmp
OBJ_FILES = arguments.o node.o graph.o graphAnalyzer.o

all : $(EXEC_NAME)

$(EXEC_NAME) : $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

clean :
	rm $(EXEC_NAME) $(OBJ_FILES)
