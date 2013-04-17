=================================================
=====               RENDU n°3               =====
=================================================






===== Répartition des tâches =====

Depuis le précédent rendu, les tâches ont été réparties essentiellement comme suit :
Alexandre :
	- Codage de la fonction de résolution de conflits (création de la clause à apprendre)
	- Première version de la fonction générant le fichier .dot pour le graphe de conflit
	- Modifications de constructeurs des clauses pour intégrer l'apprentissage de clauses (un peu)
	- Reprise des tests "moulinette" pour comparer l'apprentissage avec les anciennes versions
            (et adaptation des anciennes versions pour que tout soit compatible)
Julien :
	- Modification de la structure de donnée associée au backtrack :
            on a une pile de variables (assignées, puis déduites, puis libres) au lieu de 3 tableaux
            on se débarrasse aussi de UnassignedBucket pour simplifier un peu le code
	- Version finale de la fonction générant le graphe de conflit
	- Modifications de constructeurs des clauses pour intégrer l'apprentissage de clauses (majeure partie)





===== Compilation du code du solveur =====

La compilation se fait en utilisant `make`, depuis le dossier src. L'exécutable produit est 'release'.
Il est préférable de faire un `make clean` avant (surtout lorsque plusieurs compilations sont faites avec des options différentes)
On précise au moment de la compilation l'implémentation des clauses voulue, et l'heuristique choisie :
	make CLAUSE=[clause] CHOOSE=[heuristique] INIT_SORT=[val_sort] VERBOSE=[verbose_mode] INTERACT=[val_interact]
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
Les options peuvent être omises ou précisées dans n'importe quel ordre.
Les valeurs par défaut sont [clause]=SmartClause, [heuristique]=BASIC, [val_sort]=0, [verbose_mode]=1 et [val_interact] = 0

Est également présent le script compile_bench.sh, qui compile toutes les variantes avec VERBOSE=0,
et les déplace dans Moulinette/Executables en les renommant de manière adéquate.

Il est aussi possible de compiler en debug (make debug), ou en profile (make profile) pour profiler le code (avec callgrind par exemple).
L'option VERBOSE est fixée (à 1 et 0) dans ces deux modes.
Les exécutables produits sont 'debug' et 'profile'.





===== Points négatifs =====

Il manque certains points demandés :
 - pas de preuve par résolution dans le mode interactif
 - pas de fichier de documentation produit à la fin de l'exécution

Il est certainement possible d'améliorer la fonction resolve pour gagner un peu en efficacité (la fusion de clauses est faite de manière assez naive en utiisant la bibliothèque standard, sans utiliser les informations que l'on a sur la forme des listes)





===== Résultats des courbes =====
L'apprentissage des clauses est compétitif : le surcoût (important) engendré par l'apprentissage des clause est compensé par ce que l'apprentissage apporte.
Cependant, le gain n'est pas assez important pour que cette méthode batte les meilleurs heuristiques dont on disposait sans apprentissage.





===== Fonctionnement du programme =====

Changements depuis la dernière version :

	- Pour pouvoir gérer le mode interactif, le programme ne lit plus le problème sur l'entrée standard. Il faut lui passer en paramètre
le nom du fichier d'entrée : "release test.cnf" au lieu de "release < test.cnf"

	- On a ajouté quelques méthodes relatives à l'apprentissage de clause : on a besoin de savoir si une variable est plus "vieille" qu'une autre (ie si elle a été assignée avant), notamment si une variable est plus vieille que le dernier pari fait. Ces fonctions utilisent toutes leur attribut _posInTable (cf point suivant).

	- On n'utilise plus ni UnassignedBuckets ni toutes les structures de piles pour maintenir les variables et la pile des assignations des variables :
On utilise pour remplacer tout ça un unique tableau global de Variable* pour représenter la pile courante des variables, la file d'attente des assignations, l'état des variables et pour savoir si elle découlent du dernier pari fait :
Ce tableau (un attribut statique de la classe Variable) est organisé comme il suit :
	[variables assignées|variables déduites|variables libres]
On retrouve donc comme dans les rendus précédents les notions de variables assignées, variables libres, et variables déduites (ie celles dont on connaît la valeur, mais en attente d'être propagée dans les clauses car plusieurs déductions ont lieu en même temps).
La partie [variables assignées| représente la "pile" d'execution du programme : les variables s'y trouvent dans l'ordre dans lequel elle on été assignées.
	Une autre structure, _stackBacktrack, contient des itérateurs sur variables qui ont étés assignées suite à un pari (et non une déduction)
	De plus, chaque variable possède un itérateur _posInTable qui permet de savoir rapidement ou se trouve une variable dans le tableau (pour pouvoir déduire une variable libre quelconque en O(1)), mais aussi qui permet de comparer la position de la variable par rapport aux autres, notamment à _stackBacktrack.back() (et donc savoir si la variable est une conséquence du dernier pari ou non).
	Les séparations entre ces parties sont indiquées par des itérateurs (eux aussi statiques dans Variable) _endAssigned et _endDeducted.
Lorsque une variable n'est pas dans la partie libre, sa valeur est donnée par son attribut _varState
Cette organisation permet de passer rapidement une variable d'une zone à une autre, simplement en incrémentant/décrémentant _endAssigned et endDeducted (sauf pour passer de libre à déduit, où on doit échanger les positions de deux variables, mais cela reste en temps constant).

	- On a un peu modifié les variables pour qu'elles retiennent de quelle clause leur valeur provient. L'apprentissage des clauses de taille 1 à posé problème (pour les watched literals, mais aussi à cause de détails de fonctionnement du moteur), et un soin particulier à du être apporté pour le faire marcher sans problème (notamment : on ne crée jamais de clause de taille 1, mais on simule leur existence lorsqu'on en a besoin).

	- on a ajouté deux fonctions resolve et createConflictGraph pour chercher l'origine des conflits. Ces deux fonctions fonctionnent de manière différente :
resolve à une approche ascendante : elle cherche jusqu'où elle doit remonter dans les clauses en partant d'un conflit pour apprendre une nouvelle variable.
createConflictGraph à une approche globale : elle parcours toutes les variables qui sont conséquence du dernier pari, et crée les arêtes du graphe dans le désordre.


