#include <fct_affichage.h>

/**
 * @param maillon est le maillon de LEXEME a afficher
 * @return ne retourne rien mais affiche les informations du maillon passe en argument
 * @brief sert a afficher un maillon de LEXEME
 *
 */

void afficher_maillon_LEXEME(void* maillon){
	int line_nb = ( (LEXEME)(maillon))->line_nb;
	char* chain = ( (LEXEME)(maillon))->chain;
	char* category;
	switch (((LEXEME)(maillon))->cat) {
		case ETIQUETTE:
			category = "ETIQUETTE";
			break;
		case COMMENTAIRE:
			category = "COMMENTAIRE";
			break;
		case DIRECTIVE:
			category = "DIRECTIVE";
			break;
		case REGISTRE:
			category = "REGISTRE";
			break;
		case VIRGULE:
			category = "VIRGULE";
			break;
		case CHAINE:
			category = "CHAINE\t";
			break;
		case PARENTHESE_G:
			category = "PARENTHESE_G";
			break;
		case PARENTHESE_D:
			category = "PARENTHESE_D";
			break;
		case SAUT_LIGNE:
			category = "SAUT_LIGNE";
			break;
		case DECIMAL:
			category = "DECIMAL";
			break;
		case HEXA:
			category = "HEXA\t";
			break;
		case OCTAL:
			category = "OCTAL\t";
			break;
		case SYMBOLE:
			category = "SYMBOLE";
			break;
		case ERROR:
			category = "ERROR\t";
			WARNING_MSG("%s \t\tline : %d\t\t%s\n", category, line_nb, chain);
			break;
	}

	if(((LEXEME)(maillon))->cat != ERROR) {
		printf("%s \t\t", category);
		printf("%d\t\t", line_nb);
		printf("%s\n", chain);
	}

}

/**
 * @param maillon est le maillon de DATA a afficher
 * @return ne retourne rien mais affiche les informations du maillon passe en argument
 * @brief sert a afficher un maillon de DATA
 *
 */

void afficher_maillon_DATA(void* maillon){
	double line_nb = ( (DATA)(maillon))->line_nb;
	double decalage = ( (DATA)(maillon))->decalage;
	char* operateur = ( (DATA)(maillon))->operateur;
	int nb_op = ( (DATA)(maillon))->nb_op;
	char* type;
	switch (((DATA)(maillon))->type) {
		case DATA_DIR:
			type = "DATA_DIR";
			break;
		case DATA_ERROR:
			type = "DATA_ERROR";
			WARNING_MSG("%s \t\t%.0lf\t\t%s\n", type, line_nb, operateur);
			break;
	}

	if(((DATA)(maillon))->type != DATA_ERROR) {
		printf("%s\t\t", type);
		printf("%s\t\t", operateur);
		printf("line : %.0lf\t\t", line_nb);
		printf("dec : %.0lf\t\t", decalage);
		printf("nb_op : %d\t\t", nb_op);
		afficher_liste(( (DATA)(maillon))->l_operande , (*afficher_maillon_OPERANDE));
		printf("\n");
	}

}

/**
 * @param maillon est le maillon de BSS a afficher
 * @return ne retourne rien mais affiche les informations du maillon passe en argument
 * @brief sert a afficher un maillon de BSS
 *
 */

void afficher_maillon_BSS(void* maillon){
	double line_nb = ( (BSS)(maillon))->line_nb;
	double decalage = ( (BSS)(maillon))->decalage;
	char* operateur = ( (BSS)(maillon))->operateur;
	int nb_op = ( (BSS)(maillon))->nb_op;
	char* type;
	switch (((BSS)(maillon))->type) {
		case BSS_DIR:
			type = "BSS_DIR";
			break;
		case BSS_ERROR:
			type = "BSS_ERROR";
			WARNING_MSG("%s \t\tline : %.0lf\t\t%s\n", type, line_nb, operateur);
			break;
	}

	if(((BSS)(maillon))->type != BSS_ERROR) {
		printf("%s\t\t", type);
		printf("%s\t\t", operateur);
		printf("line : %.0lf\t\t", line_nb);
		printf("dec : %.0lf\t\t", decalage);
		printf("nb_op : %d\t\t", nb_op);
		afficher_liste(( (BSS)(maillon))->l_operande , (*afficher_maillon_OPERANDE));
		printf("\n");
	}
}

/**
 * @param maillon est le maillon de TEXT a afficher
 * @return ne retourne rien mais affiche les informations du maillon passe en argument
 * @brief sert a afficher un maillon de TEXT
 *
 */

void afficher_maillon_TEXT(void* maillon){
	double line_nb = ( (TEXT)(maillon))->line_nb;
	double decalage = ( (TEXT)(maillon))->decalage;
	char* operateur = ( (TEXT)(maillon))->operateur;
	int nb_op = ( (TEXT)(maillon))->nb_op;
	char* type;
	switch (((TEXT)(maillon))->type) {
		case TEXT_INST:
			type = "TEXT_INST";
			break;

		case TEXT_ERROR:
			type = "TEXT_ERROR";
			WARNING_MSG("%s \t\tline : %.0lf\t\t%s\n", type, line_nb, operateur);
			break;
	}

	if(((TEXT)(maillon))->type != TEXT_ERROR) {
		printf("%s\t\t", type);
		printf("%s\t\t", operateur);
		printf("line : %.0lf\t\t", line_nb);
		printf("dec : %.0lf\t\t", decalage);
		printf("nb_op : %d\t\t", nb_op);
		afficher_liste( ( ((TEXT)(maillon))->l_operande) , (*afficher_maillon_OPERANDE));
		printf("\n");
	}
}

/**
 * @param maillon est le maillon de OPERANDE a afficher
 * @return ne retourne rien mais affiche les informations du maillon passe en argument
 * @brief sert a afficher un maillon de OPERANDE
 *
 */

void afficher_maillon_OPERANDE(void* maillon){
	char* chain = ((OPERANDE)(maillon))->chain;
	char* type;
	switch ( ((OPERANDE)(maillon))->type ) {
		case OPER_REG:
			type = "OPER_REG";
			break;
		case OPER_BASE:
			type = "OPER_BASE";
			break;
		case OPER_OFFSET:
			type = "OPER_OFFSET";
			break;
		case OPER_DECIMAL:
			type = "OPER_DECIMAL";
			break;
		case OPER_HEXA:
			type = "OPER_HEXA";
			break;
		case OPER_OCTAL:
			type = "OPER_OCTAL";
			break;
		case OPER_CHAINE:
			type = "OPER_CHAINE";
			break;
		case OPER_SYMBOLE:
			type = "OPER_SYMBOLE";
			break;
		case OPER_TARGET:
			type = "OPER_TARGET";
			break;
		case OPER_ERROR:
			type = "OPER_ERROR";
			printf("\n");
			WARNING_MSG("%s\t\t%s", type, chain);
			break;
	}

	if( ((OPERANDE)(maillon))->type != OPER_ERROR) {
		printf("\n\t%s\t\t", type);
		printf("%s", chain);
	}
}

void afficher_maillon_SYMB(void* maillon){
	char* nom = ((SYMB)(maillon))->nom;
	char* zone;
	double line_nb = ((SYMB)(maillon))->line_nb;
	double decalage = ((SYMB)(maillon))->decalage;
	switch (((SYMB)(maillon))->zone) {
		case ZONE_DATA:
			zone = "ZONE_DATA";
			break;
		case ZONE_TEXT:
			zone = "ZONE_TEXT";
			break;
		case ZONE_BSS:
			zone = "ZONE_BSS";
			break;
	}

	printf("%s\t", zone);
	printf("%s\t", nom);
	printf("line:%.0lf\t", line_nb);
	printf("decalage:%.0lf\n", decalage);

}

void afficher_maillon_DIC(void* maillon){
	char* operateur = ((DIC)(maillon))->chain;
	int nb_op = ((DIC)(maillon))->nb_op;
	char* type_op = ((DIC)(maillon))->type_op;
	int i;

	printf("%s\t%d\t",operateur, nb_op);
	for (i=0; i<nb_op; i++){
		printf("%c ", type_op[i]);
	}
	printf("\n");
}
