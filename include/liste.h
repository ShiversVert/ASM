#ifndef _liste_H_
#define _liste_H_

#include <stdio.h>
#include <stdlib.h>
#include <notify.h>

/*Liste non typee*/
typedef struct _Liste_ {
	void* 				val; 
	struct _Liste_* 	suiv;
}* Liste;

Liste creer_liste(void);
int liste_vide(Liste L);
void afficher_liste(Liste L, void (*afficher_maillon)(void*));
Liste ajout_tete(void* p, Liste L);
Liste ajout_queue(void* p, Liste L);
Liste supprimer_tete(Liste L) ;
void liberer(Liste L);

#endif /*_liste_H_ */