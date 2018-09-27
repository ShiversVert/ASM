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
#include <file.h>
#include <stdio.h>
#include <ctype.h>
#include <global.h>

typedef enum{S_INIT , S_DECIMAL_ZERO, S_PARENTHESE, S_ETIQUETTE, S_DIRECTIVE, S_SYMBOLE, S_REGISTRE, S_COMMENTAIRE, S_VIRGULE, S_DEBUT_HEXA, S_HEXA, S_DECIMAL, S_OCTAL, S_CHAINE, S_DEBSTR} STATE;

File automate(File f, char* token, int i);




#endif /*_AUTOMATE_H_*/