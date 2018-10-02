/**
 * @file lex.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief Automate-related stuff.
 *
 * Automate related definitions and prototypes
 */

#ifndef _AUTOMATE_H
#define _AUTOMATE_H_

#include <types.h>
#include <lex.h>
#include <file.h>
#include <stdio.h>
#include <ctype.h>
#include <global.h>

typedef enum{S_ERROR, S_INIT , S_ZERO, S_PARENTHESE_G, S_PARENTHESE_D, S_ETIQUETTE, S_DIRECTIVE, S_SYMBOLE, S_REGISTRE, S_COMMENTAIRE, S_VIRGULE, S_HEXA, S_DECIMAL, S_OCTAL, S_CHAINE, S_DOUBLEQUOTE} STATE;

File automate(File f, char* token, int line_nb, int* cmpt_err);




#endif /*_AUTOMATE_H_*/