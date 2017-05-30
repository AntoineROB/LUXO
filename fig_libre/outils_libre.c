/**
*   \file outils_libre.c
*   \brief Fonctions qui nous sont utiles au programme pour la figure libre
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*
*   Fichier contenant les fonctions de base utile à notre projet pour la figure libre
*
*/

// Mettre ça dans outils.h


#include "outils_libre.h"


//création d'un buffer
/**
*   \fn creer_buffer
*	\brief Fonction qui créer notre buffer de position
*
*/
t_Coord* creer_buffer(int taille)
{
    t_Coord* buffer=(t_Coord*)malloc(taille*sizeof(t_Coord));
    return buffer;
}


/**
*   \fn modif_buffer
*	\brief Fonction qui remplit et modifie notre buffer
* 
*	Cette fonction va rajouter un point au buffer
*
*/
void modif_buffer(int i, t_Coord *buffer, CvPoint objectPos)
{
	buffer[i].x=objectPos.x;
	buffer[i].y=objectPos.y;
}


/**
*   \fn tracer_mouv
*	\brief Fonction qui ajoute les 20 derniers points à l'écran
* 
*	Rajoute la trace du mouvement à l'écran
*
*/
void tracer_mouv(t_Coord *buffer, IplImage *image, int index, int nbPos)
{
	
	CvPoint p;
	int i=index+1;
	int j;
	for (j=0;j<nbPos-1;j++)
	{
		p=cvPoint(buffer[i%nbPos].x, buffer[i%nbPos].y);
		cvDrawCircle(image, p, 8, CV_RGB(255, 0, 0), -1);
		i++;

	}
}




/**
*   \fn afficher_zone
*	\brief Fonction qui affiche les zones à l'écran
* 
*	On se sert de t_lim pour afficher les zones
*
*/
void afficher_zone(IplImage*image, t_zone *zone, t_lim limites)
{
	CvPoint pt1=cvPoint (limites.l1,limites.l3);
	CvPoint pt2=cvPoint (limites.l2,limites.l4);
	CvPoint pt3=cvPoint (limites.l5,limites.l2);
	CvPoint pt4=cvPoint (limites.l6,limites.l1);
	CvPoint pt5=cvPoint (limites.l9,limites.l3);
	CvPoint pt6=cvPoint (limites.l10,limites.l4);
	CvPoint pt7=cvPoint (limites.l5,limites.l7);
	CvPoint pt8=cvPoint (limites.l6,limites.l8);

	/*if(*zone==0)
	{
		printf("ZONE aucune\n");
	}
	if(*zone==1)
	{
		printf("ZONE gauche\n");
	}
	if(*zone==2)
	{
		printf("ZONE haut\n");
	}
	if(*zone==3)
	{
		printf("ZONE droite\n");
	}
	if(*zone==4)
	{
		printf("ZONE bas\n");
	}*/

	cvRectangle(image, pt1,pt2, CV_RGB(0, 255, 0), 1, 8, 0 );
	cvRectangle(image, pt3,pt4, CV_RGB(0, 255, 0), 1, 8, 0 );
	cvRectangle(image, pt5,pt6, CV_RGB(0, 255, 0), 1, 8, 0 );
	cvRectangle(image, pt7,pt8, CV_RGB(0, 255, 0), 1, 8, 0 );

}


/**
*   \fn motValide
*	\brief Fonction qui vérifie si le mot rentré au clavier correspond au mot mystère
* 
*	Cette fonction va vérifier si le mot rentrer correspond au mot à trouver ou au mot qui permet de quitter l'image courante sans faire de proposition
*	Elle renvoie un entier qui nous permet de gérer les différents cas
*	Elle prend en paramètre la structure du mot mystère et la tentative du joueurs
*/
int motValide(t_mot *motM, char tentative[])
{
	int compt=0;
	int vrai=0; //si vrai==0 on veut juste sortir sans proposition, si vrai==1 tentative réussie on pourra sortir du jeu, si vrai==-1 erreur on retire une vie
	while(compt<motM->nb_caracteres && vrai==0)	//on vérifie d'abord si le mot correspond à MotSortie qui nous permet de sortir sans faire de proposition
	{
		if(tentative[compt]!=motM->motSortie[compt])
		{
			vrai=1; //Si ce n'est pas le mot mystère on considère que c'est une vraie proposition;
		}
		compt++;
	}
	
	compt=0;
	
	while(compt<motM->nb_caracteres && vrai==1)
	{
		if(tentative[compt]!=motM->motMystere[compt])
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
