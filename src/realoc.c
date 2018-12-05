#include <realoc.h>

/**
 * Aoute l'operande a reallouer a la file de realoc_offset si c'est un offset a la file de realoc sinon
 *
 * @param p_op                 [description]
 * @param p_file_realoc         Pointeur sur la File a laquelle on ajoute le maillon a reallouer
 *                              Si on realloue un type offset, il faut passer en parametre la file file_realoc_offset
 * @param type 					Type de realloc a faire -> depend de du type de l'operande dans le dictionnaire
 * @param zone 					Zone ou est definie l'operande /!\ (et pas le symbole) 
 */
void ajout_maillon_realoc(OPERANDE* p_op, File* p_file_realoc, type_realoc type, zone_symb zone, double decalage){
	REALOC new_maillon = calloc(1, sizeof(*new_maillon));
	new_maillon->zone = zone;
	new_maillon->decalage = decalage;
	new_maillon->type = type;

	*p_file_realoc = enfiler(new_maillon, *p_file_realoc);
}

/**Permet de parcourir uniquement la liste d'operande SYMB qui ontdoivent avoir des offset comme opérandes et les remplacer*/
void reallocation_offset(File* p_file_realoc_offset, File* p_file_Symb){
	
}