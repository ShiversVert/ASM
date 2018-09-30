#ifndef _File_H
#define _File_H

#include <stdio.h>
#include <stdlib.h>
#include <types.h>

/*Untyped queue*/
typedef struct _liste_ {
		void* val;
		struct _liste_* suiv;
	}* LISTE;
	
typedef LISTE File;

File creer_file(void); 				/* Créer une file vide*/
int file_vide(File ); 				/*Teste si la file est vide */
/*void visualiser_file(File ); */
File enfiler(void* , File ); 		/*Ajoute une carte à la file par la queue*/
void* defiler(File f);				/*Renvoie la carte en tete de file ET la supprime de la file */
void liberer_file(File f);
void afficher_file_lexeme(File f);


#endif

