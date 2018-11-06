# ASM
@Author : BAILLY Romain & FOUASSIER Paul

# Objectif
On cherche a creer un compilateur assembleur MIPS en c. Pour cela, le travail est decoupe en 4 livrables :
	- 1 : Génération d'une file de Lexeme et affichage
	- 2 : Analyse grammaticale
	- 3 : Analyse grammaticale 2
	- 4 : Génération du code binaire
	

# Utilisation
L'utilisation du fichier se fait de la manière suivante :
	
	1) Compilation du code grâce à la commande "make release" à la racine du dossier
	
	2) Appel de l'executaable "as-mips" avec omme argument le chemin du fichier à compiler
	
	3) Observer
	
	4) (Optionnel) Générer la documentation grace à "make docu" si vous possez les packets Doxygen
	
# Fichiers Tests
La première ébauche du compilateur consistant en une analyse lexicale du fichier assembleur passé en paramètre est dourni avec une batterie de test mettant en évidence le fonctionnement (ou le dysfonctionnement) de certains points (ici par ordre de "complexité"):
	- azerty.s:
		Il permet de vérifier le fonctionnement compilateur sur un fichier symple : un symbole unique
	- vide.s:
		Il permet de vérifier le fonctionnement compilateur sur un fichier vide.
	- plus_256_carac.s:
		Il permet de vérifier le fonctionnement compilateur sur un fichier dont la ligne contient plus de 256 caractères. Dans ce cas elle sera tronquée quand la limite de 256 caractères est atteinte.
	- miam_sujet.s:
		C'est le fichier assembleur donné dans le sujet. Il permet de vérifier le fonctionnement compilateur sur un fichier assembleur "classique" et sans erreurs.
	- test_global.s:
		Il permet de vérifier le fonctionnement compilateur sur un fichier contenant des suites de carctères plus farfelues.
	- test_str.s:
		Il permet de vérifier le fonctionnement compilateur sur des chaines de caractères complètes et incomplètes.



