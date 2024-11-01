#include <stdio.h> //pour printf
#include <ctype.h> //pour le toupper
#include <stdlib.h> //pour le EXIT_FAILURE
#include <string.h> //pour utiliser strlen et avoir acces a la taille du mot


// Fonction pour vérifier si un mot rentre dans la matrice dans une direction donnée

int peut_rentrer(int x, int y, int dx, int dy, int len, size_t nb_lignes, size_t nb_colonnes) 
{
    int nx = x + dx * (len - 1);
    int ny = y + dy * (len - 1);

    if(nx >= 0 && nx < nb_lignes && ny >= 0 && ny < nb_colonnes)
    {
	    return 1; //true , le mot rentre
    }

    return 0; //false , le mot est trop grand
}

// Fonction principale pour résoudre le mot croisé

int* solver(char mot[], char matrice[100][100], int nb_lignes, int nb_colonnes) //matrice defini a 100 car sinon ca fonctionne pas mais a changé pour l'opti je pense 
{

    	static int resultat[4] = {-1, -1, -1, -1}; // Initialisation des coordonnées de retour
    	size_t len = strlen(mot); //def taille du mot recherché
    
    // Directions pour parcourir la matrice : droite, gauche, haut, bas, et diagonales

    int directions[8][2] = {
        {0, 1},  // droite
        {0, -1}, // gauche
        {-1, 0}, // haut
        {1, 0},  // bas
        {1, 1},  // diagonale droite-bas
        {1, -1}, // diagonale gauche-bas
        {-1, 1}, // diagonale droite-haut
        {-1, -1} // diagonale gauche-haut
    };
    
    // Parcourir la matrice pour trouver la première lettre du mot

    for (size_t i = 0; i < nb_lignes; i++) 
    {
        for (size_t j = 0; j < nb_colonnes; j++) 
	{
		//on cherche la premiere lettre

            if (matrice[i][j] == toupper(mot[0])) 
	    { 
		 
 		    // Parcourir toutes les directions possibles

                for (int d = 0; d < 8; d++) 
		{
                    int dx = directions[d][0];
                    int dy = directions[d][1];

                    // Vérifier si le mot rentre dans la matrice dans cette direction pour l'opti ahah

                    if (peut_rentrer(i, j, dx, dy, len, nb_lignes, nb_colonnes) == 1) 
		    {
                        int match = 1; //bool true

                        // Vérifier chaque lettre du mot dans cette direction

                        for (size_t k = 1; k < len; k++) 
			{
                            int nx = i + dx * k;
                            int ny = j + dy * k;
                            if (matrice[nx][ny] != toupper(mot[k])) 
			    {
                                match = 0; //bool false
                                break;
                            }
                        }

                        // Si toutes les lettres correspondent, on retourne les coordonnées

                        if (match == 1) 
			{
                            resultat[0] = i; //debut x
                            resultat[1] = j; //debut y
			    resultat[2] = i + dx * (len-1); //fin x
			    resultat[3] = j + dy * (len-1); //fin de y

                            return resultat;
                        }
                    }
                }
            }
        }
    }

    printf("Le mot n'est pas présent dans la matrice\n");
    return resultat;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
		return EXIT_FAILURE;

	FILE* file = fopen(argv[1], "r"); //r == read
	
	if(file == NULL)
	{
		return EXIT_FAILURE;
		printf("problème de fichier\n");
	}

	char matrice[100][100]; //a modifier pour mettre la taille de ma matrice
	int nb_lignes = 0;
	int nb_colonnes = 0;

	while (fscanf(file, "%s", matrice[nb_lignes]) != EOF) //EOF == end of file
	{
		nb_colonnes = strlen(matrice[nb_lignes]);
        	nb_lignes++;
    	}

    	fclose(file);

    	char* mot = argv[2];
    	int* resultat = solver(mot, matrice, nb_lignes, nb_colonnes);

    	printf("(%d, %d)(%d, %d)\n", resultat[0], resultat[1], resultat[2], resultat[3]);

    	return 0;
}

