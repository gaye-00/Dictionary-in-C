#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXTE "dictionnaire_projet_C.txt"
#define MAX_BUFFER 100

// Structures

typedef struct noeud {
    struct noeud* gauche;
    struct noeud* droite;
    char valeur;
    int occurrences;
} t_noeud, *p_noeud;

typedef struct arbre {
    p_noeud racine;
} t_arbre, *arbre;

// Prototypes des fonctions
void afficher_statistiques(arbre a);
void compter_noeud(p_noeud noeud, int *total_mots, int *mots_differents);

// Fonctions

p_noeud creer_noeud() {
    p_noeud nouveauNoeud = (p_noeud)malloc(sizeof(t_noeud));
    if (nouveauNoeud == NULL) {
        fprintf(stderr, "Problème d'allocation dynamique de la mémoire\n");
        exit(EXIT_FAILURE);
    }
    nouveauNoeud->gauche = NULL;
    nouveauNoeud->droite = NULL;
    nouveauNoeud->valeur = '\0';
    nouveauNoeud->occurrences = 0;
    return nouveauNoeud;
}

arbre creer_arbre() {
    arbre nouvelArbre = (arbre)malloc(sizeof(t_arbre));
    if (nouvelArbre == NULL) {
        fprintf(stderr, "Problème d'allocation dynamique de la mémoire\n");
        exit(EXIT_FAILURE);
    }
    nouvelArbre->racine = NULL;  // Initialisation correcte
    return nouvelArbre;
}

int ajouter_lettre(p_noeud noeud, char lettre) {
    if (noeud->valeur == lettre) {
        return 0;
    }
    if (noeud->valeur == '\0') {
        noeud->valeur = lettre;
        return 1;
    }
    if (lettre < noeud->valeur) {
        if (noeud->gauche == NULL) {
            noeud->gauche = creer_noeud();
        }
        return ajouter_lettre(noeud->gauche, lettre);
    } else {
        if (noeud->droite == NULL) {
            noeud->droite = creer_noeud();
        }
        return ajouter_lettre(noeud->droite, lettre);
    }
}

int ajouter_mot(p_noeud noeud, char *mot) {
    if (*mot == '\0') {
        noeud->occurrences++;
        return 0;
    }
    if (noeud->valeur == *mot) {
        if (*(mot + 1) == '\0') {
            noeud->occurrences++;
            return 0;
        }
        if (noeud->gauche == NULL) {
            noeud->gauche = creer_noeud();
        }
        return ajouter_mot(noeud->gauche, mot + 1);
    }
    if (noeud->valeur == '\0') {
        noeud->valeur = *mot;
        if (*(mot + 1) == '\0') {
            noeud->occurrences++;
            return 0;
        }
        if (noeud->gauche == NULL) {
            noeud->gauche = creer_noeud();
        }
        return ajouter_mot(noeud->gauche, mot + 1);
    }
    if (*mot < noeud->valeur) {
        if (noeud->gauche == NULL) {
            noeud->gauche = creer_noeud();
        }
        return ajouter_mot(noeud->gauche, mot);
    } else {
        if (noeud->droite == NULL) {
            noeud->droite = creer_noeud();
        }
        return ajouter_mot(noeud->droite, mot);
    }
}

void generer_arbres(arbre arbre_noms, arbre arbre_adjectifs, arbre arbre_verbes) {
    FILE *fichier = fopen(TEXTE, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier %s\n", TEXTE);
        exit(EXIT_FAILURE);
    }

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        char mot[MAX_BUFFER], baseForme[MAX_BUFFER], morphologie[MAX_BUFFER];
        sscanf(ligne, "%99s %99s %99s", mot, baseForme, morphologie);
        if (strstr(morphologie, "Nom:") != NULL) {
            if (arbre_noms->racine == NULL) {
                arbre_noms->racine = creer_noeud();  // Initialisation si nécessaire
            }
            ajouter_mot(arbre_noms->racine, mot);
        } else if (strstr(morphologie, "Adj:") != NULL) {
            if (arbre_adjectifs->racine == NULL) {
                arbre_adjectifs->racine = creer_noeud();  // Initialisation si nécessaire
            }
            ajouter_mot(arbre_adjectifs->racine, mot);
        } else if (strstr(morphologie, "Ver:") != NULL) {
            if (arbre_verbes->racine == NULL) {
                arbre_verbes->racine = creer_noeud();  // Initialisation si nécessaire
            }
            ajouter_mot(arbre_verbes->racine, mot);
        }
    }

    fclose(fichier);
}

void afficher_arbre(p_noeud noeud, char *buffer, int profondeur) {
    if (noeud == NULL) {
        return;
    }

    if (profondeur < MAX_BUFFER - 1) {
        buffer[profondeur] = noeud->valeur;
        buffer[profondeur + 1] = '\0';
        if (noeud->valeur != '\0' && noeud->occurrences > 0) {
            printf("%s (%d)\n", buffer, noeud->occurrences);
        }
        afficher_arbre(noeud->gauche, buffer, profondeur + 1);
        afficher_arbre(noeud->droite, buffer, profondeur);
    } else {
        fprintf(stderr, "Profondeur trop grande pour le buffer\n");
    }
}

void afficher_arbre_complet(arbre a) {
    char buffer[MAX_BUFFER];
    afficher_arbre(a->racine, buffer, 0);
}

void liberer_noeud(p_noeud noeud) {
    if (noeud == NULL) {
        return;
    }
    liberer_noeud(noeud->gauche);
    liberer_noeud(noeud->droite);
    free(noeud);
}

void liberer_arbre(arbre a) {
    if (a != NULL) {
        liberer_noeud(a->racine);
        free(a);
    }
}

void compter_noeud(p_noeud noeud, int *total_mots, int *mots_differents) {
    if (noeud == NULL) {
        return;
    }
    if (noeud->occurrences > 0) {
        (*total_mots) += noeud->occurrences;
        (*mots_differents)++;
    }
    compter_noeud(noeud->gauche, total_mots, mots_differents);
    compter_noeud(noeud->droite, total_mots, mots_differents);
}

void afficher_statistiques(arbre a) {
    int total_mots = 0;
    int mots_differents = 0;

    compter_noeud(a->racine, &total_mots, &mots_differents);
    printf("Nombre total de mots: %d\n", total_mots);
    printf("Nombre de mots différents: %d\n", mots_differents);
}

void menu(arbre arbre_noms, arbre arbre_adjectifs, arbre arbre_verbes) {
    int choix = -1;
    while (choix != 7) {
        printf("\n#########################################################\n");
        printf("############                                 ############\n");
        printf("############        PROJET LANGUAGE C        ############\n");
        printf("############         DICTIONNAIRE            ############\n");
        printf("############                                 ############\n");
        printf("#########################################################\n");
        printf("############    SELECTIONNER UNE ACTION :    ############\n");
        printf("############   RENTRER LE CHIFFRE ASSOCIE    ############\n");
        printf("############                                 ############\n");
        printf("############       1 | AFFICHER NOMS         ############\n");
        printf("############       2 | AFFICHER ADJECTIFS    ############\n");
        printf("############       3 | AFFICHER VERBES       ############\n");
        printf("############       4 | AJOUTER UN MOT        ############\n");
        printf("############       5 | AFFICHER STATISTIQUES ############\n");
        printf("############       6 | AFFICHER STATISTIQUES TOTAL ############\n");
        printf("############       7 | QUITTER               ############\n");
        printf("#########################################################\n");
        printf("CHOIX : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                printf("\nNoms:\n");
                afficher_arbre_complet(arbre_noms);
                break;
            case 2:
                printf("\nAdjectifs:\n");
                afficher_arbre_complet(arbre_adjectifs);
                break;
            case 3:
                printf("\nVerbes:\n");
                afficher_arbre_complet(arbre_verbes);
                break;
            case 4: {
                char mot[MAX_BUFFER];
                char type[MAX_BUFFER];
                printf("Entrer le mot à ajouter : ");
                scanf("%s", mot);
                printf("Entrer le type (Nom, Adj, Ver) : ");
                scanf("%s", type);
                if (strcmp(type, "Nom") == 0) {
                    if (arbre_noms->racine == NULL) {
                        arbre_noms->racine = creer_noeud();  // Initialisation si nécessaire
                    }
                    ajouter_mot(arbre_noms->racine, mot);
                } else if (strcmp(type, "Adj") == 0) {
                    if (arbre_adjectifs->racine == NULL) {
                        arbre_adjectifs->racine = creer_noeud();  // Initialisation si nécessaire
                    }
                    ajouter_mot(arbre_adjectifs->racine, mot);
                } else if (strcmp(type, "Ver") == 0) {
                    if (arbre_verbes->racine == NULL) {
                        arbre_verbes->racine = creer_noeud();  // Initialisation si nécessaire
                    }
                    ajouter_mot(arbre_verbes->racine, mot);
                } else {
                    printf("Type non valide.\n");
                }
                break;
            }
            case 5:
                printf("\nStatistiques des Noms:\n");
                afficher_statistiques(arbre_noms);
                printf("\nStatistiques des Adjectifs:\n");
                afficher_statistiques(arbre_adjectifs);
                printf("\nStatistiques des Verbes:\n");
                afficher_statistiques(arbre_verbes);
                break;
            case 6:
                printf("\nStatistiques Totales:\n");
                afficher_statistiques(arbre_noms);
                afficher_statistiques(arbre_adjectifs);
                afficher_statistiques(arbre_verbes);
                break;
            case 7:
                printf("\nAu revoir!\n");
                break;
            default:
                printf("Choix non valide.\n");
        }
    }
}

int main() {
    arbre arbre_noms = creer_arbre();
    arbre arbre_adjectifs = creer_arbre();
    arbre arbre_verbes = creer_arbre();

    generer_arbres(arbre_noms, arbre_adjectifs, arbre_verbes);
    menu(arbre_noms, arbre_adjectifs, arbre_verbes);

    liberer_arbre(arbre_noms);
    liberer_arbre(arbre_adjectifs);
    liberer_arbre(arbre_verbes);

    return 0;
}
