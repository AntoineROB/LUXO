/**
*   \file impo.c
*   \brief Fichier principal pour la figure imposée du Projet C Pan-tilt
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*
*   Ce fichier comprant des structures et des fonctions utiles au projet pour la figure imposée. 
*/

#include "outils_impo.h"

// Nécessaire à l'interface graphique
#include <cstdlib>
#include <string.h>


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
int h = 0, s = 0, v = 0, tolerance = 30;



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




/**
*   \fn main
*	\brief Fonction générale qui lance notre programme
*
*/
int main() {

	int compteur=0; //Pour nous laisser le temps de sélectionner la balle
	



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
    capture = cvCreateCameraCapture(1);
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
	// déplacement camera
	int tolerancem;
	float step;
	tolerancem=15;
	step=1;
	int diff;
	float increment;
	float facteur=0.001;

	

    // Mouse event to select the tracked color on the original image
    cvSetMouseCallback("Luxo Color Tracking", getObjectColor);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////


		

	/*Ouverture du fichier pour écrire sur la carte et commander nos servos*/
 	FILE *control_servo;
 	control_servo = fopen ("/dev/ttyACM0","w");
 	if (control_servo==NULL)
 	{
 		printf("Erreur dans l'ouverture du fichier\n");
 		perror("l'erreur ouverture fichier vient de \n");
 		return 0;
	}
	
	/* Position initiale de la caméra */
	float servo_old[2];
	servo_old[0]=60;//gère x p5
	servo_old[1]=60;//gère y p4
	fprintf(control_servo, "%d,%d\n", (int) servo_old[0], (int) servo_old[1]);
	getchar();
		    
	// While we don't want to quit
	while(key != 'Q' && key != 'q') 
	{	 		
	     // We get the current image
		image = cvQueryFrame(capture);
		//If there is no image, we exit the loop
	     if(!image)
	           continue;
	      
	     getCoordCentre(image, centreImg);
	     
	     if(compteur>=0)
	     {
	     
		 
			objectNextPos = binarisation(image, &nbPixels);
			addObjectToVideo(image, objectNextPos, nbPixels);
				   
			 //Maintenant on appelle la fonction qui va nous permettre de garder l'objet au centre de l'imageint 
	 		printf("xcentre=%d\n",centreImg->x);
			printf("ycentre=%d\n",centreImg->y);
			printf("xobj=%d\n",objectPos.x);
			printf("yobj=%d\n",objectPos.y);
				  
			//calcul sur x
			diff=(centreImg->x+tolerancem-objectPos.x);
			if(diff>0)
			{
				increment=(step);
			}
			else
			{
				increment=(-step);
			}
			servo_old[0]=servo_old[0]+facteur*abs(diff)*increment;
		
			//calcul sur y
			diff=(centreImg->y+tolerancem-objectPos.y);
			if(diff>0) 
			{
				increment=(-step);
			}
			else
			{
				increment=(step);
			}
			servo_old[1]=servo_old[1]+facteur*abs(diff)*increment;
				
			if((servo_old[0]<35)||(servo_old[0]>120) || (objectPos.x<0))
			{
				servo_old[0]=60;
			}
			if((servo_old[1]<35)||(servo_old[1]>120) || (objectPos.y<0))
			{
				servo_old[1]=60;
			}

			printf("%d,%d\n", (int) servo_old[0], (int) servo_old[1]);
			fprintf(control_servo, "%d,%d\n", (int) servo_old[0],(int) servo_old[1]);
		}
        compteur++;
			  
        key = cvWaitKey(10);
		 
	}//Fin while
	

    // Destroy the windows we have created
    cvDestroyWindow("Luxo Color Tracking");
    cvDestroyWindow("Luxo Mask");
 
    // Destroy the capture
    cvReleaseCapture(&capture);
    return 0;
 
}

