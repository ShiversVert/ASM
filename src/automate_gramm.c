#include <automate_gramm.h>

/*@param p_file_Lexeme pointeur sur la file de Lexeme a traiter
 *@param p_file_Text pointeur sur la file d'element .Text a remplir
 *@param p_file_Bss pointeur sur la file d'element .Bss a remplir
 *@param p_file_Data pointeur sur la file d'element .Data a remplir
 *@return 
 *@brief
 */

void automate_grammatical(File* p_file_Lexeme, File* p_file_Text, File* p_file_Bss, File* p_file_Data, File* p_file_Symb){

	/*if (*p_file_Lexeme != NULL)	*p_file_Lexeme = (*p_file_Lexeme)->suiv;*/ /*Si la file de lexeme n'est pas cide, on va chercher le premier lexeme de la file*/


	STATE_GRAMM S = S_GRAMM_INIT;		/*Etat initial de l'automate*/
	LEXEME lexeme_courant = calloc(1,sizeof(*lexeme_courant));

	while (*p_file_Lexeme != NULL){ /*Tant que la file de lexeme n'est pas vide*/

		/*get_current_Lexeme(&((*p_file_Lexeme)->suiv), &lexeme_courant);*/
	get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
		
		switch (S){

			case S_GRAMM_INIT :
				analyse_gramm1(p_file_Lexeme, &S, lexeme_courant); /*Cas speciaux .bss .text .data & commentaire*/
				break;

			case S_GRAMM_DATA:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/
					ajout_maillon_data(p_file_Data, p_file_Lexeme, lexeme_courant, p_file_Symb);
				}
				break;

			case S_GRAMM_TEXT:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/
					ajout_maillon_text(p_file_Text, p_file_Lexeme, lexeme_courant, p_file_Symb);
				}
				break;

			case S_GRAMM_BSS:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/
					ajout_maillon_bss(p_file_Bss, p_file_Lexeme, lexeme_courant, p_file_Symb);
				}
				break;

			default:
				WARNING_MSG("Bien joue, vous etes rentre dans la matrice");
				break;
		}


	}

	free(lexeme_courant);
}

/*@param p_file_Lexeme pointeur sur la file de Lexeme a traiter
 *@param S Etat de la machine a etat, passe par adresse
 *@return Retourne 1 si des lexemes ont etes defiles, 0 sinon
 *@brief Permet de verifier si le lexeme courant est une instruction :
	.set noreorder et la defile
	.bss et associe l'etat S_GRAMM_BSS
	.data et associe l'etat S_GRAMM_DATA
	.text et associe l'etat S_GRAMM_TEXT
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

/*@param p_file_Lexeme pointeur sur la file de Lexeme a traiter
 *@param p_lexeme_courant pointeur sur le Lexeme_courant que l'on modifie
 *@return Ne retourne rien car le Lexeme_courrant est passe par adresse
 *@brief Permet de recuperer les informations du Lexeme_courrant (et facilite donc l'ecriture des fonctions)
 */

void get_current_Lexeme(File* p_file_Lexeme, LEXEME* p_lexeme_courant){
	if((*p_file_Lexeme) == NULL) *p_lexeme_courant=NULL;
	
	else{
		(*p_lexeme_courant)->cat = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> cat; /* LEXEME*?  On va chercher la categorie du Lexeme courant*/
		(*p_lexeme_courant)->line_nb = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> line_nb;
		(*p_lexeme_courant)->chain = ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> chain;
	}
}

/*@param p_file_Data pointeur sur la file Data a completer
 *@param p_file_Lexeme pointeur sur la file de LEXEMEs
 *@param lexeme_courant Lexeme temporaire qui facilite l'acces au donnees du lexeme courant
 *@return Retourne 0 si tout ce passe correctement, 1 s'il y a une erreur
 *@brief Permet de creer un maillon data en lisant la file de Lexeme et d'ajouter ce dernier a la file Data
 */

int ajout_maillon_data(File* p_file_Data, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb){
	/*Si le lexeme est une etiquette, on doit creer un maillon symb et l'ajouter la la collection de symboles*/
	if(lexeme_courant->cat==ETIQUETTE){
		SYMB new_symb = calloc(1,sizeof(*new_symb));

		new_symb->nom = lexeme_courant->chain;
		new_symb->line_nb = lexeme_courant->line_nb;
		new_symb->zone = ZONE_DATA;
		if(*p_file_Data==NULL) new_symb->decalage = 0;
		else new_symb->decalage = ( (DATA)((*p_file_Data)->val) )-> decalage;

		*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/
		defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);/*On defile le symbole de la file de lexemes*/
	}
	/*Puis on continue a ajouter le reste de la ligne (ou la ligne suivante?) a la collection text*/ 

	DATA new_maillon = calloc(1,sizeof(*new_maillon));
	new_maillon->line_nb = lexeme_courant->line_nb;

	if(lexeme_courant->cat!=DIRECTIVE){	/*Si le lexeme n'est pas une directive (une eventuelle etiquette a deja ete enlevee)*/
		WARNING_MSG("Erreur ligne %lf, .data peut contienir que des directives\n", lexeme_courant->line_nb);
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
			new_operande->chain = strdup(lexeme_courant->chain);
			switch(lexeme_courant->cat){
				/* TODO : Ajouter la gestion des nobmres entre parentheses : base_offset*/

				case CHAINE:
					new_operande->type = OPER_CHAINE;
					break;


				case DECIMAL:
					new_operande->type = OPER_DECIMAL;
					break;


				case HEXA:
					new_operande->type = OPER_HEXA;
					break;


				case OCTAL:
					new_operande->type = OPER_OCTAL;
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
	
	calcul_decalage_Data(p_file_Data, &new_maillon);


	/*Ajout du nouveau maillon a la file de Data*/
	*p_file_Data = enfiler(new_maillon, *p_file_Data);
	return(0);
}

/*@param p_file_Bss pointeur sur la file Bss a completer
 *@param p_file_Lexeme pointeur sur la file de LEXEMEs
 *@param lexeme_courant Lexeme temporaire qui facilite l'acces au donnees du lexeme courant
 *@return Retourne 0 si tout ce passe correctement, 1 s'il y a une erreur
 *@brief Permet de creer un maillon Bss en lisant la file de Lexeme et d'ajouter ce dernier a la file Data
 */

int ajout_maillon_bss(File* p_file_Bss, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb){
	/*Si le lexeme est une etiquette, on doit creer un maillon symb et l'ajouter la la collection de symboles*/
	if(lexeme_courant->cat==ETIQUETTE){
		SYMB new_symb = calloc(1,sizeof(*new_symb));

		new_symb->nom = lexeme_courant->chain;
		new_symb->line_nb = lexeme_courant->line_nb;
		new_symb->zone = ZONE_TEXT;
		if(*p_file_Bss==NULL) new_symb->decalage = 0;
		else new_symb->decalage = ( (BSS)((*p_file_Bss)->suiv->val) )-> decalage;

		*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/
		defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);/*On defile le symbole de la file de lexemes*/
	}
	/*Puis on continue a ajouter le reste de la ligne (ou la ligne suivante?) a la collection text*/ 

	BSS new_maillon = calloc(1,sizeof(*new_maillon));
	new_maillon->line_nb = lexeme_courant->line_nb;

	if(lexeme_courant->cat!=DIRECTIVE){	/*Si le lexeme n'est pas une directive*/
		WARNING_MSG("Erreur ligne %lf, .bss peut contienir que des directives\n", lexeme_courant->line_nb);
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
			new_operande->chain = strdup(lexeme_courant->chain);
			switch(lexeme_courant->cat){
				/* TODO : Ajouter la gestion des nobmres entre parentheses : base_offset*/

				case CHAINE:
					new_operande->type = OPER_CHAINE;
					break;


				case DECIMAL:
					new_operande->type = OPER_DECIMAL;
					break;


				case HEXA:
					new_operande->type = OPER_HEXA;
					break;


				case OCTAL:
					new_operande->type = OPER_OCTAL;
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
	
	calcul_decalage_Bss(p_file_Bss, &new_maillon);

	/*Ajout du nouveau maillon a la file de Bss*/
	*p_file_Bss = enfiler(new_maillon, *p_file_Bss);
	return(0);
}

/*@param p_file_Text pointeur sur la file Text a completer
 *@param p_file_Lexeme pointeur sur la file de LEXEMEs
 *@param lexeme_courant Lexeme temporaire qui facilite l'acces au donnees du lexeme courant
 *@return Retourne 0 si tout ce passe correctement, 1 s'il y a une erreur
 *@brief Permet de creer un maillon Text en lisant la file de Lexeme et d'ajouter ce dernier a la file Data
 */

int ajout_maillon_text(File* p_file_Text, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb){
	/*Si le lexeme est une etiquette, on doit creer un maillon symb et l'ajouter la la collection de symboles*/
	if(lexeme_courant->cat==ETIQUETTE){
		SYMB new_symb = calloc(1,sizeof(*new_symb));

		new_symb->nom = lexeme_courant->chain;
		new_symb->line_nb = lexeme_courant->line_nb;
		new_symb->zone = ZONE_TEXT;
		if(*p_file_Text==NULL) new_symb->decalage = 0;
		else new_symb->decalage = ( (TEXT)((*p_file_Text)->suiv->val) )-> decalage;

		*p_file_Symb = enfiler(new_symb, *p_file_Symb); /*On enfile ce nouveau maillon a la file de symboles*/
		defiler(p_file_Lexeme); get_current_Lexeme(p_file_Lexeme, &lexeme_courant);/*On defile le symbole de la file de lexemes*/
	}
	/*Puis on continue a ajouter le reste de la ligne (ou la ligne suivante?) a la collection text*/ 

	/*TODO: gerer le pb de definition d'une etiquette au milieu d'une instruction*/
	
	/*On creer un maillon Text */
	TEXT new_maillon = calloc(1,sizeof(*new_maillon));
	new_maillon->line_nb = lexeme_courant->line_nb;

	if(lexeme_courant->cat!=SYMBOLE){	/*Si le lexeme n'est pas une chaine*/
		WARNING_MSG("Erreur ligne %lf, \"%s\" n'est pas une instruction\n", lexeme_courant->line_nb, lexeme_courant->chain);
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
			new_operande->chain = strdup(lexeme_courant->chain);
			switch(lexeme_courant->cat){
				/* TODO : Ajouter la gestion des nobmres entre parentheses : base_offset*/

				case CHAINE:
					new_operande->type = OPER_CHAINE;
					break;


				case DECIMAL:
					new_operande->type = OPER_DECIMAL;
					break;


				case HEXA:
					new_operande->type = OPER_HEXA;
					break;


				case OCTAL:
					new_operande->type = OPER_OCTAL;
					break;


				case SYMBOLE:
					new_operande->type = OPER_SYMBOLE;
					break;

				case REGISTRE:
					new_operande->type = OPER_REG;
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
	/*
	(OPERANDE)((new_maillon->l_operande)->val)
	*/
	
	if(*p_file_Text==NULL) new_maillon->decalage = 0; /*Si c'est la premiere instruction*/
	else new_maillon->decalage = ( (TEXT)((*p_file_Text)->val) )-> decalage + 4; /*Toutes les instructions decallent de 4*/

	/*Ajout du nouveau maillon a la file de Text*/
	*p_file_Text = enfiler(new_maillon, *p_file_Text);
	return(0);
}

/*@param p_file_Data Pointeur sur la file Data (pour connaitre la val precedente de decalage)
 *@param p_new_maillon pointeur sur le nouveau maillon (en cours de creation) pour acceder a ses infos et y ecrire le decalage
 *@return Ne retourne rien mais ecrit la valeure du decalage dans le nouveau maillon
 *@brief Fonction a appeler la de la creation d'un maillon data pour calculer (et ecrire) la valeur du decalage
 */

void calcul_decalage_Data(File* p_file_Data, DATA* p_new_maillon){
	double new_decalage, decalage_precedent;
	if(*p_file_Data == NULL){ /*Si la file est vide, c'est le premier element, on retourne 0*/
		new_decalage = 0;
	}

	/*SINON*/
	else{
		decalage_precedent = ( (DATA)((*p_file_Data)->val) )-> decalage;
		new_decalage = decalage_precedent;
	}

	int cmpt_operande;

	OPERANDE op_temp = calloc(1, sizeof(*op_temp));
	Liste p_operande_courante = (*p_new_maillon)->l_operande;

	for (cmpt_operande = 1; cmpt_operande <= (*p_new_maillon)->nb_op; cmpt_operande++){
		
		op_temp->chain = (((OPERANDE)((p_operande_courante)->val))->chain);
		op_temp->type = (((OPERANDE)((p_operande_courante)->val))->type);

		if(strcmp( (*p_new_maillon)->operateur , ".byte")==0){
			new_decalage += 1;
		}

		else if(strcmp( (*p_new_maillon)->operateur , ".space")==0){
			/*TODO : gerer les nombre hexa et octal (notamment atoll)*/
			if (op_temp->type==OPER_DECIMAL){ /*Si l'opperande est bien un nombre*/
				new_decalage += (double) atol(op_temp->chain);
			}
			else{
				(*p_new_maillon)->type = DATA_ERROR;
				WARNING_MSG("Erreur ligne %lf, l'operande %d n'est pas un nombre\n", (*p_new_maillon)->line_nb, cmpt_operande);
			}
		}

		else if(strcmp( (*p_new_maillon)->operateur , ".asciiz")==0){

			if (op_temp->type==OPER_CHAINE ){ /*Si l'opperande est bien une chaine*/
				new_decalage += strlen(op_temp->chain);
			}
			else{
				(*p_new_maillon)->type = DATA_ERROR;
				WARNING_MSG("Erreur ligne %lf, l'operande %d n'est pas une chaine de caractere\n", (*p_new_maillon)->line_nb, cmpt_operande);
			}
		}

		else if(strcmp( (*p_new_maillon)->operateur , ".word")==0){

			if((int)new_decalage%4 == 0){
				new_decalage += 4; 
			}
			/*On aligne les .word modulo 4*/
			else{
				new_decalage += new_decalage + 4 + (int)new_decalage%4; 	
			}
		}

		else{
			WARNING_MSG("Operateur : \"%s\" non reconnu dans un .data\n", (*p_new_maillon)->operateur);
			break;
		}

		/*On passe a l'operande suivante*/
		p_operande_courante = p_operande_courante->suiv;
	}

	(*p_new_maillon)->decalage = new_decalage;
	free(op_temp);

}

/*@param p_file_Data Pointeur sur la file Data (pour connaitre la val precedente de decalage)
 *@param p_new_maillon pointeur sur le nouveau maillon (en cours de creation) pour acceder a ses infos et y ecrire le decalage
 *@return Ne retourne rien mais ecrit la valeure du decalage dans le nouveau maillon
 *@brief Fonction a appeler la de la creation d'un maillon data pour calculer (et ecrire) la valeur du decalage
 */

void calcul_decalage_Bss(File* p_file_Bss, BSS* p_new_maillon){
	double new_decalage, decalage_precedent;
	if(*p_file_Bss == NULL){ /*Si la file est vide, c'est le premier element, on retourne 0*/
		new_decalage = 0;
	}

	/*SINON*/
	else{
		decalage_precedent = ( (BSS)((*p_file_Bss)->val) )-> decalage;
		new_decalage = decalage_precedent;
	}
	int cmpt_operande=1;

	OPERANDE op_temp = calloc(1, sizeof(*op_temp));
	Liste p_operande_courante = (*p_new_maillon)->l_operande;

	while(p_operande_courante!=NULL){
		
		op_temp->chain = (((OPERANDE)((p_operande_courante)->val))->chain);
		op_temp->type = (((OPERANDE)((p_operande_courante)->val))->type);

		if(strcmp((*p_new_maillon)->operateur, ".space")==0){
			/*TODO : gerer les nombre hexa et octal (notamment atol)*/

			if (op_temp->type==OPER_DECIMAL){ /*Si l'opperande est bien un nombre*/
				new_decalage += (double) atol(op_temp->chain);
			}
			else{
				(*p_new_maillon)->type = DATA_ERROR;
				WARNING_MSG("Erreur ligne %lf, l'operande %d n'est pas un nombre", (*p_new_maillon)->line_nb, cmpt_operande);
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

	(*p_new_maillon)->decalage = new_decalage;
	free(op_temp);
}