/**
 * @file lex.h
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexem-related stuff.
 *
 * Contains lexem types definitions, some low-level syntax error codes,
 * the lexem structure definition and the associated prototypes.
 */

#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>
#include <file.h>

File 	lex_read_line( char *line, int nline, File file_lexeme, int* cmpt_err);
File 	lex_load_file( char *file, unsigned int *nlines, int* cmpt_err);
char* 	getNextToken( char** , char* );
int 	isspecial(char* c);
int 	ishexa(char* c);

#endif /* _LEX_H_ */

