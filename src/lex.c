
/**
 * @file lex.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexical analysis routines for MIPS assembly syntax.
 *
 * These routines perform the analysis of the lexeme of an assembly source code file.
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <file.h>
#include <automate.h>

/**
 * @param token The pointeur to the lexeme to be extracted.
 * @param current_line The address from which the analysis must be performed.
 * @return the address at which the analysis stopped or NULL if the analysis is terminated.
 * @brief This function get an input line, extracts a token from it and return the pointeur to the next place where the analysis should continue.
 */

/* note that MIPS assembly supports distinctions between lower and upper case*/

char* getNextToken(char** token, char* current_line) {
    char* start = current_line;
    char* end=NULL;
    int token_size=0;

    /* check input parameter*/
    if (current_line ==NULL) return NULL;

    /* eats any blanks before the token*/
    while (*start!='\0' && isblank(*start)){
	start++;
    }
    /* go till next blank or end*/
    end=start;

    if ((*start!='#') && (*start!='"')){
        while (*end!='\0' && !isblank(*end) && !isspecial(end)) end++;

        if (start == end && isspecial(start)) end++; /*Si le caractere est special*/
        if (*start == '\\') end++; /*On prend un caractere de plus si le \ sert d'echapement*/
    }

    else if (*start=='"'){
    	if(*(end+1)!='#') end++;

        while (*end!='"' && *end!='\0' && *end!='#'){
    		if(*end =='\\'){end++;}
    		end++;
    	}
        if(*end!='#') end++;    
    }

    else if (*start=='#'){
        while (*end!='\0') end++;
    }
    /*compute size : if zero there is no more token to extract*/
    token_size=end-start;
    if (token_size>0){
	*token 	= calloc(token_size+1,sizeof(*start));
	strncpy(*token,start,token_size);
	(*token)[token_size]='\0';
	return end;

    }
    return NULL;
}




/**
 * @param line String of the line of source code to be analysed.
 * @param nline the line number in the source code.
 * @param cmpt_err est le compteur d'erreur, passe par adresse pour le modifier dans la boucle de l'automate_lexical
 * @return should return the collection of lexemes that represent the input line of source code.
 * @brief This function performs lexical analysis of one standardized line.
 *
 */
File lex_read_line( char *line, int nline, File file_lexeme, int* cmpt_err) {
    char* token = NULL;
    char* current_address=line;

    while( (current_address= getNextToken(&token, current_address)) != NULL){
        /*DEBUG_MSG("%s\n", token);*/
        file_lexeme =  automate_lexical(file_lexeme, token, nline, cmpt_err);
    }

    return file_lexeme;
}

/**
 * @param file Assembly source code file name.
 * @param nlines Pointer to the number of lines in the file.
 * @return should return the collection of lexemes
 * @param cmpt_err est le compteur d'erreur, passe par adresse pour le modifier dans la boucle de l'automate_lexical
 * @brief This function loads an assembly code from a file into memory.
 *
 */
File lex_load_file( char *file, unsigned int *nlines, int* cmpt_err) {

    FILE        *fp   = NULL;
    char         line[STRLEN]; /* original source line */


    fp = fopen( file, "r" );
    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    *nlines = 0;
    File file_lexeme = NULL;

    while(!feof(fp)) {

        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {
            if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';  /* eat final '\n'  */
            (*nlines)++;

            if ( 0 != strlen(line) ) {
                file_lexeme = lex_read_line(line,*nlines, file_lexeme, cmpt_err);
            }
        }
    }

    fclose(fp);
    return file_lexeme;
}

/**
 * @param Charactere c
 * @return Retourne 1 si le carctere est "special", 0 sinon
 * @brief Retourne 1 si le carctere est "special" : , () , 0 sinon
 *
 */

int isspecial(char* c){
    if(*c == ','|| *c == '('|| *c == ')'|| *c == ';' || *c == '\\' ) {
        /*  || *c == '\\' || *c == '\'')*/
        return(1);
    }
    return(0);
} 

int ishexa(char* c){
    if (isdigit(c)|| *c=='a' || *c=='A' || *c=='b' || *c=='B' || *c=='c' || *c=='C' || *c=='d' || *c=='D'|| *c=='e'|| *c=='E' || *c=='f' || *c=='F') return 1;
    return 0;
}
