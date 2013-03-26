=================================================
=====               RENDU n°2               =====
=================================================






===== Repartition des tâches =====

Depuis le précédent rendu, les tâches ont été réparties essentiellement comme suit :
Alexandre :
	- UnassignedBucket : modularisation du code et amélioration (ajout/suppression de variables libres en O(1)).
	- Implémentation de l'heuristique RAND
	- Création d'un petit programme (gen.c) qui génère des tests selon des paramètres récupérés en entrée.
	  (il crée aussi un .sh qui permet de lancer un sat solveur sur le batch de fichiers d'entrée créé)
	- Les fichiers liés à l'utilisation de gnuplot (Moulinette)
	- Tests des différentes versions/améliorations

Julien :
	- Unification du moteur du solveur pour les watched literals et le reste
	- Création de Variable (pour utiliser l'adresse de l'objet comme identifiant de variable)
	- SmartClause : fonctionne comme ConstAssignClause, mais évite la propagation des littéraux dans les clauses déjà satisfaites
	- SmartWatchedClause : même optimisation que SmartClause, mais pour les Watched.
	- chooseMOMS (dans UnassignedBucket) : implémentation de l'heuristique MOMS
	- compile_bench.sh : script pour compiler les différentes variantes du programme et générer les exécutables

On a fait en commun les analyses finales et conclusions sur les différentes versions en fonction des types d'entrées.
	





===== Fonctionnement du programme =====

Globalement, le code fonctionne de la même manière qu'au rendu précédent.
On peut noter les changements suivants :

	- ajout d'une classe Variable pour rendre le code plus clair et modulable.
Le but est d'avoir un objet pour chaque variable, et ainsi utiliser son adresse comme identifiant de variable (au lieu d'un numéro).
Il contient la valeur courrante de chaque variable, ainsi que la liste des clauses qu'elle surveille.

	- ajout d'une structure UnassignedBucket, qui gère la liste des variables libres.
C'est cet objet qui sert d'interface pour les heuristiques de choix de variable non assignée.
Il contient une liste des variables libres (ni assignées, ni déduites en attente d'être assignées),
et un tableau d'indirection pour trouver la position d'une variable dans la liste en temps constant. 

	- ajout des implémentations de clause SmartClause et SmartWatchedClause :
Ces deux implémentations fonctionnent de la même manière que ConstAssignClause et WatchedClause,
mais évite de propager les litéraux dans les clauses déjà satisfaites.

	- Les fonctions setLitTrue et setLitFalse renvoie un booléen :
True indique que le littéral passé en paramêtre ne doit plus surveiller la clause.
Ceci sert pour délier efficacement une variable et une clause (dans Watched et Smart); et résoud un problème d'implémentation qui était présent dans Variable::propagateVariable, qui obligeait à dupliquer la listes des clauses dans laquelle la variable était présente. 

	- Les implémentations des clauses sont maintenant dans le .hh où elles sont définies au lieu d'un .inline.hh séparé
Les implémentations sont toutes assez courtes, et le code est plus clair/maintenable (pas besoin de chercher deux fichiers différents pour avoir une implémentation)






===== Compilation du code du solveur =====

La compilation se fait en utilisant `make`. L'exécutable produit est 'release'.
Il est préférable de faire un `make clean` avant (surtout lorsque plusieurs compilations sont faites avec des options différentes)
On précise au moment de la compilation l'implémentation des clauses voulue, et l'heuristique choisie :
	make CLAUSE=[clause] CHOOSE=[heuristique] VERBOSE=[verbose_mode]
avec:
[clause]       = BasicClause, ConstAssignClause, SmartClause, WatchedClause, SmartWatchedClause
[heuristique]  = BASIC, RAND, MOMS
[verbose_mode] = entier entre 0 et 10, précise le détail de ce qu'affiche le programme :
    0: rien
    1: uniquement le résultat du programme
    2: quelques warnings sur les erreurs d'entrées/sorties
    3: affiche la pile des assignations/déductions
   >3: de plus en plus de détails sur le déroulement du programme
Les options peuvent être omises ou précisées dans n'importe quel ordre.
Les valeurs par défaut sont [clause]=SmartClause, [heuristique]=BASIC et [verbose_mode]=1

Est également présent le script compile_bench.sh, qui compile toutes les variantes avec VERBOSE=0,
et les déplace dans Moulinette/Executables en les renommant de manière adéquate.

Il est aussi possible de compiler en debug (make debug), ou en profile (make profile) pour profiler le code (avec valgrind par exemple).
L'option VERBOSE est fixée (à 10 et 0) dans ces deux modes.
Les exécutables produits sont 'debug' et 'profile'.






===== Fonctionnement/utilisation du générateur de tests =====

On compile le fichier gen en un exécutable gen avec :
	cd ./GenTest
	gcc -Wall -Wextra -O2 -o gen gen.c

On lance alors ./gen v c k n. Ceci a pour effet de créer :
	* Un dossier tests-v-c-k-n contenant n fichiers à v variables, c clauses, toutes de taille k.
	  Ces fichiers d'entrés sont générés aléatoirement.
	* Un script batch-v-c-k-n.sh, par exemple :
		#!/bin/bash
		for i in tests-v-c-k-n/*.cnf
		do
			$* < $i
		done
		exit 0
	Ce script lance l'exécutable passé en paramètre sur tous les tests du dossier associé.
Il faut alors rajouter manuellement, pour chaque courbe : script-plot.p et run-tests.sh.
On a un dossier "Moulinette", qui contient tout ce qui concerne les benchmarks. Dedans :
	* Un dossier "Exécutables" qui contient les différentes versions du solveur.
	* Des dossiers, un par courbe, contenant script-plot.p, run-tests.sh, bath-v-c-k-n, ainsi que le dossier d'entrées associé.
		- script-plot.p contient juste les informations sur le nombre de courbes à tracer ainsi que le nom des graduation de l'axe des abscisses, le style de tracé, et la sortie de gnuplot.
		- run-tests.sh va exécuter chacun des exécutables spécifiés à l'intérieur sur chacun des batch-v-c-k-n.sh et écrire le temps pris dans comparaison.dat.
Ainsi pour lancerle benchmark sur une série d'entrées d'un certain type, il suffit de faire ./run-tests.sh, puis de tracer la courbe avec "gnuplot -persist script-plot.p".




===== Fonctionnement et utilisation de la moulinette =====

Fonctionnement et utilisation du générateur de tests et des scripts pour gnuplot :
On compile le fichier gen.c en un exécutable gen.

