EXEC = flightsim

DEPS = $(EXEC).o

COMPILER = g++
COMPILER_OPTS = -c -g -Wall

LINKER = g++
LINKER_OPTS = -framework GLUT -framework OpenGL -framework Carbon

all : $(EXEC)

$(EXEC) : $(DEPS)
	$(LINKER) -o $(EXEC) $(LINKER_OPTS) $(DEPS)

$(EXEC).o : $(EXEC).cpp
	$(COMPILER) $(COMPILER_OPTS) $(EXEC).cpp

clean : 
	-rm *.o $(EXEC)
