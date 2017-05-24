/**
*   \file outils.c
*   \brief Fonctions qui nous sont utiles au fichier fonctions
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*
*   Fichier contenant les fonctions de base utile à notre projet
*
*/

// Mettre ça dans outils.h


#include "outils.h"


//création d'un buffer
t_Coord* creer_buffer(int taille)
{
    t_Coord* buffer=(t_Coord*)malloc(taille*sizeof(t_Coord));
    return buffer;
}


void modif_buffer(int i, t_Coord *buffer, CvPoint objectPos)
{
	buffer[i].x=objectPos.x;
	buffer[i].y=objectPos.y;
	printf("buffer[%d]= %d ; %d\n",i,buffer[i].x,buffer[i].y);
}

void tracer_mouv(t_Coord *buffer, IplImage *image, int index, int nbPos)
{
	
	CvPoint p;
	int i=index+1;
	int j;
	for (j=0;j<nbPos-1;j++)
	{
		p=CvPoint(buffer[i%nbPos].x, buffer[i%nbPos].y);
		cvDrawCircle(image, p, 8, CV_RGB(255, 0, 0), -1);
		i++;

	}
}

//fonction qui retourne dans quelle zone on est
void verif_zone(t_Coord *buffer, IplImage *image, int i, int nbPos, t_lim limites, t_zone *zone)
{
	int j=0;
	bool t=true;

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

	if(!t)
	{
		*zone=aucune;
	}	
}

void afficher_zone(IplImage*image, t_zone *zone, t_lim limites)
{
	CvPoint pt1=CvPoint (limites.l1,limites.l3);
	CvPoint pt2=CvPoint (limites.l2,limites.l4);
	CvPoint pt3=CvPoint (limites.l5,limites.l2);
	CvPoint pt4=CvPoint (limites.l6,limites.l1);
	CvPoint pt5=CvPoint (limites.l9,limites.l3);
	CvPoint pt6=CvPoint (limites.l10,limites.l4);
	CvPoint pt7=CvPoint (limites.l5,limites.l7);
	CvPoint pt8=CvPoint (limites.l6,limites.l8);

	if(*zone==0)
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
	}

	cvRectangle(image, pt1,pt2, CV_RGB(0, 255, 0), 1, 8, 0 );
	cvRectangle(image, pt3,pt4, CV_RGB(0, 255, 0), 1, 8, 0 );
	cvRectangle(image, pt5,pt6, CV_RGB(0, 255, 0), 1, 8, 0 );
	cvRectangle(image, pt7,pt8, CV_RGB(0, 255, 0), 1, 8, 0 );

}