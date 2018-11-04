#ifndef _File_H_
#define _File_H_

#include <liste.h>
#include <stdio.h>
#include <stdlib.h>
#include <notify.h>

/*Liste non typee*/ 
typedef struct _liste_ {
	void* val;
	struct _liste_* suiv;
}* File;



File creer_file(void); 				/* Creer une file vide*/
int file_vide(File ); 				/*Teste si la file est vide */
/*void visualiser_file(File ); */
File enfiler(void* p, File ); 		/*Ajoute une carte a la file par la queue*/
void* defiler(File* f);				/*Renvoie la carte en tete de file ET la supprime de la file */
void liberer_file(File f);
void afficher_file(File f,  void (*afficher_maillon)(void*));

void afficher_maillon_LEXEME(void* maillon);



#endif /*_File_H_*/

