# options de compilation
CC = g++
CCFLAGS = -Wall -g 
LIBS = -L $(OPENCV)
LDFLAGS = -lm -lsfml-graphics -lsfml-window -lsfml-system 
OPENCV = `pkg-config opencv --cflags --libs`


# fichiers du projet
SRC = libre.c outils_libre.c
OBJ = $(SRC:.c=.o)
EXEC = libre


# règle initiale
all: $(EXEC)

# dépendance des .h
libre.o: outils_libre.h
outils_libre.o: outils_libre.h

# règles de compilation
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $<

# règles d'édition de liens
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

# règles supplémentaires
clean:
	rm -f *.o 
rmproper:
	rm -f $(EXEC) *.o
