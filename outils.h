/**
*   \file imaging.c
*   \brief Fonctions liées au centrage de la balle
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*
*   Ce fichier regroupe toutes les fonctions liées au traitement d'image
*
*/

#include <stdio.h>
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/core.hpp"
#include "opencv2/core/types.hpp"
#include <time.h>
#include <unistd.h>


typedef struct ball
{
	int x;
	int y;
	int r;
} t_ball;

typedef struct donneesImg
{
	int hauteur;
	int largeur;
	CvSize taille;
} t_dImg;

typedef enum type_mode
{
	SUIVI=0,
	JEU=1
} t_mode;

//Structure coordonnées
typedef struct _t_Coord
{
	int x;
	int y; 
	
} t_Coord;

//structure zone
typedef struct _t_lim
{
	int l1;
	int l2;
	int l3;
	int l4;
	int l5;
	int l6;
	int l7;
	int l8;
	int l9;
	int l10;
} t_lim;
typedef enum _t_zone
{
	aucune=0,
	gauche=1,
	haut=2,
	droite=3,
	bas=4
} t_zone;




t_Coord* creer_buffer(int taille);
void modif_buffer(int i, t_Coord *buffer, CvPoint objectPos);
void tracer_mouv(t_Coord *buffer, IplImage *image, int index, int nbPos);
void verif_zone(t_Coord *buffer, IplImage *image, int i, int nbPos, t_lim limites, t_zone *zone);
void afficher_zone(IplImage*image, t_zone *zone, t_lim limites);
