 Le programme affiche la liste des léxèmes du livrable 1, le dictionnaire d'instruction qui nous permet de faire l'analyse grammaticale mais aussi la file de .BSS, la file .TEXT, la file .DATA et la file des symboles qui ont été modifié dans leurs structures afin de s'adapter aux exigences du livrable 3 (notamment afin de gérer les pseudo-instructions).

Dans le fichier src l'automate du livrable 1, la gestion des erreurs, le calcul des décalages et l'analyse grammaticale sont gérées dans automate_gramm.c et affiché à l'aide de fct_affichage.c . Dans le fichier include on peut tous les « .h » qui définissent toutes les structures.

La liste des instructions pour exécuter le code :

    -Compilation du code grâce à la commande "make debug" à la racine du dossier

    -Appel de l'exécutable "as-mips" avec comme argument le chemin du fichier à compiler sous la forme ./as-mips FICHIER.S . Sans argument, ça sera miam_sujet.s qui sera exécuté par défaut.

    -Observer les collections qui s'affichent

    -(Optionnel) Générer la documentation grace à "make docu" si vous possez les packets Doxygen

    -src : qui contient le code C de votre interpréteur.

    -include : qui contient toutes les définitions de types et prototypes de votre programme. Lisez attentivement les headers avant de commencer à coder et de les modifier.

    -doc : contient la documentation et le répertoire ou stocker les rapports de livrable.

    -README3.txt : ce document

    -Makefile pour compiler soit en mode debug (afficher les traces du programme) soit en mode release (produit final)

    -Doxyfile : fichier de configuration du générateur automatique de documentation doxygen

    -Génération de l'archive :Tapez "make archive". Vous aurez produit un fichier se terminant par "tgz". C'est l'archive que vous nous enverrez par email.

    -Documentation du code Si vous voulez documenter votre code, regardez la syntaxe appropriée pour les tags Doxygen :http://www.stack.nl/~dimitri/doxygen/docblocks.html Une fois votre code commente, tapez "make documentation" (nous vous fournissons un Doxyfile qui ira bien pour ce que vous avez a faire). Note : il faut avoir installé le package doxygen. Sous Ubuntu / Debian, tapez : sudo apt-get install doxygen
