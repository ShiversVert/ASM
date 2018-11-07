/**
 * @file dictionnaire_instruction.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief
 *
 */

#ifndef _DICTIONNAIRE_INSTRUCTIONS_H_
#define _DICTIONNAIRE_INSTRUCTIONS_H_

#include <types.h>
#include <liste.h>
#include <file.h>

typedef struct {
  char* chaine;      /*Chaine de caractere de l'instruction*/
  int nb_op;         /*Nombre d'opérandes de l'instruction*/
  char[3] type_op;   /*Tableau de 3 char contenant le type de l'opperande i*/
  /*
  R => REGISTRE
  O => offset
  B => offset(base)
  T => target
  I => valeur immediate
  S => sa ?
  */
}*DIC;

#endif /*_DICTIONNAIRE_INSTRUCTIONS_H_*/
