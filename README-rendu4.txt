=================================================
=====               RENDU n°4               =====
=================================================






===== Répartition des tâches =====

Depuis le précédent rendu, les tâches ont été réparties essentiellement comme suit :
Alexandre :
	- Implementation du Carré Latin
	- Implementation du Sudoku
	- Implementation de la coloration de graphe
	- Implementation du solveur SMT
Julien :
	- Correction et finalisation du rendu 3 :
	    faire fonctionner les Watched Literals et autres corrections de bugs
	    modifier le code pour prendre en compte les remarques à propos du rendu précédent
	- Implementation de l'additionneur
	- Implementation de la transformation de Tseitin






===== Compilation du code du solveur =====

Pour compiler les exécutables correspondants à chaque heuristique du solveur.
Les binaires compilés sont placés dans le dossier bin/ à la racine du projet.
Ils seront ensuite utilisés par les différentes parties du projet (GenTest, Moulinette, ...)
depuis la racine du projet :
  cd src
  ./compile_test.sh
note :
   il est possible de compiler les exécutables pour les benchmarks avec compile_bench.sh
   les exécutables normaux ont un nom commençant par exe, ceux pour le benchmark commencent par bench


Il est toujours possible de compiler un exécutable "sur mesure" :
En utilisant `make`, depuis le dossier src. L'exécutable produit est 'release'.
Il est préférable de faire un `make clean` avant (surtout lorsque plusieurs compilations sont faites avec des options différentes)
On précise au moment de la compilation l'implémentation des clauses voulue, et l'heuristique choisie :
	make CLAUSE=[clause] CHOOSE=[heuristique] INIT_SORT=[val_sort] VERBOSE=[verbose_mode] INTERACT=[val_interact]
es options peuvent être omises ou précisées dans n'importe quel ordre.
Les valeurs par défaut sont [clause]=SmartClause, [heuristique]=BASIC, [val_sort]=0, [verbose_mode]=1 et [val_interact]=0
avec:
[clause]       = SmartClause, WatchedClause
[heuristique]  = BASIC, RAND, DLIS
[val_sort]     = 0 ou 1  (si on doit ou non faire un tri des variables suivant leur nombre d'occurence)
[verbose_mode] = entier entre 0 et 10, précise le détail de ce qu'affiche le programme :
    0 : rien
    1 : uniquement le résultat du programme
    2 : quelques warnings sur les erreurs d'entrées/sorties
    3 : affiche la pile des assignations/déductions
   >3 : de plus en plus de détails sur le déroulement du programme
[val_interact] = 0 ou 1 (mode interactif ou non)

Il est aussi possible de compiler en debug (make debug), ou en profile (make profile) pour profiler le code (avec callgrind par exemple).
L'option VERBOSE est fixée (à 1 et 0) dans ces deux modes.
Les exécutables produits sont 'debug' et 'profile' et non plus 'release'.






===== Points négatifs =====

Il manque certains points demandés :
 - pas de preuve par résolution dans le mode interactif
 - pas de fichier de documentation produit à la fin de l'exécution
Il est certainement possible d'améliorer la fonction resolve pour gagner un peu en efficacité (la fusion de clauses est faite de manière assez naïve en utilisant la bibliothèque standard, sans utiliser les informations que l'on a sur la forme des listes)





===== Résultats des courbes =====
L'apprentissage des clauses est compétitif : le surcoût (important) engendré par l'apprentissage des clause est compensé par ce que l'apprentissage apporte.
Cependant, le gain n'est pas assez important pour que cette méthode batte les meilleurs heuristiques dont on disposait sans apprentissage. Cependant, on peut peut-être
passer devant les autres heuristiques si l'on optimise un peu la façon dont on fait la résolution.





===== Fonctionnement du programme =====

Changements depuis la dernière version :

	- Pour pouvoir gérer le mode interactif, le programme ne lit plus le problème sur l'entrée standard. Il faut lui passer en paramètre
le nom du fichier d'entrée : "release test.cnf" au lieu de "release < test.cnf". On a modifié les fichiers de scripts pour prendre cela en compte.
Cependant, pour comparer courbes avec la version précédente du rendu, nous avons adapté l'entrée du rendu précédent. Les sources ne sont pas fournies, le but étant seulement de nous faciliter un peu la génération des courbes.

	- On a ajouté quelques méthodes relatives à l'apprentissage de clause : on a besoin de savoir si une variable est plus « vieille » qu'une autre (ie si elle a été assignée avant), notamment si une variable est plus vieille que le dernier pari fait. Ces fonctions utilisent toutes leur attribut _posInTable (cf point suivant).

	- On n'utilise plus ni UnassignedBuckets ni toutes les structures de piles pour maintenir les variables et la pile des assignations des variables :
On utilise pour remplacer tout ça un unique tableau global de Variable* pour représenter la pile courante des variables, la file d'attente des assignations, l'état des variables et pour savoir si elle découlent du dernier pari fait :
Ce tableau (un attribut statique de la classe Variable) est organisé comme il suit :
	[variables assignées|variables déduites|variables libres]
On retrouve donc comme dans les rendus précédents les notions de variables assignées, variables libres, et variables déduites (ie celles dont on connaît la valeur, mais en attente d'être propagées dans les clauses car plusieurs déductions ont lieu en même temps).
La partie [variables assignées| représente la « pile » d'exécution du programme : les variables s'y trouvent dans l'ordre dans lequel elle on été assignées.
	Une autre structure, _stackBacktrack, contient des itérateurs sur les variables qui ont été assignées suite à un pari (et non une déduction).
	De plus, chaque variable possède un itérateur _posInTable qui permet de savoir rapidement ou se trouve cette variable dans le tableau (pour pouvoir déduire une variable libre quelconque en O(1)), mais aussi qui permet de comparer la position de la variable par rapport aux autres, notamment à _stackBacktrack.back() (et donc de savoir si la variable est une conséquence du dernier pari ou non).
	Les séparations entre ces parties sont indiquées par des itérateurs (eux aussi statiques dans Variable) _endAssigned et _endDeducted.
Lorsque une variable n'est pas dans la partie libre, sa valeur est donnée par son attribut _varState.
Cette organisation permet de passer rapidement une variable d'une zone à une autre, simplement en incrémentant/décrémentant _endAssigned et endDeducted (sauf pour passer de libre à déduit, où on doit échanger les positions de deux variables, mais cela reste en temps constant).

	- On a un peu modifié les variables pour qu'elles retiennent de quelle clause leur valeur provient. L'apprentissage des clauses de taille 1 à posé problème (pour les watched literals, mais aussi à cause de détails de fonctionnement du moteur), et un soin particulier a dû être apporté pour le faire marcher sans problème (notamment : on ne crée jamais de clause de taille 1, mais on simule leur existence lorsqu'on en a besoin).

	- on a ajouté deux fonctions resolve et createConflictGraph pour chercher l'origine des conflits. Ces deux fonctions fonctionnent de manière différente :
resolve à une approche ascendante : elle cherche jusqu'où elle doit remonter dans les clauses en partant d'un conflit pour apprendre une nouvelle variable.
createConflictGraph à une approche globale : elle parcourt toutes les variables qui sont conséquence du dernier pari, et crée les arêtes du graphe dans le désordre.



