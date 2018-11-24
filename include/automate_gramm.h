/**
 * @file automate_gramm.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief Grammatical Automate-related stuff.
 *
 * Grammatical Automate related definitions and prototypes
 */

#ifndef _AUTOMATE_GRAMM_H
#define _AUTOMATE_GRAMM_H_

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <global.h>
#include <string.h>

#include <types.h>
/*#include <lex.h>*/
#include <liste.h>
#include <file.h>
#include <notify.h>

void automate_grammatical(File* p_file_Lexeme, File* p_file_Text, File* p_file_Bss, File* p_file_Data, File* p_file_Symb, File file_Dic);
int analyse_gramm1(File* p_file_Lexeme, STATE_GRAMM* S, LEXEME lexeme_courant);
void get_current_Lexeme(File* p_file_Lexeme, LEXEME* p_lexeme_courant);

int ajout_maillon_data(File* p_file_Data, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb, double* p_offset_data);
int ajout_maillon_bss(File* p_file_Bss, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb, double* p_offset_bss);
int ajout_maillon_text(File* p_file_Text, File* p_file_Lexeme, LEXEME lexeme_courant, File* p_file_Symb, double* p_offset_text, File file_Dic);

void calcul_decalage_Data(File* p_file_Data, DATA* p_new_maillon,double* p_offset_data);
void calcul_decalage_Text(File* p_file_Text, TEXT* p_new_maillon, double* p_offset_text);
void calcul_decalage_Bss(File* p_file_Bss, BSS* p_new_maillon, double* p_offset_bss);

int is_in_dic(File file_Dic, File* p_file_Text_maillon_courant);
void replace_in_Text(File* p_file_Text, File* p_file_Symb, File file_Dic);
int replace_SYMB(OPERANDE* op, File* p_file_Symb);
int is_registre(OPERANDE* p_op, double line_nb);

#endif /*_AUTOMATE_GRAMM_H_*/
