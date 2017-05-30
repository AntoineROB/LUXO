/**
*   \file outils_impo.h
*   \brief Fichier utiles pour le programme de la figure imposée
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*
*
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



