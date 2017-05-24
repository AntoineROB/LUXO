#include "ros/ros.h"
#include "ros/console.h"
#include "math.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"
#include "geometry_msgs/Twist.h"
#include <sstream>


// Variables globales
int X_centre_imagette=0, Y_centre_imagette=0, Xref = 0, Yref = 0; //ROI_SIZE = 120;
int Traitement_OK = 0, Fin_programme = 0,Initialisation = 0;
int i, j, Nb, fd;
int Gx = 0, Gy = 0;
double Vx = 0, Vy = 0;
double stepX = 0, stepY = 0;
int ROI_SIZE=200;
int SEUIL=150;
CvRect roi;

ros::Publisher chatter_pub;
ros::Publisher gripper_pub;
int count = 0;

// Création des imagettes pour le traitement des images 
IplImage *image_gris = cvCreateImage( cvSize( ROI_SIZE, ROI_SIZE ), 8, 1 );
IplImage *image_laplace = cvCreateImage( cvSize( ROI_SIZE, ROI_SIZE ), 32, 1 );
IplImage *image_normalisee = cvCreateImage( cvSize( ROI_SIZE, ROI_SIZE ), 8, 1 );
IplImage *image_binarisee = cvCreateImage( cvSize( ROI_SIZE, ROI_SIZE ), 8, 1 );

//IplImage * image_couleur;
int SIZE_X = 0;
int SIZE_Y = 0;

void envoieCommandesRobot(float vx,float vy,float vz)
{
        if (Fin_programme==1){
		vx = 0.0;
		vy = 0.0;
		vz = 0.0;
	}
	if (fabs(vx) > 0.05)
		vx = 0.05*fabs(vx)/vx;
	if (fabs(vy) > 0.05)
		vy = 0.05*fabs(vy)/vy;
	if (fabs(vz) > 0.05)
		vz = 0.05*fabs(vz)/vz;
	//vx = 0.0;
	//vy = 0.0;
	geometry_msgs::Twist twist_msg;
  	twist_msg.linear.x = vx;
  	twist_msg.linear.y = vy;
  	twist_msg.linear.z = vz;
  	twist_msg.angular.x = 0.0;
  	twist_msg.angular.y = 0.0;
  	twist_msg.angular.z = 0.0;
	//printf("pos y: %f \n",twist_msg.linear.y);
	chatter_pub.publish(twist_msg);
}

void fermerPinceEtRemonter()
{
	std_msgs::Bool msg_bool;
	msg_bool.data = true;
	gripper_pub.publish(msg_bool);
	envoieCommandesRobot(0,0,-0.02);
	Traitement_OK = 0;
}

/* Callback de gestion de la souris */
void mouseEvents( int event, int x, int y, int flags )
{
/* cette fonction est appelée lorsque l'on clique sur la fenere de visualisation d'une image.
Elle passe en paramètres la position de la souris dans l'image en pixels (x et y) ainsi que l'évenement (sur quel bouton on a cliqué) et un flag (par exemple, "le bouton shift est appuyé"). */

	if( ( event == CV_EVENT_LBUTTONDOWN ) ) { // APPUI SUR LE GAUCHE
		if ( flags & CV_EVENT_FLAG_CTRLKEY ) { // AVEC CTRL, ON PLACE L'IMAGETTE
			X_centre_imagette = x;
			Y_centre_imagette = y;
		}
		else if ( flags & CV_EVENT_FLAG_SHIFTKEY  ){ // AVEC SHIFT, ON PLACE LE POINT DESIRE
			Xref = x;
			Yref = y;
			printf("Xref =%d Yref=%d\n",Xref,Yref);
		}
	}
	
	if( ( event == CV_EVENT_RBUTTONDOWN ) ) { // APPUI SUR LE DROIT
		if ( flags & CV_EVENT_FLAG_CTRLKEY ) { // avec CTRL ON DEMARRE LE TRAITEMENT D'IMAGE
			Traitement_OK = 1;
		}
		else if ( flags & CV_EVENT_FLAG_SHIFTKEY  ){ // AVEC SHIFT ON MET FIN AU PROGRAMME
			envoieCommandesRobot(0.0,0.0,0.0);
			Fin_programme = 1;
		}
	}
}


/*****
 
	ICI SE TROUVE LA SEULE FONCTION DANS LAQUELLE VOUS POUVEZ CODER

*****/

void asservissementRobot(IplImage * image_couleur)
{
	// NE PAS TOUCHER AU DEBUT DE LA FONCTION
	// NE PAS TOUCHER AU DEBUT DE LA FONCTION
	// NE PAS TOUCHER AU DEBUT DE LA FONCTION
	// NE PAS TOUCHER AU DEBUT DE LA FONCTION
	// NE PAS TOUCHER AU DEBUT DE LA FONCTION
	// NE PAS TOUCHER AU DEBUT DE LA FONCTION
	if (Initialisation == 0)
	{
		printf("initialisation\n");
		Initialisation = 1;

		cvNamedWindow("Image initiale", CV_WINDOW_AUTOSIZE);
		cvNamedWindow("Image traitee", CV_WINDOW_AUTOSIZE);
		cvStartWindowThread();
	
		SIZE_X = image_couleur->width;
		SIZE_Y = image_couleur->height;
		
		printf("Taille de l'image X=%d Y=%d\n",SIZE_X,SIZE_Y);
		
		X_centre_imagette = round(SIZE_X/2.0);
		Y_centre_imagette = round(SIZE_Y/2.0);
		Xref = round(SIZE_X/2.0);
		Yref = round(SIZE_Y/2.0);

		// Lancement du Callback souris
		cvSetMouseCallback("Image initiale",(CvMouseCallback) mouseEvents , NULL);
		cvSetMouseCallback("Image initiale",(CvMouseCallback) mouseEvents , NULL);
	
	}
	// On ne fait le traitement d'image que si la zone d'intérêt est entièrement dans l'image et si on a cliqué sur le bouton 	droit
	if( Traitement_OK ){

		if ( X_centre_imagette < ( ROI_SIZE / 2 ) ) 
			X_centre_imagette = (int) (ROI_SIZE/2) + 1;
		if ( Y_centre_imagette < ( ROI_SIZE / 2 ) )
			Y_centre_imagette = (int) (ROI_SIZE/2) + 1;
		if ( X_centre_imagette > ( SIZE_X - ROI_SIZE / 2 ) )
			X_centre_imagette  = ( SIZE_X - (int) ROI_SIZE / 2 ) -1;
		if ( Y_centre_imagette > ( SIZE_Y - ROI_SIZE / 2 ) )
			Y_centre_imagette  = ( SIZE_Y - (int) ROI_SIZE / 2 ) -1;

		// Définition de la ROI
		roi = cvRect( ( X_centre_imagette - ( ROI_SIZE / 2 ) ) , ( Y_centre_imagette - ( ROI_SIZE / 2 ) ), ROI_SIZE, ROI_SIZE );
		cvSetImageROI( image_couleur, roi ); // réduit l'image couleur à sa zone d'intérêt. A partir de maintenant, le pixel 			(0,0) de image_couleur est le coin en haut à gauche de l'imagette)
			

		// ICI COMMENCE LA PARTIE A CODER. NE RIEN CHANGER AU DESSUS
		// ICI COMMENCE LA PARTIE A CODER. NE RIEN CHANGER AU DESSUS
		// ICI COMMENCE LA PARTIE A CODER. NE RIEN CHANGER AU DESSUS
		// ICI COMMENCE LA PARTIE A CODER. NE RIEN CHANGER AU DESSUS
		// ICI COMMENCE LA PARTIE A CODER. NE RIEN CHANGER AU DESSUS
		// ICI COMMENCE LA PARTIE A CODER. NE RIEN CHANGER AU DESSUS
		// ICI COMMENCE LA PARTIE A CODER. NE RIEN CHANGER AU DESSUS
		// ICI COMMENCE LA PARTIE A CODER. NE RIEN CHANGER AU DESSUS

		/////////////////
		// PARTIE 1 : C'est ici que l'on doit faire les traitements d'images, en construisant des nouvelles images, dans 			l'ordre
		// image_gris, image_binarisee
		
		// Conversion de l'image couleur en niveaux de gris 
		cvCvtColor(image_couleur, image_gris, CV_RGB2GRAY);

		// Binarisation
		cvThreshold(image_gris, image_binarisee,200,255,CV_THRESH_BINARY);
		
		// Affichage dans la zone trait�e.
		cvShowImage("Image traitee", image_binarisee); 

		
		/////////////////
		// PARTIE 2 : CALCUL DU BARYCENTRE ET DU DIAMETRE DE LA TACHE BLANCHE
		
		// Calcul du centre de la tâche, Gx, et Gy 
		
		int Gx,Gy,i,j;
		int Xmin,Xmax,Ymin,Ymax,d,sX,sY,nbP;
		sX=0;
		sY=0;
		Xmax=0;
		Xmin=ROI_SIZE-1;
		Ymax=0;
		Ymin=ROI_SIZE-1;
		d=0;
		nbP=0;
		
		for(i=0;i<ROI_SIZE-1;i++)
		{
			for(j=0;j<ROI_SIZE-1;j++)
			{
				if(cvGet2D(image_binarisee,j,i).val[0]==255)
				{
					sX+=i;
					sY+=j;
					if(i<Xmin)
						Xmin=i;
					if(j<Ymin)
						Ymin=j;
					if(i>Xmax)
						Xmax=i;
					if(j>Ymax)
						Ymax=j;
					nbP++;
				}	
				
			}
		}
		Gx=cvRound(sX/nbP);
		Gy=cvRound(sY/nbP);
		
		
		// Calcul du diametre de la tâche
		d=(Xmax-Xmin + Ymax-Ymin)/2;
		
		// Tracé de deux cercles : un pour le centre et un pour le contour de la taĉhe
		cvCircle(image_couleur, cvPoint(Gx,Gy),5,CV_RGB(255,0,0),2,8,0);
		cvCircle(image_couleur, cvPoint(Gx,Gy),d/2,CV_RGB(0,255,0),2,8,0);
		
		//printf("\n d=%d pixels \n ",d);
				
		//////////////////
		// PARTIE 3 c'est ici que l'on modifie X_centre_imagette et Y_centre_imagette pour que la ROI suive le point tracké
		float alpha=0.5;
		X_centre_imagette += alpha*(Gx-ROI_SIZE/2);
		Y_centre_imagette += alpha*(Gy-ROI_SIZE/2);

		//////////////////
		// PARTIE 4 : c'est ici qu'on envoie les commandes de mouvement du bras en m/s, max = 0.05m/s
		double vitRobotCart_x = 0.0;
		double vitRobotCart_y = 0.0;
		double vitRobotCart_z = 0.0;

		// Calculer les coordonnées du point tracké dans l'image de départ sachant que ses coordonnées dans l'imagette sont 			Gx et Gy
		int Xi=X_centre_imagette - ROI_SIZE/2+Gx;
		int Yi=Y_centre_imagette - ROI_SIZE/2+Gy;
		

		
		// Calculez l'erreur de position dans l'image XY (la consigne en pixels en stockée dans int Xref et int Yref
		Xref=380;
		Yref=115;
		int Dref=95;
		
		int erreur_x, erreur_y,er_z;
		erreur_x=Xref-Xi;
		erreur_y=Yref-Yi;
		er_z=Dref-d;
		
		//std::cout<< "vit" << erreur_x<< " "<<erreur_y<<std::endl;
		

		// Calculez la vitesse désirée en pixels / secondes
		double vitPixel_x, vitPixel_y,vitPix_z;
		float lambda=0.8;
		vitPixel_x=lambda*erreur_x;
		vitPixel_y=lambda*erreur_y;
		vitPix_z=lambda*er_z;
		
		

		// Calculez les commandes à envoyer au bras;
		float beta=0.00073;
		vitRobotCart_x = -beta * vitPixel_y;
		vitRobotCart_y = beta * vitPixel_x;
		vitRobotCart_z=beta*vitPix_z;

		// PARTIE 5 : ASSERVISSEMENT DE LA PROFONDEUR

		// Calcul de l'erreur
		int diametreRef;
		int erreur_z;

		// Calculez la vitesse désirée en pixels / secondes
		double vitPixel_z;

		// ENVOI DES COMMANDES SELON Z SI L'ERREUR EN X ET Y EST SUFFISAMMENT PETITE (SINON COMMANDE NULLE)
		//vitRobotCart_z = 

		// FERMETURE DE LA PINCE ET return(); SI OBJECTIF ATTEINT
		
		if(erreur_x<10&&erreur_y<10&&er_z<10)
		{
			fermerPinceEtRemonter();
			return;
		}
		
		
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		// NE PAS CODER SOUS CETTE LIGNE
		////////////////////////////////
		

		envoieCommandesRobot(vitRobotCart_x,vitRobotCart_y,vitRobotCart_z);


		cvResetImageROI( image_couleur );  
		cvRectangle( image_couleur, cvPoint( cvRound( X_centre_imagette - ROI_SIZE / 2 ), cvRound( Y_centre_imagette - ROI_SIZE / 2 ) ), 
		cvPoint( cvRound( X_centre_imagette + ROI_SIZE / 2 ),cvRound( Y_centre_imagette + ROI_SIZE / 2 ) ), CV_RGB( 255, 0, 0 ), 2, 8, 0); 

	}
	cvShowImage( "Image initiale", image_couleur);
		
}



void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	try
	{
		asservissementRobot(new IplImage(cv_bridge::toCvShare(msg, "bgr8")->image));
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
	}
}



int main(int argc, char **argv)
{
	ros::init(argc, argv, "tp");
	ros::NodeHandle nh;
	image_transport::ImageTransport it(nh);
	image_transport::Subscriber sub = it.subscribe("cameras/right_hand_camera/image", 1, imageCallback);
	ros::NodeHandle n;
	chatter_pub = n.advertise<geometry_msgs::Twist>("chatter",100);
	gripper_pub = n.advertise<std_msgs::Bool>("gripperAction",100);
	printf("Appuyer sur 'o' pour ouvrir ou 'f' pour fermer le gripper\n");
	std_msgs::Bool msg_bool;
	cvWaitKey(100);
	
	while(ros::ok() && Fin_programme == 0){
		ros::spinOnce();
		int key = cvWaitKey(10);
		//if (key != -1) printf("key:%d \n",key);
		if (key == 1048687){
			printf("'o' pressed\n");
			msg_bool.data = false;
			gripper_pub.publish(msg_bool);
		}
		else if (key == 1048678){
			printf("'f' pressed\n");
			msg_bool.data = true;
			gripper_pub.publish(msg_bool);
		}
	}
	cvDestroyWindow("Image initiale");
}




