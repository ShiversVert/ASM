#include <realoc.h>

/**
 * Aoute l'operande a reallouer a la file de realoc_offset si c'est un offset a la file de realoc sinon
 *
 * @param p_op                 [description]
 * @param p_file_realoc         Pointeur sur la File a laquelle on ajoute le maillon a reallouer
 *                              Si on realloue un type offset, il faut passer en parametre la file file_realoc_offset
 * @param type 					Type de realloc a faire -> depend de du type de l'operande dans le dictionnaire
 * @param zone 					Zone ou est definie l'operande /!\ (et pas le symbole)
 */
void ajout_maillon_realoc(OPERANDE* p_op, File* p_file_realoc, type_realoc type, zone_symb zone, double decalage){
	REALOC new_maillon = calloc(1, sizeof(*new_maillon));
	new_maillon->zone = zone;
	new_maillon->decalage = decalage;
	new_maillon->type = type;
	new_maillon->p_op = p_op;

	*p_file_realoc = enfiler(new_maillon, *p_file_realoc);
}

/**Permet de parcourir uniquement la liste d'operande SYMB qui ontdoivent avoir des offset comme opérandes et les remplacer*/
void reallocation_offset(File* p_file_realoc_offset, File* p_file_Symb){
	while(*p_file_realoc_offset != NULL){
		/*On parcours la liste de realoc_offset*/
		OPERANDE* p_op = (((REALOC)((*p_file_realoc_offset)->val))->p_op);

		remplace_realoc_offset(p_file_Symb, p_op);

		defiler(p_file_realoc_offset);
	}
}

int remplace_realoc_offset(File* p_file_Symb, OPERANDE* p_op){
	char* chaine_op = calloc(1, sizeof(*chaine_op)); strcpy(chaine_op, (*p_op)->chain);
	File file_SYMB_temp = *p_file_Symb;
	
	if((*p_file_Symb)!=NULL){
		File dernier_elem = *p_file_Symb; file_SYMB_temp = file_SYMB_temp->suiv;

		do
		{/*On parcours la liste de symboles*/

			/*Si on trouve le symbole dans la liste de symboles*/
			if (!strcasecmp(chaine_op, ((SYMB)(file_SYMB_temp->val))->nom)){

				sprintf((*p_op)->chain, "%.0lf", (((SYMB)(file_SYMB_temp->val))->decalage));
				(*p_op)->type = OPER_TARGET;
				(*p_op)->bin = atol((*p_op)->chain);
				free(chaine_op);
				return(1);
			}

			file_SYMB_temp = file_SYMB_temp->suiv;
		} while (file_SYMB_temp!=dernier_elem->suiv);
	}
	/*On ne l'a pas trouve dans la table de symbole donc on l'ajoute avec ZONE_UNKW*/
	SYMB new_symb = calloc(1, sizeof(*new_symb));

	new_symb->zone = ZONE_UNKW;
	new_symb->nom = chaine_op;
	new_symb->line_nb = 0;
	new_symb->decalage = 0;

	*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/
	return(0);
}