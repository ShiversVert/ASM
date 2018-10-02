/**
 * @file types.h
 * @author
 * @brief Types definition
 *
 * Contains the definition of the differents types used in the program
 */

#ifndef _TYPES_H_
#define _TYPES_H_

typedef enum {ETIQUETTE, COMMENTAIRE, DIRECTIVE, REGISTRE, VIRGULE, CHAINE, PARENTHESE_G, PARENTHESE_D, SAUT_LIGNE, DECIMAL, HEXA, OCTAL, SYMBOLE, ERROR} type_lexeme;
/*Lexeme*/

typedef struct _lexeme_ {
		type_lexeme cat; 		/*category*/
		double line_nb;			/*Line number*/
		char* chain;			/*Exact chain*/
	}*LEXEME ;



#endif /* _TYPES_H_ */
