/**
 * @file automate_gramm.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief Grammatical Realoc-related stuff.
 *
 * Grammatical Realoc related definitions and prototypes
 */

#ifndef _REALOC_H_
#define _REALOC_H_

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <global.h>
#include <string.h>

#include <types.h>
#include <liste.h>
#include <file.h>
#include <notify.h>

void ajout_maillon_realoc(OPERANDE* p_op, File* p_file_realoc, type_realoc type, zone_symb zone, double decalage);
void reallocation_offset(File* p_file_realoc_offset, File* p_file_Symb);
int remplace_realoc_offset(File* p_file_Symb, OPERANDE* p_op);


#endif /*_REALOC_H_*/
