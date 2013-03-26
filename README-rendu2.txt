Depuis le précédent rendu, les tâches ont été réparties essentiellement comme suit :
	* Alexandre :
		- UnassignedBucket : modularisation du code et amélioration : avec un système de deux tableaux d'indirection, les opérations de choix/ajout/suppression de variables libres se fait en temps constant.
		- Implémentation de RAND
		- Création d'un petit programme (gen.c) qui génère des tests selon des paramètres récupérés en entrée. De plus, le programme crée également un .sh qui permet de lancer un sat solveur sur le batch de fichiers d'entrée créé.
		- Les fichiers liés à l'utilisation de gnuplot
		- Tests des différentes versions/améliorations
		
	* Julien :
	- unification du moteur du solveur pour les watched literals et le reste
	- smartClause
	- smartWatched
	- chooseMOMS
	- compile_bench
On a fait en commun les analyses finales et conclusions sur les différentes versions en fonction des types d'entrées.
	


Fonctionnement de UnassignedBucket :
On a un tableau qui contient la liste des variables libres (ni assignées ni déduites).









Fonctionnement et utilisation du générateur de tests et des scripts pour gnuplot :
On compile le fichier gen.c en un exécutable gen.
On lance alors ./gen v c k n. Ceci a pour effet de créer :
	* Un dossier tests-v-c-k-n contenant n fichiers à v variables, c clauses, toutes de taille k. Ces fichiers d'entrés sont générés aléatoirement.
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