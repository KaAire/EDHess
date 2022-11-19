/*
Projet EDHess par Malo DOUAIRET : e2100408
Virtualisation d'un reseau electrique entre une/des source(s) et un/des consommateur(s)
Version 1.0 : 25/10/22
Dans cette version, l'IHM est le plus haut niveau que je puisse developper avec les cours
Et aussi en utilisant un peu internet
Internet utile uniquement pour : sprintf(), atoi(), et la methode curseur (juste l'idee de treeees loin)
*/
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<string.h>
/*
On definit les parametres lies a la presentation graphique du programme
HD = Haut Droit
*/
#define HD_Fonctions_x 		0
#define HD_Fonctions_y 		0
#define HD_Centrales_x 		0
#define HD_Centrales_y 		45
#define HD_Villes_x 		0
#define HD_Villes_y 		80
#define HD_Lignes_x 		17
#define HD_Lignes_y 		0
#define HD_Inputs_x			10
#define HD_Inputs_y			0
#define longueur_Fonctions 	40
#define longueur_Centrales 	30
#define longueur_Villes 	30
#define longueur_Lignes 	40

///////////////////////////////////////////////////////////////////
////////// TYPEDEF ////////////////////////////////////////////////

typedef struct ville {
	int codePostal;
	struct ville * villeSuivante;
} Tville;

typedef Tville * PTville;

typedef struct lignesElectrique {
	int puissance;
	// pointeur sur une ville
	PTville villeDesservie;	
	// liste simplement chain�e
	struct lignesElectrique * ligneSuivante ;
} TlignesElectrique;

typedef TlignesElectrique * PTligneElectrique;
		
typedef struct centrale{
	int codeCentrale;
	int puissanceMax;
	// Pointeur sur la liste des lignes
	PTligneElectrique villeDependante;
	// Liste doublement chainee
	struct centrale * ptsuivant;
	struct centrale * ptprecedent;
}Tcentrale;

typedef Tcentrale * PTcentrale;

typedef struct curseur{
	int x;
	int y;
	char *phrase;
}Tcurseur;

///////////////////////////////////////////////////////////////////
/////////// AFFICHAGE /////////////////////////////////////////////

void gotoLigCol( int lig, int col ){
	// ressources
	COORD mycoord;
	
	mycoord.X = col;
	mycoord.Y = lig;
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), mycoord );
}

void cadre(int x, int y, int haut, int larg){
	int i,j;
	char c,cvertical,  	chorizontal,   	cangleGH,    	cangleGB,    	cangleDH,    	cangleDB;
	cvertical=186;
	chorizontal=205;
	cangleGH=201;
	cangleGB=200;
	cangleDH=187;
	cangleDB=188;
	
	//ligne haut
	gotoLigCol(x,y);
	printf("%c",cangleGH);
	i=larg; 
	while(i>0)	{	
		printf("%c",chorizontal);
		i=i-1;
	};
	printf("%c",cangleDH);
	
	// ligne intermediaires
	j= haut;

	while(j>0){
		gotoLigCol(x+j,y);	
		printf("%c",cvertical);
		i=larg; 
		while(i>0)	{	
			printf("%c",' ');
			i=i-1;
		};
		j=j-1;
		printf("%c",cvertical);
	}
	//ligne bas
	gotoLigCol(x+haut,y);
	printf("%c",cangleGB);
	i=larg; 
	while(i>0)	{	
		printf("%c",chorizontal);
		i=i-1;
	};
	printf("%c",cangleDB);
}

void effacerLigne(int x, int y, int taille){
	/*
	Efface Taille caracteres vers la gauche via la position x,y
	*/
	gotoLigCol(x,y);
	while(taille>0){
		printf(" ");
		taille--;
	}
}

void afficherCurseur(Tcurseur cursor){
	/*
	Affiche en fond blanc, lettres noires, une chaine de caracteres
	*/
	if(cursor.x != -1){
		gotoLigCol(cursor.x, cursor.y);
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 240);
		printf("%s",cursor.phrase);
		SetConsoleTextAttribute(hConsole, 15);
		gotoLigCol(cursor.x, cursor.y);
	}
}

int lireCaract(){
	char c=10;
	int fnt=0;	
	
	c=getch();
	fnt=c;
	if (c==-32){c=getch();fnt = 400+c;}
	if (c==0)  {c=getch();fnt = 500+c;}
	return fnt;	
}

void effacerMenuFonctions(){
	/*
	Efface le contenu du cadre des Fonctions
	Repetition selon l'axe x de la fonction effacerLigne
	*/
	int taille=8-1;
	while(taille > 0){
		effacerLigne(HD_Fonctions_x+taille,HD_Fonctions_y+1,longueur_Fonctions);
		taille--;	
	}
}

void effacerCadreCentrales(PTcentrale pdebutC){
	/*
	De la meme facon, efface le cadre des centrales 
	*/
	int taille=longueurListeCentrale(pdebutC)+1;
	while(taille>0){
		effacerLigne(HD_Centrales_x+taille,HD_Centrales_y+1,longueur_Centrales);
		taille--;
	}
}

void cadreCentrale(PTcentrale pdebutC){
	/*
	Affiche aux positions definies le cadre des centrales
	Avec le titre dans le cadre
	*/
	int taille=longueurListeCentrale(pdebutC);
	cadre(HD_Centrales_x,HD_Centrales_y,taille+2,longueur_Centrales);
	gotoLigCol(HD_Centrales_x,HD_Centrales_y+2);
	printf("Liste des centrales :");
	gotoLigCol(HD_Centrales_x+1,HD_Centrales_y+2);
}

void afficherPuissanceAlloueeDesCentrales(PTcentrale pdebutC){
	/*
	Remplace les centrales avec leurs puissance maximale
	par les centrales et la puissance allouee
	*/
	PTcentrale px=pdebutC->ptsuivant;
	int ligneX=2;
	cadreCentrale(pdebutC);
	printf("Code Centrale | P allouee");
	while(px->ptsuivant != NULL){
 		gotoLigCol(ligneX,HD_Centrales_y+3);
		printf("%12d | %12d", px->codeCentrale, puissanceConsommee(px));
		ligneX++;
		px = px->ptsuivant;
	}	
}

void afficherPuissanceRestante(PTcentrale pdebutC){
	/*
	Remplace le texte dans le cadre des centrales
	par les centrales avec la puissance restante
	*/
	PTcentrale px=pdebutC->ptsuivant;
	int ligneX=2, sommePuissance=0;
	cadreCentrale(pdebutC);
	printf("Code Centrale | P restante");
	while(px->ptsuivant != NULL){
		gotoLigCol(HD_Centrales_x+ligneX, HD_Centrales_y+3);
		printf("%12d | %12d", px->codeCentrale, puissanceRestante(px));
		sommePuissance = sommePuissance + puissanceRestante(px);
		ligneX++;
		px = px->ptsuivant;
	}
	gotoLigCol(HD_Inputs_x+4,HD_Inputs_y+1);
	printf("Puissance restante max : %5d", sommePuissance);
}
		
void menuFonctionsCentrales(){
	gotoLigCol(HD_Fonctions_x+1,HD_Fonctions_y+1);
	printf("F1 - Ajouter une centrale");
	gotoLigCol(HD_Fonctions_x+2,HD_Fonctions_y+1);
	printf("F2 - Supprimer une centrale");
	gotoLigCol(HD_Fonctions_x+3,HD_Fonctions_y+1);
	printf("F3 - Modifier la puissance");
	gotoLigCol(HD_Fonctions_x+4,HD_Fonctions_y+1);
	printf("F4 - Ajouter une liaison");
	gotoLigCol(HD_Fonctions_x+5,HD_Fonctions_y+1);
	printf("F5 - Supprimer une liaison");
	gotoLigCol(HD_Fonctions_x+6,HD_Fonctions_y+1);
	printf("ESC - Quitter ce menu");
}

void menuFonctionsVilles(){
	gotoLigCol(HD_Fonctions_x+1,HD_Fonctions_y+1);
	printf("F1 - Ajouter une ville");
	gotoLigCol(HD_Fonctions_x+2,HD_Fonctions_y+1);
	printf("F2 - Supprimer une ville");
	gotoLigCol(HD_Fonctions_x+3,HD_Fonctions_y+1);
	printf("F3 - Ajouter une ligne");
	gotoLigCol(HD_Fonctions_x+4,HD_Fonctions_y+1);
	printf("F4 - Supprimer une ligne");
	gotoLigCol(HD_Fonctions_x+5,HD_Fonctions_y+1);
	printf("ESC - Quitter ce menu");
}

void cadreInputs(){
	/*
	Realise le cadre des entrees
	*/
	cadre(HD_Inputs_x, HD_Inputs_y, 5, longueur_Fonctions);
	gotoLigCol(HD_Inputs_x, HD_Inputs_y+2);
	printf("Inputs :");
}

void effacerCadreInputs(){
	/*
	Efface le cadre interieur des entrees
	*/
	int l=4;
	while(l>0){
		effacerLigne(HD_Inputs_x+l,HD_Inputs_y+1,longueur_Fonctions);
		l--;
	}
}

///////////////////////////////////////////////////////////////////
///////////// INITIALISATIONS & FONCTIONS UTILES //////////////////

int countCharDansString(char *line, char searched){
	/*
	Retourne les occurences d'un caractere dans une 
	chaine de caracteres
	*/
	int somme=0, i;
	for(i=0; i<strlen(line); i++){
		if(line[i] == searched) somme++;
	}
	return somme;
}

PTcentrale initCentrales(){
	/*
	Initialise la liste doublement chainee des centrales
	*/
	PTcentrale pdebut, pfin;
	
	pdebut = (PTcentrale)malloc(sizeof(Tcentrale));	
	pfin = (PTcentrale)malloc(sizeof(Tcentrale));
	pdebut->ptsuivant = pfin;
	pfin->ptprecedent = pdebut;
	pdebut->ptprecedent = NULL;
	pfin->ptsuivant = NULL;
	
	return pdebut;
}

PTville initVilles(){
	/*
	Initialise la liste simplement chainee des villes
	*/
	PTville pdebut, pfin;
	
	pdebut = (PTville)malloc(sizeof(Tville));	
	pfin = (PTville)malloc(sizeof(Tville));
	pdebut->villeSuivante = pfin;
	pfin->villeSuivante = NULL;
	
	return pdebut;
}

PTligneElectrique initLigne(){
	/*
	Initialise une liste simplement chainee pour 
	les lignes electriques
	*/
	PTligneElectrique pdebut, pfin;
	
	pdebut = (PTligneElectrique)malloc(sizeof(TlignesElectrique));	
	pfin = (PTligneElectrique)malloc(sizeof(TlignesElectrique));
	pdebut->ligneSuivante = pfin;
	pfin->ligneSuivante = NULL;
	
	return pdebut;
}

void afficheMenu(){
	gotoLigCol(HD_Fonctions_x+0,HD_Fonctions_y+2);
	printf("Menu des fonctions :");
	gotoLigCol(HD_Fonctions_x+1,HD_Fonctions_y+1);
	printf("F1 - Enregistrer");
	gotoLigCol(HD_Fonctions_x+2,HD_Fonctions_y+1);
	printf("F2 - Utiliser une sauvegarde");
	gotoLigCol(HD_Fonctions_x+3,HD_Fonctions_y+1);
	printf("F3 - Aller dans les centrales");
	gotoLigCol(HD_Fonctions_x+4,HD_Fonctions_y+1);
	printf("F4 - Aller dans les villes");
	gotoLigCol(HD_Fonctions_x+5,HD_Fonctions_y+1);
	printf("F5 - Afficher les puissance allouees");
	gotoLigCol(HD_Fonctions_x+6,HD_Fonctions_y+1);
	printf("F6 - Afficher les centrales libres");
	gotoLigCol(HD_Fonctions_x+7,HD_Fonctions_y+1);
	printf("ESC - Quitter");
}

void cursInput(){
	/*
	C'est juste plus agreable de faire appel a cette fonction
	Place notre curseur dans le cadre des entrees (inputs)
	*/
	gotoLigCol(HD_Inputs_x+1, HD_Inputs_y+1);
}

///////////////////////////////////////////////////////////////////
///////////// SOUS-PROGRAMMES LIGNES //////////////////////////////

int nombreDeLignes(PTcentrale pdebutC){
	/*
	Retourne le nombre de lignes electriques
	presentes dans toutes les centrales
	(dans le but de pouvoir determiner la taille du cadre des lignes par exemple)
	*/
	PTcentrale px=pdebutC->ptsuivant;
	PTligneElectrique pl;
	int somme=0;
	while(px->ptsuivant != NULL){
		// On ne prends pas en compte les balises des listes en compte
		//SUPPRIMER LE IF
		if(px->villeDependante != NULL && px->villeDependante->ligneSuivante != NULL){
			pl = px->villeDependante;
			while(pl->ligneSuivante->ligneSuivante != NULL){
				somme++;
				pl = pl->ligneSuivante;
			}
		}
		px = px->ptsuivant;
	}
	return somme;
}

void cadreLignes(PTcentrale pdebutC){
	/*
	Affiche le cadre des lignes avec le nom de 
	chaque colonne
	*/
	int taille=nombreDeLignes(pdebutC);
	cadre(HD_Lignes_x, HD_Lignes_y,taille+2,longueur_Lignes);
	gotoLigCol(HD_Lignes_x,HD_Lignes_y+2);
	printf("Liste des lignes :");
	gotoLigCol(HD_Lignes_x+1,HD_Lignes_y+1);
	printf("Code Centrale | Code Postal | Puissance");
	cursInput();
}

void insertionLigne(PTligneElectrique px, int puissance, PTville Pville){
	/*
	Insertion d'une ligne electrique dans la liste associee a une 
	certaine centrale
	*/
	PTligneElectrique py=px->ligneSuivante;
	px->ligneSuivante = (PTligneElectrique)malloc(sizeof(TlignesElectrique));
	px->ligneSuivante->ligneSuivante = py;
	
	px->ligneSuivante->puissance = puissance;
	px->ligneSuivante->villeDesservie = Pville;
}

void ajouterLigne(PTcentrale Pcentrale, int puissance, PTville Pville){
	/*
	Ajoute une ligne electrique en trouvant la bonne position d'insertion
	Initialise la liste si besoin
	Conditions : 
		- Si puissance Ligne > puissance Restante alors ANNULATION et EXPLICATIONS
	*/
	PTligneElectrique pxLigne;
	
	if(Pcentrale->villeDependante == NULL){	// Si y'a pas de lignes sur la centrale
		Pcentrale->villeDependante = initLigne();
	}
	
	if (puissanceRestante(Pcentrale) < puissance){
		cursInput();
		printf("Puissance de la ligne trop forte");
	} else {
		pxLigne = Pcentrale->villeDependante;
		while (pxLigne->ligneSuivante->ligneSuivante != NULL){pxLigne=pxLigne->ligneSuivante;}
		insertionLigne(pxLigne, puissance, Pville);
	}
}

void supprimerLigne(PTcentrale pCentrale, PTville pVille){
	/*
	Supprime une ligne de la liste associee 
	Conditions :
		- Supprime rien si il n'y a aucune ligne associee a la centrale
	*/
	PTligneElectrique pl;
	if(pCentrale->villeDependante != NULL && pCentrale->villeDependante->ligneSuivante != NULL){
		pl = pCentrale->villeDependante;
		while(pl->ligneSuivante != NULL){
			if(pl->ligneSuivante->villeDesservie == pVille){
				pl->ligneSuivante->puissance = 0;
				pl->ligneSuivante->villeDesservie = NULL;
				pl->ligneSuivante = pl->ligneSuivante->ligneSuivante;
			}
			pl = pl->ligneSuivante;
		}
	} else {
		effacerLigne(25,0,29);
		cursInput();
		printf("La centrale ne possede pas de lignes.");
	}
}

void afficherLignes(PTcentrale pdebutC){
	/*
	Affiche les lignes dans le cadre sous la forme :
		CENTRALE | VILLE | PUISSANCE
	*/
	PTcentrale px=pdebutC->ptsuivant;
	PTligneElectrique pl;
	int ligneX=HD_Lignes_x+2;
	cadreLignes(pdebutC);
	while(px->ptsuivant != NULL){
		if(px->villeDependante != NULL){
			pl = px->villeDependante->ligneSuivante;
			while(pl->ligneSuivante != NULL){
				gotoLigCol(ligneX,HD_Lignes_y+1);
				printf("%13d |%12d |%10d ", px->codeCentrale, pl->villeDesservie->codePostal, pl->puissance);
				ligneX++;
				pl = pl->ligneSuivante;
			}		
			
		}
		px = px->ptsuivant;
	}
}

///////////////////////////////////////////////////////////////////
////////////// SOUS-PROGRAMMES CENTRALES //////////////////////////

void insertionCentrale(PTcentrale px, int code, int puissance){
	/*
	Insertion centrale dans la liste doublement chainee
	*/
	px->ptsuivant->ptprecedent = (PTcentrale)malloc(sizeof(Tcentrale));
	px->ptsuivant->ptprecedent->ptsuivant = px->ptsuivant;
	px->ptsuivant->ptprecedent->ptprecedent = px;
	px->ptsuivant = px->ptsuivant->ptprecedent;
	
	px->ptsuivant->codeCentrale = code;
	px->ptsuivant->puissanceMax = puissance;
	px->ptsuivant->villeDependante = NULL;
}

void ajouterCentrale(PTcentrale pdebutC, int codeCentrale, int pMax){
	/*
	Ajouter une centrale dans la liste en donnant la position 
	puis en faisant appel a insertionCentrale
	*/
	PTcentrale px;

	px = pdebutC;
	while(px->ptsuivant->ptsuivant != NULL){px=px->ptsuivant;}
	insertionCentrale(px, codeCentrale, pMax);
}

void supprimerCentrale(PTcentrale pdebutC, PTcentrale pCentraleRemove){
	/*
	Supprime une centrale de la liste doublement chainee
	*/
	PTcentrale px=pdebutC;
	while(px->ptsuivant != pCentraleRemove){px=px->ptsuivant;}
	px->ptsuivant = px->ptsuivant->ptsuivant;
	px->ptsuivant->ptprecedent = px;
}

void modifierPuissance(PTcentrale px, int nouvellePuissance){
	/*
	Modifie la valeur de la puissance maximale d'une centrale
	*/
	px->puissanceMax = nouvellePuissance;
}

int puissanceConsommee(PTcentrale pCentrale){
	/*
	Retourne la puissance consommee d'une centrale
	en sommant les puissances de chaque lignes
	*/
	if(pCentrale->villeDependante == NULL) return 0;
	PTligneElectrique pxL=pCentrale->villeDependante->ligneSuivante;
	unsigned int somme=0;
	while(pxL->ligneSuivante != NULL){
		if(pxL->villeDesservie != NULL) somme = somme + pxL->puissance;
		pxL = pxL->ligneSuivante;
	} 
	return somme;
}

int puissanceRestante(PTcentrale pCentrale){
	/*
	Retourne la puissance restante d'une centrale 
	En prenant la puissance max - la puissance consommee
	*/
	return pCentrale->puissanceMax-puissanceConsommee(pCentrale);
}

int longueurListeCentrale(PTcentrale pdebut){
	/*
	Retourne le nombre de centrales 
	*/
	PTcentrale px=pdebut->ptsuivant;
	int longueur=0;
	while(px->ptsuivant != NULL){longueur++; px = px->ptsuivant;}
	return longueur;
}

void afficheCentrale(PTcentrale pdebutC){
	/*
	Affiche les centrales dans le cadre prevu pour
		CODE CENTRALE | PUISSANCE MAX
	*/
	PTcentrale px=pdebutC->ptsuivant;
	int ligneX=2;
	cadreCentrale(pdebutC);
	printf("Code Centrale | Puissance max");
	while(px->ptsuivant != NULL){
		gotoLigCol(HD_Centrales_x+ligneX,HD_Centrales_y+3);
		printf("%12d | %12d", px->codeCentrale, px->puissanceMax);
		ligneX++;
		px = px->ptsuivant;
	}
}

PTcentrale centraleViaCode(PTcentrale pdebut, int code){
	/*
	Retourne le pointeur de la centrale dont le codeCentrale
	Correspond avec le parametre code
	*/
	PTcentrale px=pdebut;
	while(px->ptsuivant->codeCentrale != code) px = px->ptsuivant;
	return px->ptsuivant;
}

///////////////////////////////////////////////////////////////////
///////////// SOUS-PROGRAMMES VILLES //////////////////////////////

void insertionVille(PTville px, int codePostal){
	/*
	Insertion d'une ville dans la liste simplement chainee
	*/
	PTville py=px->villeSuivante;
	px->villeSuivante = (PTville)malloc(sizeof(Tville));
	px->villeSuivante->villeSuivante = py;
	
	px->villeSuivante->codePostal = codePostal;
}

void ajouterVille(PTville pdebutV, int codePostal){
	/*
	Ajoute une ville a la liste
	en donnant la position anterieur
	(on ajoute la ville a la fin de la liste)
	*/
	PTville px;
	
	px = pdebutV;
	while(px->villeSuivante->villeSuivante != NULL){px=px->villeSuivante;}
	insertionVille(px, codePostal);
}

void supprimerVille(PTville pdebutV, PTville pVilleRemove){
	/*
	Supprime la ville de la liste via son pointeur
	*/
	PTville px=pdebutV;
	while(px->villeSuivante != pVilleRemove){px = px->villeSuivante;}
	px->villeSuivante = px->villeSuivante->villeSuivante;
}

PTville villePrecedente(PTville pdebutV, PTville px){
	/*
	Retourne la ville Precedente d'une certaine ville px
	*/
	PTville py=pdebutV;
	while(py->villeSuivante != px)py = py->villeSuivante;
	return py;
}

int longueurListeVille(PTville pdebut){
	/*
	Retourne le nombre de villes presentes dans la liste 
	*/
	PTville px=pdebut->villeSuivante;
	int longueur=0;
	while(px->villeSuivante != NULL){longueur++; px = px->villeSuivante;}
	return longueur;
}

void afficheVille(PTville pdebutV){
	/*
	Affiche les villes dans le cadre prevu
		CODE POSTAL
	*/
	PTville px=pdebutV->villeSuivante;
	int ligneX=2, taille;
	taille = longueurListeVille(pdebutV);
	cadre(HD_Villes_x,HD_Villes_y,taille+2,longueur_Villes);
	gotoLigCol(HD_Villes_x,HD_Villes_y+2);
	printf("Liste des villes :");
	gotoLigCol(HD_Villes_x+1,HD_Villes_y+2);
	printf("Code postal");
	while(px->villeSuivante != NULL){
		gotoLigCol(ligneX,HD_Villes_y+1);
		printf("%6d",px->codePostal);
		ligneX++;
		px = px->villeSuivante;
	}
}

PTville villeViaCode(PTville pdebutV, int code){
	/*
	Retourne le pointeur de la ville via son codePostal
	*/
	PTville px=pdebutV;
	while(px->villeSuivante->codePostal != code) px = px->villeSuivante;
	return px->villeSuivante;
}

///////////////////////////////////////////////////////////////////
///////////// PROGRAMME PRINCIPAL /////////////////////////////////

void affichagePrincipal(PTcentrale pdebutC, PTville pdebutV, Tcurseur curs){
	// Affichage du menu
	cadre(HD_Fonctions_x,HD_Fonctions_y,8,longueur_Fonctions);
	afficheMenu();
	// Affichage cadre des centrales
	afficheCentrale(pdebutC);
	// Affichage cadre des villes
	afficheVille(pdebutV);
	// Afficher le curseur si il existe
	afficherCurseur(curs);
	// Afficher cadre des lignes
	afficherLignes(pdebutC);
	// Afficher cadre des inputs
	cadreInputs();
}

PTcentrale selectionnerCentrale(PTcentrale pdebutC, PTville pdebutV, Tcurseur cursor){
	/*
	Dans la liste des centrales, on fait un curseur de liste : CodeCentrale fond blanc, ecriture noire
	qui se deplace entre la premiere centrale et la derniere
	Et lorsque la touche ENTREE est enfoncee, retourne le pointeur centrale
	associe au codeCentrale du curseur
	*/
	PTcentrale px=pdebutC->ptsuivant;
	int c=0;
	char acc[20];
	// On mets a jour le curseur sur la premiere centrale
	cursor.x = -1;
	affichagePrincipal(pdebutC, pdebutV, cursor);
	cursInput();
	
	cursor.x = HD_Centrales_x+2;
	cursor.y = HD_Centrales_y+2;
	sprintf(acc, " %12d ", px->codeCentrale);
	cursor.phrase = acc;
	afficherCurseur(cursor);
	
	while(c!=13){
		c = lireCaract();
		if(c==27)break;
		if(c==480){ // Fleche bas
			if(px->ptsuivant->ptsuivant!=NULL){
				printf("%s", acc);
				px = px->ptsuivant;
				cursor.x++;				
			} 
		}else if(c==472){ // Fleche haut
			if(px->ptprecedent != pdebutC){
				printf("%s", acc);
				px = px->ptprecedent;
				cursor.x--;
			}
		}
		sprintf(acc, " %12d ", px->codeCentrale);
		cursor.phrase = acc;
		afficherCurseur(cursor);
	}
	if(c==13)return px;
	else return NULL;
	
}

PTville selectionnerVille(PTcentrale pdebutC, PTville pdebutV, Tcurseur cursor){
	/*
	Dans la liste des villes, on fait un curseur de liste : CodePostal fond blanc, ecriture noire
	qui se deplace entre la premiere ville et la derniere
	Et lorsque la touche ENTREE est enfoncee, retourne le pointeur ville
	associe au codePostal du curseur
	*/
	PTville px=pdebutV->villeSuivante, py;
	int c=0;
	char acc[20];
	// On mets a jour le curseur sur la premiere centrale
	cursor.x = -1;
	affichagePrincipal(pdebutC, pdebutV, cursor);
	cursInput();
	
	cursor.x = HD_Villes_x+2;
	cursor.y = HD_Villes_y+2;
	sprintf(acc, " %6d ", px->codePostal);
	cursor.phrase = acc;
	afficherCurseur(cursor);
	
	while(c!=13){
		c = lireCaract();
		if(c==27)break;
		if(c==480){ // Fleche bas
			if(px->villeSuivante->villeSuivante!=NULL){
				printf("%s", acc);
				px = px->villeSuivante;
				cursor.x++;				
			} 
		}else if(c==472){ // Fleche haut
			py = villePrecedente(pdebutV, px);
			if(py != pdebutV){
				printf("%s", acc);
				px = py;
				cursor.x--;
			}
		}
		sprintf(acc, " %6d ", px->codePostal);
		cursor.phrase = acc;
		afficherCurseur(cursor);
	}
	if(c==13)return px;
	else return NULL;
}

int main(){
	//// Initialisation ////
	FILE *f;
	PTcentrale pdebutC=NULL, pxC;
	PTligneElectrique pdebutL, pxL;
	PTville pdebutV=NULL, pxV;
	int puissance, code, code1, longueur, choix=0;
	int menuAllouee=0, menuPuissanceRestante=0;
	
	// On creer un curseur
	Tcurseur cursor;
	cursor.phrase = (char *)malloc(40);
	cursor.x = -1;
	
	// Initialisations des listes 
	pdebutC = initCentrales();
	pdebutV = initVilles();
	
	while(choix!=27){
		// On rafraichis la page si on ne demande pas precedemment
		// D'afficher les puissances restantes ou allouees
		if(choix!=563 && choix!=564)affichagePrincipal(pdebutC, pdebutV, cursor);
		cursInput();
		choix = lireCaract();
		
		if(choix==13){}// Entree
		else if(choix == 559){ // Touche F1
			/*
			Pour l'enregistrement, chaque ligne correspond a une structure :
				- Ligne 1 : Les centrales avec pour schema : codeCentrale,PuissanceMax,etc...
				- Ligne 2 : Les villes avec pour schema : codePostal,etc...
				- Ligne 3 : Les lignes elec avec pour schema : codeCentrale,CodePostal,Puissance,etc...
			Et on rajoute une virgule a chaque fin de ligne, pour le bon fonctionnement du programme
			*/
			
			// Enregistrement des donnees
			f = fopen("edhess_data.txt", "w");

			pxC = pdebutC->ptsuivant;
			while(pxC->ptsuivant != NULL){
				// fprintf pour ecrire dans le fichier
				if(pxC->ptsuivant->ptsuivant == NULL) fprintf(f, "%d,%d", pxC->codeCentrale, pxC->puissanceMax);
				else fprintf(f, "%d,%d,", pxC->codeCentrale, pxC->puissanceMax);
				pxC = pxC->ptsuivant;
			}
			fprintf(f,"\n");
			
			pxV = pdebutV->villeSuivante;
			while(pxV->villeSuivante != NULL){
				if(pxV->villeSuivante->villeSuivante == NULL) fprintf(f,"%d",pxV->codePostal);
				else fprintf(f,"%d,",pxV->codePostal);
				pxV = pxV->villeSuivante;
			}
			fprintf(f,"\n");
			
			pxC = pdebutC->ptsuivant;
			while(pxC->ptsuivant != NULL){
				if(pxC->villeDependante != NULL && pxC->villeDependante->ligneSuivante != NULL){
					pxL = pxC->villeDependante->ligneSuivante;
					while(pxL->ligneSuivante != NULL){
						if(pxL->puissance != 0 && pxL->villeDesservie != NULL){
							if(pxL->ligneSuivante->ligneSuivante == NULL) fprintf(f,"%d,%d,%d",pxC->codeCentrale,pxL->villeDesservie->codePostal,pxL->puissance);
							else fprintf(f,"%d,%d,%d,",pxC->codeCentrale,pxL->villeDesservie->codePostal,pxL->puissance);
						}
						pxL = pxL->ligneSuivante;
					}
				}
				pxC = pxC->ptsuivant;
			}
			fclose(f);
		}// F1
		else if(choix == 560){ // TOUCHE F2
			/*
			On lit chaque ligne une par une, et en connaissant la forme type lors de l'enregistrement
			On regroupe les donnees essentielles pour apres ajouter les centrales, villes et lignes
			
			L'utilisation de l'operateur modulo a ete tres utile pour facilement diviser 
			en sous-groupes de 2 ou 3 chaque ligne
			*/
			
			// Utilisation d'un enregistrement
			if(pdebutC != NULL && pdebutV != NULL){
				// On oublie les emplacements d'avant pour "tout supprimer" meme si ca conserve en memoire 
				// les malloc d'avant, donc c'est pas optimise du tout
				pdebutC = initCentrales();
				pdebutV = initVilles();
				system("cls");
			}
			// Initalisation 
			const char s[2]= ", ";
			char *token, *token2;
			int i, ligne=0, comma;
			f = fopen("edhess_data.txt", "r");
			
			if(f!=NULL){
				char line[1024];
				while(fgets(line, sizeof(line), f) != NULL){
					/*
					On calcule le nombre de virgules dans la ligne qu'on etudie
					Ce que nous sert a parcourir la ligne plus aisement et a mieux
					troncater les donnees.
					*/ 
					comma = countCharDansString(line, ',');
					
					if(ligne==0){ // Ligne des centrales
						for(i=0; i<=comma; i++){
							if(i==0)token = strtok(line,",");
							else if(i>0) token = strtok(NULL,",");
							if(i%2 == 0){ // Recupere le code centrale
								code = atoi(token);
							}
							else if(i%2 == 1){ // Recupere la puissance maximale puis ajoute la centrale
								puissance = atoi(token);
								ajouterCentrale(pdebutC, code, puissance);
							}
						}
					} else if(ligne == 1){ // Ligne des villes
						for(i=0; i<=comma; i++){
							if(i==0)token = strtok(line,",");
							else if(i>0) token = strtok(NULL,",");
							ajouterVille(pdebutV, atoi(token));
						}
					} else if(ligne == 2){ // Ligne des lignes electriques
						for(i=0; i<=comma; i++){
							if(i==0)token = strtok(line,",");
							else if(i>0) token = strtok(NULL,",");
							if(i%3==0){ // Recupere le code centrale
								code = atoi(token);
								pxC = centraleViaCode(pdebutC, code);
							} else if(i%3==1){ // Recupere le code postal
								code = atoi(token);
								pxV = villeViaCode(pdebutV, code);
							} else if(i%3==2){ // Recupere la puissance de la ligne puis ajoute la ligne
								puissance = atoi(token);
								ajouterLigne(pxC, puissance, pxV);
							}
						}                                   
					}               
					ligne++;
				}
			}
			
		}// F2
		else if(choix == 561){
			/*
			Entrer dans le mode centrale nous permet de realiser des operations sur celles-ci
			Il faut donc modifier le menu des fonctions pour le mettre a jour
			Puis placer le curseur sur les centrales pour choisir la centrale
			Qui subira les modifications.
			
			On peut aussi ne pas prendre en compte le curseur lorsqu'il s'agit d'ajouter une centrale.
			*/
			
			// Afin d'indiquer a l'utilisateur dans quelle liste il est
			// On mets le titre du cadre selectionne comme un curseur
			cursor.x = HD_Centrales_x;
			cursor.y = HD_Centrales_y+2;
			cursor.phrase = "Liste des centrales :";

			while(1){
				system("cls");
				affichagePrincipal(pdebutC, pdebutV, cursor);
				effacerMenuFonctions();
				menuFonctionsCentrales();
				cursInput();
				choix = lireCaract();
				
				if(choix==559){ // TOUCHE F1
					// Ajouter centrale
					printf("Entrez le code centrale : ");
					scanf("%d", &code);
					gotoLigCol(HD_Inputs_x+2,HD_Inputs_y+1);
					printf("Entrez la puissance : ");
					scanf("%d", &puissance);
					
					ajouterCentrale(pdebutC, code, puissance);
				} else if(choix==560){ // TOUCHE F2
					// Supprimer centrale a condition qu'elle existe
					pxC = selectionnerCentrale(pdebutC, pdebutV, cursor);
					if(pxC != NULL)supprimerCentrale(pdebutC, pxC);
				} else if(choix==561){ // TOUCHE F3
					// Modifier puissance maximale centrale
					pxC = selectionnerCentrale(pdebutC, pdebutV, cursor);
					cursInput();
					printf("Entrez la nouvelle puissance : ");
					scanf("%d", &puissance);
					modifierPuissance(pxC, puissance);
				} else if(choix==562){ // TOUCHE F4
					// Ajouter ligne entre une centrale et une ville
					pxC = selectionnerCentrale(pdebutC, pdebutV, cursor);
					pxV = selectionnerVille(pdebutC, pdebutV, cursor);
					cursInput();
					printf("Entrez la puissance de la ligne : ");
					scanf("%d", &puissance);
				
					ajouterLigne(pxC, puissance, pxV);
				} else if(choix==563){ // TOUCHE F5
					// Supprimer ligne entre centrale et ville
					pxC = selectionnerCentrale(pdebutC, pdebutV, cursor);
					pxV = selectionnerVille(pdebutC, pdebutV, cursor);	
					supprimerLigne(pxC, pxV);	
				} else if(choix==27){ // TOUCHE ESCAPE
					// Quitter le menu centrale et revenir au menu principal
					choix = 0;
					cursor.x = -1;
					break;
				}
			}
		}// F3
		else if(choix == 562){
			/*
			Entrer dans le mode ville nous permet de realiser des operations sur celles-ci
			Il faut donc modifier le menu des fonctions pour le mettre a jour
			Puis placer le curseur sur les villes pour choisir la ville
			Qui subira les modifications.
			
			On peut aussi ne pas prendre en compte le curseur lorsqu'il s'agit d'ajouter une ville.
			*/
			
			// De la meme facon que pour les centrales, on colorie le titre du cadre
			// Pour indiquer a l'utilisateur dans quel cadre il est.
			cursor.x = 0;
			cursor.y = 82;
			cursor.phrase = "Liste des villes :";
			
			while(1){
				system("cls");
				affichagePrincipal(pdebutC, pdebutV, cursor);
				effacerMenuFonctions();
				menuFonctionsVilles();
				cursInput();
				choix = lireCaract();
				
				if(choix==559){ // TOUCHE F1
					// Ajouter une ville
					printf("Entrez le code postal : ");
					scanf("%d", &code);
					
					ajouterVille(pdebutV, code);	
				} else if(choix==560){ // TOUCHE F2
					// Supprimer une ville
					pxV = selectionnerVille(pdebutC, pdebutV, cursor);
					if(pxV != NULL){
						pxC = pdebutC->ptsuivant;
						while(pxC->ptsuivant != NULL){
							if(pxC->villeDependante != NULL && pxC->villeDependante->ligneSuivante != NULL) supprimerLigne(pxC, pxV);
							pxC = pxC->ptsuivant;
						}
						supprimerVille(pdebutV, pxV);
					}
				} else if(choix==561){ // TOUCHE F3
					// Ajouter une ligne entre une ville et une centrale
					pxC = selectionnerCentrale(pdebutC, pdebutV, cursor);
					pxV = selectionnerVille(pdebutC, pdebutV, cursor);
					cursInput();
					printf("Entrez la puissance de la ligne : ");
					scanf("%d", &puissance);
				
					ajouterLigne(pxC, puissance, pxV);
				} else if(choix==562){ // TOUCHE F4
					// Supprimer une ligne entre une ville et une centrale
					pxC = selectionnerCentrale(pdebutC, pdebutV, cursor);
					pxV = selectionnerVille(pdebutC, pdebutV, cursor);
					supprimerLigne(pxC, pxV);
				} else if(choix==27){ // TOUCHE ESCAPE
					// Quitter le menu centrale et revenir au menu principal
					choix = 0;
					cursor.x = -1;
					break;
				}
			}
		} // F4
		else if(choix == 563){
			/*
			On intervetit l'affichage entre celui des puissance allouees
			Et celui normal, en fonction de la l'affichage actuel
			menuAllouee sert de "bouleen"
			*/

			if(!menuAllouee){
				effacerCadreCentrales(pdebutC);
				afficherPuissanceAlloueeDesCentrales(pdebutC);
			} else if(menuAllouee){
				effacerCadreCentrales(pdebutC);
				afficheCentrale(pdebutC);
			}
			menuAllouee = !menuAllouee;
		} // F5
		else if(choix == 564){
			/*
			On intervetit l'affichage entre celui des puissance restantes
			Et celui normal, en fonction de la l'affichage actuel
			menuPuissanceRestante sert de "bouleen"
			*/
			if(!menuPuissanceRestante){
				effacerCadreCentrales(pdebutC);
				afficherPuissanceRestante(pdebutC);
			} else if(menuPuissanceRestante){
				effacerCadreCentrales(pdebutC);
				effacerCadreInputs();
				afficheCentrale(pdebutC);
			}
			menuPuissanceRestante = !menuPuissanceRestante;
		} // F6
	}
	
	// On va bas pour pas gener ce qu'on a deja fait.
	gotoLigCol(50,0);
	return 1;
}
