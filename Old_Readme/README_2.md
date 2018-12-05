Le programme affiche les résultats du livrable 1 et du livrable 2. Il contient de ce fait tous les documents du livrable 1 associé à ceux du livrable 2.

Les documents du livrable 1 ont été modifié.
Dans le fichier src l'automate, la gestion des erreurs et la calcul des décalages est géré dans automate_gramm.c   et affiché à l'aide de fct_affichage.c .
Dans le fichier include on peut y trouver types.h qui définit toutes les nouvelles structures.

La liste des instructions pour exécuter le code :
1) Compilation du code grâce à la commande "make debug" à la racine du dossier
2) Appel de l'exécutable "as-mips" avec comme argument le chemin du fichier à compiler sous la forme ./as-mips FICHIER.S . Sans argument, ça sera miam_sujet.s qui sera exécuté par défaut.
3) Observer les collections qui s'affichent
4) (Optionnel) Générer la documentation grace à "make docu" si vous possez les packets Doxygen


- src : qui contient le code C de votre interpréteur.
- include : qui contient toutes les définitions de types et prototypes de votre programme. Lisez attentivement les headers avant de commencer à coder et de les modifier.
- doc : contient la documentation et le répertoire ou stocker les rapports de livrable.
- README.txt : Le readme du livrable 1
- README2.txt : ce document
- Makefile pour compiler soit en mode debug (afficher les traces du programme) soit en mode release (produit final)
- Doxyfile : fichier de configuration du générateur automatique de documentation doxygen

  -Génération de l'archive :Tapez "make archive". Vous aurez produit un fichier se terminant par "tgz". C'est l'archive que vous nous enverrez par email.

    -Documentation du code
Si vous voulez documenter votre code, regardez la syntaxe appropriée pour les tags Doxygen :
http://www.stack.nl/~dimitri/doxygen/docblocks.html
Une fois votre code commente, tapez "make documentation" (nous vous fournissons un Doxyfile qui ira bien pour ce que vous avez a faire).
Note : il faut avoir installé le package doxygen. Sous Ubuntu / Debian, tapez : sudo apt-get install doxygen