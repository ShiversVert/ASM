/**
 * @file types.h
 * @author
 * @brief Types definition
 *
 * Contains the definition of the differents types used in the program
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#include <liste.h>
#include <file.h>

/*########## RELATIFS AUX LEXEMES ##########*/

typedef enum {
	ETIQUETTE,
	COMMENTAIRE,
	DIRECTIVE,
	REGISTRE,
	VIRGULE,
	CHAINE,
	PARENTHESE_G,
	PARENTHESE_D,
	SAUT_LIGNE,
	DECIMAL,
	HEXA,
	OCTAL,
	SYMBOLE,
	ERROR
}type_lexeme;
/*Lexeme*/

typedef struct _lexeme_ {
	type_lexeme 	cat; 		/*category*/
	double 			line_nb;	/*Line number*/
	char* 			chain;		/*Exact chain*/
}*LEXEME ;

/*####### RELATIFS A .TEXT .DATA .BSS #######*/

typedef enum{
	S_GRAMM_INIT,
	S_GRAMM_DATA,
	S_GRAMM_BSS,
	S_GRAMM_TEXT,
} STATE_GRAMM;

typedef enum{
	OPER_REG,
	OPER_BASE,
	OPER_OFFSET,
	OPER_DECIMAL,
	OPER_HEXA,
	OPER_OCTAL,
	OPER_CHAINE,
	OPER_SYMBOLE,
	OPER_TARGET,
	OPER_ERROR,
	OPER_ERROR_UNK_REGISTER
}type_operande;

typedef struct _operande_{
	char*			chain;
	unsigned long	bin;
	type_operande 	type;
}*OPERANDE;


typedef enum{
	TEXT_INST,
	/*TEXT_SYMB,*/
	TEXT_ERROR
}type_text;

typedef struct _text_{
	char* 		operateur;
	type_text 	type;
	int 		nb_op;
	double 		line_nb;
	double 		decalage;
	Liste 		l_operande;
	int 		bin;
}*TEXT ;


typedef enum{
	BSS_DIR,
	BSS_ERROR
}type_bss;

typedef struct _bss_{
	char* 		operateur;
	type_bss 	type;
	int 		nb_op;
	double 		line_nb;
	double 		decalage;
	Liste 		l_operande;
}*BSS;


typedef enum{
	DATA_DIR,
	DATA_ERROR
}type_data;

typedef struct _data_{
	char* 		operateur;
	type_data 	type;
	int 		nb_op;
	double 		line_nb;
	double 		decalage;
	Liste 		l_operande;
}*DATA ;


typedef enum{
	ZONE_DATA,
	ZONE_TEXT,
	ZONE_BSS,
	ZONE_UNKW
}zone_symb;

typedef struct _symb_{
	char* 		nom;
	zone_symb   zone;
	double 		line_nb;
	double 		decalage;
}*SYMB;

typedef struct _dic_ {
  char*     chain;        /*Chaine de caractere de l'instruction*/
  int       nb_op;        /*Nombre d'operandes de l'instruction*/
  char      type_op[3];   /*Tableau de 3 char contenant le type de l'opperande i*/

  int      bin[6][2];   /*Tableau de 5 int contenant le nombre a coder et le nombre de bits*/
  /*
  Rd Rs Rt => registre
  O => offset
  B => base (On a donc O B pour un offset(base) )
  T => target
  I => valeur immediate
  S => shift amount
  */
}* DIC;

typedef enum{
	R_MIPS_32,
	R_MIPS_26,
	R_MIPS_HI16,
	R_MIPS_LO16,
	R_OFFSET
}type_realoc;

typedef struct _realoc_ {
	zone_symb	zone;		/*Zone (ou est situe l'operande) de la realoc*/
	double 		decalage;	/*Decalage de l'instruction ou la realoc doit etre faite*/
	type_realoc	type;		/*Type de realoc a faire*/
	OPERANDE*	p_op;			/*Pointeur vers l'operande a realouer... Utilitee?*/
	/*SYMB* 	symb;		Pointeur vers l'operande dans la table des symboles*/
} * REALOC;

#include <fct_affichage.h> /*Oblige de le mettre apres la definition du type file*/

#endif /* _TYPES_H_ */
