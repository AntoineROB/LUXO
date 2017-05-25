# options de compilation
CC = g++
CCFLAGS = -Wall -g 
LIBS = -L $(OPENCV)
LDFLAGS = -lm -lsfml-graphics -lsfml-window -lsfml-system 
OPENCV = `pkg-config opencv --cflags --libs`


# fichiers du projet
SRC = main.c outils.c imaging.c centerBall.c recognition.c followMove.c
OBJ = $(SRC:.c=.o)
EXEC = main


# règle initiale
all: $(EXEC)

# dépendance des .h
main.o: recognition.h 
recognition.o: recognition.h
followMove.o: followMove.h
centerBall.o: centerBall.h
imaging.o: imaging.h
outils.o: outils.h

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
