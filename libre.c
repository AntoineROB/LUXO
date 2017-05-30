
/**
*   \file main.c
*   \brief Fichier principal du Projet C Pan-tilt
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*
*   Ce fichier comprant des structures et des fonctions utiles au projet, pour la figure libre et la figure imposée. 
*/


#include "outils.h"

// Nécessaire à l'interface graphique
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <string.h>


using namespace sf; //Espace de nommage nous permet de ne pas utiliser sf:: à chaque fois


/*****************************IMAGING*********************/
// Maths methods
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))  
#define abs(x) ((x) > 0 ? (x) : -(x))
#define sign(x) ((x) > 0 ? 1 : -1)
 
// Step mooving for object min & max-
#define STEP_MIN 5
#define STEP_MAX 100 

IplImage *image;
 
// Position de notre objet
CvPoint objectPos = cvPoint(-1, -1);
// La couleur que l'on cherche et notre tolérance
int h = 0, s = 0, v = 0, tolerance = 10;


/***********INTERFACE*********************/

/**
*   \struct t_image
*	\brief Structure d'un noeud image
* 
*	Possède 8 champs :
*	Un numéro d'image, une sf::texture à partir d'une image chargée, un sprite associé
*	5 pointeurs vers un autre noeud image, l'un correspond à un indice caché dans l'image, les autres aux liens existants entre les images
* 	
*/
typedef struct T_image
{
	int numero;
	sf::Texture f_image;
	sf::Sprite S_image;

	T_image *left;
	T_image *right;
	T_image *up;
	T_image *down;
	T_image *indice;

} t_image;


/**
*   \struct t_map
*	\brief Structure de la map
* 
*	Correspond à la map du jeu, regroupe dans une liste tous les noeuds, et le nombre d'images que possède la map 
* 	
*/
typedef struct T_map
{
	int nombre_images;
	t_image *head;
} t_map;


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




/********FONCTION TRAITEMENT D'IMAGES CAMERA***********/

/**
*   \fn binarisation
*	\brief Fonction transforme l'image en deux images colorées, l'une de la couleur que nous voulons suivre, l'autre composée des autres couleurs
* 
*	On renvoie également le nombre de pixels détectés et le barycentre
*
*/
CvPoint binarisation(IplImage* image, int *nbPixels) {
 
    int x, y;
    //CvScalar pixel;
    IplImage *hsv, *mask;
    IplConvKernel *kernel;
    int sommeX = 0, sommeY = 0;
    *nbPixels = 0;
 
    // Create the mask &initialize it to white (no color detected)
    mask = cvCreateImage(cvGetSize(image), image->depth, 1);
 
    // Create the hsv image
    hsv = cvCloneImage(image);
    cvCvtColor(image, hsv, CV_BGR2HSV);
 
    // We create the mask
    cvInRangeS(hsv, cvScalar(h - tolerance -1, s - tolerance, 0), cvScalar(h + tolerance -1, s + tolerance, 255), mask);
 
    // Create kernels for the morphological operation
    kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);
 
    // Morphological opening (inverse because we have white pixels on black background)
    cvDilate(mask, mask, kernel, 1);
    cvErode(mask, mask, kernel, 1);  
 
    // We go through the mask to look for the tracked object and get its gravity center
    for(x = 0; x < mask->width; x++) {
        for(y = 0; y < mask->height; y++) { 
 
            // If its a tracked pixel, count it to the center of gravity's calcul
            if(((uchar *)(mask->imageData + y*mask->widthStep))[x] == 255) {
                sommeX += x;
                sommeY += y;
                (*nbPixels)++;
            }
        }
    }
 
    // Show the result of the mask image
    cvShowImage("Luxo Mask", mask);
 
    // We release the memory of kernels
    cvReleaseStructuringElement(&kernel);
 
    // We release the memory of the mask
    cvReleaseImage(&mask);
    // We release the memory of the hsv image
        cvReleaseImage(&hsv);
 
    // If there is no pixel, we return a center outside the image, else we return the center of gravity
    if(*nbPixels > 0)
        return cvPoint((int)(sommeX / (*nbPixels)), (int)(sommeY / (*nbPixels)));
    else
        return cvPoint(-1, -1);
}


/**
*   \fn addObjectToVideo
*	\brief Fonction qui trace un cercle pour repérer notre objet
* 
*	Nous utilisons cvDrawCircle et le barycentre de l'objet
*
*/
void addObjectToVideo(IplImage* image, CvPoint objectNextPos, int nbPixels) {
 
    int objectNextStepX, objectNextStepY;
 
    // Calculate circle next position (if there is enough pixels)
    if (nbPixels > 10) {
 
        // Reset position if no pixel were found
        if (objectPos.x == -1 || objectPos.y == -1) {
            objectPos.x = objectNextPos.x;
            objectPos.y = objectNextPos.y;
        }
 
        // Move step by step the object position to the desired position
        if (abs(objectPos.x - objectNextPos.x) > STEP_MIN) {
            objectNextStepX = max(STEP_MIN, min(STEP_MAX, abs(objectPos.x - objectNextPos.x) / 2));
            objectPos.x += (-1) * sign(objectPos.x - objectNextPos.x) * objectNextStepX;
        }
        if (abs(objectPos.y - objectNextPos.y) > STEP_MIN) {
            objectNextStepY = max(STEP_MIN, min(STEP_MAX, abs(objectPos.y - objectNextPos.y) / 2));
            objectPos.y += (-1) * sign(objectPos.y - objectNextPos.y) * objectNextStepY;
        }
 
    // -1 = object isn't within the camera range
    } else {
 
        objectPos.x = -1;
        objectPos.y = -1;
 
    }
 
    // Draw an object (circle) centered on the calculated center of gravity
    if (nbPixels > 10)
        cvDrawCircle(image, objectPos, 15, CV_RGB(255, 0, 0), -1);
 
    // We show the image on the window
    cvShowImage("Luxo Color Tracking", image);
 
}


/**
*   \fn getOjectColor
*	\brief Fonction qui récupère la couleur de l'objet à suivre
* 
*	Elle réagit au clic de la souris et récupère les valeurs de la couleur à suivre
*
*/
void getObjectColor(int event, int x, int y, int flags, void *param) {
 
    // Vars
    CvScalar pixel;
    IplImage *hsv;

    if(event == CV_EVENT_LBUTTONUP) {
 
        // Get the hsv image
        hsv = cvCloneImage(image);
        cvCvtColor(image, hsv, CV_BGR2HSV);
 
        // Get the selected pixel
        pixel = cvGet2D(hsv, y, x);
 
        // Change the value of the tracked color with the color of the selected pixel
        h = (int)pixel.val[0];
        s = (int)pixel.val[1];
        v = (int)pixel.val[2];
 
        // Release the memory of the hsv image
            cvReleaseImage(&hsv);
 
    }
 
}

/*********************************CONTROLE MOTEURS***********************************/
/**
*   \fn getCoordCentre
*	\brief Fonction qui récupère le centre de l'image
*
*/
void getCoordCentre(IplImage *image, t_Coord *centreImg)
{
	centreImg->x= image->width/2;
	centreImg->y = image->height/2;
}




/***************************Fonction utiles pour le jeu***************************************/
/**
*   \fn motValide
*	\brief Fonction qui vérifie si le mot rentré au clavier correspond au mot mystère
* 
*	Cette fonction va vérifier si le mot rentrer correspond au mot à trouver ou au mot qui permet de quitter l'image courante sans faire de proposition
*	Elle renvoie un entier qui nous permet de gérer les différents cas
*	Elle prend en paramètre la structure du mot mystère et la tentative du joueurs
*/
int motValide(t_mot *motM, char *tentative[])
{
	int compt=0;
	int vrai=0; //si vrai==0 on veut juste sortir sans proposition, si vrai==1 tentative réussie on pourra sortir du jeu, si vrai==-1 erreur on retire une vie
	while(compt<motM->nb_caracteres && vrai!=-1)	//on vérifie d'abord si le mot correspond à MotSortie qui nous permet de sortir sans faire de proposition
	{
		if(*tentative[compt]!=motM->motSortie[compt])
		{
			vrai=-1;
		}
		compt++;
	}
	if(vrai!=0)	//Si le mot entré en correspond pas au motSortie on va le considérer comme une vraie proposition
	{
		compt=0;
		vrai=0;
		while(compt<motM->nb_caracteres && vrai!=-1)
		{
			if(*tentative[compt]!=motM->motMystere[compt])
			{
				vrai=-1;
			}
			compt++;
		}

		//On regarde maintenant si on a bien trouver le mot mystère
		if(vrai==-1)
		{
			motM->nb_chances--;
		}
	}
	return vrai;
	
}

/** 
*	\fn viderBuffer
*	\brief Fonction qui permet de vider le buffer
*
*/
void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
*   \fn verif_zone
*	\brief Fonction qui trouve dans quelle zone on se trouve
* 
*	Elle modifie l'énum zone en fonction de où l'on se trouve
*	Les 20 positions du buffer doivent être comprises dans cette zone
*
*/
void verif_zone(t_Coord *buffer, IplImage *image, int i, int nbPos, t_lim limites, t_zone *zone, t_image *scene_courante)
{
	int j=0;
	bool t=true;
	
	*zone=aucune;

	//zone gauche
	if((buffer[i].x>=limites.l1)&&(buffer[i].x<=limites.l2)&&(buffer[i].y>=limites.l3)&&(buffer[i].y<=limites.l4))
	{
		while((j<nbPos)&&(t==true))
		{
			if((buffer[j].x>=limites.l1)&&(buffer[j].x<=limites.l2)&&(buffer[j].y>=limites.l3)&&(buffer[j].y<=limites.l4))
			{
				j++;
			}
			else
			{
				t=false;
			}
		}
		if(t)
		{
			*zone=gauche;
		}
	}

	//zone haut
	if((buffer[i].x>=limites.l5)&&(buffer[i].x<=limites.l6)&&(buffer[i].y>=limites.l1)&&(buffer[i].y<=limites.l2))
	{
		while((j<nbPos)&&(t==true))
		{
			if((buffer[j].x>=limites.l5)&&(buffer[j].x<=limites.l6)&&(buffer[j].y>=limites.l1)&&(buffer[j].y<=limites.l2))
			{
				j++;
			}
			else
			{
				t=false;
			}
		}
		if(t)
		{
			*zone=haut;
		}
	}

	//zone droite
	if((buffer[i].x>=limites.l9)&&(buffer[i].x<=limites.l10)&&(buffer[i].y>=limites.l3)&&(buffer[i].y<=limites.l4))
	{
		while((j<nbPos)&&(t==true))
		{
			if((buffer[j].x>=limites.l9)&&(buffer[j].x<=limites.l10)&&(buffer[j].y>=limites.l3)&&(buffer[j].y<=limites.l4))
			{
				j++;
			}
			else
			{
				t=false;
			}
		}
		if(t)
		{
			*zone=droite;
		}
	}

	//zone bas
	if((buffer[i].x>=limites.l5)&&(buffer[i].x<=limites.l6)&&(buffer[i].y>=limites.l7)&&(buffer[i].y<=limites.l8))
	{
		while((j<nbPos)&&(t==true))
		{
			if((buffer[j].x>=limites.l5)&&(buffer[j].x<=limites.l6)&&(buffer[j].y>=limites.l7)&&(buffer[j].y<=limites.l8))
			{
				j++;
			}
			else
			{
				t=false;
			}
		}
		if(t)
		{
			*zone=bas;
		}
	}

	
	/* Tests supplémentaires pour déterminer si on est dans zoom */
	//Chambre
	if(scene_courante->numero==6)
	{
		if((buffer[i].x>=195)&&(buffer[i].x<=275)&&(buffer[i].y>=225)&&(buffer[i].y<=305))
		{	
			while((j<nbPos)&&(t==true))
			{
				if((buffer[j].x>=195)&&(buffer[j].x<=275)&&(buffer[j].y>=225)&&(buffer[j].y<=305))
				{
					j++;
				}
				else
				{
					t=false;
				}
			}
			if(t)
			{
				*zone=zoom;
			}
		}
	}

	//Cuisine
	if(scene_courante->numero==8)
	{
		if((buffer[i].x>=245)&&(buffer[i].x<=325)&&(buffer[i].y>=81)&&(buffer[i].y<=150))
		{	
			while((j<nbPos)&&(t==true))
			{
				if((buffer[j].x>=245)&&(buffer[j].x<=325)&&(buffer[j].y>=81)&&(buffer[j].y<=150))
				{
					j++;
				}
				else
				{
					t=false;
				}
			}
			if(t)
			{
				*zone=zoom;
			}
		}
	}

	//Télé
	if(scene_courante->numero==9)
	{
		if((buffer[i].x>=295)&&(buffer[i].x<=375)&&(buffer[i].y>=200)&&(buffer[i].y<=280))
		{	
			while((j<nbPos)&&(t==true))
			{
				if((buffer[j].x>=295)&&(buffer[j].x<=375)&&(buffer[j].y>=200)&&(buffer[j].y<=280))
				{
					j++;
				}
				else
				{
					t=false;
				}
			}
			if(t)
			{
				*zone=zoom;
			}
		}
	}


	//Tv x=335 y=240
	//chambre x=235 y=265
	//cuisine x=285 y=110
}



int main()
{
	
	//On crée la fenêtre du projet
	sf::RenderWindow ecran(VideoMode(640, 480, 32), " Projet Milot-Diot ");
	
	

	/*------------------------CREATION MAP + IMAGES, EDIFICATION DES LIENS -------------------------*/
	
	t_map *AppartAntoine=(t_map*)malloc(sizeof(t_map));
	AppartAntoine->head=NULL;
	AppartAntoine->nombre_images=14; //A CHANGER 

	//Compteur global qui va nous servir à numéroter nos scènes
	int compt_glob=0;
	
	/*Declaration de toutes les images*/ 
	t_image tI_h1;         
	t_image tI_h2;          
	t_image tI_exit;        
	t_image tI_chambre;     
	t_image tI_Ptable;      
	t_image tI_Pcuisine;    
	t_image tI_Ptele;	   
	t_image tI_Phall;	   
	t_image tI_messageE;    
	t_image tI_messageInd;  
	t_image tI_messageD;
	t_image tI_IndChambre;
	t_image tI_IndSalon;
	t_image tI_IndCuisine;


	/*On fait le chargement de chaque image*/
	/*La création de chaque image suit toujours les mêmes étapes*/

	printf("test1 : avant texture\n");

	/*Texture 0 ; Chargement des règles*/
	sf::Texture I_messageD;
	sf::Sprite S_messageD;
	if(!(I_messageD.loadFromFile("messageD_dir.jpg"))) //Chargement de l'image, s'il ne se fait pas message d'erreur
	{
		printf("Erreur dans le chargement de l'image %d\n",compt_glob);
	}
	else
	{
		S_messageD.setTexture(I_messageD);	//On crée le sprite à partir de la texture chargée
		tI_messageD.numero=compt_glob;		//On donne à la structure un numéro
		tI_messageD.f_image=I_messageD;		//On complète associe les champs texture/sprite de la structure aux variables correspondantes
		tI_messageD.S_image=S_messageD;

		//On tisse les liens de cette image vers les autres 
		tI_messageD.left=&tI_h1;
		tI_messageD.right=&tI_h1;
		tI_messageD.up=&tI_h1;
		tI_messageD.down=&tI_h1;
		compt_glob++;

		//C'est cette image qui nous sert de commencement
		AppartAntoine->head=&tI_messageD;
	}

	/*Texture 1 ; Image correspondant à l'écran pour rentrer la solution*/
	Texture I_messageE;
	Sprite S_messageE;
	if (!(I_messageE.loadFromFile("messageE.bmp"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_messageE.setTexture(I_messageE);
		tI_messageE.S_image=S_messageE;
		tI_messageE.numero=compt_glob;
		tI_messageE.f_image=I_messageE;

		tI_messageE.left=&tI_exit;
		tI_messageE.right=&tI_exit;
		tI_messageE.up=&tI_exit;
		tI_messageE.down=&tI_exit;
		compt_glob++;
	}

	/*Texture 2 ; Texture pour le message sur l'indice*/
	Texture I_messageInd;
	Sprite S_messageInd;
	if (!(I_messageInd.loadFromFile("messageInd_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_messageInd.setTexture(I_messageInd);
		tI_messageInd.S_image=S_messageInd;
		tI_messageInd.numero=compt_glob;
		tI_messageInd.f_image=I_messageInd;

		tI_messageInd.left=&tI_chambre;
		tI_messageInd.right=&tI_chambre;
		tI_messageInd.up=&tI_chambre;
		tI_messageInd.down=&tI_chambre;
		compt_glob++;
	}

	/*Texture 3 ; hall1*/
	Texture I_h1;
	Sprite S_h1;
	if (!(I_h1.loadFromFile("h1_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_h1.setTexture(I_h1);
		tI_h1.S_image=S_h1;
		tI_h1.numero=compt_glob;
		tI_h1.f_image=I_h1;

		tI_h1.left=&tI_h2;
		tI_h1.right=&tI_exit;
		tI_h1.up=&tI_Pcuisine;
		tI_h1.down=&tI_exit;
		compt_glob++;
	}

	/*Texture 4 ; hall2*/
	Texture I_h2;
	Sprite S_h2;
	if (!(I_h2.loadFromFile("h2_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_h2.setTexture(I_h2);
		tI_h2.S_image=S_h2;
		tI_h2.numero=compt_glob;
		tI_h2.f_image=I_h2;

		tI_h2.left=&tI_exit;
		tI_h2.right=&tI_h1;
		tI_h2.up=&tI_chambre;
		tI_h2.down=&tI_Phall;
		compt_glob++;
	}

	/*Texture 5 ; Texture de la porte*/
	Texture I_exit;
	Sprite S_exit;
	if (!(I_exit.loadFromFile("Exit_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_exit.setTexture(I_exit);
		tI_exit.S_image=S_exit;
		tI_exit.numero=compt_glob;
		tI_exit.f_image=I_exit;

		tI_exit.left=&tI_h1;
		tI_exit.right=&tI_h2;
		tI_exit.up=&tI_messageE;
		tI_exit.down=&tI_h1;
		compt_glob++;
	}

	/*Texture 6 ; visions de la chambre*/
	Texture I_chambre;
	Sprite S_chambre;
	if (!(I_chambre.loadFromFile("chambre_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_chambre.setTexture(I_chambre);
		tI_chambre.S_image=S_chambre;
		tI_chambre.numero=compt_glob;
		tI_chambre.f_image=I_chambre;

		tI_chambre.left=&tI_chambre;
		tI_chambre.right=&tI_chambre;
		tI_chambre.up=&tI_messageInd;
		tI_chambre.down=&tI_h1;
		compt_glob++;
	}

	/*Texture 7 ; Texture de la table*/
	Texture I_Ptable;
	Sprite S_Ptable;
	if (!(I_Ptable.loadFromFile("ptable_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_Ptable.setTexture(I_Ptable);
		tI_Ptable.S_image=S_Ptable;
		tI_Ptable.numero=compt_glob;
		tI_Ptable.f_image=I_Ptable;

		tI_Ptable.left=&tI_Pcuisine;
		tI_Ptable.right=&tI_Ptele;
		tI_Ptable.up=&tI_messageInd;
		tI_Ptable.down=&tI_h1;
		compt_glob++;
	}

	/*Texture 8 ; Texture de la cuisine*/
	Texture I_Pcuisine;
	Sprite S_Pcuisine;
	if (!(I_Pcuisine.loadFromFile("p_cuisine_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_Pcuisine.setTexture(I_Pcuisine);
		tI_Pcuisine.S_image=S_Pcuisine;
		tI_Pcuisine.numero=compt_glob;
		tI_Pcuisine.f_image=I_Pcuisine;

		tI_Pcuisine.left=&tI_Phall;
		tI_Pcuisine.right=&tI_Ptable;
		tI_Pcuisine.up=&tI_messageInd;
		tI_Pcuisine.down=&tI_h1;
		tI_Pcuisine.indice=&tI_IndCuisine;
		compt_glob++;
	}

	/*Texture 9 ; Texture de la tele*/
	Texture I_Ptele;
	Sprite S_Ptele;
	if (!(I_Ptele.loadFromFile("p_tele_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_Ptele.setTexture(I_Ptele);
		tI_Ptele.S_image=S_Ptele;
		tI_Ptele.numero=compt_glob;
		tI_Ptele.f_image=I_Ptele;

		tI_Ptele.left=&tI_Ptable;
		tI_Ptele.right=&tI_Phall;
		tI_Ptele.up=&tI_messageInd;
		tI_Ptele.down=&tI_h1;
		tI_Ptele.indice=&tI_IndSalon;
		compt_glob++;
	}

	/*Texture 10 ; image du hall à partir de la cuisine*/
	Texture I_Phall;
	Sprite S_Phall;
	if (!(I_Phall.loadFromFile("phall_dir.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_Phall.setTexture(I_Phall);
		tI_Phall.S_image=S_Phall;
		tI_Phall.numero=compt_glob;
		tI_Phall.f_image=I_Phall;

		tI_Phall.left=&tI_Ptable;
		tI_Phall.right=&tI_Pcuisine;
		tI_Phall.up=&tI_exit;
		tI_Phall.down=&tI_Phall;
		compt_glob++;
	}

	/*Texture 11 ; Image indice chambre*/
	Texture I_IndChambre;
	Sprite S_IndChambre;
	if (!(I_IndChambre.loadFromFile("IndChambre.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_IndChambre.setTexture(I_IndChambre);
		tI_IndChambre.S_image=S_IndChambre;
		tI_IndChambre.numero=compt_glob;
		tI_IndChambre.f_image=I_IndChambre;

		tI_IndChambre.left=&tI_chambre;
		tI_IndChambre.right=&tI_chambre;
		tI_IndChambre.up=&tI_chambre;
		tI_IndChambre.down=&tI_chambre;
		compt_glob++;
	}

	/*Texture 12 ; Image Indice Cuisine*/
	Texture I_IndCuisine;
	Sprite S_IndCuisine;
	if (!(I_IndCuisine.loadFromFile("IndCuisine.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_IndCuisine.setTexture(I_IndCuisine);
		tI_IndCuisine.S_image=S_IndCuisine;
		tI_IndCuisine.numero=compt_glob;
		tI_IndCuisine.f_image=I_IndCuisine;

		tI_IndCuisine.left=&tI_Pcuisine;
		tI_IndCuisine.right=&tI_Pcuisine;
		tI_IndCuisine.up=&tI_Pcuisine;
		tI_IndCuisine.down=&tI_Pcuisine;
		compt_glob++;
	}
	
	/*Texture 13 ; Image Indice Salon */
	
	Texture I_IndSalon;
	Sprite S_IndSalon;
	if (!(I_IndSalon.loadFromFile("IndSalon.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_IndSalon.setTexture(I_IndSalon);
		tI_IndSalon.S_image=S_IndSalon;
		tI_IndSalon.numero=compt_glob;
		tI_IndSalon.f_image=I_IndSalon;

		tI_IndSalon.left=&tI_Ptele;
		tI_IndSalon.right=&tI_Ptele;
		tI_IndSalon.up=&tI_Ptele;
		tI_IndSalon.down=&tI_Ptele;
		compt_glob++;
	}

	//Si toutes les images n'ont pas été chargées on quitte le jeu tout de suite
	if(compt_glob<AppartAntoine->nombre_images)
	{
		printf("%d images n'ont pas été chargées\n", AppartAntoine->nombre_images-compt_glob);
		return 0;
	}
	
	printf("test2 : après texture\n");



/*----------Initialisation capture vidéo et fenêtre pour le tracking----------------*/
    // Video Capture
    CvCapture *capture;
    // Key for keyboard event
    char key;
    // Number of tracked pixels
    int nbPixels;
    // Next position of the object we overlay
    CvPoint objectNextPos;
    // On initialise le flux vidéo par rapport à la camera 0 ou 1
    capture = cvCreateCameraCapture(0);
    // Check if the capture is ok
        if (!capture) {
        printf("Can't initialize the video capture.\n");
            return -1;
    }
    // Create the windows
    cvNamedWindow("Luxo Color Tracking", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Luxo Mask", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("Luxo Color Tracking", 0, 100);
    cvMoveWindow("Luxo Mask", 650, 100);    
	
/*-------------Déclarations nécéssaires au mode SUIVI------------*/
    //coordonnées centreImg
	t_Coord *centreImg = (t_Coord*) malloc(sizeof(t_Coord));
	
	/*-------------Déclarations nécéssaires au mode JEU------------*/
	int nbPos=20;
	t_Coord* buffer=creer_buffer(nbPos); 
	int index=0;
	int cpt=0;
	t_zone *zone=(t_zone*)malloc(sizeof(t_zone));
	t_lim limites;
	limites.l1=0;
	limites.l2=80;
	limites.l3=200;
	limites.l4=280;
	limites.l5=280;
	limites.l6=360;
	limites.l7=400;
	limites.l8=480;
	limites.l9=560;
	limites.l10=640;

	//  Mot mystère
	t_mot *tM_mystere;
	tM_mystere->nb_caracteres=5;
	tM_mystere->nb_chances=3;
	tM_mystere->motMystere[0]='p';
	tM_mystere->motMystere[1]='i';
	tM_mystere->motMystere[2]='x';
	tM_mystere->motMystere[3]='a';
	tM_mystere->motMystere[4]='r';
	tM_mystere->motSortie[0]='l';
	tM_mystere->motSortie[1]='a';
	tM_mystere->motSortie[2]='t';
	tM_mystere->motSortie[3]='e';
	tM_mystere->motSortie[4]='r';

	//On commence par initialiser toutes les variables utiles
	t_image *scene_courante=AppartAntoine->head;
	char *motRentre[5];
	int exitAutorisee=-1; //Par défaut on ne peut pas sortir, si on passe à 0 c'est qu'on a débloqué la sortie

	
    // Mouse event to select the tracked color on the original image
    cvSetMouseCallback("Luxo Color Tracking", getObjectColor);
    
     // While we don't want to quit
		    while(key != 'Q' && key != 'q') 
		    {
		 		printf("test4 : boucle\n");

		        // We get the current image
		        image = cvQueryFrame(capture);
		        printf("test10 : après image =\n");
				// If there is no image, we exit the loop
		        if(!image)
		        {
		        	printf("image non récupérée\n");
		        	return 0;
		        }
			    
		        getCoordCentre(image, centreImg);
		        
		 		
		 		/*On binarise l'image*/
		        objectNextPos = binarisation(image, &nbPixels);
		        addObjectToVideo(image, objectNextPos, nbPixels);
		        
		        //On récupère les coordonnées de l'objet et du centre de l'image
				printf("xcentre=%d\n",centreImg->x);
				printf("ycentre=%d\n",centreImg->y);
				printf("xobj=%d\n",objectPos.x);
				printf("yobj=%d\n",objectPos.y);
			    
			    printf("test9 : avant modif buffer\n");


			    //remplissage buffer de positions (taille nbPos)
			    modif_buffer(index%nbPos, buffer, objectPos);
			    index++;

			    /* On trace le chemin du mouvement */
			    if(cpt>nbPos-1)
			    {
			    	tracer_mouv(buffer, image, index, nbPos);
		 		}
		 		cpt++;

		 		printf("test8 : avant aff zone\n");

		 		/* On cherche la zone où on se trouve et on les affiches sur l'image capturée */
		 		verif_zone(buffer,image, index%nbPos, nbPos,limites,zone,scene_courante);
		 		afficher_zone(image,zone,limites);


		 		printf("test7 : après aff zone\n");

		 		/*interface graph*/
		 		Event event; //Variable qui nous permet de gérer les événements clavier

		        //Chargement d'une musique
		        /*
		        sf::Music music;
				if (!music.openFromFile("Sound1.wav"))
		    	{
		    		printf("Erreur dans le chargement de la musique\n");
		    		//Comme la musique n'est pas un point indispensable du programme on lance quand même le jeu sans musique
				}
				else 
				{
					music.play();
				}
				*/

				//Boucle qui gère les événements, reste dans la boucle tant qu'elle n'a pas gérer tous les événements
				printf("test6 : avant boucle event\n");

		        while (ecran.pollEvent(event))
		        {            
		        	/*Event qui permet de fermer la fenêtre*/
		            if (event.type == Event::Closed)
		                ecran.close();

		            /*Event qui permet de naviguer entre les images à partir du clavier, le but est de ne pas s'en servir*/
		            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
					{
		   				scene_courante=scene_courante->left;
					}
					else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
					{
						scene_courante=scene_courante->right;
					}
					else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
					{
						scene_courante=scene_courante->up;
					}
					else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
					{
						scene_courante=scene_courante->down;
					}
		         }

		         printf("test5 : après boucle event\n");


		    	//On regarde si on se trouve dans l'image ou on peut rentrer le mot à trouver         
		        if(scene_courante->numero==1)
		         {
		         	 //Dans ce cas si, on va demander au joueur de rentrer un mot et regarder s'il est valide
		         	printf("Vous pensez pouvoir sortir ??\n");
		         	printf("Alors quel est le mot mystère ?\n");
		         	fgets(*motRentre, 5, stdin); //permet de rentrer une chaine de 5 caractères
		         	viderBuffer(); //On vide le buffer pour la prochaine execution

		         	//On appelle ensuite la fonction qui va nous permetttre de voir si le mot est valide
		         	exitAutorisee=motValide(tM_mystere,motRentre);
		         	if(exitAutorisee==1) // On gère le résultat de la validation ou non du mot rentrer
		         	{
		         		printf("Vous avez réussi à trouver le mot mystère bien joué !\n");
		         		ecran.close();
		         	}
		         	else if(exitAutorisee==0)
		         	{
		         		printf("\n\nVous avez choisi de ne pas faire de proposition !\n");
		         		printf("Retourner au jeu pour continuer à chercher des indices ! \n");
		         		scene_courante=scene_courante->down;
		         	}
		         	else if(exitAutorisee==-1 && tM_mystere->nb_chances==0)
		         	{
		         		printf("Vous avez perdu ! Dommage, c'est la fin pour vous...\n");
		         		ecran.close();
		         	}
		         } 
		         /* Si non, on va regarder dans quelle zone d'intérêt se trouve notre balle et naviguer entre les différentes images en fonction de ça*/
		         /*Appeler la fonction qui permet de savoir dans quelle zone on se trouve*/
		         if(*zone==zoom) //cette zone ne prends en compte que 3 images, la fonction qui nous permet de trouver la zone gère directement les cas
		         {
		         	 scene_courante=scene_courante->indice;
		         } 
		         else if(*zone==gauche)
		         {
		         	 scene_courante=scene_courante->left;
		         }
		         else if(*zone==droite)
		         {
		         	 scene_courante=scene_courante->right;
		         }
		         else if(*zone==haut)
		         {
		         	 scene_courante=scene_courante->up;
		         }
		         else if(*zone==bas)
		         {
		         	 scene_courante=scene_courante->down;
		         }
		            
		        // Clean de la fenêtre d'affichage
		        ecran.clear();
		        //draw le sprite
		        ecran.draw(scene_courante->S_image);
		        // Affichage de la fenêtre à l'écran
		        ecran.display();
			    
		 		/*On affiche l'image*/
		 		cvShowImage("Luxo Color Tracking", image);
		 		
 	         	key = cvWaitKey(10);
		    }
	// Destroy the windows we have created
    cvDestroyWindow("Luxo Color Tracking");
    cvDestroyWindow("Luxo Mask");
 
    // Destroy the capture
    cvReleaseCapture(&capture);    
		    
		    
free(AppartAntoine);
    free(zone);
    free(centreImg);
    
    return 0;
}
