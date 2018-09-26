#include <file.h>

File creer_file(void){
	return(NULL);
}

int file_vide(File f){
	return(!f);
}

/*
void visualiser_file(File f){
	if (file_vide(f)){
		printf("File VIde\n");
	}

	else{
		File f_init = f;
		f = f->suiv;
		while(f!=f_init){
			affiche(&f->val);
			printf("\n");
			f = f->suiv;
		}
		affiche(&f_init->val);
		printf("\n");
	}
} */


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


void* defiler(File f){ 					/*enleve le premier elem de la file et le renvoie*/
	if (f->suiv == f){					/*si l elem est tout seul dans la file*/
		void* p = (void*)f->val;
		File f_temp = f;

		free(f_temp);
		f = creer_file();
		return(p);
	}
										/*Sinon*/
	File f_premier = f->suiv; 			/*On recupere le premier elem de la file*/

	void* p = (void*) f_premier->val;			/*On recupere l element du maillon qui va etre supprime*/
	f->suiv = f_premier->suiv; 			/*On fait le nouveau branchement*/
	free(f_premier);					/*On libere le premier qui n est plus utilise*/

	return(p);
}

void liberer_file(File f){
	while(f!=NULL){
		defiler(f);
	}
}
