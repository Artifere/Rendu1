        Rendu1 du Projet2 - L3
        ======================





Sans Watched Literals :
=======================
compilation :
    cd src-nowatched
    make
exécution :
    ./release < ../sample/ex1.cnf

notes à propos du code :

Nous avons surtout cherché à optimiser cette partie du code (sans watched literals), et à la rendre le plus fonctionnelle possible.


Le code utilise une pile des assignations de litéral couplés avec un booléen indiquant si cette assignation était libre ou contrainte.
Il utilise en plus une pile des déductions faites mais pas encore propagées.
Il fonctionne de la manière suivante :
	tant qu'on a pas assigné toutes les variables
		si déductions = vide
			choisir un literal libre et une valeur booléenne
		sinon choisir une déduction

		propager la nouvelle valeur dans les clauses où la variable apparaît.
		pour chaque clause dont on peut alors déduire un littéral :
			si ce littéral est libre (pas encore assigné)
				ajouter le littéral aux déductions
			sinon, si il contredit une valeur déjà assignée ou déduite
				contradiction
			sinon (déductions déjà faite précédemment)
				ne rien faire

		si contradiction :
		faire le callback :
			vider deductions
			faire
				si plus de choix (pile des assignations vide)
					renvoyer faux (UNSAT)
				sinon :
					on concidère la variale du haut de _stackCallback :
					si cette variable est un choix libre
						ajouter son contraire à deductions
					la liberer dans toutes les clauses où elle apparaît
			tant que deductions n'est pas vide
		fin faire callback
	fin tant que
	renvoyer vrai (SAT)


Il a été fait de telle manière que différentes implémentations des Clauses sont utilisables.

On peut utiliser ces implémentations de deux manières :
Soit on n'utilise qu'un type de Clause :
    dans ce cas, les méthodes de manipulation de clauses peuvent êtres inlinées
    le code gagne beaucoup en vitesse (jusqu'à un facteur 2)
    Pour l'utiliser, il faut vérifier que le flag -DINLINED_CLAUSE est bien défini dans le Makefile
    l'implémentation de clause utilisée est celle spécifiée en haut du fichier satSolver.hh
    (par la ligne  typedef ConstAssignClause UsedClause)
Soit on veut pouvoir utiliser plusieurs implémentations différentes :
    ce cas peut arriver par exemple si certaines implémentations se comportent mieux sur les petites clause, et d'autres sur les grandes clauses.
    (on peut imaginer d'autres critères)
    pour l'utiliser, il faut suppimmer l'option -DINLINED_CLAUSE du Makefile
    actuellement, une seule implémentation est utilisée
    (elle est spécifiée de la même manière que celle utilisée lorsque INLINED_CLAUSE est activé)
    donc pour le moment, la seule différence avec la méthode précédente, c'est que celle-ci est plus lente

il y a actuellement deux implémentations différentes :
BasicClause :
    une version simple de l'implémentation :
    la clause contient l'ensemble de ses littéraux libres (non encore assignés), et la pile de ses assignations
    quand on assigne un littéral contenu dans la clause, la clause se "gèle" jusqu'à ce qu'on libère cette variable.
    quand on assigne un littéral dont le contraire est contenu dans la clause, on passe cette variable de l'ensemble des littéraux libres vers le sommet de la pile
    l'assignation (et la libération) d'une variable dans une clause se fait donc en O(log(C)) ou C est la taille de la clause
    on sait donc facilement la liste des variables libres de la clause, et ce à tout moment
ConstAssignClause :
    une version un peu plus évoluée de la précédente :
    l'assignation (et la libération) d'une variable dans une clause se fait en temps constant
    elle contient le nombre de littéraux libre, et la somme de ces littéraux :
    quand on assigne un littéral contenu dans la clause, la clause se "gèle" jusqu'à ce qu'on libère cette variable.
    quand on assigne un littéral dont le contraire est contenu dans la clause, on soustrait ce littéral de la somme des littéraux libres
    l'assignation (et la libération) d'une variable dans une clause se fait donc en temps constant
    on également récupérer le dernier littéral libre de la clause lorsqu'il n'en reste plus qu'un

Par défaut, INLINED_CLAUSE est activée, et l'implémentation utilisée est ConstAssignClause





Avec Watched Literals :
=======================
compilation :
    cd src-watched
    make
exécution :
    ./release < ../sample/ex1.cnf

notes à propos du code :

Le code de cette version est assez similaire à celui sans watched litterals.
Cependant, une des particularités des Watched Litterals est qu'on ne propage pas la libération des variables lorsque on a détecté une contradiction, et il est nécessaire de manipuler les liens entre les variables et les clauses (lorsqu'on change de littéral surveillé).
Ces différences ont imposées de petites différences entre les interfaces des classes, empechant de garder une interface commune.
Nous avons donc séparé les deux implémentation entre la version avec et sans Watched Litterals (pour plus de simplicité dans le code).
Une autre option aurait été de mélanger les deux codes (avec des macros et des options de compilation pour choisir le mode souhaité).





Autour du code :
================

Nous avons sérialisé une série de tests pour vérifier no programmes et en mesurer le temps d'exécution.
Les tests sont dans sample/testsSatisfiable et sample/testsUnsatisfiable.
Nous avons entre-autres pu mesurer un gain substantiel entre le mode INLINED_CLAUSE et sans ce mode;
et avec l'utilisation de ConstAssignClause par rapport à BasicClause.
Cependant, le code avec les Watched Litteral s'est révélé beaucoup plus lent que les autres implémentations.
Nous avons tenté quelques autre approches et optimisations pour le rendre plus rapide
  (1 ou 3 watched litterals, différentes structures de données, propagation des littéraux vrais ou non, ...)
  mais il reste plus lent que le code sans watched litterals.
La version courante est celle à deux watched litterals par clause,
et dans laquelle on propage les littéraux dans les clauses qui les contiennent
(en plus de celles qui contiennent leur contraire).



