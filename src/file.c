#include <file.h>

File creer_file(void){
	return(NULL);
}

int file_vide(File f){
	return(!f);
}

/*@param p pointeur sur l element a enfiler
 *@param f file a laquelle on doit rajouter l element *p
 *@return retourne la nouvelle file
 *@brief a utiliser de la maniere suivante : f = enfiler(p, f);
 */

File enfiler(void* p, File f){						/*f = enfiler(pointeur vers l elem a enfiler, file actuelle)*/

	File f_new = calloc(1, sizeof(*f_new)); /*creation d'un nouveau maillon*/
	f_new->val = p;													/*On place le pointeur vers l'elem de la file*/

	if(f == NULL){													/*Si la file actuelle est vide*/
		f_new->suiv = f_new;									/*C'est le seul maillon de la file, il pointe donc sur lui meme*/
	}

	else{																		/*Sinon*/
		f_new->suiv = f->suiv;								/*On insere le nouveau maillon a la fin de la file*/
		f->suiv = f_new;
	}
	return f_new;														/*On retourne le nouveau maillon*/
}


/**
 * @param file f
 * @return Retourne un pointeur vers l'element defile
 * @brief enleve le premier elem de la file et le renvoie
 *
 */

void* defiler(File* f){
	if ((*f)->suiv == (*f)){			/*si l elem est tout seul dans la file*/
		void* p = (*f)->val;
		File f_temp = *f;

		free(f_temp);
		*f = creer_file();
		return(p);
	}

	/*Sinon*/

	File f_premier = (*f)->suiv;		/*On recupere le premier elem de la file*/
	
	void* p = f_premier->val;			/*On recupere l element du maillon qui va etre supprime*/
	(*f)->suiv = f_premier->suiv;	/*On fait le nouveau branchement*/
	free(f_premier);					/*On libere le premier qui n est plus utilise*/
	
	return(p);
}
/**
 * @param file f
 * @return Ne retourne rien
 * @brief libere l'espace en memoire alloue a la file
 *
 */

void liberer_file(File f){
	while(f!=NULL){
		defiler(&f);
	}
}

/**
 * @param file a afficher
 * @param fonction d'affichage d'un maillon
 * @return Ne retourne rien
 * @brief Parcours toute la file pour l'imprimer dans la console
 *
 */

void afficher_file(File f, void (*afficher_maillon)(void*) ){
	if (f==NULL){
		printf("File Vide\n");
	}

	else{
		File dernier_elem = f;
		f = f->suiv;
		printf("###############\n\n");
		
		do{
			(*afficher_maillon)(f->val);
			f = f->suiv;
		}while(f!=dernier_elem->suiv);
		
		printf("###############\n\n");
	}
}

