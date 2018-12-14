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
    char new_name[strlen(name)+9], extension[2], dir[8]; //strcpy(new_name, name);
	//memset(new_name, '\0', sizeof(new_name));

	strcpy(new_name, "./output");
	strcat(new_name, strchr(name, '/'));
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


    print_section( text );
    print_section( data );
    print_section( bss );
    print_section( strtab );
    print_section( symtab );

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
		(*p_file_Text) = (*p_file_Text)->suiv;
		for(int i =0; i< *p_taille_text; i++){
			instructions[i] = ((TEXT)((*p_file_Text)->val))->bin;
			defiler(p_file_Text);
		}

		write_section(text, (unsigned char *)(instructions), sizeof(*instructions)*(*p_taille_text), 0);
	}
	return text;
}

section make_data_section(File *p_file_Data, long* p_taille_data){
	section        data = new_section( ".data", SECTION_CHUNK_SZ );
	int data_prog[(*p_taille_data)];

	if((*p_taille_data)>0){
		(*p_file_Data) = (*p_file_Data)->suiv;
		for(int i =0; i< *p_taille_data; i++){
			data_prog[i] = ((TEXT)((*p_file_Data)->val))->bin;
			defiler(p_file_Data);
		}

		write_section(data, (unsigned char *)(data_prog), sizeof(*data_prog)*(*p_taille_data), 0);
	}
	return data;
}

section make_bss_section(File* p_file_Bss){
    section bss = new_section( ".bss", SECTION_CHUNK_SZ );
    long compteur = 0;
    while((*p_file_Bss)!=NULL){
    	while(((((BSS)((*p_file_Bss)->val))->l_operande) != NULL) ){
    		compteur += ((OPERANDE)((((BSS)((*p_file_Bss)->val))->l_operande)->val))->bin;
    		((BSS)((*p_file_Bss)->val))->l_operande = supprimer_tete(((BSS)((*p_file_Bss)->val))->l_operande);
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
        add_string_to_table(strtab, ((SYMB)((*p_file_Symb)->val))->nom);

        syms[i].st_name = elf_get_string_offset( strtab->start, strtab->sz, ((SYMB)((*p_file_Symb)->val))->nom);
    	syms[i].st_size = 0;
    	syms[i].st_value = ((SYMB)((*p_file_Symb)->val))->decalage;
    	syms[i].st_other = 0;
    	switch (((SYMB)((*p_file_Symb)->val))->zone){
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
	
	long cmpt_text = 0, cmpt_data=0;

	while((*p_file_realoc)!=NULL){
		switch(((REALOC)((*p_file_realoc)->val))->zone){
			case ZONE_TEXT:
				text_reloc[cmpt_text].r_offset = ((REALOC)((*p_file_realoc)->val))->decalage;
				text_reloc[cmpt_text].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab, strtab,".text"), ((REALOC)((*p_file_realoc)->val))->type );
				cmpt_text++;
				break;

			case ZONE_DATA:
				data_reloc[cmpt_data].r_offset = ((REALOC)((*p_file_realoc)->val))->decalage;
				data_reloc[cmpt_data].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab, strtab,".data"), ((REALOC)((*p_file_realoc)->val))->type );
				cmpt_data++;
				break;

			default:
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

char* name_s_to_name_o(char* name){
	char new_name[strlen(name)+9], extension[2], dir[8]; //strcpy(new_name, name);
	memset(new_name, '\0', sizeof(new_name));

	strcpy(new_name, "./output");
	strcat(new_name, strchr(name, '/'));
	new_name[strlen(new_name)-1] = 'o';

	return(new_name);
}