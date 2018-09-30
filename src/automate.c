/**
 * @file lex.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief Automate-related stuff.
 *
 * Automate related definitions and prototypes
 */

#include <automate.h>

/**
 * @param File de lexemes a remplir
 * @param token courant
 * @param line_nb est le numéro de la ligne courante
 * @return Retourne le file f avec le nouveau LEXEME ajoute
 * @brief 
 *
 */

File automate(File f, char* token, int line_nb){

	int j = 0;
	STATE S=S_INIT ;  	/*Current state*/
char c = token[0];
	if (f == NULL) f = creer_file();
	type_lexeme new_lexeme_cat = ERROR; /*On initialise le type du nouveau lexeme par ERROR */


	switch(S){
		case S_INIT :

			if (c=='0') 			S=S_ZERO;			
			else if(isdigit(c))     S=S_DECIMAL;
			else if(c=='-')			S=S_DECIMAL;
			else if(c==',') 		S=S_VIRGULE ;
			else if(c=='#') 		S=S_COMMENTAIRE;								
			else if(isalpha(c))		S=S_SYMBOLE; 									/*isalpha est connu*/
			else if(c=='.') 		S=S_DIRECTIVE;
			else if(c=='$') 		S=S_REGISTRE;
			else if(c=='('||c==')')	S=S_PARENTHESE;
			else if(c=='"') 		S=S_DEBSTR;
			else 					S = S_ERROR;
			break;

		case S_ZERO :
			if (token[j+1] == 'x' || token[j+1] == 'X'){
				S = S_HEXA;
				j++;
			}

			while (token[j]!='\0'){				/*On parcours tout le reste du chiffre*/
				if (S == S_HEXA){
					if (!ishexa(&token[j])) S=S_ERROR;
				}

				else{
					if (isdigit(token[j])){
						if (token[j]<8) S = S_OCTAL;
						else S = S_DECIMAL;
					}

					else S = S_ERROR;
				} 
				j++;
			}

			break;

		case S_DECIMAL:
			while (token[j]!='\0'){
				if (!isdigit(token[j])) S = S_ERROR; /*Si ce n'est pas un chiffre => Erreur*/
			}

			break;

		case S_SYMBOLE:
			while(token[j]!='\0') j++;
			if (token[j] == ':') S = S_ETIQUETTE;
			break;


		default : 
			S=S_ERROR;

	}

	switch(S){
		case S_ERROR:					
			new_lexeme_cat = ERROR ;
			break;

		case S_INIT:					/*N'est pas cense arriver => error*/
			new_lexeme_cat = ERROR;
			break;

		case S_ZERO:					/*Le token ne fait qu'un caractere : '0' =>*/
			new_lexeme_cat = DECIMAL;
			break;

		case S_PARENTHESE:
			new_lexeme_cat = PARENTHESE;
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

		case S_DEBSTR:
			new_lexeme_cat = SYMBOLE;
			break;
	} 


	LEXEME new_lexeme = calloc(1,sizeof(*new_lexeme));
	
	new_lexeme->cat = new_lexeme_cat;	
	new_lexeme->line_nb = line_nb;
	new_lexeme->chain = token;

	f = enfiler(new_lexeme, f);
	return(f);
}



