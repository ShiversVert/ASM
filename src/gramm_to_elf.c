/**
 * @file automate_gramm.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief Grammatical Transform the grammatical stuff into elf-understandable stuff
 *
 * Grammatical Transform the grammatical stuff into elf-understandable stuff
 */

#include <gramm_to_elf.h>

void gramm_to_elf(	File* p_file_Text, 		File* p_file_Bss, 		File* p_file_Data, 
					File* p_file_Symb, 		File* p_file_realoc, 	char* name, 
					long* p_taille_symb, 	long* p_taille_data, 	long* p_taille_text,
					long* p_taille_bss, 	long* p_taille_realoc){
	
	/*--------------INITIALISATIONS--------------*/

    section shstrtab = NULL;
	section     text = NULL;
    section     data = NULL;
    section      bss = NULL;
    section   strtab = NULL;
    section   symtab = NULL;
    section  reltext = NULL;
    section  reldata = NULL;

    long taille_text_reloc = 0, taille_data_reloc = 0;
    taille_tab_rel(&taille_text_reloc, &taille_data_reloc, p_file_realoc);

    Elf32_Rel text_reloc[taille_text_reloc], data_reloc[taille_data_reloc];

    Elf32_Sym syms[(*p_taille_symb)];

    char* machine = "mips";
    int noreorder = 1;

    /*------------ECRITURE DU NOM DU .O------------*/
    char new_name[strlen(name)];// extension[2], dir[8]; //strcpy(new_name, name);
	memset(new_name, '\0', sizeof(new_name));
    strcpy(new_name, name);

	//strcpy(new_name, "/tests");///output");
	//strcat(new_name, strchr(name, '/'));
	new_name[strlen(new_name)-1] = 'o';

    DEBUG_MSG("Fichier de sortie : %s\n", new_name);
    /*-------------------COEUR-------------------*/
    shstrtab = make_shstrtab_section();

    text = make_text_section(p_file_Text, p_taille_text);
    if ( !text ) {
        ERROR_MSG("Impossible d'ecrire la ssection .text (information manquante)\n" );
    }

    data = make_data_section(p_file_Data, p_taille_data);
    if ( !data ) {
        ERROR_MSG("Impossible d'ecrire la section .data (information manquante)\n" );
    }

    bss = make_bss_section(p_file_Bss);
    if ( !bss ) {
        ERROR_MSG("Impossible d'ecrire la section .bss (information manquante)\n" );
    }

    strtab   = make_strtab_syms_section(p_file_Symb, p_taille_symb, syms, shstrtab);

    symtab   = make_symtab_section( shstrtab, strtab, syms, (*p_taille_symb));

    file_realoc_to_tab_realoc(p_file_realoc, text_reloc, data_reloc, 
								taille_text_reloc, taille_data_reloc, symtab,
								shstrtab, strtab);

    reltext  = make_rel32_section( ".rel.text", text_reloc, taille_text_reloc);
    reldata  = make_rel32_section( ".rel.data", data_reloc, taille_data_reloc);

    /*--------------------Ecriture dans le fichier--------------------*/

    elf_write_relocatable( new_name, machine, noreorder,
                           text->start, text->sz,
                           data->start, data->sz,
                           bss->start, bss->sz,
                           shstrtab->start, shstrtab->sz,
                           strtab->start, strtab->sz,
                           symtab->start, symtab->sz,
                           reltext->start, reltext->sz,
                           reldata->start, reldata->sz);

    /*
    print_section( text );
    print_section( data );
    print_section( bss );
    print_section( strtab );
    print_section( symtab );
    */
   
    /*--------------------Liberation--------------------*/
    del_section(     text );
    del_section(     data );
    del_section(      bss );
    del_section( shstrtab );
    del_section(   strtab );
    del_section(   symtab );
    del_section(  reltext );
    del_section(  reldata );

}

#define add_string_to_table( section, symbol ) /* Explicitly add terminal '\0' */ \
  write_section( section, (unsigned char *)symbol, 1+strlen(symbol), section->sz)

/**
* @brief Generates a predefined shstrtab section containing the basic sections names
* @return the completed shstrtab section
*/

section make_shstrtab_section( void ) {
    section shstrtab = new_section( ".shstrtab", SECTION_CHUNK_SZ );
    add_string_to_table( shstrtab, "" ); /* ELF string tables start with a '0' */
    add_string_to_table( shstrtab, ".text" );
    add_string_to_table( shstrtab, ".rel.text" );
    add_string_to_table( shstrtab, ".data" );
    add_string_to_table( shstrtab, ".rel.data" );
    add_string_to_table( shstrtab, ".bss" );
    add_string_to_table( shstrtab, ".symtab" );
    add_string_to_table( shstrtab, ".strtab" );
    add_string_to_table( shstrtab, ".shstrtab" );

    return shstrtab;
}

section make_text_section(File *p_file_Text, long* p_taille_text){
	section        text = new_section( ".text", SECTION_CHUNK_SZ );
	int instructions[(*p_taille_text)];

	if((*p_taille_text)>0){
		//(*p_file_Text) = (*p_file_Text)->suiv;
		for(int i =0; i< *p_taille_text; i++){
			instructions[i] = ((TEXT)((*p_file_Text)->suiv->val))->bin;
			defiler(p_file_Text);
		}

		write_section(text, (unsigned char *)(instructions), sizeof(*instructions)*(*p_taille_text), 0);
	}

    DEBUG_MSG("\nInstructions : \n");
    printf("\n");
    for (int k = 0; k <(*p_taille_text); k++) {
        printf("%8.8x ", instructions[k]);
        if ((k+1)%4 == 0) printf("\n");
    }
    printf("\n");
	return text;
}

section make_data_section(File *p_file_Data, long* p_taille_data){
	section        data = new_section( ".data", SECTION_CHUNK_SZ );
    int cmpt_octet = 0;
    int i = 0; int n, cmpt_inter,j ;
    int taille_data_prog = (int)((*p_taille_data)/4);
	int data_prog[taille_data_prog]; /*EN OCTETS*/
    memset(data_prog, 0x0, *p_taille_data);
    DATA data_courant = NULL;

    /*On raisonne sur les octets*/
	if((*p_taille_data)>0){

		while((*p_file_Data) != NULL){
            data_courant = (DATA)((*p_file_Data)->suiv->val);
			switch(data_courant->type){
                case DATA_BYTE :
                    /*On trouve .byte n => Il faut faire 1 OCTETS de valeur "n" dans data_prog */
                    while(data_courant->l_operande != NULL){
                        n = ((OPERANDE)((data_courant->l_operande)->val))->bin;
                        
                        data_prog[i] = data_prog[i] | deplace_octet(n, 3-cmpt_octet%4);
                        cmpt_octet++; i = (int)(cmpt_octet/4);
                        
                        (data_courant->l_operande) = (data_courant->l_operande)->suiv;
                    }
                    break;
                    break;

                case DATA_SPACE :
                    /*On trouve .space n => Il faut faire n OCTETS nuls dans data_prog */
                    while(data_courant->l_operande != NULL){
                        n = ((OPERANDE)((data_courant->l_operande)->val))->bin;
                        cmpt_inter = cmpt_octet + i;
                        for(;cmpt_octet<cmpt_inter; cmpt_octet++){
                            data_prog[i] = data_prog[i] | deplace_octet(0x0, 3-cmpt_octet%4);
                            cmpt_octet++; i = (int)(cmpt_octet/4);
                        }
                        (data_courant->l_operande) = (data_courant->l_operande)->suiv;
                    }
                    break;

                case DATA_ASCIIZ :
                    /*On trouve .ascizz "chaine" => il faut remplir un octet par caractere de la chaine*/
                    while(data_courant->l_operande != NULL){
                        n = strlen( ((OPERANDE)((data_courant->l_operande)->val))->chain ) - 1;
                        char chaine[n]; 
                        strcpy(chaine, ((OPERANDE)((data_courant->l_operande)->val))->chain + 1); /*On enleve la premiere quote*/
                        chaine[n] = '\0';
                        cmpt_inter = n + i;
                        for(j =0;j<n; j++){
                            data_prog[i] = data_prog[i] | deplace_octet(chaine[j], 3-cmpt_octet%4);
                            cmpt_octet++; i = (int)(cmpt_octet/4);
                        }

                        (data_courant->l_operande) = (data_courant->l_operande)->suiv;
                    }
                    break;

                case DATA_WORD :
                    /*On trouve .word n, on ecrit n sur un int (4 octets) swappes*/
                    while(data_courant->l_operande != NULL){
                        n = ((OPERANDE)((data_courant->l_operande)->val))->bin;
                        
                        if(cmpt_octet%4!=0 && cmpt_octet != 0){
                            i++; cmpt_octet += 4 - i%4;
                        }
                        data_prog[i] = n;
                        data_prog[i] = ((data_prog[i]>>24)  & 0x000000ff) | // move byte 3 to byte 0
                                        ((data_prog[i]<<8)  & 0x00ff0000) | // move byte 1 to byte 2
                                        ((data_prog[i]>>8)  & 0x0000ff00) | // move byte 2 to byte 1
                                        ((data_prog[i]<<24) & 0xff000000);  // byte 0 to byte 3
                        
                        i++; cmpt_octet+=4;

                        (data_courant->l_operande) = (data_courant->l_operande)->suiv;
                    }
                    break;

                default : 
                    ERROR_MSG("Impossible de lire une directive");
                    break;
            }
            
			defiler(p_file_Data);
		}

		write_section(data, (unsigned char *)(data_prog), sizeof(*data_prog)*(taille_data_prog), 0);
	}

    DEBUG_MSG("\nData_prog : \n");
    printf("\n");
    for (int k = 0; k <taille_data_prog; k++) {
        printf("%8.8x ", data_prog[k]);
        if ((k+1)%4 == 0) printf("\n");
    }
    printf("\n");

    return data;
}

section make_bss_section(File* p_file_Bss){
    section bss = new_section( ".bss", SECTION_CHUNK_SZ );
    long compteur = 0;
    BSS bss_courant;
    while((*p_file_Bss)!=NULL){
        bss_courant = ((BSS)((*p_file_Bss)->suiv->val));

    	while(( bss_courant->l_operande != NULL) ){
            compteur += ((OPERANDE)((bss_courant->l_operande)->val))->bin;
    		bss_courant->l_operande = supprimer_tete(bss_courant->l_operande);
    	}
    	defiler(p_file_Bss);
    }
    bss -> sz =compteur;

    return bss;
}

section make_strtab_syms_section( File* p_file_Symb, long* p_taille_symb, Elf32_Sym syms[], section shstrtab) {
    section strtab = new_section( ".strtab", SECTION_CHUNK_SZ);

    add_string_to_table( strtab, "" ); /* ELF string tables start with a '0' */
    int i= 0;
    while((*p_file_Symb)!=NULL){
        add_string_to_table(strtab, ((SYMB)((*p_file_Symb)->suiv->val))->nom);

        syms[i].st_name = elf_get_string_offset( strtab->start, strtab->sz, ((SYMB)((*p_file_Symb)->suiv->val))->nom);
    	syms[i].st_size = 0;
    	syms[i].st_value = ((SYMB)((*p_file_Symb)->suiv->val))->decalage;
    	syms[i].st_other = 0;
    	switch (((SYMB)((*p_file_Symb)->suiv->val))->zone){
    		case ZONE_UNKW:
    			syms[i].st_info = ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE);
    			break;

    		case ZONE_TEXT:
    			syms[i].st_info = ELF32_ST_INFO(STB_LOCAL, STT_NOTYPE);
    			syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".text" );
    			break;
    		
    		case ZONE_BSS:
    			syms[i].st_info = ELF32_ST_INFO(STB_LOCAL, STT_NOTYPE);
    			syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".bss" );
    			break;

            case ZONE_DATA:
                syms[i].st_info = ELF32_ST_INFO(STB_LOCAL, STT_NOTYPE);
                syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".data" );
                break;
    		
    		default:
    			ERROR_MSG("Impossible de generer la table des symboles");
    	}

        i++;
        defiler(p_file_Symb);
    }

    return strtab;
}

section make_symtab_section(section shstrtab, section strtab,  Elf32_Sym symbols[], int nb_syms  ) {

    section    symtab = new_section( ".symtab", SECTION_CHUNK_SZ );
    int i;
    unsigned int offset = 0;

    Elf32_Sym   entry= {0};
    entry.st_name  = 0;
    entry.st_value = 0;
    entry.st_size  = 0;
    entry.st_info  = 0;
    entry.st_other = 0;
    entry.st_shndx = SHN_UNDEF;

    write_section( symtab, (unsigned char *)&entry, sizeof(entry), offset);


    entry.st_name  = 0;
    entry.st_value = 0;
    entry.st_size  = 0;
    entry.st_info  = ELF32_ST_INFO( STB_LOCAL, STT_SECTION );
    entry.st_other = 0;
    entry.st_shndx = elf_get_string_index( shstrtab->start, shstrtab->sz, ".text" );

    write_section( symtab, (unsigned char *)&entry, sizeof(entry), symtab->sz);

    entry.st_name  = 0;
    entry.st_value = 0;
    entry.st_size  = 0;
    entry.st_info  = ELF32_ST_INFO( STB_LOCAL, STT_SECTION );
    entry.st_other = 0;
    entry.st_shndx = elf_get_string_index( shstrtab->start, shstrtab->sz, ".data" );

    write_section( symtab, (unsigned char *)&entry, sizeof(entry), symtab->sz);

    entry.st_name  = 0;
    entry.st_value = 0;
    entry.st_size  = 0;
    entry.st_info  = ELF32_ST_INFO( STB_LOCAL, STT_SECTION );
    entry.st_other = 0;
    entry.st_shndx = elf_get_string_index( shstrtab->start, shstrtab->sz, ".bss" );

    write_section( symtab, (unsigned char *)&entry, sizeof(entry), symtab->sz);

    for (i =0; i< nb_syms ; i++) {
        write_section( symtab, (unsigned char *)&symbols[i], sizeof(symbols[i]), symtab->sz);
    }

    return symtab;
}

int taille_tab_rel(long* taille_text_reloc, long* taille_data_reloc, File* p_file_realoc){
	if ((*p_file_realoc)==NULL) return 0;
	File pointeur_realoc = (*p_file_realoc)->suiv;
	do{
		switch(((REALOC)(pointeur_realoc->val))->zone){
			case ZONE_TEXT:
				(*taille_text_reloc) ++;
				break;
			case ZONE_DATA:
				(*taille_data_reloc)++;
				break;
			default:
				break;
		}

		pointeur_realoc = pointeur_realoc->suiv;
	}while(pointeur_realoc!=(*p_file_realoc)->suiv);

	return(1);
}

void file_realoc_to_tab_realoc(File* p_file_realoc, Elf32_Rel text_reloc[], Elf32_Rel data_reloc[], 
								long taille_text_reloc, long taille_data_reloc, section symtab,
								section shstrtab, section strtab){
    char temp[256] = {0};
	char* zone[4] = {".data", ".text", ".bss", temp};
	zone_symb zone_def = ZONE_UNKW;
    long cmpt_text = 0, cmpt_data=0;

	while((*p_file_realoc)!=NULL){
        zone_def = ((REALOC)((*p_file_realoc)->suiv->val))->zone_def;
		strcpy(zone[3], (*(((REALOC)((*p_file_realoc)->suiv->val))->p_op))->chain);

        switch(((REALOC)((*p_file_realoc)->suiv->val))->zone){
			case ZONE_TEXT:
				text_reloc[cmpt_text].r_offset = ((REALOC)((*p_file_realoc)->suiv->val))->decalage;
				text_reloc[cmpt_text].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab, strtab,zone[zone_def]), ((REALOC)((*p_file_realoc)->suiv->val))->type );
				cmpt_text++;
				break;

			case ZONE_DATA:
				data_reloc[cmpt_data].r_offset = ((REALOC)((*p_file_realoc)->suiv->val))->decalage;
				data_reloc[cmpt_data].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab, strtab,zone[zone_def]), ((REALOC)((*p_file_realoc)->suiv->val))->type );
				cmpt_data++;
				break;

			default:
                ERROR_MSG("Impossible de generer les tables de realocation");
				break;
		}

		defiler(p_file_realoc);
	}
}

section make_rel32_section(char *relname, Elf32_Rel relocations[], int nb_reloc) {

    section reltab = new_section( relname, SECTION_CHUNK_SZ );
    int i ;

    for (i=0; i<nb_reloc; i++) {
        Elf32_Rel rel = relocations[i];
        write_section( reltab, (unsigned char *)&rel, sizeof( rel ), reltab->sz);

    }
    return reltab;
}

int elf_get_sym_index_from_name(section symtab, section shstrtab, section strtab, char* sym_name) {
    int i=0;
    Elf32_Sym* symboles = (Elf32_Sym*) symtab->start;
    int section_offset = elf_get_string_index( shstrtab->start, shstrtab->sz, sym_name );
    int offset = elf_get_string_offset( strtab->start, strtab->sz, sym_name );

    if (offset > 0 || section_offset > 0 ) {
        for (i=0; i < (symtab->sz)/sizeof(Elf32_Sym); i++) {
            if (ELF32_R_TYPE(symboles[i].st_info) == STT_SECTION) {
                if (symboles[i].st_shndx == section_offset) {
                    return i;
                }
            }
            else if (symboles[i].st_name == offset)
                return i;
        }
    }
    return -1;

}

/**
 * Deplace l'octet "a" en num_octet position
 * @param  a         octet a deplacer
 * @param  num_octet numero de l'octet dans un int (compris entre 0 et 3, inutile pour 3)
 * @return           renvoie l'octet deplace
 */
int deplace_octet(int a, int num_octet){
    if (num_octet > 3) return a;

    int masque[4]= {0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff};
    int decalage[4] = {24, 16, 8 ,0};
    
    return (a << decalage[num_octet]) & masque[num_octet];
}