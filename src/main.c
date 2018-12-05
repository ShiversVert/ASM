/**
 * @file main.c
 * @author François Portet <francois.portet@imag.fr> from François Cayre
 * @brief Main entry point for MIPS assembler.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <types.h>
#include <liste.h>
#include <file.h>
#include <fct_affichage.h>
#include <dictionnaire_instruction.h>
#include <automate.h>
#include <automate_gramm.h>

/**
 * @param exec Name of executable.
 * @return Nothing.
 * @brief Print usage.
 *
 */
void print_usage( char *exec ) {
    fprintf(stderr, "Usage: %s file.s\n",
            exec);
}

/**
 * @param argc Number of arguments on the command line.
 * @param argv Value of arguments on the command line.
 * @return Whether this was a success or not.
 * @brief Main entry point for MIPS assembler.
 *
 */
int main ( int argc, char *argv[] ) {

    unsigned int 	nlines 	= 0;
    char* file = NULL;

    /* exemples d'utilisation des macros du fichier notify.h */
    /* WARNING_MSG : sera toujours affiche */
    /*WARNING_MSG("Un message WARNING_MSG !");*/

    /* macro INFO_MSG : uniquement si compile avec -DVERBOSE. Cf. Makefile*/
    /*INFO_MSG("Un message INFO_MSG : Debut du programme %s", argv[0]);*/

    /* macro DEBUG_MSG : uniquement si compile avec -DDEBUG (ie : compilation avec make debug). Cf. Makefile */
    /*DEBUG_MSG("Un message DEBUG_MSG !");*/

    /* La macro suivante provoquerait l'affichage du message
       puis la sortie du programme avec un code erreur non nul (EXIT_FAILURE) */
    /* ERROR_MSG("Erreur. Arret du programme"); */


    if ( argc <2 ) {/*
        INFO_MSG("Vous ne savez pas utiliser le programme...")
        print_usage(argv[0]);
        exit( EXIT_FAILURE );*/
        file = "./tests/miam_sujet.s";
    }

    else file = argv[argc-1];


    if ( NULL == file ) {
        WARNING_MSG("Pas de fichier assembleur");
        fprintf( stderr, "Missing ASM source file, aborting.\n" );
        exit( EXIT_FAILURE );
    }



    /* ---------------- Do the lexical analysis ---------------------*/
    int cmpt_err = 0;
    File file_Lexeme = lex_load_file( file, &nlines, &cmpt_err);
    if (file_Lexeme!=NULL) afficher_file(file_Lexeme, (*afficher_maillon_LEXEME));

    if(cmpt_err>0){
        WARNING_MSG("%d erreurs\n detectee", cmpt_err);
        ERROR_MSG("Erreur(s) lexicale detectee(s), terminaison du programme.\n");
        exit( EXIT_FAILURE );
    }

    /* ---------------- Do the grammatical analysis -----------------*/
    File file_Dic = NULL;
    File file_Text = NULL;
    File file_Bss = NULL;
    File file_Data = NULL;
    File file_Symb = NULL;
    File file_Realoc = NULL;
    DEBUG_MSG("\n\n###############################\nCHARGEMENT DU DICTIONNAIRE\n###############################\n\n");

    file_Dic = import_dictionnaire("./dictionnaires/dictionnaire_instructions.txt");
    DEBUG_MSG("\n\n######### file_Dic ##########\n\n");afficher_file(file_Dic, (*afficher_maillon_DIC));

    DEBUG_MSG("\n\n###############################\nDEBUT DE L'ANALYSE GRAMMATICALE\n###############################\n\n");

    automate_grammatical(&file_Lexeme, &file_Text, &file_Bss, &file_Data, &file_Symb, file_Dic, &file_Realoc);

    DEBUG_MSG("\t file_Lexeme\n");afficher_file(file_Lexeme, (*afficher_maillon_LEXEME));
    DEBUG_MSG("\t file_Text\n");afficher_file(file_Text, (*afficher_maillon_TEXT));
    DEBUG_MSG("\t file_Data\n");afficher_file(file_Data, (*afficher_maillon_DATA));
    DEBUG_MSG("\t file_Bss\n");afficher_file(file_Bss, (*afficher_maillon_DATA));
    DEBUG_MSG("\t file_Symb\n");afficher_file(file_Symb, (*afficher_maillon_SYMB));
    DEBUG_MSG("\t file_Realoc\n");afficher_file(file_Realoc, (*afficher_maillon_REALOC));

    /* ---------------- Free memory and terminate -------------------*/

    /* TODO free everything properly*/
    
    liberer_file(file_Dic);
    liberer_file(file_Text);
    liberer_file(file_Bss);
    liberer_file(file_Data);
    liberer_file(file_Symb);
    liberer_file(file_Realoc);

    exit( EXIT_SUCCESS );
}
