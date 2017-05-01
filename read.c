/*
 * read.c
 *
 *  Created on: 31 mars 2017
 *      Author: robin
 */
#include "read.h"

double min(double x, double y){
	return x > y ? y : x;
}

double max(double x, double y){
	return x < y ? y : x;
}

int readFile(char * filename, Element*** pIndex, int* pN, int** pEmptyLines, double** pNabla, double** pDelta){

	/* Ouverture du fichier */
	FILE * f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "Le fichier n'existe pas");
		return -1;
	}

	/* Lecture du nombre de sommets et du nombre d'arcs */
	int m;
	if(fscanf(f, "%d", pN) == 0 || fscanf(f, "%d", &m) == 0){
		return -1;
	}

	int n = (*pN);
	printf("n = %d ; m = %d \n", n, m);


	Element** index = (*pIndex) = calloc(n, sizeof(Element*));

	int* emptyLines = (*pEmptyLines) = calloc(n, sizeof(*emptyLines));

	int * columnLength = calloc(n, sizeof(*columnLength));

	double* nabla = (*pNabla) = malloc(n*sizeof(*nabla));
	double* delta = (*pDelta) = malloc(n*sizeof(*delta));

	/* Initialisation Nabla et delta (cout en N au lieu de M tests dans le min)*/
	double precalcSurfer = (1-ALPHA)/n;
	for(int k = 0 ; k < n ; k++){
		nabla[k] = 1;
		delta[k] = precalcSurfer;
	}

	/* Lecture des arcs */
	for(int i = 0 ; i < n ; i++){
		int rowNumber, degree;
		double lastProb = 1.0;

		if(fscanf(f, "%d %d", &rowNumber, &degree) != 2){
			return -1;
		}
		rowNumber--;

		if(degree == 0){
			emptyLines[rowNumber] = 1;

			//TODO Optimisation, ne pas refaire lorsqu'une ligne vide a déjà été vue
			for(int k = 0 ; k < n ; k++){
				nabla[k] = min(nabla[k], ALPHA/n);
				delta[k] = max(delta[k], ALPHA/n);
			}

		}

		for(int numCouple = 0 ; numCouple < degree ; numCouple++){
			int columnNumber;
			double value;
			Element * e = calloc(1, sizeof(Element));

			if(fscanf(f, "%d %lf", &columnNumber, &value) != 2){
				return -1;
			}
			columnNumber --;

			e->rowNumber = rowNumber ;

			//Insertion dans la liste
			e->son = index[columnNumber];
			index[columnNumber] = e;

			columnLength[columnNumber]++;


			//Pour assurer la cohérence, on calcule la dernière proba
			//TODO A cause de l'arrondissement, la dernière proba est
			// souvent majorée alors qu'elle ne devrait pas l'être
			// exemple ligne 281899 de Stanford
			if(numCouple == (degree-1)){
				value = lastProb;
			} else {
				lastProb -= value;
			}

			value = value * ALPHA;
			e->value = value;

			// nabla[0] c'est le minimum de la colonne 0 de G
			nabla[columnNumber] = min(nabla[columnNumber], value + precalcSurfer);
			delta[columnNumber] = max(delta[columnNumber], value + precalcSurfer);

		}

		char suite = fgetc(f);
		while(suite != '\n' && suite != EOF){
			suite = fgetc(f); //Termine de lire la ligne
		}
	}
	fclose(f);

	/* Gestion des potentiels 0 dans les colonnes, qui ne seraient pas compris dans des lignes vides */
	for(int k = 0 ; k < n ; k++){
		if(columnLength[k] != n){
			nabla[k] = min(nabla[k], precalcSurfer);
			delta[k] = max(delta[k], precalcSurfer);
		}
	}

	free(columnLength);
	return 0;
}

