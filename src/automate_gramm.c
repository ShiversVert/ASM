#include <automate_gramm.h>

/**
 * Fonction principale de l'automate grammatical
 *
 * @param p_file_Lexeme pointeur sur la file de Lexeme a traiter
 * @param p_file_Text   pointeur sur la file d'element .Text a remplir
 * @param p_file_Bss    pointeur sur la file d'element .Bss a remplir
 * @param p_file_Data   pointeur sur la file d'element .Data a remplir
 * @param p_file_Symb   pointeur sur la file d'element de Symb a remplir
 * @param file_Dic      file de Dictionnaire
 */

void automate_grammatical(File* p_file_Lexeme, File* p_file_Text, File* p_file_Bss, File* p_file_Data, File* p_file_Symb, File file_Dic, File* p_file_realoc){

	/*if (*p_file_Lexeme != NULL)	*p_file_Lexeme = (*p_file_Lexeme)->suiv;*/ /*Si la file de lexeme n'est pas cide, on va chercher le premier lexeme de la file*/

	double offset_data=0; double offset_bss=0;double offset_text=0;;
	STATE_GRAMM S = S_GRAMM_INIT;		/*Etat initial de l'automate*/
	LEXEME lexeme_courant = calloc(1,sizeof(*lexeme_courant));
	File file_realoc_offset = NULL;

	while (*p_file_Lexeme != NULL){ /*Tant que la file de lexeme n'est pas vide*/

	get_current_Lexeme(p_file_Lexeme, &lexeme_courant);

		switch(S){

			case S_GRAMM_INIT :
				analyse_gramm1(p_file_Lexeme, &S, lexeme_courant); /*Cas speciaux .bss .text .data & commentaire*/
				break;

			case S_GRAMM_DATA:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/
					ajout_maillon_data(p_file_Data, p_file_Lexeme, lexeme_courant, p_file_Symb, &offset_data, p_file_realoc);
				}
				break;

			case S_GRAMM_TEXT:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/
					ajout_maillon_text(p_file_Text, p_file_Lexeme, lexeme_courant, p_file_Symb, &offset_text, file_Dic, p_file_realoc, &file_realoc_offset);
				}
				break;

			case S_GRAMM_BSS:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/
					ajout_maillon_bss(p_file_Bss, p_file_Lexeme, lexeme_courant, p_file_Symb, &offset_bss);
				}
				break;

			default:
				WARNING_MSG("Bien joue, vous etes rentre dans la matrice");
				break;
		}


	}

	free(lexeme_courant);
	reallocation_offset(&file_realoc_offset, p_file_Symb);
	/*replace_in_Text(p_file_Text, p_file_Symb, file_Dic);*/
}


/**
 * Permet de verifier si le lexeme courant est une instruction :
 * 	.set noreorder et la defile
 *	.bss et associe l'etat S_GRAMM_BSS
 *	.data et associe l'etat S_GRAMM_DATA
 *	.text et associe l'etat S_GRAMM_TEXT
 *
 * @param  p_file_Lexeme  pointeur sur la file de Lexeme a traiter
 * @param  S              Etat grammatical courant
 * @param  lexeme_courant lexeme courrant (facilite l'acces a ses valeurs)
 *
 * @return                Retourne 1 si des lexemes ont etes defiles, 0 sinon
 */

int analyse_gramm1(File* p_file_Lexeme, STATE_GRAMM* S, LEXEME lexeme_courant){

	if (lexeme_courant->cat == DIRECTIVE){
		if ( strcmp( lexeme_courant->chain , ".data") == 0 ){
			*S = S_GRAMM_DATA;
			defiler(p_file_Lexeme);
			return(1);
		}

		else if ( strcmp( lexeme_courant->chain , ".bss") == 0 ){
			*S = S_GRAMM_BSS;
			defiler(p_file_Lexeme);
			return(1);
		}

		else if( strcmp( lexeme_courant->chain , ".text") == 0 ){
			*S = S_GRAMM_TEXT;
			defiler(p_file_Lexeme);
			return(1);
		}

		else if (strcmp( lexeme_courant->chain , ".set" ) == 0 && strcmp(  ( (LEXEME)((*p_file_Lexeme)->suiv->suiv->val) )-> chain , "noreorder" ) == 0){
			defiler(p_file_Lexeme); /*On defile les LEXEMEs .set & noreorder car on ne les considere pas*/
			defiler(p_file_Lexeme);
			return(1);
		}
	}

	else if (lexeme_courant->cat == COMMENTAIRE){
		defiler(p_file_Lexeme);
		return(1);
	}


	if(*S == S_GRAMM_INIT)*S = S_GRAMM_TEXT; /*Par defaut, on est dans une zone de texte */
	return(0);
}

/**
 * Permet de recuperer les informations du Lexeme_courrant (et facilite donc l'ecriture des fonctions)
 *
 * @param p_file_Lexeme    pointeur sur la file de Lexeme a traiter
 * @param p_lexeme_courant pointeur sur le Lexeme_courant que l'on modifie
 */

void get_current_Lexeme(File* p_file_Lexeme, LEXEME* p_lexeme_courant){
	if((*p_file_Lexeme) == NULL) *p_lexeme_courant=NULL;

	else{
		(*p_lexeme_courant)->cat = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> cat; /* LEXEME*?  On va chercher la categorie du Lexeme courant*/
		(*p_lexeme_courant)->line_nb = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> line_nb;
		(*p_lexeme_courant)->chain = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> chain;
	}
}

/**
 * Permet de creer un maillon data en lisant la file de Lexeme et d'ajouter ce dernier a la file Data
 *
 * @param  p_file_Data    pointeur sur la file Data a completer
 * @param  p_file_Lexeme  pointeur sur la file de LEXEMEs a traiter
 * @param  lexeme_courant Lexeme temporaire qui facilite l'acces au donnees du lexeme courant
 * @param  p_file_Symb    pointeur sur la file Symb a completer
 * @param  p_offset_data  pointeur vers le dernier offset calcule
 *
 * @return                Retourne 0 si tout ce passe correctement, 1 s'il y a une erreur
 */

int ajout_maillon_data(File* p_file_Data, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb, double* p_offset_data, File* p_file_realoc){
	/*Si le lexeme est une etiquette, on doit creer un maillon symb et l'ajouter la la collection de symboles*/
	if(lexeme_courant->cat==ETIQUETTE){
		SYMB new_symb = calloc(1,sizeof(*new_symb));

		new_symb->nom = lexeme_courant->chain;
		new_symb->line_nb = lexeme_courant->line_nb;
		new_symb->zone = ZONE_DATA;
		if(*p_file_Data==NULL) new_symb->decalage = 0;
		else {
			if ( !strcasecmp(((LEXEME)((*p_file_Lexeme)->suiv->suiv->val))->chain, ".word")){ /*Si le lexeme suivant est un .word, on doit realigner*/
				new_symb->decalage = (*p_offset_data) + 4 - (int)(*p_offset_data)%4;
			}
			else{
				new_symb->decalage = *p_offset_data; /*Cas general*/
			}
		}

		*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/
		defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);/*On defile le symbole de la file de lexemes*/
	}
	/*Puis on continue a ajouter le reste de la ligne (ou la ligne suivante?) a la collection text*/

	DATA new_maillon = calloc(1,sizeof(*new_maillon));
	new_maillon->line_nb = lexeme_courant->line_nb;

	if(lexeme_courant->cat!=DIRECTIVE){	/*Si le lexeme n'est pas une directive (une eventuelle etiquette a deja ete enlevee)*/
		WARNING_MSG("Erreur ligne %.0lf, .data peut contienir que des directives\n", lexeme_courant->line_nb);
		new_maillon->type = DATA_ERROR;

		do{ /*On saute la ligne du fichier*/
			defiler(p_file_Lexeme);
			get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
		}while(new_maillon->line_nb == lexeme_courant->line_nb);

		*p_file_Data = enfiler(new_maillon, *p_file_Data);
		return(1);
	}

	/*SINON*/

	new_maillon->operateur = lexeme_courant->chain; /*On recuppere l'operateur*/
	new_maillon->type = DATA_DIR; /*On ne peut avoir que des directives dans .data*/
	defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
	int nb_op = 0;
	Liste liste_operande = creer_liste();

	while(lexeme_courant!=NULL && lexeme_courant->line_nb == new_maillon->line_nb){ /*tant qu'on est sur la meme ligne*/

		if(lexeme_courant->cat == VIRGULE || lexeme_courant->cat == COMMENTAIRE) defiler(p_file_Lexeme);

		else {
			OPERANDE new_operande = calloc(1,sizeof(new_operande));
			new_operande->chain = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> chain;
			switch(lexeme_courant->cat){

				case CHAINE:
					new_operande->type = OPER_CHAINE;
					break;

				case DECIMAL:
					new_operande->type = OPER_DECIMAL;
					new_operande->bin = atol(new_operande->chain);
					break;

				case HEXA:
					new_operande->type = OPER_HEXA;
					new_operande->bin = strtol(new_operande->chain, NULL, 16);
					break;


				case OCTAL:
					new_operande->type = OPER_OCTAL;
					new_operande->bin = strtol(new_operande->chain, NULL, 8);
					break;


				case SYMBOLE:
					new_operande->type = OPER_SYMBOLE;
					break;

				default:
					new_operande->type = OPER_ERROR;
					break;
			}
			nb_op ++;
			liste_operande = ajout_queue(new_operande, liste_operande);
			defiler(p_file_Lexeme);
		}

		get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
	}

	new_maillon->nb_op = nb_op;
	new_maillon->l_operande=liste_operande;

	calcul_decalage_Data(p_file_Data, &new_maillon, p_offset_data);
	Liste l_op_temp = new_maillon->l_operande;
	while(l_op_temp!=NULL){
			if (((OPERANDE)(l_op_temp->val))->type == OPER_SYMBOLE) {
				ajout_maillon_realoc((OPERANDE*)&(l_op_temp->val), p_file_realoc, R_MIPS_32, ZONE_DATA, new_maillon->decalage);
			}
			l_op_temp = l_op_temp->suiv;
	}
	/*Ajout du nouveau maillon a la file de Data*/
	*p_file_Data = enfiler(new_maillon, *p_file_Data);
	return(0);
}

/**
 * Permet de creer un maillon Bss en lisant la file de Lexeme et d'ajouter ce dernier a la file Data
 *
 * @param  p_file_Bss     pointeur sur la file Bss a completer
 * @param  p_file_Lexeme  pointeur sur la file de LEXEMEs a traiter
 * @param  lexeme_courant Lexeme temporaire qui facilite l'acces au donnees du lexeme courant
 * @param  p_file_Symb    pointeur sur la file Symb a completer
 * @param  p_offset_bss   pointeur vers le dernier offset calcule
 *
 * @return                Retourne 0 si tout ce passe correctement, 1 s'il y a une erreur
 */

int ajout_maillon_bss(File* p_file_Bss, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb, double* p_offset_bss){
	/*Si le lexeme est une etiquette, on doit creer un maillon symb et l'ajouter la la collection de symboles*/
	if(lexeme_courant->cat==ETIQUETTE){
		SYMB new_symb = calloc(1,sizeof(*new_symb));

		new_symb->nom = lexeme_courant->chain;
		new_symb->line_nb = lexeme_courant->line_nb;
		new_symb->zone = ZONE_BSS;
		if(*p_file_Bss==NULL) new_symb->decalage = 0;
		else new_symb->decalage = *p_offset_bss;

		*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/
		defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);/*On defile le symbole de la file de lexemes*/
	}
	/*Puis on continue a ajouter le reste de la ligne (ou la ligne suivante?) a la collection text*/

	BSS new_maillon = calloc(1,sizeof(*new_maillon));
	new_maillon->line_nb = lexeme_courant->line_nb;

	if(lexeme_courant->cat!=DIRECTIVE){	/*Si le lexeme n'est pas une directive*/
		WARNING_MSG("Erreur ligne %.0lf, .bss peut contienir que des directives\n", lexeme_courant->line_nb);
		new_maillon->type = BSS_ERROR;

		do{ /*On saute la ligne du fichier*/
			defiler(p_file_Lexeme);
			get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
		}while(new_maillon->line_nb == lexeme_courant->line_nb);

		*p_file_Bss = enfiler(new_maillon, *p_file_Bss);
		return(1);
	}

	/*SINON*/

	new_maillon->operateur = lexeme_courant->chain; /*On recuppere l'operateur*/
	new_maillon->type = BSS_DIR; /*On ne peut avoir que des directives dans .bss*/
	defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
	int nb_op = 0;
	Liste liste_operande = creer_liste();

	while(lexeme_courant!=NULL && lexeme_courant->line_nb == new_maillon->line_nb){ /*tant qu'on est sur la meme ligne*/

		/*Si le lexeme est un commentaire ou une virgule, on ne le considere pas*/
		if(lexeme_courant->cat == VIRGULE || lexeme_courant->cat == COMMENTAIRE) defiler(p_file_Lexeme);

		else {
			OPERANDE new_operande = calloc(1,sizeof(new_operande));
			new_operande->chain = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> chain;
			switch(lexeme_courant->cat){

				case CHAINE:
					new_operande->type = OPER_CHAINE;
					break;


				case DECIMAL:
					new_operande->type = OPER_DECIMAL;
					new_operande->bin = atol(new_operande->chain);
					break;

				case HEXA:
					new_operande->type = OPER_HEXA;
					new_operande->bin = strtol(new_operande->chain, NULL, 16);
					break;


				case OCTAL:
					new_operande->type = OPER_OCTAL;
					new_operande->bin = strtol(new_operande->chain, NULL, 8);
					break;


				case SYMBOLE:
					new_operande->type = OPER_SYMBOLE;
					break;

				default:
					new_operande->type = OPER_ERROR;
					break;
			}
			nb_op ++;
			liste_operande = ajout_queue(new_operande, liste_operande);
			defiler(p_file_Lexeme);
		}

		get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
	}

	new_maillon->nb_op = nb_op;
	new_maillon->l_operande=liste_operande;

	calcul_decalage_Bss(p_file_Bss, &new_maillon, p_offset_bss);

	/*Ajout du nouveau maillon a la file de Bss*/
	*p_file_Bss = enfiler(new_maillon, *p_file_Bss);
	return(0);
}

 /**
  * Permet de creer un maillon Text en lisant la file de Lexeme et d'ajouter ce dernier a la file Data
  *
  * @param  p_file_Text    pointeur sur la file Text a completer
  * @param  p_file_Lexeme  pointeur sur la file de LEXEMEs a traiter
  * @param  lexeme_courant Lexeme temporaire qui facilite l'acces au donnees du lexeme courant
  * @param  p_file_Symb    pointeur sur la file Symb a completer
  * @param  p_offset_text  pointeur vers le dernier offset calcule
  * @param  file_Dic       Dictionnaire d'instructions. La verification du dictionnaire n'est cependant plus geree ici.
  *
  * @return                Retourne 0 si tout ce passe correctement, 1 s'il y a une erreur
  */

int ajout_maillon_text(File* p_file_Text, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb, double* p_offset_text, File file_Dic, File* p_file_realoc, File* p_file_realoc_offset){
	/*Si le lexeme est une etiquette, on doit creer un maillon symb et l'ajouter la la collection de symboles*/
	if(lexeme_courant->cat==ETIQUETTE){
		SYMB new_symb = calloc(1,sizeof(*new_symb));

		new_symb->nom = lexeme_courant->chain;
		new_symb->line_nb = lexeme_courant->line_nb;
		new_symb->zone = ZONE_TEXT;
		if(*p_file_Text==NULL) new_symb->decalage = 0;
		else new_symb->decalage = *p_offset_text;

		*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/
		defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);/*On defile le symbole de la file de lexemes*/
		return(0);
	}
	/*Puis on continue a ajouter le reste de la ligne (ou la ligne suivante?) a la collection text*/

	/*TODO: gerer le pb de definition d'une etiquette au milieu d'une instruction*/

	/*On creer un maillon Text */
	TEXT new_maillon = calloc(1,sizeof(*new_maillon));
	new_maillon->line_nb = lexeme_courant->line_nb;

	if(lexeme_courant->cat!=SYMBOLE){	/*Si le lexeme n'est pas une chaine*/
		WARNING_MSG("Erreur ligne %.0lf, \"%s\" n'est pas une instruction\n", lexeme_courant->line_nb, lexeme_courant->chain);
		new_maillon->type = TEXT_ERROR;

		do{ /*On saute la ligne du fichier*/
			defiler(p_file_Lexeme);
			get_current_Lexeme(p_file_Lexeme, &lexeme_courant); /*A ctualisation du lexeme_courant pour la boucle*/
		}while(new_maillon->line_nb == lexeme_courant->line_nb);

		*p_file_Text = enfiler(new_maillon, *p_file_Text);
		return(1);
	}

	new_maillon->operateur = lexeme_courant->chain; /*On recuppere l'operateur*/

	new_maillon->type = TEXT_INST; /*On ne peut avoir plus que des instructions dans .text*/
	defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
	int nb_op = 0;
	Liste liste_operande = creer_liste();
	get_current_Lexeme(p_file_Lexeme, &lexeme_courant);

	while(lexeme_courant!=NULL && lexeme_courant->line_nb == new_maillon->line_nb){ /*tant qu'on est sur la meme ligne*/

		/*Si le lexeme est un commentaire ou une virgule, on ne le considere pas*/
		if(lexeme_courant->cat == VIRGULE || lexeme_courant->cat == COMMENTAIRE) defiler(p_file_Lexeme);

		else {
			OPERANDE new_operande = calloc(1,sizeof(new_operande));
			new_operande->chain = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> chain;
			switch(lexeme_courant->cat){

				case CHAINE:
					new_operande->type = OPER_CHAINE;
					break;


				case DECIMAL:
					new_operande->type = OPER_DECIMAL;
					/*Si on trouve une structure offset(base)*/
					if (( (LEXEME)((*p_file_Lexeme)->suiv->suiv->val) )->cat  == PARENTHESE_G) { /*Lexeme suivant?*/
						if (( (LEXEME)((*p_file_Lexeme)->suiv->suiv->suiv->val) )->cat  == REGISTRE) { /*Encore suivant*/
							if (( (LEXEME)((*p_file_Lexeme)->suiv->suiv->suiv->suiv->val) )->cat  == PARENTHESE_D) { /*Encore encore suivant*/
								new_operande->type = OPER_OFFSET; /*Le premier opperande est l'offset*/
								nb_op += 2;
								liste_operande = ajout_queue(new_operande, liste_operande); /*On l'ajoute a la liste d'OPERANDE*/
								defiler(p_file_Lexeme); /*On l'enleve de la file de LEXEME*/
								defiler(p_file_Lexeme); /*On enleve la '('*/

								OPERANDE new_operande2 = calloc(1,sizeof(new_operande2));
								new_operande2->type = OPER_BASE;
								new_operande2->chain = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> chain; /*On ajoute le registre comme nouvel operande*/
								is_registre(&new_operande2, new_maillon->line_nb);
								liste_operande = ajout_queue(new_operande2, liste_operande);
								defiler(p_file_Lexeme); /*On enleve le registre de la file de lexeme*/
								defiler(p_file_Lexeme); /*On enleve la ')'*/
							}
						}
					}
					new_operande->bin = atol(new_operande->chain);
					break;

				case HEXA:
					new_operande->type = OPER_HEXA;
					new_operande->bin = strtol(new_operande->chain, NULL, 16);
					break;


				case OCTAL:
					new_operande->type = OPER_OCTAL;
					new_operande->bin = strtol(new_operande->chain, NULL, 8);
					break;


				case SYMBOLE:
					new_operande->type = OPER_SYMBOLE;
					break;

				case REGISTRE:
					new_operande->type = OPER_REG;
					is_registre(&new_operande, new_maillon->line_nb);
					new_operande->bin = atol(new_operande->chain);
					break;

				default:
					new_operande->type = OPER_ERROR;
					break;
			}

			if (new_operande->type != OPER_OFFSET) {
				nb_op ++;
				liste_operande = ajout_queue(new_operande, liste_operande);
				defiler(p_file_Lexeme);
			}

		}
		get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
	}

	new_maillon->nb_op = nb_op;
	new_maillon->l_operande=liste_operande;

	calcul_decalage_Text(p_file_Text, &new_maillon, p_offset_text);

	/*Verification de l'existence de l'operateur et des argument*/
	/*is_in_dic(&new_maillon, file_Dic);*/
	/*Ajout du nouveau maillon a la file de Text*/
	*p_file_Text = enfiler(new_maillon, *p_file_Text);

	is_in_dic(file_Dic, p_file_Text, p_file_realoc, p_file_realoc_offset);
	return(0);
}

/**
 * Fonction a appeler lors de la creation d'un maillon data pour calculer (et ecrire) la valeur du decalage
 *
 * @param p_file_Data   pointeur sur la file Data
 * @param p_new_maillon pointeur sur le nouveau maillon (en cours de creation) pour acceder a ses infos et y ecrire le decalage
 * @param p_offset_data dernier offset calcule
 */
void calcul_decalage_Data(File* p_file_Data, DATA* p_new_maillon, double* p_offset_data){
	(*p_new_maillon)->decalage = (*p_offset_data);

	int cmpt_operande;

	OPERANDE op_temp = calloc(1, sizeof(*op_temp));
	Liste p_operande_courante = (*p_new_maillon)->l_operande;

	for (cmpt_operande = 1; cmpt_operande <= (*p_new_maillon)->nb_op; cmpt_operande++){

		op_temp->chain = (((OPERANDE)((p_operande_courante)->val))->chain);
		op_temp->type = (((OPERANDE)((p_operande_courante)->val))->type);

		if(strcmp( (*p_new_maillon)->operateur , ".byte")==0){
			(*p_offset_data) += 1;
		}

		else if(strcmp( (*p_new_maillon)->operateur , ".space")==0){
			/*TODO : gerer les nombre hexa et octal (notamment atof)*/
			if (op_temp->type==OPER_DECIMAL){ /*Si l'opperande est bien un nombre*/
				(*p_offset_data) += atof(op_temp->chain);
			}
			else{
				(*p_new_maillon)->type = DATA_ERROR;
				WARNING_MSG("Erreur ligne %.0lf, l'operande %d n'est pas un nombre\n", (*p_new_maillon)->line_nb, cmpt_operande);
			}
		}

		else if(strcmp( (*p_new_maillon)->operateur , ".asciiz")==0){

			if (op_temp->type==OPER_CHAINE ){ /*Si l'opperande est bien une chaine*/
				(*p_offset_data) += strlen(op_temp->chain);
			}
			else{
				(*p_new_maillon)->type = DATA_ERROR;
				WARNING_MSG("Erreur ligne %.0lf, l'operande %d n'est pas une chaine de caractere\n", (*p_new_maillon)->line_nb, cmpt_operande);
			}
		}

		else if(strcmp( (*p_new_maillon)->operateur , ".word")==0){
			if((int)(*p_offset_data) == 0){
				(*p_new_maillon)->decalage = (*p_offset_data);
				(*p_offset_data) += 4;
			}
			else{
				if((int)(*p_offset_data)%4 == 0){
					(*p_new_maillon)->decalage = (*p_offset_data);
					(*p_offset_data) += 4;
				}
				/*On aligne les .word modulo 4*/
				else{
					(*p_offset_data) +=  4 - (int)(*p_offset_data)%4;
					(*p_new_maillon)->decalage = (*p_offset_data); /*On overrite la valeur dans le cas d'un .word*/
				}
			}
		}

		else{
			WARNING_MSG("Operateur : \"%s\" non reconnu dans un .data\n", (*p_new_maillon)->operateur);
			break;
		}
		/*On passe a l'operande suivante*/
		p_operande_courante = p_operande_courante->suiv;

	}
	free(op_temp);
}


/**
 * Fonction a appeler lors de la creation d'un maillon Text pour calculer (et ecrire) la valeur du decalage
 *
 * @param p_file_Text   pointeur sur la file Text
 * @param p_new_maillon pointeur sur le nouveau maillon (en cours de creation) pour acceder a ses infos et y ecrire le decalage
 * @param p_offset_text dernier offset calcule
 */
void calcul_decalage_Text(File* p_file_Text, TEXT* p_new_maillon, double* p_offset_text){
	(*p_new_maillon)->decalage = (*p_offset_text);

	/*Gestion des pseudos instructions*/
	if(strcasecmp((*p_new_maillon)->operateur, "lw")==0 && (*p_new_maillon)->nb_op == 2){
		if (((OPERANDE)(((*p_new_maillon)->l_operande)->suiv)->val)->type == OPER_SYMBOLE) {
			*p_offset_text += 8;
		}
	}

	else *p_offset_text += 4;
}

/**
 * Fonction a appeler lors de la creation d'un maillon Bss pour calculer (et ecrire) la valeur du decalage
 *
 * @param p_file_Bss    pointeur sur la file Bss
 * @param p_new_maillon pointeur sur le nouveau maillon (en cours de creation) pour acceder a ses infos et y ecrire le decalage
 * @param p_offset_bss  dernier offset calcule
 */

void calcul_decalage_Bss(File* p_file_Bss, BSS* p_new_maillon, double* p_offset_bss){
 	(*p_new_maillon)->decalage = (*p_offset_bss);

 	int cmpt_operande=1;

 	OPERANDE op_temp = calloc(1, sizeof(*op_temp));
 	Liste p_operande_courante = (*p_new_maillon)->l_operande;

 	while(p_operande_courante!=NULL){

 		op_temp->chain = (((OPERANDE)((p_operande_courante)->val))->chain);
 		op_temp->type = (((OPERANDE)((p_operande_courante)->val))->type);

 		if(strcmp((*p_new_maillon)->operateur, ".space")==0){
 			/*TODO : gerer les nombre hexa et octal (notamment atof)*/

 			if (op_temp->type==OPER_DECIMAL){ /*Si l'opperande est bien un nombre*/
 				(*p_offset_bss) += atof(op_temp->chain);
 			}
 			else{
 				(*p_new_maillon)->type = DATA_ERROR;
 				WARNING_MSG("Erreur ligne %.0lf, l'operande %d n'est pas un nombre", (*p_new_maillon)->line_nb, cmpt_operande);
 			}
 		}

 		else{
 			WARNING_MSG("Operateur : \"%s\" non reconnu dans un .bss", (*p_new_maillon)->operateur);
 			break;
 		}
 		/*On passe a l'operande suivante*/
 		p_operande_courante = p_operande_courante->suiv;
 		cmpt_operande ++;
 	}

 	free(op_temp);
 }

/**
 * Verifie l'existence d'une instruction et ses operandes. Remplace les pseudos instruction en leur(s) instructions
 *
 * @param  file_Dic                    File de dictionnaire
 * @param  p_file_Text_maillon_courant Pointeur sur le maillon courant de la file de texte, permet d'inserer des instruction si on rencontre une peusdo instruction
 * @param  p_file_realoc 			   Table de realoc a completer si l'operande est une etiquette
 * @param  p_file_realoc_offset 	   Table a completer si l'operande est une etiquette et de type O
 *
 * @return         retourne 1 si tout se passe bien, 0 sinon
 */

int is_in_dic(File file_Dic, File* p_file_Text_maillon_courant, File* p_file_realoc, File* p_file_realoc_offset){

	TEXT* p_maillon = (TEXT*)(&((*p_file_Text_maillon_courant)->val)); /*Utile si on veut rajouter un maillon*/

	int i;
	OPERANDE operande_courante = calloc(1, sizeof(operande_courante));
	Liste l_operande = (*p_maillon)->l_operande;

	File dernier_elem = file_Dic;
	file_Dic = file_Dic->suiv;


	/*CAS PARTICULIER DES PSEUDOS INSTRUCTIONS*/

	if(is_pseudo_inst(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset, p_maillon, l_operande)){
		return(1);
	}


	/*CAS GENERAL*/
	/*On parcours le dictionnaire*/

	do{
		if(strcasecmp((*p_maillon)->operateur, ((DIC)((file_Dic)->val))->chain)==0){
			/*On a trouve notre operateur dans le dictionnaire*/
			if ((*p_maillon)->nb_op == ((DIC)((file_Dic)->val))->nb_op){
				/*Si le nombre d'operateur est le bon*/
				char* type_op = ((DIC)((file_Dic)->val))->type_op;
				for(i=0; i<(*p_maillon)->nb_op; i++){
					type_operande type_op_courrante = ((OPERANDE)(l_operande->val))->type;
					switch (type_op[i]) {

						case 'R':
							if (type_op_courrante != OPER_REG){
								((OPERANDE)(l_operande->val))->type = OPER_ERROR;
								WARNING_MSG("Erreur ligne %.0lf, l'opperande no %d de \"%s\" doit etre un registre\n", (*p_maillon)->line_nb, i, (*p_maillon)->operateur);
							}
							
							break;

						case 'O':
							if (type_op_courrante == OPER_SYMBOLE){
								ajout_maillon_realoc((OPERANDE*)&(l_operande->val), p_file_realoc_offset, R_OFFSET, ZONE_TEXT, (*p_maillon)->decalage);
							}
							else if (type_op_courrante != OPER_OFFSET){
								((OPERANDE)(l_operande->val))->type = OPER_ERROR;
								WARNING_MSG("Erreur ligne %.0lf, l'opperande no %d de \"%s\" doit etre un offset\n", (*p_maillon)->line_nb, i, (*p_maillon)->operateur);
							}
							break;

						case 'B':
							if (type_op_courrante != OPER_BASE){
								((OPERANDE)(l_operande->val))->type = OPER_ERROR;
								WARNING_MSG("Erreur ligne %.0lf, l'opperande no %d de \"%s\" doit etre une base\n", (*p_maillon)->line_nb, i, (*p_maillon)->operateur);
							}
							break;

						case 'T':
							if (type_op_courrante == OPER_SYMBOLE){
								ajout_maillon_realoc((OPERANDE*)&(l_operande->val), p_file_realoc, R_MIPS_26, ZONE_TEXT, (*p_maillon)->decalage);
							}
							else if (type_op_courrante != OPER_DECIMAL && type_op_courrante != OPER_HEXA && type_op_courrante != OPER_REG){
								((OPERANDE)(l_operande->val))->type = OPER_ERROR;
								WARNING_MSG("Erreur ligne %.0lf, l'opperande no %d de \"%s\" doit etre un offset\n", (*p_maillon)->line_nb, i, (*p_maillon)->operateur);
							}

							break;

						case 'I':
							if (type_op_courrante == OPER_SYMBOLE){
								ajout_maillon_realoc((OPERANDE*)&(l_operande->val), p_file_realoc, R_MIPS_LO16, ZONE_TEXT, (*p_maillon)->decalage);
							}
							else if (type_op_courrante != OPER_DECIMAL && type_op_courrante != OPER_HEXA){
								((OPERANDE)(l_operande->val))->type = OPER_ERROR;
								WARNING_MSG("Erreur ligne %.0lf, l'opperande no %d doit etre une valeure immediate\n", (*p_maillon)->line_nb, i);
							}
							break;

						case 'S':
							if (type_op_courrante != OPER_DECIMAL && type_op_courrante != OPER_HEXA){
								((OPERANDE)(l_operande->val))->type = OPER_ERROR;
								WARNING_MSG("Erreur ligne %.0lf, l'opperande no %d doit etre un nombre de decalage\n", (*p_maillon)->line_nb, i);
							}
							break;

					}
					l_operande = l_operande->suiv;
				}
				free(operande_courante);
				generation_bin_instr(*p_maillon, (DIC)((file_Dic)->val));
				return(0);
			}

			else{
				(*p_maillon)->type = TEXT_ERROR;
				WARNING_MSG("Erreur ligne %.0lf, le nombre d'operateur de \"%s\" est %d au lieu de %d\n", (*p_maillon)->line_nb, (*p_maillon)->operateur, (*p_maillon)->nb_op, ((DIC)((file_Dic)->val))->nb_op );
				free(operande_courante);
				free(p_maillon);
				return(1);
			}
		}

		file_Dic = file_Dic->suiv;
	}while(file_Dic!=dernier_elem->suiv); /*Tant qu'on ne revient pas au 1er elem*/
	file_Dic = dernier_elem;

	/*On a parcouru tout le dic sans trouver l'instruction*/
	(*p_maillon)->type = TEXT_ERROR;
	WARNING_MSG("Instruction \"%s\" ligne: %.0lf inconnue\n", (*p_maillon)->operateur, (*p_maillon)->line_nb);
	free(operande_courante);
	return(1);
}

/**
 * Partie de la fonction is_in_dic pour les pseudo-instructions (codee en dur)
 *
 * @param  file_Dic                    File de dictionnaire
 * @param  p_file_Text_maillon_courant Pointeur sur le maillon courant de la file de texte, permet d'inserer des instruction si on rencontre une peusdo instruction
 * @param  p_file_realoc 			   Table de realoc a completer si l'operande est une etiquette
 * @param  p_file_realoc_offset 	   Table a completer si l'operande est une etiquette et de type O
 * @param  p_maillon                   Pointeur sur le maillon courant de TEXT
 * @param  l_operande                  Pointeur sur la liste des operandes de l'instruction courante
 *
 * @return                             retourne 1 si on trouve une pseudo instruction, 0 sinon
 */
int is_pseudo_inst(File file_Dic, File* p_file_Text_maillon_courant, File* p_file_realoc, File* p_file_realoc_offset, TEXT* p_maillon, Liste l_operande){
	if(!strcasecmp((*p_maillon)->operateur,"LW")){
		if( (*p_maillon)->nb_op==2 ){
			if ( (((OPERANDE)(((*p_maillon)->l_operande)->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->val))->type == OPER_ERROR_UNK_REGISTER) && ((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type ==  OPER_SYMBOLE){
				/*Si on trouve une instruction
					LW $R, etiquette
				On la remplace par
					LUI $at, etiquette_poidsfort>>16
					LW $R, etiquette_poidsfaible($at)
				*/
			/*
				if(is_registre((OPERANDE*)(&(l_operande->val)), (*p_maillon)->line_nb)==1){
					((OPERANDE)(l_operande->val))->bin = atol(((OPERANDE)(l_operande->val))->chain);
				}
			*/
				type_operande type_op = ((OPERANDE)(((*p_maillon)->l_operande)->val))-> type;
				char* registre = calloc(1,sizeof(char*)); registre = ((OPERANDE)(((*p_maillon)->l_operande)->val))-> chain;
				/*------------------------------------------*/
				/*--------Modification du maillon LW--------*/
				/*------------------------------------------*/
				char* lui = "lui";
				(*p_maillon)->operateur = lui;

				((OPERANDE)(((*p_maillon)->l_operande)->val))->chain = "1";	/*$at*/
				((OPERANDE)(((*p_maillon)->l_operande)->val))->type = OPER_REG;	/*$at*/
				((OPERANDE)(((*p_maillon)->l_operande)->val))->bin = 1;	/*$at*/

				/*TODO : TROUVER LE BON TYPE POUR etiquette_poidsfort>>16 */
				/*TODO : TROUVER COMMENT FAIRE etiquette_poidsfort>>16*/
				((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type = OPER_SYMBOLE;
				/*On ne change pas la chaine
				((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->chain = "etiquette_poidsfort>>16";
				Mais on ajoute l'operande a laa table de realoc*/
				ajout_maillon_realoc((OPERANDE*)&(((*p_maillon)->l_operande)->suiv->val), p_file_realoc, R_MIPS_HI16, ZONE_TEXT, (*p_maillon)->decalage);
				
				/*Generation du code binaire*/
				is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);
				/*------------------------------------------*/
				/*Creation et remplissage du nouveau maillon*/
				/*------------------------------------------*/
				TEXT new_maillon_text = calloc(1,sizeof(*new_maillon_text));

				char* operateur = "lw";
				new_maillon_text->operateur = operateur;
				new_maillon_text->type = TEXT_INST;
				new_maillon_text->nb_op = 3;
				new_maillon_text->line_nb = - ((*p_maillon)->line_nb) ;
				new_maillon_text->decalage = (*p_maillon)->decalage+4;

				/*Creation et remplissage de la liste d'operandes*/

				OPERANDE op1 = calloc(1,sizeof(*op1));
				op1->type = type_op;
				op1->chain = registre;
				op1->bin = ((OPERANDE)(((*p_maillon)->l_operande)->val))->bin;

				OPERANDE op2 = calloc(1,sizeof(*op2));
				op2->type = OPER_SYMBOLE;
				/*TODO : POIDS FORTS DE L'ETIQUETTE*/
				/*On ne change pas la chaine
				op2->chain = "etiquette_poidsfaible";
				Mais on ajoute l'operande a laa table de realoc*/
				op2->chain = ((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->chain;
				ajout_maillon_realoc(&op2, p_file_realoc, R_MIPS_LO16, ZONE_TEXT, new_maillon_text->decalage);

				OPERANDE op3 = calloc(1,sizeof(*op3));
				op3->type = OPER_BASE;
				op3->chain = "1";	/*$at*/
				op3->bin = 1;

				Liste new_l_op = NULL;
				new_l_op=ajout_tete((op3), new_l_op);
				new_l_op=ajout_tete((op2), new_l_op);
				new_l_op=ajout_tete((op1), new_l_op);

				new_maillon_text->l_operande = new_l_op;

				/*------------------------------------------*/
				/*----------NOUVEAUX  BRANCHEMENTS----------*/
				/*------------------------------------------*/
				(*p_file_Text_maillon_courant) = enfiler(new_maillon_text, *p_file_Text_maillon_courant);
				/*File new_maillon_file = calloc(1, sizeof(*new_maillon_file));
				new_maillon_file->val = new_maillon_text;
				new_maillon_file->suiv = (*p_file_Text_maillon_courant)->suiv;

				(*p_file_Text_maillon_courant)->suiv = new_maillon_file;
				*/
				is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);
				return(1);
			}
		}
	}

	else if(!strcasecmp((*p_maillon)->operateur,"SW")){
		if( (*p_maillon)->nb_op==2 ){
			if ( (((OPERANDE)(((*p_maillon)->l_operande)->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->val))->type == OPER_ERROR_UNK_REGISTER) && ((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type ==  OPER_SYMBOLE){

				/*Si on trouve une instruction
					SW $R, etiquette
				On la remplace par
					LUI $at, etiquette_poidsfort>>16
					SW $R, etiquette_poidsfaible($at)
				*/
			
				type_operande type_op = ((OPERANDE)(((*p_maillon)->l_operande)->val))-> type;
				char* registre = calloc(1,sizeof(char*)); registre = ((OPERANDE)(((*p_maillon)->l_operande)->val))-> chain;
				/*------------------------------------------*/
				/*--------Modification du maillon LW--------*/
				/*------------------------------------------*/
				char* lui = "lui";
				(*p_maillon)->operateur = lui;

				((OPERANDE)(((*p_maillon)->l_operande)->val))->chain = "1";	/*$at*/
				((OPERANDE)(((*p_maillon)->l_operande)->val))->bin = 1;	/*$at*/

				/*TODO : TROUVER LE BON TYPE POUR etiquette_poidsfort>>16 */
				/*TODO : TROUVER COMMENT FAIRE etiquette_poidsfort>>16*/
				((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type = OPER_SYMBOLE;
				/*On ne change pas la chaine
				((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->chain = "etiquette_poidsfort>>16";
				Mais on ajoute l'operande a laa table de realoc*/
				ajout_maillon_realoc((OPERANDE*)&(((*p_maillon)->l_operande)->suiv->val), p_file_realoc, R_MIPS_HI16, ZONE_TEXT, (*p_maillon)->decalage);
				/*generation du code binaire*/
				is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);
				/*------------------------------------------*/
				/*Creation et remplissage du nouveau maillon*/
				/*------------------------------------------*/
				TEXT new_maillon_text = calloc(1,sizeof(*new_maillon_text));

				char* operateur = "sw";
				new_maillon_text->operateur = operateur;
				new_maillon_text->type = TEXT_INST;
				new_maillon_text->nb_op = 3;
				new_maillon_text->line_nb = - ((*p_maillon)->line_nb) ;
				new_maillon_text->decalage = (*p_maillon)->decalage+4;

				/*Creation et remplissage de la liste d'operandes*/

				OPERANDE op1 = calloc(1,sizeof(*op1));
				op1->type = type_op;
				op1->chain = registre;
				op1->bin = ((OPERANDE)(((*p_maillon)->l_operande)->val))->bin;

				OPERANDE op2 = calloc(1,sizeof(*op2));
				op2->type = OPER_SYMBOLE;
				/*TODO : POIDS FORTS DE L'ETIQUETTE*/
				/*On ne change pas la chaine
				op2->chain = "etiquette_poidsfaible";
				Mais on ajoute l'operande a laa table de realoc*/
				op2->chain = ((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->chain;
				ajout_maillon_realoc(&op2, p_file_realoc, R_MIPS_LO16, ZONE_TEXT, new_maillon_text->decalage);

				OPERANDE op3 = calloc(1,sizeof(*op3));
				op3->type = OPER_BASE;
				op3->chain = "1";	/*$at*/
				op3->bin = 1;

				Liste new_l_op = NULL;
				new_l_op=ajout_tete((op3), new_l_op);
				new_l_op=ajout_tete((op2), new_l_op);
				new_l_op=ajout_tete((op1), new_l_op);

				new_maillon_text->l_operande = new_l_op;

				/*------------------------------------------*/
				/*----------NOUVEAUX  BRANCHEMENTS----------*/
				/*------------------------------------------*/
				(*p_file_Text_maillon_courant) = enfiler(new_maillon_text, *p_file_Text_maillon_courant);
				
				/*generation du code binaire*/
				is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);

				return(1);
			}
		}
	}

	else if(!strcasecmp((*p_maillon)->operateur,"MOVE")){
		if( (*p_maillon)->nb_op==2 ){
			if ( (((OPERANDE)(((*p_maillon)->l_operande)->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->val))->type == OPER_ERROR_UNK_REGISTER) && (((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type == OPER_ERROR_UNK_REGISTER) ){
				/*Si on trouve une instruction
					MOVE $rt, $rs
				On la remplace par
					ADD $rt, $rs, $0
				*/
				/*
				if(is_registre((OPERANDE*)(&(l_operande->val)), (*p_maillon)->line_nb)==1){
					((OPERANDE)(l_operande->val))->bin = atol(((OPERANDE)(l_operande->val))->chain);
				}
				if(is_registre((OPERANDE*)(&(l_operande->suiv->val)), (*p_maillon)->line_nb)==1){
					((OPERANDE)(l_operande->suiv->val))->bin = atol(((OPERANDE)(l_operande->suiv->val))->chain);
				}*/

				char* zero = "0";
				/*------------------------------------------*/
				/*--------Modification du maillon MOVE--------*/
				/*------------------------------------------*/
				char* add = "add";
				(*p_maillon)->operateur = add;
				(*p_maillon)->nb_op =3;

				OPERANDE new_op = calloc(1,sizeof(*new_op));
				new_op->chain = zero;
				new_op->type = OPER_REG;
				new_op->bin = 0;


				l_operande = ajout_queue(new_op, l_operande);
				/*generation du code binaire*/

				is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);

				return(1);
			}
		}
	}

	else if(!strcasecmp((*p_maillon)->operateur,"NEG")){
		if( (*p_maillon)->nb_op==2 ){
			if ( (((OPERANDE)(((*p_maillon)->l_operande)->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->val))->type == OPER_ERROR_UNK_REGISTER) && (((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type == OPER_ERROR_UNK_REGISTER) ){
				/*Si on trouve une instruction
					NEG $Rs, $Rt
				On la remplace par
					SUB $rt, $0, $rs
				*/
				/*
				if(is_registre((OPERANDE*)(&(l_operande->val)), (*p_maillon)->line_nb)==1){
					((OPERANDE)(l_operande->val))->bin = atol(((OPERANDE)(l_operande->val))->chain);
				}
				if(is_registre((OPERANDE*)(&(l_operande->suiv->val)), (*p_maillon)->line_nb)==1){
					((OPERANDE)(l_operande->suiv->val))->bin = atol(((OPERANDE)(l_operande->suiv->val))->chain);
				}*/

				char* zero = "0"; /*calloc(1,sizeof(char*)); zero = "0\0";*/
				/*------------------------------------------*/
				/*--------Modification du maillon LW--------*/
				/*------------------------------------------*/
				char* sub = "sub";
				(*p_maillon)->operateur = sub;
				(*p_maillon)->nb_op =3;

				OPERANDE new_op = calloc(1,sizeof(*new_op));
				new_op->chain = zero;
				new_op->type = OPER_REG;
				new_op->bin = 0;

				l_operande->suiv = ajout_tete(new_op, l_operande->suiv);

				is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);
				return(1);
			}
		}
	}

	else if(!strcasecmp((*p_maillon)->operateur,"LI")){
		if( (*p_maillon)->nb_op==2 ){
			if ( (((OPERANDE)(((*p_maillon)->l_operande)->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->val))->type == OPER_ERROR_UNK_REGISTER) ){
				/*Si on trouve une instruction
					LI $rt, immediate
				On la remplace par
					ADDI $rt, $0, immediate
				*/
				int continuer = 0;
				/*
				if(is_registre((OPERANDE*)(&(l_operande->val)), (*p_maillon)->line_nb)==1){
					((OPERANDE)(l_operande->val))->bin = atol(((OPERANDE)(l_operande->val))->chain);
				}*/
				

				/*Si immediate est un symbole:*/
				if(((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type == OPER_SYMBOLE){
					ajout_maillon_realoc((OPERANDE*)&(((*p_maillon)->l_operande)->suiv->val), p_file_realoc_offset, R_OFFSET, ZONE_TEXT, (*p_maillon)->decalage);
					continuer = 1;
				}
				
				/*Si immediate est un nombre*/
				else if (((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type == OPER_DECIMAL){					
					continuer = 1;
				}
				else if ( (((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type == OPER_HEXA)){
					continuer = 1;
				}
				if(continuer){
									
				char* zero = "0"; /*calloc(1,sizeof(char*)); zero = "0\0";*/
				/*------------------------------------------*/
				/*--------Modification du maillon LW--------*/
				/*------------------------------------------*/
				char* addi = "addi";
				(*p_maillon)->operateur = addi;
				(*p_maillon)->nb_op++;

				OPERANDE new_op = calloc(1,sizeof(*new_op));
				new_op->chain = zero;
				new_op->type = OPER_REG;
				new_op->bin = 0;

				l_operande->suiv = ajout_tete(new_op, l_operande->suiv);

				is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);
				return(1);
				}
			}	
		}
	}

	else if(!strcasecmp((*p_maillon)->operateur,"BLT")){
		if( (*p_maillon)->nb_op==3 ){
			if ( (((OPERANDE)(((*p_maillon)->l_operande)->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->val))->type == OPER_ERROR_UNK_REGISTER) && (((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type ==  OPER_REG || ((OPERANDE)(((*p_maillon)->l_operande)->suiv->val))->type == OPER_ERROR_UNK_REGISTER) ){
				int continuer = 0;
				/*Si on trouve une instruction
					BLT $rt, $rs, target
				On la remplace par
					SLT $1, $rt, $rs
					BNE $1, $zero, target
				*/
				if (((OPERANDE)(((*p_maillon)->l_operande)->suiv->suiv->val))->type ==  OPER_SYMBOLE){
					ajout_maillon_realoc((OPERANDE*)&(((*p_maillon)->l_operande)->suiv->suiv->val), p_file_realoc_offset, R_OFFSET, ZONE_TEXT, (*p_maillon)->decalage);
					continuer = 1;
				}

				else if(((OPERANDE)(((*p_maillon)->l_operande)->suiv->suiv->val))->type ==  OPER_DECIMAL || ((OPERANDE)(((*p_maillon)->l_operande)->suiv->suiv->val))->type ==  OPER_HEXA){
					continuer =1;
				}
				if (continuer){
					/*type_operande type_op = ((OPERANDE)(((*p_maillon)->l_operande)->val))-> type;
					char* registre = calloc(1,sizeof(char*)); registre = ((OPERANDE)(((*p_maillon)->l_operande)->val))-> chain;*/
					/*------------------------------------------*/
					/*--------Modification du maillon LW--------*/
					/*------------------------------------------*/
					char* slt = "slt";
					(*p_maillon)->operateur = slt;

					OPERANDE op0 = calloc(1,sizeof(*op0));
					op0->type = OPER_REG;
					op0->chain = "1";
					op0->bin = 1;

					/*On enleve le maillon target pour le reinserer apres*/
					Liste l_target = ((*p_maillon)->l_operande)->suiv->suiv;
					((*p_maillon)->l_operande)->suiv->suiv = NULL;

					(*p_maillon)->l_operande = ajout_tete(op0, (*p_maillon)->l_operande);

					is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);
					/*------------------------------------------*/
					/*Creation et remplissage du nouveau maillon*/
					/*------------------------------------------*/
					TEXT new_maillon_text = calloc(1,sizeof(*new_maillon_text));

					char* operateur = "bne";
					new_maillon_text->operateur = operateur;
					new_maillon_text->type = TEXT_INST;
					new_maillon_text->nb_op = 3;
					new_maillon_text->line_nb = - ((*p_maillon)->line_nb) ;
					new_maillon_text->decalage = (*p_maillon)->decalage+4;

					/*Creation et remplissage de la liste d'operandes*/

					OPERANDE op1 = calloc(1,sizeof(*op1));
					op1->type = OPER_REG;
					op1->chain = "1";
					op1->bin = 1;

					OPERANDE op2 = calloc(1,sizeof(*op2));
					op2->type = OPER_REG;
					op2->chain = "0";
					op2->bin = 0;

					Liste new_l_op = l_target; /*On reinsere le maillon target*/
					new_l_op=ajout_tete((op2), new_l_op);
					new_l_op=ajout_tete((op1), new_l_op);

					new_maillon_text->l_operande = new_l_op;

					/*------------------------------------------*/
					/*----------NOUVEAUX  BRANCHEMENTS----------*/
					/*------------------------------------------*/
					(*p_file_Text_maillon_courant) = enfiler(new_maillon_text, *p_file_Text_maillon_courant);
					
					is_in_dic(file_Dic, p_file_Text_maillon_courant, p_file_realoc, p_file_realoc_offset);
					return(1);
				}
			}
		}
	}
	return(0);
}

/**
 * Verifie que l'opperande courrante (qui est un registre) est un registre existant
 *
 * @param  p_op    operande courante contenant un regisre
 * @param  line_nb numero de ligne de l'operande pour afficher une erreur si besoin
 *
 * @return         retourne 1 si tout se passe bien, 0 sinon
 */
int is_registre(OPERANDE* p_op, double line_nb){
	char* registre = (*p_op)->chain; char reg[3];
	int reg_int;

	if(!strcasecmp(registre, "$zero")){(*p_op)->chain = "0"; return(1);}
	if((!strcasecmp(registre, "$at") || !strcasecmp(registre, "$1"))){
		(*p_op)->chain = "1"; return(1);
	}
	if(strlen(registre)>3){
		(*p_op)->type = OPER_ERROR_UNK_REGISTER;
		(*p_op)->chain = (*p_op)->chain + 1;
		WARNING_MSG("Erreur ligne %.0lf : le registre \"$%s\" n'existe pas\n", line_nb, registre); return(0);
	}

	if(isdigit(registre[1])){/*Si le premier caractere du registre est un chiffre => Registre*/
		if (registre[2]=='\0'){
			(*p_op)->chain = (*p_op)->chain + 1; /*On enleve le dollar*/
			return(1); /*Le registre ne contient qu'un carac qui est un chiffre*/
		}
		else if(isdigit(registre[2])){ /*Le registre contient deux carac qui ne sont que des chiffres*/
			reg[0] = registre[1];reg[1] = registre[2];reg[2] = '\0';
			reg_int= strtol(reg, (char **)NULL, 10);
			if (reg_int>=0 && reg_int<=31){
				/* PAS NECESSAIRE
				if(reg_int == 26 || reg_int==27){
					(*p_op)->type = OPER_ERROR_UNK_REGISTER;
					WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" est reserve au kernel\n", line_nb, registre); return(0);
				}*/
				(*p_op)->chain = (*p_op)->chain + 1; /*On enleve le dollar*/
				return(1);
			}
			else{ /*Le deuxieme carac du registre est une lettre IMPOSSIBLE*/
				(*p_op)->type = OPER_ERROR_UNK_REGISTER;
				WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" n'existe pas\n", line_nb, registre); return(0);
			}
		}
		else{ /*Le deuxieme carac du registre est une lettre IMPOSSIBLE*/
			(*p_op)->type = OPER_ERROR_UNK_REGISTER;
			WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" n'existe pas\n", line_nb, registre); return(0);
		}
	}

	else{ /*Sinon (1er carac n'est pas un chiffre) => Mnemonique*/
		if(registre[1]=='a'){
			if(registre[2]=='t'){(*p_op)->chain = "1"; return(1);}
			if(isdigit(registre[2])){
				if (atoi(&registre[2])<=3){sprintf((*p_op)->chain, "%d", atoi(&registre[2]) + 4); return 1;}
			}
			(*p_op)->type = OPER_ERROR_UNK_REGISTER;
			WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" n'existe pas\n", line_nb, registre);return(0);
		}

		if(registre[1]=='v'){
			if(isdigit(registre[2])){
				if (atoi(&registre[2])<=1){sprintf((*p_op)->chain, "%d", atoi(&registre[2]) + 2);return 1;}
			}
			(*p_op)->type = OPER_ERROR_UNK_REGISTER;
			WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" n'existe pas\n", line_nb, registre);return(0);
		}

		if(registre[1]=='t'){
			if(isdigit(registre[2])){
				if (atoi(&registre[2])<=7) sprintf((*p_op)->chain, "%d", atoi(&registre[2]) + 8);
				else sprintf((*p_op)->chain, "%d", atoi(&registre[2]) + 16);
				return 1;
			}
			(*p_op)->type = OPER_ERROR_UNK_REGISTER;
			WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" n'existe pas\n", line_nb, registre);return(0);
		}

		if(registre[1]=='s'){
			if(registre[2]=='p'){(*p_op)->chain = "29"; return 1;}
			if(isdigit(registre[2])){
				if (atoi(&registre[2])<=7){sprintf((*p_op)->chain, "%d", atoi(&registre[2]) + 16); return 1;}
			}
			(*p_op)->type = OPER_ERROR_UNK_REGISTER;
			WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" n'existe pas\n", line_nb, registre);return(0);
		}

		if(registre[1]=='k'){
			if(isdigit(registre[2])){
				if (atoi(&registre[2])<=1){
					/*PAS NECESSAIRE
					(*p_op)->type = OPER_ERROR_UNK_REGISTER;
					WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" est reserve au kernel\n", line_nb, registre); return(0);
					*/
					sprintf((*p_op)->chain, "%d", atoi(&registre[2]) + 26);return 1;
				}
			}
			(*p_op)->type = OPER_ERROR_UNK_REGISTER;
			WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" n'existe pas\n", line_nb, registre);return(0);
		}
		if(registre[1]=='g' && registre[2]=='p'){(*p_op)->chain = "28"; return 1;}
		if(registre[1]=='f' && registre[2]=='p'){(*p_op)->chain = "30"; return 1;}
		if(registre[1]=='r' && registre[2]=='a'){(*p_op)->chain = "31"; return 1;}

		(*p_op)->type = OPER_ERROR_UNK_REGISTER;
		WARNING_MSG("Erreur ligne %.0lf : le registre \"%s\" n'existe pas\n", line_nb, registre);return(0);
	}
	return 0;
}

void generation_bin_instr(TEXT maillon, DIC definition){
	int i=1, j, masque, decalage = 32;
	Liste l_op;

	masque = pow(2, definition->bin[0][1]) -1; /*que des 1 sur le nombre de bit qu'il faut*/
	decalage -= definition->bin[0][1];
	maillon->bin = (masque & definition->bin[0][0])<<decalage; /*On recupere l'opcode*/
	

	while(definition->bin[i][1]!=0){
		/*On parcours le tableau binaire tant que l'operande/l'inof n'est pas codee sur 0 bits*/
		decalage -= definition->bin[i][1];
		masque = pow(2, definition->bin[i][1]) -1; /*que des 1 sur le nombre de bit qu'il faut*/

		if(definition->bin[i][0]>=0){ /*Si le nombre est positif, on le code sur "decalage" bits*/
			maillon->bin = maillon->bin | ((masque & definition->bin[i][0] )<<decalage);
		}	
		else{ /*Sinon il faut chercher l'operande correspondante*/
			l_op = maillon->l_operande;
			for(j=1; j<-definition->bin[i][0]; j++){ /*On cherche la bonne operande*/
				l_op = l_op->suiv; 
			}
			maillon->bin = maillon->bin | ((masque & (((OPERANDE)(l_op->val))->bin))<<decalage);
		}

		i++;
	}

	maillon->bin = ((maillon->bin>>24)  & 0x000000ff) | // move byte 3 to byte 0
                    ((maillon->bin<<8)  & 0x00ff0000) | // move byte 1 to byte 2
                    ((maillon->bin>>8)  & 0x0000ff00) | // move byte 2 to byte 1
                    ((maillon->bin<<24) & 0xff000000); 	// byte 0 to byte 3
}


