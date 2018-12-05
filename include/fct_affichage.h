/**
 * @file types.h
 * @author
 * @brief Types definition
 *
 * Contains the definition of the differents types used in the program
 */

#ifndef FCT_AFFICHAGE_H
#define FCT_AFFICHAGE_H

#include <types.h>
#include <file.h>
#include <liste.h>
#include <automate_gramm.h>

void afficher_maillon_LEXEME(void* maillon);
void afficher_maillon_DATA(void* maillon);
void afficher_maillon_BSS(void* maillon);
void afficher_maillon_TEXT(void* maillon);
void afficher_maillon_OPERANDE(void* maillon);
void afficher_maillon_SYMB(void* maillon);
void afficher_maillon_DIC(void* maillon);
void afficher_maillon_REALOC(void* maillon);

#endif /*FCT_AFFICHAGE_H*/
