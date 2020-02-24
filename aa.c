/* ---- Taner MUSTAFA ---- 325 CB ---- Analiza Algoritmilor ---- Tema 2 ----*/

#include <stdio.h>

int main(int argc, char **argv) {

	// se deschid fisierele de input si output
	FILE *input = fopen("graph.in", "r");
	if (input == NULL) {
		printf("Input file could not be opened!\n");
		return 0;
	}

	FILE *output = fopen("bexpr.out", "w");
	if (output == NULL) {
		printf("Output file could not be opened!\n");
		return 0;
	}


	int nrNodes;
	fscanf(input, "%d", &nrNodes);	//nr de noduri

	//se initializeaza matricea de adiacenta cu 0-uri
	int matrix[nrNodes + 1][nrNodes + 1];
	for (int i = 1; i <= nrNodes; i++) {
		for (int j = 1; j <= nrNodes; j++) {
			matrix[i][j] = 0;
		}
	}

	//se citesc muchiile si se face matricea de adiacenta
	int x, y;
	while (1) {			//se realizeaza matricea de adiacenta
		fscanf(input, "%d", &x);
		if (x == -1) {
			break;
		}

		fscanf(input, "%d", &y);
		matrix[x][y] = matrix[y][x] = 1;
	}

	//se creeaza prima "categorie" de ecuatii care verifica daca prin
	//oricare nod se trece de exact 2 ori si daca exista o cale de la
	//nodul 1 la acesta, de maxim N/2+1 distanta.
	int found = 0;	//in astfel de variabile retin daca este necesar sa
					//pun sau nu in fata ecuatiei operatori de genul "&" / "\",
					//(cand este vorba de o prima ecuatie, nu se pune operator).
	for (int h = 1; h <= nrNodes; h++){	//pentru fiecare nod in parte se eval.
										//nodurile adiacente.
		int gasit = 0;
		int contor = 0;	//retine numarul de noduri cu care este adiacent
		for (int g = 1; g <= nrNodes; g++) {
			contor = contor + matrix[h][g];
		}
		if(contor == 0) {
			continue;	//daca exista nod independent, raspunsul va fi mereu FALS.
		} else if (contor == 1) {	//din nod pleaca doar o muchie
			if (found == 0) {
				fprintf(output, "(");
			} else {
				fprintf(output, "&(");
			}
			for (int g = 1; g <= nrNodes; g++) {
				if (matrix[h][g] == 1) {	//asa se verifica adiacenta
					fprintf(output, "(x%d-%d)", h, g);
				}
			}
			fprintf(output, ")");
			found = 1;


			if (h == 1) {
				continue;
			}
			fprintf(output, "&(");
			for (int g = 1; g <= nrNodes / 2 + 1; g++) {
				if (g == 1) {
					fprintf(output, "a%d-%d", g, h);
				} else {
					fprintf(output, "|a%d-%d", g, h);
				}
			}
			fprintf(output, ")");

			continue;
		}
		//aici se ajunge cand din nod pleaca minim 2 muchii
		if (found == 0) {
			fprintf(output, "(");
		} else {
			fprintf(output, "&(");
		}
		for (int i = 1; i <= nrNodes; i++) {	//i si j sunt primele noduri
												//adiacente care se pun in
												//ecuatie, iar k reprezinta
												//nodurile negate din ecuatie.
			if (matrix[h][i] == 1) {
				for (int j = i + 1; j <= nrNodes; j++) {
					if (matrix[h][j] == 1) {
						if (!gasit) {
							fprintf(output, "(x%d-%d&x%d-%d", h, i, h, j);
						} else {
							fprintf(output, "|(x%d-%d&x%d-%d", h, i, h, j);
						}
						gasit = 1;
						for (int k = 1; k <= nrNodes; k++) {
							if ((k != i) && (k != j)) {
								if (matrix[h][k] == 1) {
									fprintf(output, "&~x%d-%d", h, k);
								}
							}
						}
						fprintf(output, ")");
					}
				}
			}
		}
		fprintf(output, ")");
		found = 1;

		if (h == 1) {
			continue;
		}
		fprintf(output, "&(");
		for (int g = 1; g <= nrNodes / 2 + 1; g++) {
			if (g == 1) {
				fprintf(output, "a%d-%d", g, h);
			} else {
				fprintf(output, "|a%d-%d", g, h);
			}
		}
		fprintf(output, ")");
	}


	//se creeaza a 2a "categorie" de ecuatii care testeaza daca parcurgerea
	//drumului dintre 2 noduri este la fel pe ambele sensuri (adevarata sau 
	//falsa) (probabil este utila pentru HCP cu grafuri orientate).
	for (int i = 1; i <= nrNodes; i++) {
		for (int j = i + 1; j <= nrNodes; j++) {
			if (matrix[i][j] == 1) {
				fprintf(output, "&((x%d-%d|~x%d-%d)&(~x%d-%d|x%d-%d))", i, j, j, i, i, j, j, i);
			}
		}
	}

	//Aici incepe crearea celei de-a treia categorii, care verifica pentru
	//fiecare 2 noduri distanta si adiacenta acestora.
	//Am impartit totusi in 2 grupe: prima =>> face verificarea pentru nodurile
	//de distanta 1; iar a 2a pentu nodurile care nu sunt direct adiacente cu
	//nodul 1, testand totodata si nodurile prin care se trece de la 1 pana la 
	//cel cautat. Se observa un fel de recursivitate.

	//se creeaza prima grupa
	//nodurile adiacente cu 1.
	for (int i = 1; i <= nrNodes; i++) {
		if (matrix[1][i] == 1) {
			fprintf(output, "&((a1-%d|~x1-%d)&(~a1-%d|x1-%d))", i, i, i, i);
		}
	}
	//nodurile neadiacente cu 1.
	for (int i = 1; i <= nrNodes; i++) {
		if (matrix[1][i] == 0) {
			fprintf(output, "&~a1-%d", i);
		}
	}

	//se creeaza a 2a grupa
	for (int i = 2; i <= nrNodes / 2 + 1; i++) {
		for (int j = 2; j <= nrNodes; j++) {
			fprintf(output, "&((a%d-%d|~", i, j);	//distanta curenta testata
			int is = 0;
			fprintf(output, "((");
			for (int k = 2; k <= nrNodes; k++) {
				if (matrix[j][k] == 1) {
					if (is == 0) {	//nodurile adiacente testate
						fprintf(output, "(a%d-%d&x%d-%d)", i - 1, k, k, j);
						is = 1;
					} else {
						fprintf(output, "|(a%d-%d&x%d-%d)", i - 1, k, k, j);
					}
				}
			}
			fprintf(output, ")&~(");
			int este = 0;
			//distantele anterior testate (se verifica daca sunt false, altfel 
			//nu s-ar mai f ajuns aici).
			for (int l = 1; l < i; l++) {
				if (este == 0) {
					fprintf(output, "a%d-%d", l, j);
					este = 1;
				} else {
					fprintf(output, "|a%d-%d", l, j);
				}
			}
			fprintf(output, ")))");

			//aici se printeaza aceeasi ecuatie, dar cu termenii invers negati.
			fprintf(output, "&(~a%d-%d|", i, j);
			is = 0;
			fprintf(output, "((");
			for (int k = 2; k <= nrNodes; k++) {
				if (matrix[j][k] == 1) {
					if (is == 0) {
						fprintf(output, "(a%d-%d&x%d-%d)", i - 1, k, k, j);
						is = 1;
					} else {
						fprintf(output, "|(a%d-%d&x%d-%d)", i - 1, k, k, j);
					}
				}
			}
			fprintf(output, ")&~(");
			este = 0;
			for (int l = 1; l < i; l++) {
				if (este == 0) {
					fprintf(output, "a%d-%d", l, j);
					este = 1;
				} else {
					fprintf(output, "|a%d-%d", l, j);
				}
			}
			fprintf(output, "))))");
		}
	}

	return 0;
}
