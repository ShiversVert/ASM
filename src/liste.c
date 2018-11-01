#include <liste.h>

Liste creer_liste(void){
	return NULL;
}

int liste_vide(Liste L){
	return(!L);
}

/*
void visualiser(Liste L){
	while(L!=NULL){
		affiche(&(L->val));
		L = L->suiv;
	}
}
*/

Liste ajout_tete(void* p, Liste L){
	Liste new_maillon = calloc(1, sizeof(*new_maillon));
	
	if (p == NULL) return NULL;
	
	new_maillon->val = p;
	new_maillon->suiv = L;
	
	return(new_maillon);
}

Liste supprimer_tete(Liste L){
	Liste p = NULL;
	if (L!=NULL){
		p = L->suiv;
		free(L);
		return p;
	}
	else return NULL;
}

void liberer(Liste L){
	while(L!=NULL){
		L = supprimer_tete(L);
	}
}

Liste ajout_queue(void* p, Liste L){
	/*On chercher la fin de la Liste*/
	
	if (!L) return(ajout_tete(p, L));
	
	Liste L_init = L;
	
	while(L->suiv!=NULL){
		L = L->suiv;
	}
	
	Liste new_maillon = calloc(1, sizeof(*new_maillon));

	L->suiv = new_maillon;

	new_maillon->val = p;
	new_maillon->suiv = NULL;	
	
	return(L_init);
}
