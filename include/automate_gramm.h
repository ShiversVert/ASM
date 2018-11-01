/**
 * @file automate_gramm.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief Grammatical Automate-related stuff.
 *
 * Grammatical Automate related definitions and prototypes
 */

#ifndef _AUTOMATE_GRAMM_H
#define _AUTOMATE_GRAMM_H_

#include <types.h>
#include <lex.h>
#include <file.h>
#include <notify.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <global.h>
#include <liste.h>
/*TODO : mettre les definitions de types dans types.h*/

typedef enum{
	S_INIT, 
	S_DATA,
	S_BSS,
	S_TEXT,
} STATE;


typedef enum{
	OPER_REG, 
	OPER_BASE_OFFSET, 
	OPER_DECIMAL, 
	OPER_HEXA,
	OPER_OCTAL,
	OPER_CHAINE,
	OPER_SYMBOLE,
	OPER_ERROR
}type_operande;


typedef enum{
	TXT_INST,
	TXT_SYMB,
	TXT_ERROR
}type_txt;


typedef enum{
	BSS_DIR,
	BSS_ERROR
}type_bss;


typedef enum{
	DATA_DIR,
	DATA_ERROR
}type_data;


typedef struct _text_{
	char* 		operateur;
	type_txt 	type;
	int 		nb_op;
	double 		line_nb;
	double 		decalage;
	Liste 		l_operande;
}*TEXT ;


typedef struct _data_{
	char* 		operateur;
	type_data 	type;
	int 		nb_op;
	double 		line_nb;
	double 		decalage;
	Liste 		l_operande;
}*DATA ;


typedef struct _bss_{
	char* 		operateur;
	type_bss 	type;
	int 		nb_op;
	double 		line_nb;
	double 		decalage;
	Liste 		l_operande;
}*BSS;

typedef struct _operande_{
	char*			chain;
	type_operande 	type;
}*OPERANDE;	

void automate_grammatical(File* p_file_Lexeme, File* p_file_Text, File* p_file_Bss, File* p_file_Data);
int analyse_gramm1(File* p_file_Lexeme, STATE* S, LEXEME lexeme_courant);
void get_current_Lexeme(File* p_file_Lexeme, LEXEME* p_lexeme_courant);
int ajout_maillon_data(File* p_file_Data, File* p_file_Lexeme, LEXEME lexeme_courant);
int ajout_maillon_bss(File* p_file_Bss, File* p_file_Lexeme, LEXEME lexeme_courant);
int ajout_maillon_Text(File* p_file_Text, File* p_file_Lexeme, LEXEME lexeme_courant);

void calcul_decalage_Data(File* p_file_Data, DATA* p_new_maillon);
void calcul_decalage_Bss(File* p_file_Bss, BSS* p_new_maillon);
#endif /*_AUTOMATE_GRAMM_H_*/