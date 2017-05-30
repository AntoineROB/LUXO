# options de compilation
CC = g++
CCFLAGS = -Wall -g 
LIBS = -L $(OPENCV)
LDFLAGS = -lm
OPENCV = `pkg-config opencv --cflags --libs`


# fichiers du projet
SRC = impo.c outils_impo.c
OBJ = $(SRC:.c=.o)
EXEC = impo


# règle initiale
all: $(EXEC)

# dépendance des .h
impo.o: outils_impo.h
outils_impo.o: outils_impo.h

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
