/**
 * @file dictionnaire_instruction.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief
 *
 */

#ifndef _DICTIONNAIRE_INSTRUCTIONS_H_
#define _DICTIONNAIRE_INSTRUCTIONS_H_

#include <stdio.h>
#include <stdlib.h>

#include <types.h>
#include <liste.h>
#include <file.h>

File import_dictionnaire(char* fichier);
File dic_read_line(char* line, File file_Dictionnaire);

#endif /*_DICTIONNAIRE_INSTRUCTIONS_H_*/
