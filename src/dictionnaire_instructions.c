#include <dictionnaire_instruction.h>

/**
 * @file dictionnaire_instruction.c
 * @author
 * @brief Chargement du dictionnaire d'instructions
 */

 File import_dictionnaire(char* fichier){
   FILE        *fp   = NULL;
   char         line[STRLEN]; /* original source line */

   fp = fopen( fichier, "r" );
   if ( NULL == fp ) {
       /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
       ERROR_MSG("Error while trying to open %s file --- Aborts",fichier);
   }

   File file_Dictionnaire = NULL;

   while(!feof(fp)) {

       /*read source code line-by-line */
       if ( NULL != fgets( line, STRLEN-1, fp) ) {
           if ( 0 != strlen(line) ) {
               file_Dictionnaire = dic_read_line(line, file_Dictionnaire);
           }
       }
   }

   fclose(fp);
   return(file_Dictionnaire);
 }

File dic_read_line(char* line, File file_Dictionnaire){
    DIC new_maillon = calloc(1,sizeof(*new_maillon));
    new_maillon->chain = calloc(3,sizeof(char)); /*Peut potentiellement contenir 6 char : rs rd rt*/

    new_maillon->nb_op = 0;
    int i = 0;  char temp[3]; int coeff = 1;
    int j = 0;  int op_bin = 0;

    /*Ajout de la chaine de l'intruction*/
    while(line[i]!=' ' && line[i]!='\n'){
        if (line[i] != '\n') {
            (new_maillon->chain)[i]=line[i];
            i++;
        }
    }

    /*stocker le type des opperandes*/
    /*Tant qu'on ne rencontre pas un chiffre => C'est le type d'une opperande*/
    while(!isdigit(line[i])) { 
        if (line[i]==' '){i++;} /*Mange les blancs*/
        else{
            (new_maillon->type_op)[(new_maillon->nb_op)] = line[i];           

            /*Cas d'un registre: 
            if(line[i]== 'R'){
                i++; //On passe au nom du registre
                (new_maillon->type_op)[3 + (new_maillon->nb_op)] = line[i]; //On va stocker les lettres des registres dans les indices 3/4/5
            }*/
            i++;
            (new_maillon->nb_op)++; /*compteur nombre operandes*/

        }
    }
    /*On a rencontre un chiffre (c'est l'opcode)*/
    /*Chargement de l'opcode*/
    while(isdigit(line[i])){
        temp[j] = line[i];
        i++; j++;
    }
    temp[j] = '\0';
    new_maillon->bin[op_bin][0] = atoi((const char *)&temp); /*VAleur de l'opcode*/
    new_maillon->bin[op_bin][1] = 6; /*Toujours sur 6 bits*/
    j = 0; i++; /*Reinitialisation de j et on mange le blanc apres l'opcode*/
    op_bin ++;

    while(line[i]!='\n'){
        if (line[i]==' '){
            i++;
            coeff = 1;
        }

        if(line[i]=='-'){ /*Si on trouve un "-" => C'est le numero de l'operande correspondante*/
            coeff = -1;
            i++;
        }

        while(isdigit(line[i])){ /*On recupere le premier nombre*/
            temp[j] = line[i];
            i++; j++;
        }
        temp[j] = '\0';

        new_maillon->bin[op_bin][0] = coeff * atoi((const char *)&temp); /*VAleur de l'opcode*/
        i++; j = 0;/*On enleve le ":"*/

        while(isdigit(line[i])){/*On recupere le second nombre*/
            temp[j] = line[i];
            i++; j++;
        }
        temp[j] = '\0';
        new_maillon->bin[op_bin][1] = atoi((const char *)&temp);
        j = 0;
        
        op_bin++;
    }

    /*stocker les valeurs et leur nombre de bits */
    /*c'est c'est un chiffre, il faut ecrire ce chiffre sur le nombre de bit qui suit le :*/
    /*si c'est une lettre, il faut ecrire l'operande correspondante*/
    
    return(enfiler(new_maillon, file_Dictionnaire));
}