//Partie du code sur l'interface graphique

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <time.h>

using namespace sf; //Espace de nommage nous permet de ne pas utiliser sf:: à chaque fois


//Structure d'une image
typedef struct T_image
{
	int numero;
	sf::Texture f_image;
	sf::Sprite S_image;

	T_image *left;
	T_image *right;
	T_image *up;
	T_image *down;

} t_image;


//Structure de la map
typedef struct T_map
{
	int nombre_images;
	t_image *head;
} t_map;



int main()
{
	//On crée la fenêtre du projet
	sf::RenderWindow ecran(VideoMode(5000, 4000, 32), " Projet Milot-Diot ");

	/*Ici on doit créer tous les mallocs pour les structures et les différentes images*/
	t_map *AppartAntoine=(t_map*)malloc(sizeof(t_map));
	AppartAntoine->head=NULL;
	AppartAntoine->nombre_images=11;

	//Compteur global qui va nous servir à numéroter nos scènes
	int compt_glob=0;
	
	/*Malloc de chaque image  // faire les tests de réussites des mallocs*/
	t_image tI_h1;          // OK
	t_image tI_h2;          // OK
	t_image tI_exit;        // OK
	t_image tI_chambre;     // OK
	t_image tI_Ptable;      // OK
	t_image tI_Pcuisine;    // OK
	t_image tI_Ptele;	   // OK
	t_image tI_Phall;	   // OK
	t_image tI_messageE;    // OK
	t_image tI_messageInd;  
	t_image tI_messageD;

	
	
	/*On fait le chargement de chaque image*/

	/*Texture 0 ; Chargement des règles*/
	sf::Texture I_messageD;
	sf::Sprite S_messageD;
	
	if(!(I_messageD.loadFromFile("messageD.bmp")))
	{
		printf("Erreur dans le chargement de l'image %d\n",compt_glob);
	}
	else
	{

		S_messageD.setTexture(I_messageD);	
		tI_messageD.numero=compt_glob;
		tI_messageD.f_image=I_messageD;
		tI_messageD.S_image=S_messageD;
	


		//On tisse les liens avec cette image
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

		ecran.clear();
		ecran.draw(tI_messageD.S_image);
		ecran.display();

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
	
	if (!(I_messageInd.loadFromFile("messageInd.bmp"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		S_messageInd.setTexture(I_messageInd);
		tI_messageInd.S_image=S_messageInd;
		/*ecran.clear();
		ecran.draw(S_messageD);
		ecran.display();*/
		

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
	
	if (!(I_h1.loadFromFile("h1.jpg"))) // Si le chargement du fichier a échoué
	{
    	printf("Erreur dans le chargement de l'image n° %d\n", compt_glob);
	}
	else
	{
		//Si le chargement de l'image s'est fait, on crée le sprite et on le met dans la structure
		S_h1.setTexture(I_h1);
		tI_h1.S_image=S_h1;
		tI_h1.numero=compt_glob;
		tI_h1.f_image=I_h1;

		//On tisse les liens avec cette image
		tI_h1.left=&tI_h2;
		tI_h1.right=&tI_exit;
		tI_h1.up=&tI_Pcuisine;
		tI_h1.down=&tI_exit;
		compt_glob++;
	}

	/*Texture 4 ; hall2*/
	Texture I_h2;
	Sprite S_h2;
	
	if (!(I_h2.loadFromFile("h2.jpg"))) // Si le chargement du fichier a échoué
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
	
	if (!(I_exit.loadFromFile("Exit.jpg"))) // Si le chargement du fichier a échoué
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
	
	if (!(I_chambre.loadFromFile("chambre.jpg"))) // Si le chargement du fichier a échoué
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
	
	if (!(I_Ptable.loadFromFile("ptable.jpg"))) // Si le chargement du fichier a échoué
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
	
	if (!(I_Pcuisine.loadFromFile("p_cuisine.jpg"))) // Si le chargement du fichier a échoué
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
		compt_glob++;
	}

	/*Texture 9 ; Texture de la tele*/
	Texture I_Ptele;
	Sprite S_Ptele;
	
	if (!(I_Ptele.loadFromFile("p_tele.jpg"))) // Si le chargement du fichier a échoué
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
		compt_glob++;
	}

	/*Texture 10 ; image du hall à partir de la cuisine*/
	Texture I_Phall;
	Sprite S_Phall;
	
	if (!(I_Phall.loadFromFile("phall.jpg"))) // Si le chargement du fichier a échoué
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

	//On veut savoir combien d'images n'ont pas été chargées
	if(compt_glob<AppartAntoine->nombre_images)
	{
		printf("%d images n'ont pas été chargées\n", AppartAntoine->nombre_images-compt_glob);
		return 0;
	}

	t_image *scene_courante=AppartAntoine->head;

    
    // Boucle principale
    while (ecran.isOpen())
    {
        Event event;

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


        while (ecran.pollEvent(event))
        {
            if (event.type == Event::Closed)
                ecran.close();

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
            
        // Clean de la fenêtre d'affichage
        ecran.clear();

        //draw le sprite
        
        ecran.draw(scene_courante->S_image);

        // Affichage de la fenêtre à l'écran
        ecran.display();
    }
    
    return 0;
}
