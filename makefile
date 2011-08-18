CC = g++

CFLAGS = -Wall `sdl-config --cflags`

NAME = 'dungepic'

SOURCES = main.cpp init.cpp input.cpp logic.cpp graphics.cpp map.cpp 
	 	

LIBS = `sdl-config --libs` -lSDL_image -lSDL_ttf
      
	
      

$(NAME): $(SOURCES)
	$(CC) -o $(NAME) $(SOURCES) $(CFLAGS) $(LIBS)

clean: 
	rm -f *.o 
