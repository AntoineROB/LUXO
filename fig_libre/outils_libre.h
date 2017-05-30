/**
*   \file outils_libre.h
*   \brief Fichier utiles pour le pgramme de la figure libre
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*	
*   On peut trouver des structures très similaire à celles dans outils_impo
*/

#include <stdio.h>
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
//#include "opencv2/core.hpp"
//#include "opencv2/core/types.hpp"
#include <time.h>
#include <unistd.h>


/**
*   \struct ball
*	\brief Structure de la balle
* 
*	Possède 3 champs :
*	les coordonéees de son barycentre ainsi que le rayon
* 	
*/
typedef struct ball
{
	int x;
	int y;
	int r;
} t_ball;


/**
*   \struct t_dImg
*	\brief Structure regroupant les données sur l'image capturée
* 
*	Possède 3 champs :
*	La hauteur, la largeur et la taille en nombre de pixels
* 	
*/
typedef struct donneesImg
{
	int hauteur;
	int largeur;
	CvSize taille;
} t_dImg;



/**
*   \struct t_Coord
*	\brief Structure coordonnées
* 
*	Possède 2 champs :
*	x et y, les coordonnées sont en pixels
* 	
*/
typedef struct _t_Coord
{
	int x;
	int y; 
	
} t_Coord;


/**
*   \struct t_lim
*	\brief Structure servant à la définition des zones, on regroupe toutes les limites nécessaires
* 
*	Possède 10 champs :
*	Chaque champs correspond à une limite utile, pour rappel les dimensions sont 640*480 
* 	
*/
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

/**
*   \enum t_zone
*	\brief Enumération des zones
* 
*	Possède 6 champs :
*	aucune, gauche, haut, droite, bas, zoom
*	Ainsi on peut dire dans quelle notre balle se trouve
* 	
*/
typedef enum _t_zone
{
	aucune=0,
	gauche=1,
	haut=2,
	droite=3,
	bas=4,
	zoom=5
} t_zone;

/**
*   \struct t_mot
*	\brief Structure qui correspond au mot mystere
* 
*	Connait le nombre de caractère qui compose le mot, et le mot lui même
*	Le nombre de chances qu'on a pour deviner le mot
* 	Connait également un mot qui permet de sortir du mode proposition sans décompte d'une chance
*/
typedef struct T_mot
{
	int nb_caracteres;
	int nb_chances;
	char motMystere[5];
	char motSortie[5];

} t_mot;


t_Coord* creer_buffer(int taille);
void modif_buffer(int i, t_Coord *buffer, CvPoint objectPos);
void tracer_mouv(t_Coord *buffer, IplImage *image, int index, int nbPos);
//void verif_zone(t_Coord *buffer, IplImage *image, int i, int nbPos, t_lim limites, t_zone *zone, t_image scene_courante);
void afficher_zone(IplImage*image, t_zone *zone, t_lim limites);
int motValide(t_mot *motM, char tentative[]);
void viderBuffer();
