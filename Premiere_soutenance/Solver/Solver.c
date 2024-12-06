#include <stdio.h> //for printf
#include <ctype.h> //for toupper
#include <stdlib.h> //for EXIT_FAILURE
#include <string.h> //for strlen


//function for checking whether a word enters the matrix in a given direction
int peut_rentrer(int x, int y, int dx, int dy, int len, size_t nb_lignes, 
size_t nb_colonnes) 
{
    int nx = x + dx * (len - 1);
    int ny = y + dy * (len - 1);

    if(nx >= 0 && nx < nb_lignes && ny >= 0 && ny < nb_colonnes)
    {
	    return 1; //true , the word enters
    }

    return 0; //false , the word is too big
}

// Main fonction for solving the crossword

int* solver(char mot[], char matrice[100][100], int nb_lignes, int nb_colonnes)
{

    	static int resultat[4] = {-1, -1, -1, -1}; 
    	size_t len = strlen(mot);
    
    // Matrix directions : right, left, up, down and diagonals

    int directions[8][2] = {
        {0, 1},  // right
        {0, -1}, // left
        {-1, 0}, // up
        {1, 0},  // down
        {1, 1},  // diagonal right-down
        {1, -1}, // diagonal left-down
        {-1, 1}, // diagonal right-up
        {-1, -1} // diagonal left-up
    };
    
    // Scqn the matrix to find the first letter of the word

    for (size_t i = 0; i < nb_lignes; i++) 
    {
        for (size_t j = 0; j < nb_colonnes; j++) 
	{
		//search for the first letter

            if (matrice[i][j] == toupper(mot[0])) 
	    { 
		 
 		    //travel in all possible directions

                for (int d = 0; d < 8; d++) 
		{
                    int dx = directions[d][0];
                    int dy = directions[d][1];

                    //check if the word fits into the matrix in this direction 
//for optimization 

                    if (peut_rentrer(i, j, dx, dy, len, nb_lignes, nb_colonnes)
 == 1) 
		    {
                        int match = 1; //bool true

                        //check each letter of the word in this direction

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

                        if (match == 1) 
			{
                            resultat[0] = i; //start x
                            resultat[1] = j; //start y
			    resultat[2] = i + dx * (len-1); //end x
			    resultat[3] = j + dy * (len-1); //end y

                            return resultat;
                        }
                    }
                }
            }
        }
    }

    printf("The word is not present in the matrix\n");
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

	char matrice[100][100]; 
	int nb_lignes = 0;
	int nb_colonnes = 0;

	while (fscanf(file, "%s", matrice[nb_lignes]) != EOF) //EOF == end of 
//file
	{
		nb_colonnes = strlen(matrice[nb_lignes]);
        	nb_lignes++;
    	}

    	fclose(file);

    	char* mot = argv[2];
    	int* resultat = solver(mot, matrice, nb_lignes, nb_colonnes);

    	printf("(%d, %d)(%d, %d)\n", resultat[0], resultat[1], resultat[2], 
resultat[3]);

    	return 0;
}

