/**
*   \file imaging.c
*   \brief Fonctions liées au traitement d'image
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*
*   Ce fichier regroupe toutes les fonctions liées au traitement d'image
*
*/

#include "imaging.h"





/*
void findBall(IplImage *img, t_dImg *dImg)
{
	// Initialize different images that are going to be used in the program
	IplImage*  hsv_frame = cvCreateImage(dImg->taille, IPL_DEPTH_8U, 3);
	IplImage*  thresholded   = cvCreateImage(dImg->taille, IPL_DEPTH_8U, 1); // final thresholded image
	IplImage*  thresholded1   = cvCreateImage(dImg->taille, IPL_DEPTH_8U, 1); // Component image threshold
	IplImage*  thresholded2   = cvCreateImage(dImg->taille, IPL_DEPTH_8U, 1);
	IplImage*  thresholded3   = cvCreateImage(dImg->taille, IPL_DEPTH_8U, 1);
	IplImage*  filtered   = cvCreateImage(dImg->taille, IPL_DEPTH_8U, 1);  //smoothed image
    
	cvCvtColor(img, hsv_frame, CV_BGR2HSV); //Convert the captured frame from BGR to HSV
	//affiche hsv_frame
	cvNamedWindow("hsv_frame", CV_WINDOW_AUTOSIZE);
	cvShowImage( "hsv_frame", hsv_frame);
	
	
	//on sépare l'image hsv en 3
	cvSplit( hsv_frame,thresholded1, thresholded2, thresholded3, NULL );
    
	
    
}
*/

//search_ball

//Fonction cercle

//Voir comment Baxter fait 

