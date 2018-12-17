/**
 * @file lex.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief Automate-related stuff.
 *
 * Automate related definitions and prototypes
 */

#include <automate.h>

/**
 * @param f 		File de lexemes a remplir
 * @param token 	courant
 * @param line_nb 	est le numero de la ligne courante
 * @param cmpt_err 	est le compteur d'erreur, passe par adresse pour le modifier dans la boucle de l'automate
 * @return 			Retourne lae file f avec le nouveau LEXEME ajoute
 * @brief
 *
 */

File automate_lexical(File f, char* token, int line_nb, int* cmpt_err){

	int j = 0;
	STATE S=S_INIT ;  	/*Current state*/
	char c = token[0];
	if (f == NULL) f = creer_file();
	type_lexeme new_lexeme_cat = ERROR; /*On initialise le type du nouveau lexeme par ERROR */

	if (c=='0') 			S=S_ZERO;
	else if(isdigit(c))     S=S_DECIMAL;
	else if(c=='-')			S=S_DECIMAL;
	else if(c==',') 		S=S_VIRGULE ;
	else if(c=='#') 		S=S_COMMENTAIRE;
	else if(c=='.') 		S=S_DIRECTIVE;
	else if(c=='$') 		S=S_REGISTRE;
	else if(c=='(')			S=S_PARENTHESE_G;
	else if(c==')')			S=S_PARENTHESE_D;
	else if(c=='"') 		S=S_DOUBLEQUOTE;
	else if(isalpha(c))		S=S_SYMBOLE; 									/*isalpha est connu*/
	else 					S=S_ERROR;

	switch(S){
		case S_INIT :
			S = S_ERROR;
			break;

		case S_ZERO :
			if (token[j+1] == 'x' || token[j+1] == 'X'){
				S = S_HEXA;
				j++;
				j++;
			}

			while (token[j]!='\0'){				/*On parcours tout le reste du chiffre*/
				if (S == S_HEXA){
					if (!isxdigit(token[j])) S=S_ERROR;
				}

				else if (S!=S_ERROR){
					if (isdigit(token[j])){
						if (token[j] < '8') S = S_OCTAL;
						else S = S_DECIMAL;
					}

					else S = S_ERROR;
				}
				j++;
			}

			break;

		case S_DECIMAL:
			if (token[j+1]!='\0'){ /*Si le decimal ne contient pas que un chiffre*/
				j++;
				while (token[j]!='\0'){
					if (!isdigit(token[j])) S = S_ERROR; /*Si ce n'est pas un chiffre => Erreur*/
					j++;
				}
			}
			break;

		case S_SYMBOLE:
			while(token[j]!='\0') j++;
			if (token[j-1] == ':'){
				token[j-1] = '\0';
				S = S_ETIQUETTE;
			}
			break;

		case S_DOUBLEQUOTE:
			j++;
			while(token[j]!='\0' && token[j]!='"') j++;
			if (token[j] != '"') S = S_ERROR;
			break;

		case S_VIRGULE  :
			break;

		case S_COMMENTAIRE :
			break;

		case S_DIRECTIVE :
			break;

		case S_REGISTRE :
			break;

		case S_PARENTHESE_G :
			break;

		case S_PARENTHESE_D :
			break;



		default :
			S=S_ERROR;

	}

	switch(S){
		case S_ERROR:
			new_lexeme_cat = ERROR ;
			(*cmpt_err)++;
			break;

		case S_INIT:					/*N'est pas cense arriver => error*/
			new_lexeme_cat = ERROR;
			break;

		case S_ZERO:					/*Le token ne fait qu'un caractere : '0' =>*/
			new_lexeme_cat = DECIMAL;
			break;

		case S_PARENTHESE_G:
			new_lexeme_cat = PARENTHESE_G;
			break;

		case S_PARENTHESE_D:
			new_lexeme_cat = PARENTHESE_D;
			break;

		case S_ETIQUETTE:
			new_lexeme_cat = ETIQUETTE;
			break;

		case S_DIRECTIVE:
			new_lexeme_cat = DIRECTIVE;
			break;

		case S_SYMBOLE:
			new_lexeme_cat = SYMBOLE;
			break;

		case S_REGISTRE:
			new_lexeme_cat = REGISTRE;
			break;

		case S_COMMENTAIRE:
			new_lexeme_cat = COMMENTAIRE;
			break;

		case S_VIRGULE:
			new_lexeme_cat = VIRGULE;
			break;

		case S_HEXA:
			new_lexeme_cat = HEXA;
			break;

		case S_DECIMAL:
			new_lexeme_cat = DECIMAL;
			break;

		case S_OCTAL:
			new_lexeme_cat = OCTAL;
			break;

		case S_CHAINE:
			new_lexeme_cat = CHAINE;
			break;

		case S_DOUBLEQUOTE:
			remove_backslash(token);
			new_lexeme_cat = CHAINE;
			break;
	}


	LEXEME new_lexeme = calloc(1,sizeof(*new_lexeme));

	new_lexeme->cat = new_lexeme_cat;
	new_lexeme->line_nb = line_nb;
	new_lexeme->chain = token;

	f = enfiler(new_lexeme, f);
	return(f);
}

void remove_backslash(char* chaine){
	int size = strlen(chaine);
	int c, cmpt_backslash = 0;
	char chaine_tmp[size]; //strcpy(chaine_tmp, chaine);

	for(c = 0; c<size; c++){
		if(chaine[c] == '\\' && (chaine[c+1] == '"' || chaine[c+1] == '\\' || chaine[c+1] == 'n') ){ 
			cmpt_backslash ++;
			strcpy(chaine_tmp+c, chaine+c+1);
			strcpy(chaine+c, chaine_tmp+c);
			if(chaine[c] == 'n') chaine[c] = '\n';
		}	
	}
	chaine[size-1 - cmpt_backslash] = '\0';
	
}