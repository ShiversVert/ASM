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
       ERROR_MSG("Error while trying to open %s file --- Aborts",file);
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

  new_maillon->nb_op = 0;
  int i = 0;

  while(line[i]!=' '){
    (new_maillon->chain)[i]=line[i];
    i++;
  }
  i++;


  while (line[i]!='\n') {
    if (line[i]!=" "){
      (new_maillon->type_op)[(new_maillon->nb_op)] = line[i];
      (new_maillon->nb_op)++;
    }
    i++;
  }

  return(enfiler(new_maillon, file_Dictionnaire));
}
