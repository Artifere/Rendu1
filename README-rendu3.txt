=================================================
=====               RENDU n°3               =====
=================================================






===== Répartition des tâches =====

Depuis le précédent rendu, les tâches ont été réparties essentiellement comme suit :
Alexandre :
	- Codage de la fonction de résolution de conflits (création de la clause à apprendre)
	- Première version de la fonction générant le fichier .dot pour le graphe de conflit
	- Modifications de constructeurs des clauses pour intégrer l'apprentissage de clauses (un peu)


Julien :
	- Modification de la structure de donnée associée au backtrack : on a une pile de variables (assignées, puis déduites, puis libres) au lieu de 3 tableaux
	- Version finale de la fonction générant le graphe de conflit
	- Modifications de constructeurs des clauses pour intégrer l'apprentissage de clauses (majeure partie)
	- Intégration du module UnassignedBucket dans le module Variable



===== Fonctionnement du programme =====

Changements depuis la dernière version :

	- pour pouvoir gérer le mode interactif, le programme ne lit plus le problème sur l'entrée standard. Il faut lui passer en paramètre
le nom du fichier d'entrée : "release test.cnf".

	- 

	- ajout d'une structure UnassignedBucket, qui gère la liste des variables libres.
C'est cet objet qui sert d'interface pour les heuristiques de choix de variable non assignée.
Il contient une liste des variables libres (non assignées), et un tableau d'indirection pour 
trouver la position d'une variable dans la liste en temps constant. 

	- ajout des implémentations de clause SmartClause et SmartWatchedClause :
Ces deux implémentations fonctionnent de la même manière que ConstAssignClause et WatchedClause,
mais évitent de propager les litéraux dans les clauses déjà satisfaites.
On repère au moment d'assigner un litéral dans une clause que celle-ci est déjà satisfaite,
et dans ce cas, on « délie » la variable et la clause (on ne surveille plus la clause à travers cette variable).
Lorsque la variable qui rendait la clause vraie est libérée, on lie à nouveau toutes les variables qu'on avait délié à la clause.

	- Les fonctions setLitTrue et setLitFalse renvoient un booléen :
True indique que le litéral passé en paramêtre ne doit plus être surveillé pour la clause.
Ceci sert pour délier efficacement une variable et une clause (dans Watched et Smart); et résoud un problème d'implémentation qui était présent dans Variable::propagateVariable, et qui obligeait à dupliquer la listes des clauses dans laquelle la variable était présente. 

	- Les implémentations des clauses sont maintenant dans le .hh où elles sont définies au lieu d'un .inline.hh séparé.
Les implémentations sont toutes assez courtes, et le code est plus clair/maintenable (pas besoin de chercher dans deux fichiers différents pour une même implémentation)






===== Compilation du code du solveur =====

La compilation se fait en utilisant `make`, depuis le dossier src. L'exécutable produit est 'release'.
Il est préférable de faire un `make clean` avant (surtout lorsque plusieurs compilations sont faites avec des options différentes)
On précise au moment de la compilation l'implémentation des clauses voulue, et l'heuristique choisie :
	make CLAUSE=[clause] CHOOSE=[heuristique] INIT_SORT=[val_sort] VERBOSE=[verbose_mode] INTERACT=[val_interact]
avec:
[clause]       = BasicClause, ConstAssignClause, SmartClause, WatchedClause, SmartWatchedClause
[heuristique]  = BASIC, RAND, DLIS
[val_sort]     = 0 ou 1  (si on doit ou non faire un tri des variables suivant leur nombre d'occurence)
[verbose_mode] = entier entre 0 et 10, précise le détail de ce qu'affiche le programme :
    0 : rien
    1 : uniquement le résultat du programme
    2 : quelques warnings sur les erreurs d'entrées/sorties
    3 : affiche la pile des assignations/déductions
	>3 : de plus en plus de détails sur le déroulement du programme
[val_interact] = 0 ou 1 (interaction ou non)
Les options peuvent être omises ou précisées dans n'importe quel ordre.
Les valeurs par défaut sont [clause]=SmartClause, [heuristique]=BASIC, [val_sort]=0, [verbose_mode]=1 et [val_interact] = 0

Est également présent le script compile_bench.sh, qui compile toutes les variantes avec VERBOSE=0,
et les déplace dans Moulinette/Executables en les renommant de manière adéquate.

Il est aussi possible de compiler en debug (make debug), ou en profile (make profile) pour profiler le code (avec callgrind par exemple).
L'option VERBOSE est fixée (à 1 et 0) dans ces deux modes.
Les exécutables produits sont 'debug' et 'profile'.






===== Fonctionnement/utilisation du générateur de tests =====

On compile le fichier gen en un exécutable gen avec :
	cd ./GenTest
	gcc -Wall -Wextra -O2 -s -o gen gen.c

On lance alors ./gen v c k n. Ceci a pour effet de créer :
	- Un dossier tests-v-c-k-n contenant n fichiers à v variables, c clauses, toutes de taille k.
	  Les fichiers d'entrés sont générés aléatoirement.
	- Un script batch-v-c-k-n.sh, par exemple :
	   #!/bin/bash
	   for i in tests-v-c-k-n/*.cnf
	   do
	      $* $i
	   done
	   exit 0
	Ce script lance l'exécutable passé en paramètre sur tous les tests du dossier associé.
	Il suffit alors de mesurer de temps d'exécution du script.






===== Fonctionnement/utilisation de la moulinette =====

On utilise les tests créés par le générateur de tests.

Le dossier Moulinette contient tout ce qui concerne les benchmarks :
	- Un dossier Executables qui contient les différentes versions du solveur
	- Un dossier par courbe, contenant :
		- des dossiers tests-v-c-k-n (générés par le générateur de tests)
		- sript-plot.p (informations sur le nombre de courbes à tracer, le nom des graduations de l'axe des abscisses, le style de tracé, et la sortie de gnuplot)
		- run-tests.sh (éxécute chacun des éxécutables spécifiés dans le script sur chacun des batch-v-c-k-n.sh, et écrit le temps pris dans comparaison.dat)
		- comparaison.dat (le résultat des temps d'exécutions des solveurs)

Dans le dossier d'une courbe :
	./run-tests.sh    (pour lancer les calculs (temps d'exécution) d'une courbe)
	gnuplot -persist script-plot.p  (pour tracer/visualiser la courbe)
NOTE :
	Pour que gnuplot affiche la courbe directement à l'éxécution de la commande,
	il est nécessaire que le packet gnuplot-x11 soit installé sur la machine (et non pas gnuplot-nox)


	


===== Points négatifs =====








===== Résulats des courbes =====

