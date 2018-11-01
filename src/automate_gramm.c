#include <automate_gramm.h>

/*@param p_file_Lexeme pointeur sur la file de Lexeme a traiter
 *@param p_file_Text pointeur sur la file d'element .Text a remplir
 *@param p_file_Bss pointeur sur la file d'element .Bss a remplir
 *@param p_file_Data pointeur sur la file d'element .Data a remplir
 *@return retourne la nouvelle file
 *@brief a utiliser de la maniere suivante : f = enfiler(p, f);
 */

void automate_grammatical(File* p_file_Lexeme, File* p_file_Text, File* p_file_Bss, File* p_file_Data){

	if (*p_file_Lexeme != NULL)	*p_file_Lexeme = (*p_file_Lexeme)->suiv; /*Si la file de lexeme n'est pas cide, on va chercher le premier lexeme de la file*/


	STATE S = S_INIT;		/*Etat initial de l'automate*/
	LEXEME lexeme_courant = calloc(1,sizeof(*lexeme_courant));

	while (*p_file_Lexeme != NULL){ /*Tant que la file de lexeme n'est pas vide*/

		get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
		
		switch (S){

			case S_INIT :
				analyse_gramm1(p_file_Lexeme, &S, lexeme_courant); /*Cas speciaux .bss .text .data & commentaire*/
				break;

			case S_DATA:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/
					ajout_maillon_data(p_file_Data, p_file_Lexeme, lexeme_courant);
				}
				break;

			case S_TEXT:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/

				}
				break;

			case S_BSS:
				if(analyse_gramm1(p_file_Lexeme, &S, lexeme_courant) != 1){ /*Cas speciaux .bss .text .data & commentaire*/

				}
				break;

			default:
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
	.bss et associe l'etat S_BSS
	.data et associe l'etat S_DATA
	.text et associe l'etat S_TEXT
 */

int analyse_gramm1(File* p_file_Lexeme, STATE* S, LEXEME lexeme_courant){

	if (lexeme_courant->cat == DIRECTIVE){
		if ( strcmp( lexeme_courant->chain , ".data") == 0 ){
			*S = S_DATA;
			defiler(p_file_Lexeme);
			return(1);
		}

		else if ( strcmp( lexeme_courant->chain , ".bss") == 0 ){
			*S = S_BSS;
			defiler(p_file_Lexeme);
			return(1);
		}

		else if( strcmp( lexeme_courant->chain , ".text") == 0 ){
			*S = S_DATA;
			defiler(p_file_Lexeme);
			return(1);
		}

		else if (strcmp( lexeme_courant->chain , ".set" ) == 0 && strcmp(  ( (LEXEME)((*p_file_Lexeme)->suiv->val) )-> chain , "noreorder" ) == 0){
			defiler(p_file_Lexeme); /*On defile les LEXEMEs .set & noreorder car on ne les considere pas*/
			defiler(p_file_Lexeme);
			return(1);
		}
	}

	else if (lexeme_courant->cat == COMMENTAIRE){
		defiler(p_file_Lexeme);
		return(1);
	}

	*S = S_TEXT; /*Par defaut, on est dans une zone de texte */
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
		(*p_lexeme_courant)->cat = ( (LEXEME)((*p_file_Lexeme)->val) )-> cat; /* LEXEME*?  On va chercher la categorie du Lexeme courant*/
		(*p_lexeme_courant)->line_nb = ( (LEXEME)((*p_file_Lexeme)->val) )-> line_nb;
		(*p_lexeme_courant)->chain = ( (LEXEME)((*p_file_Lexeme)->val) )-> chain;
	}
}

/*@param p_file_Data pointeur sur la file Data a completer
 *@param p_file_Lexeme pointeur sur la file de LEXEMEs
 *@param lexeme_courant Lexeme temporaire qui facilite l'acces au donnees du lexeme courant
 *@return Retourne 0 si tout ce passe correctement, 1 s'il y a une erreur
 *@brief Permet de creer un maillon data en lisant la file de Lexeme et d'ajouter ce dernier a la file Data
 */
int ajout_maillon_data(File* p_file_Data, File* p_file_Lexeme, LEXEME lexeme_courant){
	DATA new_maillon = calloc(1,sizeof(*new_maillon));
	new_maillon->line_nb = lexeme_courant->line_nb;

	if(lexeme_courant->cat!=DIRECTIVE){	/*Si le lexeme n'est pas une directive*/
		WARNING_MSG("Erreur ligne %lf, .data peut contienir que des directives", lexeme_courant->line_nb);
		new_maillon->type = DATA_ERROR;

		do{ /*On saute la ligne du fichier*/
			defiler(p_file_Lexeme);
			get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
		}while(new_maillon->line_nb == lexeme_courant->line_nb);
		
		*p_file_Data = enfiler(&new_maillon, *p_file_Data);
		return(1);
	}

	/*SINON*/

	new_maillon->operateur = lexeme_courant->chain; /*On recuppere l'operateur*/
	new_maillon->type = DATA_DIR; /*On ne peut avoir que des directives dans .data*/
	defiler(p_file_Lexeme);
	int nb_op = 0;
	Liste liste_operande = creer_liste();
	
	while(lexeme_courant->line_nb == new_maillon->line_nb){ /*tant qu'on est sur la meme ligne*/
		get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
		
		if(lexeme_courant->cat == VIRGULE || lexeme_courant->cat == COMMENTAIRE) defiler(p_file_Lexeme);
		
		else {
			OPERANDE new_operande = calloc(1,sizeof(new_operande));
			new_operande->chain = lexeme_courant->chain;
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
			ajout_queue((void*)&new_operande, liste_operande);
		}

	}

	new_maillon->nb_op = nb_op;
	new_maillon->l_operande=liste_operande;
	
	calcul_decalage_Data(p_file_Data, &new_maillon);


	/*Ajout du nouveau maillon a la file de Data*/
	*p_file_Data = enfiler(&new_maillon, *p_file_Data);
	return(0);
}

/*@param p_file_Bss pointeur sur la file Bss a completer
 *@param p_file_Lexeme pointeur sur la file de LEXEMEs
 *@param lexeme_courant Lexeme temporaire qui facilite l'acces au donnees du lexeme courant
 *@return Retourne 0 si tout ce passe correctement, 1 s'il y a une erreur
 *@brief Permet de creer un maillon Bss en lisant la file de Lexeme et d'ajouter ce dernier a la file Data
 */

int ajout_maillon_bss(File* p_file_Bss, File* p_file_Lexeme, LEXEME lexeme_courant){
	BSS new_maillon = calloc(1,sizeof(*new_maillon));
	new_maillon->line_nb = lexeme_courant->line_nb;

	if(lexeme_courant->cat!=DIRECTIVE){	/*Si le lexeme n'est pas une directive*/
		WARNING_MSG("Erreur ligne %lf, .bss peut contienir que des directives", lexeme_courant->line_nb);
		new_maillon->type = BSS_ERROR;

		do{ /*On saute la ligne du fichier*/
			defiler(p_file_Lexeme);
			get_current_Lexeme(p_file_Lexeme, &lexeme_courant);
		}while(new_maillon->line_nb == lexeme_courant->line_nb);
		
		*p_file_Bss = enfiler(&new_maillon, *p_file_Bss);
		return(1);
	}

	/*SINON*/

	new_maillon->operateur = lexeme_courant->chain; /*On recuppere l'operateur*/
	new_maillon->type = BSS_DIR; /*On ne peut avoir que des directives dans .bss*/
	defiler(p_file_Lexeme);
	int nb_op = 0;
	Liste liste_operande = creer_liste();
	
	while(lexeme_courant->line_nb == new_maillon->line_nb){ /*tant qu'on est sur la meme ligne*/
		get_current_Lexeme(p_file_Lexeme, &lexeme_courant);

		/*Si le lexeme est un commentaire ou une virgule, on ne le considere pas*/
		if(lexeme_courant->cat == VIRGULE || lexeme_courant->cat == COMMENTAIRE) defiler(p_file_Lexeme);
		
		else {
			OPERANDE new_operande = calloc(1,sizeof(new_operande));
			new_operande->chain = lexeme_courant->chain;
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
			ajout_queue((void*)&new_operande, liste_operande);
		}

	}

	new_maillon->nb_op = nb_op;
	new_maillon->l_operande=liste_operande;
	
	calcul_decalage_Bss(p_file_Bss, &new_maillon);

	/*Ajout du nouveau maillon a la file de Data*/
	*p_file_Bss = enfiler(&new_maillon, *p_file_Bss);
	return(0);
}

int ajout_maillon_Text(File* p_file_Text, File* p_file_Lexeme, LEXEME lexeme_courant){
	return 0;
}

/*@param p_file_Data Pointeur sur la file Data (pour connaitre la val precedente de decalage)
 *@param p_new_maillon pointeur sur le nouveau maillon (en cours de creation) pour acceder a ses infos et y ecrire le decalage
 *@return Ne retourne rien mais ecrit la valeure du decalage dans le nouveau maillon
 *@brief Fonction a appeler la de la creation d'un maillon data pour calculer (et ecrire) la valeur du decalage
 */

void calcul_decalage_Data(File* p_file_Data, DATA* p_new_maillon){
	if(*p_file_Data == NULL){ /*Si la file est vide, c'est le premier element, on retourne 0*/
		(*p_new_maillon)->decalage = 0;
	}

	/*SINON*/

	double decalage_precedent = ( (DATA)((*p_file_Data)->val) )-> decalage;
	double new_decalage = decalage_precedent;
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
				WARNING_MSG("Erreur ligne %lf, l'operande %d n'est pas un nombre", (*p_new_maillon)->line_nb, cmpt_operande);
			}
		}

		else if(strcmp( (*p_new_maillon)->operateur , ".asciiz")==0){

			if (op_temp->type==OPER_CHAINE){ /*Si l'opperande est bien une chaine*/
				new_decalage += strlen(op_temp->chain);
			}
			else{
				(*p_new_maillon)->type = DATA_ERROR;
				WARNING_MSG("Erreur ligne %lf, l'operande %d n'est pas une chaine de cractere", (*p_new_maillon)->line_nb, cmpt_operande);
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
			WARNING_MSG("Operateur : \"%s\" non reconnu dans un .data", (*p_new_maillon)->operateur);
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
	if(*p_file_Bss == NULL){ /*Si la file est vide, c'est le premier element, on retourne 0*/
		(*p_new_maillon)->decalage = 0;
	}

	/*SINON*/

	double decalage_precedent = ( (BSS)((*p_file_Bss)->val) )-> decalage;
	double new_decalage = decalage_precedent;
	int cmpt_operande;

	OPERANDE op_temp = calloc(1, sizeof(*op_temp));
	Liste p_operande_courante = (*p_new_maillon)->l_operande;

	for (cmpt_operande = 1; cmpt_operande <= (*p_new_maillon)->nb_op; cmpt_operande++){
		
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
	}

	(*p_new_maillon)->decalage = new_decalage;
	free(op_temp);
}