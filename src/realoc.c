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
void ajout_maillon_realoc(OPERANDE* p_op, File* p_file_realoc, type_realoc type, zone_symb zone, double decalage, File file_text){	int ajouter = 0;
	REALOC new_maillon = calloc(1, sizeof(*new_maillon));
	new_maillon->zone = zone;
	new_maillon->decalage = decalage;
	new_maillon->type = type;
	new_maillon->p_op = p_op;
	new_maillon->file_text = file_text;
	new_maillon->zone_def = ZONE_UNKW;

	*p_file_realoc = enfiler(new_maillon, *p_file_realoc);
	
}

/**Parcous la file de realoc a la fin de la passe grammaticale pour mettre la zone de DEFINITION du symbole*/
void zone_def_symb_realoc(File* p_file_realoc, File* p_file_Symb, long* p_taille_symb, File file_Dic){
	File pointeur_realoc = (*p_file_realoc)->suiv;

	do/*On parcours toute la file de realoc*/
	{
		remplace_realoc(p_file_Symb, (REALOC)(pointeur_realoc->val), p_taille_symb, file_Dic);		 
		pointeur_realoc = pointeur_realoc->suiv;
	} while (pointeur_realoc!=(*p_file_realoc)->suiv);
}

/**Permet de parcourir uniquement la liste d'operande SYMB qui ontdoivent avoir des offset comme operandes et les remplacer*/
void reallocation_offset(File* p_file_realoc_offset, File* p_file_Symb, long* p_taille_symb, File file_Dic){
	long inutile; 
	while(*p_file_realoc_offset != NULL){
		/*On parcours la liste de realoc_offset*/
		OPERANDE* p_op = (((REALOC)((*p_file_realoc_offset)->suiv->val))->p_op);

		remplace_realoc_offset(p_file_Symb, p_op, p_taille_symb, ((REALOC)((*p_file_realoc_offset)->suiv->val))->file_text);

		is_in_dic(file_Dic, &(((REALOC)((*p_file_realoc_offset)->suiv->val))->file_text)
			, NULL, NULL, &inutile, &inutile, (int*)(&inutile) );
		//int is_in_dic(File file_Dic, File* p_file_Text_maillon_courant, File* p_file_realoc, File* p_file_realoc_offset,long* p_taille_text, long* p_taille_realoc, int* p_cmpt_err)

		defiler(p_file_realoc_offset);		
	}
}

int remplace_realoc(File* p_file_Symb, REALOC maillon_realoc, long* p_taille_symb, File file_Dic){
	File pointeur_symb = (*p_file_Symb)->suiv;
	char* chaine_op = calloc(1, sizeof(*chaine_op)); strcpy(chaine_op, (*(maillon_realoc->p_op))->chain);
	long inutile;

	do/*On parcours toute la file de realoc*/
	{
		if(!strcmp(chaine_op, ((SYMB)(pointeur_symb->val))->nom )){
			maillon_realoc->zone_def = ((SYMB)(pointeur_symb->val))->zone;

			if(maillon_realoc->type == R_MIPS_26){
				(*(maillon_realoc->p_op))->bin = ((SYMB)(pointeur_symb->val))->decalage;
				(*(maillon_realoc->p_op))->type = OPER_DECIMAL;
				is_in_dic(file_Dic, &(maillon_realoc->file_text), NULL, NULL, &inutile, &inutile, (int*)(&inutile) );
			}
			return(1);
		}				 
		pointeur_symb = pointeur_symb->suiv;
	} while (pointeur_symb!=(*p_file_Symb)->suiv);
	

	SYMB new_symb = calloc(1, sizeof(*new_symb));

	new_symb->zone = ZONE_UNKW;
	new_symb->nom = chaine_op;
	new_symb->line_nb = 0;
	new_symb->decalage = 0;

	(*p_taille_symb)++;
	*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/

	return(0);
}


int remplace_realoc_offset(File* p_file_Symb, OPERANDE* p_op, long* p_taille_symb, File file_text){
	char* chaine_op = calloc(1, sizeof(*chaine_op)); strcpy(chaine_op, (*p_op)->chain);
	File file_SYMB_temp = *p_file_Symb;
	
	if((*p_file_Symb)!=NULL){
		File dernier_elem = *p_file_Symb; file_SYMB_temp = file_SYMB_temp->suiv;

		do
		{/*On parcours la liste de symboles*/

			/*Si on trouve le symbole dans la liste de symboles*/
			if (!strcasecmp(chaine_op, ((SYMB)(file_SYMB_temp->val))->nom)){

				//sprintf((*p_op)->chain, "%.0lf", (((SYMB)(file_SYMB_temp->val))->decalage));
				(*p_op)->type = OPER_OFFSET;
				(*p_op)->bin =  ((long) (((SYMB)(file_SYMB_temp->val))->decalage)) - ((long)(((TEXT)(file_text->val))->decalage)) - 4;
				/*Final - initial - 4 /4*/
				if ((*p_op)->bin % 4 == 0){
					(*p_op)->bin /= 4;
				}
				else ERROR_MSG("Valeur de saut non multiple de 4");

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

	(*p_op)->type = OPER_OFFSET;
	(*p_op)->bin = 0;

	(*p_taille_symb)++;
	*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/

	return(0);
}
