//Partie du code sur l'interface graphique

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <time.h>

using namespace sf; //Espace de nommage nous permet de ne pas utiliser sf:: à chaque fois


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
typedef struct t_mot
{
	int nb_caracteres;
	int nb_chances;
	char motMystere[nb_caracteres];
	char motSortie[5];

} t_mot;

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
		if(tentative[compt]!=motM->motSortie[compt])
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


/**
*	
*	Fonction générale qui permet de lancer le jeu
*/
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

	/*Texture 0 ; Chargement des règles*/
	sf::Texture I_messageD;
	sf::Sprite S_messageD;
	if(!(I_messageD.loadFromFile("messageD_dir.bmp"))) //Chargement de l'image, s'il ne se fait pas message d'erreur
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

	/*Texture 1 ; Texture si on veut sortir sans avoir fait les formes*/
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

	/*Texture 2 ; Texture si on veut sortir sans avoir fait les formes*/
	Texture I_messageInd;
	Sprite S_messageInd;
	if (!(I_messageInd.loadFromFile("messageInd_dir.bmp"))) // Si le chargement du fichier a échoué
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

/*-------------------------------OUVERTURE DE LA FENÊTRE CAMERA + SES VARIABLES --------------------------------*/


/*-------------------------------DECLARATION DU MOT MYSTERE -----------------------------*/

	t_mot *tM_mystere;
	tM_mystere->nb_caracteres=5;
	tM_mystere->nb_chances=3;
	tM_mystere->mot="pixar";
	tM_mystere->motSortie="later";

/*-------------------------------BOUCLE PRINCIPALE DU JEU ----------------------------*/

	//On commence par initialiser toutes les variables utiles
	t_image *scene_courante=AppartAntoine->head;
	char *motRentre[5];
	int exitAutorisee=-1; //Par défaut on ne peut pas sortir, si on passe à 0 c'est qu'on a débloqué la sortie

    
    //Boucle principale
    while (ecran.isOpen())
    {
        Event event; //Variable qui nous permet de gérer les événements clavier

        //Chargement d'une musique
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

		//Bouclle qui gère les événements, reste dans la boucle tant qu'elle n'a pas gérer tous les événements
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

      //On regarde si on se trouve dans l'image ou on peut rentrer le mot à trouver         
        if(scene_courante->numero==1)
         {
         	 //Dans ce cas si, on va demander au joueur de rentrer un mot et regarder s'il est valide
         	printf("Vous pensez pouvoir sortir ??\n");
         	printf("Alors quel est le mot mystère ?\n");
         	fgets(motRentre, 5, stdin); //permet de rentrer une chaine de 5 caractères
         	viderBuffer() //On vide le buffer pour la prochaine execution

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
         if(zone==ZOOM) //cette zone ne prends en compte que 3 images, la fonction qui nous permet de trouver la zone gère directement les cas
         {
         	 scene_courante=scene_courante->indice;
         } 
         else if(zone==GAUCHE)
         {
         	 scene_courante=scene_courante->left;
         }
         else if(zone==DROITE)
         {
         	 scene_courante=scene_courante->right;
         }
         else if(zone==HAUT)
         {
         	 scene_courante=scene_courante->up;
         }
         else if(zone==BAS)
         {
         	 scene_courante=scene_courante->down;
         }
            
        // Clean de la fenêtre d'affichage
        ecran.clear();
        //draw le sprite
        ecran.draw(scene_courante->S_image);
        // Affichage de la fenêtre à l'écran
        ecran.display();

        //On a fini ce test on revient en haut de la boucle
    }
    
    return 0;
}
