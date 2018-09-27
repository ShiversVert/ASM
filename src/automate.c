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
 * @param i est le numéro de la ligne courante
 * @return 
 * @brief 
 *
 */

File automate(File f, char* token, int i){

	int j = 0;
	char c = token[j];		/*Caractere courant*/
	STATE S=S_INIT ;  	/*Current state*/

	if (f == NULL) f = creer_file();

	while(token[j] != '\0'){ 		/*TAnt qu'on est pas à la fin du token*/
		switch(S){
			case S_INIT :

				if (c=='0') 			S=S_DECIMAL_ZERO;
				else if(isdigit(c))     S=S_DECIMAL;
				else if(c==',') 		S=S_VIRGULE ;
				else if(c=='#') 		S=S_COMMENTAIRE;								
				else if(isalpha(c))		S=S_SYMBOLE; 									/*isalpha est connu*/
				else if(c=='.') 		S=S_DIRECTIVE;
				else if(c=='$') 		S=S_REGISTRE;
				else if(c=='('||c==')')	S=S_PARENTHESE;
				else if(c=='"') 		S=S_DEBSTR;
				/*else return erreur_caractere(token,i,c);*/ 							/*i est le numéro de la ligne*/
				break;

			case S_DECIMAL_ZERO:/*reperage du  prefixe de l’hexa*/
				if( c == 'x'  ||  c == 'X' ) S=S_HEXA;
				else if( isdigit(c) && c<8) S=S_OCTAL; /*c’est un octal*/
				/*else  return erreur_caractere (token,i,c) ;*/
				break;

			case S_DEBUT_HEXA: /*il faut au moins un chiffre apres x*/
				if (isxdigit(c))  S=S_HEXA;
				/*else  return erreur_caractere (token,i,c) ;*/
				break;

			case S_HEXA: /*tant que c'est un chiffre hexa*/
				if(isxdigit( c ))  S=S_HEXA;
				/*else  return erreur_caractere (token,i,c) ;*/
				break;

			case S_DECIMAL:/*tant que c'est un chiffre*/
				if(isdigit( c ))   S=S_DECIMAL;
				/*else return erreur_caractere (token,i,c) ;*/
				break;

			case S_OCTAL: /*tant que c'est un chiffre*/
				if (isdigit (c)&& c<8 ) S=S_OCTAL;
				/*else return erreur_caractere (token,i,c) ;*/
				break;

			case S_COMMENTAIRE:
				if (c==EOF || c=='\n')   /*stockage et */S=S_INIT ;
				else /*stockage et */ S=S_COMMENTAIRE;
				break;

			case S_SYMBOLE:
				if (isalpha(c)) S=S_SYMBOLE;  /*+stockage+étiquette */
				else if (c==':') S=S_ETIQUETTE;
				/*else  return erreur_caractere (token,i,c) ;*/
				break;

			case S_REGISTRE:
				if (isalpha(c) || isdigit(c)) S=S_REGISTRE;
				/*else  return erreur_caractere (token,i,c) ;*/
				break;

			case S_DEBSTR: 
				if (isalpha(c) || isdigit(c)) S=S_DEBSTR; /*+mémoire seulement alphabet et digit? */
				else if(isalpha(c))  S=S_CHAINE; /*fin token??*/
				/*else  return erreur_caractere (token,i,c) ;*/
				break;

			case S_DIRECTIVE:
				if (isalpha(c)) S=S_DIRECTIVE;
				/*else  return erreur_caractere (token,i,c) ;*/
				break;

			case S_PARENTHESE: /*idem que virgule??*/
				if( isdigit( c )) S=S_PARENTHESE;
				/*else  return erreur_caractere (token,i,c) ;*/
				break;

			case S_VIRGULE:
				/*return erreur_caractere (token,i,c) ;*/ /*si on rentre dans comma ça veut dire qu'il y a utre chose que virgule dans le token, erreur*/
				break;

			case S_ETIQUETTE:
				/*return erreur_caractere (token,i,c) ;*/
				break;
			case S_CHAINE: /*si on rentre dans str ça veut dire qu'il y a autre chose après" exemple "ertre"e, erreur*/
				/*return erreur_caractere (token,i,c) ;*/
				break;
			default:
				return(f);

			j++;

		}
	}

		f = enfiler((void*)token, f);
	return f;
}



