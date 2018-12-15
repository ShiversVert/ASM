/**
 * @file automate_gramm.h
 * @author Romain BAILLY <romain.bailly1@grenoble-inp.org>
 * @brief Grammatical Transform the grammatical stuff into elf-understandable stuff
 *
 * Grammatical Transform the grammatical stuff into elf-understandable stuff
 */

#ifndef _GRAMM_TO_ELF_
#define _GRAMM_TO_ELF_

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
#include <automate_gramm.h>

#include <pelf.h>
#include <section.h>
#include <elf-mips32.h>
#include <elf.h>

void gramm_to_elf(	File* p_file_Text, 		File* p_file_Bss, 		File* p_file_Data, 
					File* p_file_Symb, 		File* p_file_realoc, 	char* name, 
					long* p_taille_symb, 	long* p_taille_data, 	long* p_taille_text,
					long* p_taille_bss, 	long* p_taille_realoc);

section make_shstrtab_section( void ) ;
section make_text_section(File *p_file_Text, long* p_taille_text);
section make_data_section(File *p_file_Data, long* p_taille_data);
section make_bss_section(File* p_file_Bss);
section make_strtab_syms_section( File* p_file_Symb, long* p_taille_symb, Elf32_Sym syms[], section shstrtab);
section make_symtab_section(section shstrtab, section strtab,  Elf32_Sym symbols[], int nb_syms  ) ;
int taille_tab_rel(long* taille_text_reloc, long* taille_data_reloc, File* p_file_realoc);
void file_realoc_to_tab_realoc(File* p_file_realoc, Elf32_Rel text_reloc[], Elf32_Rel data_reloc[], 
								long taille_text_reloc, long taille_data_reloc, section symtab,
								section shstrtab, section strtab);
section make_rel32_section(char *relname, Elf32_Rel relocations[], int nb_reloc);
int elf_get_sym_index_from_name(section symtab, section shstrtab, section strtab, char* sym_name);
char* name_s_to_name_o(char* name);

#endif //_GRAMM_TO_ELF_