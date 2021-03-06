=================================================
=====               RENDU n°4               =====
=================================================






===== Répartition des tâches =====

Depuis le précédent rendu, les tâches ont été réparties essentiellement comme suit :
Alexandre :
	- Implémentation du Carré Latin
	- Implémentation du Sudoku
	- Implémentation de la coloration de graphe
	- Implémentation du solveur SMT
Julien :
	- Correction et finalisation du rendu 3 :
	    faire fonctionner les Watched Literals et autres corrections de bugs
	    modifier le code pour prendre en compte les remarques à propos du rendu précédent
	- Implémentation de l'additionneur
	- Implémentation de la transformation de Tseitin






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
[val_sort]     = 0 ou 1  (si on doit ou non faire un tri des variables suivant leur nombre d'occurrence)
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
rendu 3 :
 - pas de preuve par résolution dans le mode interactif
 - pas de fichier de documentation produit à la fin de l'exécution
rendu 4 :
 - pas de carré magique fonctionnel (comme la méthode avec les carrés latins orthogonaux ne marchait pas pour les ordres 3 et 6, on avait pensé à utiliser un solveur SMT offline pour l'arithmétique, mais on n'a pas eu le temps)

Certains aspects du code peuvent être améliorés :
 - Il est certainement possible d'améliorer la fonction resolve pour gagner un peu en efficacité (la fusion de clauses est faite de manière assez naïve en utilisant la bibliothèque standard, sans utiliser les informations que l'on a sur la forme des listes)
 - L'implémentation actuelle de MOMS re-calcule entièrement les valeurs associées à chaque variable, ce qui a un coût important. Une implémentation efficace pourrait mettre à jour en temps réel ces valeurs lorsqu'elles sont modifiées par les assignations.






===== Modifications par rapport au rendu précédents =====

La partie GenTest à été modifiée :
On peut maintenant lancer une série de tests sur une série d'exécutable, pour tester la validité des programmes.
Exemple d'utilisation :
  cd GenTest
  ./gen <nbrVar> <nbrClause> <tailleClause> <nbrDeTests>
  ./test-<nbrVar>-<nbrClause>-<tailleClause>-<nbrDeTests>.sh ../bin/exe*
Ces commandes lancent une série de tests sur des problèmes sat aléatoires, sur tous les exécutables contenus dans bin
Il est aussi possible de lancer le script bench créé par ./gen pour avoir une idée du temps d'exécution de ces tests.

On a rajouté l'heuristique MOMS qui manquait au rendu précédent.
Voir les courbes dans Graphes moulinette pour les comparaison entre les différentes heuristiques.
Sur les tests aléatoires, MOMS est plus lent que DLIS, et surtout est très sensible au pics à la limite entre problèmes satisfiables et unsatisfiables (au ratio nbrClause/nbrVars = 4.5).
Cependant, sur les extensions, c'est souvent MOMS qui a montré les meilleurs performances. De même, sur le fichier unsat-41-272.cnf (fichier qui nous a servit de référence comme problème "difficile" tout au long du projet, car malgré sa taille, minisat met 5.5s à le résoudre), SmartMOMS réalise de meilleures performances que minisat en résolvant le problème en 2.8s. (les autres heuristiques prennent des temps plus importants : SmartDLIS met 9.1s, et RAND autour de 15s)
Ces tests ont étés réalisés avant qu'on ne "répare" les watched literals pour les adapter à l'apprentissage de clauses. Ils ne font donc pas apparaître les watched literals.





=================================
===== Extensions du solveur =====
=================================

Les extensions comprennent :
	- Carré Latin
	- Additionneur
	- Sudoku
	- Coloration de graphe
	- Transformation de Tseitin
	- Satisfaction Modulo Theory
Les codes des extensions sont tous dans src/extensions
Toutes les extensions utilisent Tseitin : il faut donc compiler cette partie avant de pouvoir utiliser d'autres extensions.

Ces scripts utilisent l'exécutable src/release comme solveur.
Il est donc nécessaire de copier un binaire à cet endroit pour les faire fonctionner.
Par exemple, depuis la racine du projet, faire :
  cp bin/exeSmartMOMS src/release

Le carré latin, l'additionneur, le sudoku et la coloration de graphes sont des utilisations simples du solveur, qui utilisent Tseitin.
Ces extensions sont formée de deux exécutables indépendants :
  - un exécutable qui réalise la Transformation proprement dite
  - un exécutable Print qui lit l'assignation renvoyée par Tseitin et affiche le résultat
Il y a également un script qui s'occupe de faire le lien entre les deux et fourni une interface d'utilisation simplifiée.



===== Carré Latin =====

Formalisation du problème :
    Pour chaque case du carré, on a <n> variable, la k-ieme variable indiquant si dans la case en question est associé le nombre k. On rajoute ensuite des conditions nécessaires et suffisantes pour qu'une assignation soit valide :
    - pour chaque case, une et une seule variable associée est vraie
    - chaque nombre est présent au moins une fois sur chaque ligne
    On rajoute également une autre condition :
    - il n'y a pas deux fois le même nombre sur une ligne ou colonne
    Cette dernière condition (qui n'est pas nécessaire) permet au solveur de détecter facilement lorsque une assignation est incorrecte.
    Elle améliore grandement les temps de résolution du carré (cf les courbes résultat)

Compilation :
    cd LatinSquare
    g++ -Wall -Wextra -O2 -o latinSquarePrint latinSquarePrint.cpp
    g++ -Wall -Wextra -O2 -o latinSquareTransform latinSquareTransform.cpp

Utilisation :
    ./latinSquare.sh <n> [--no-extra-cond]
    où <n> est la longueur d'un coté du carré latin qu'on cherche à calculer
    si  --no-extra-cond est précisé, on n'utilise pas les conditions supplémentaires

Résultats :
    On remarque que les conditions supplémentaires ont un réel impact sur les temps de résolutions :
    À partir de <n>=9, la résolution sans --no-extra-cond devient beaucoup plus rapide.
    (ceci se voit en utilisant n'importe quelle heuristique, aussi bien qu'avec minisat)
   Pour information, SmartMOMS met environ 7h à résoudre ./latinSquare.sh 12 --no-extra-cond, alors qu'au bout de 14h, minisat n'a toujours pas fini :-)  Dans le cas --no-extra-cond, il y a plus de liberté sur le choix des paris, et donc les heuristiques de choix utilisées prennent plus d'importance.



===== Additionneur =====

Formalisation du problème :
    L'additionneur réalise l'addition binaire sur 32 bits comme le ferait un circuit électrique classique.
    L'addition de deux bits a_i et b_i, avec un bit de retenue entrante r_i donne un bit de sortie s_i et un bit de retenue sortante r_(i+1)
    On a les conditions suivantes :
    - s_i est la somme modulo 2 de a_i, b_i et r_i :
      Ça s'exprime facilement avec l'opérateur <=>, équivalent au contraire du ou-exclusif
    - r_(i+1) est vrai ssi deux au moins des a_i, b_i, r_i est vrai
    Comme notre implémentation de Tseitin permet l'utilisation de l'opérateur <=>, ces deux conditions s'expriment facilement.
    On ajoute ensuite l'assignation des a_i et b_i en fonction des nombres donnés en arguments.

Compilation :
    cd Addition
    g++ -Wall -Wextra -O2 -o additionPrint additionPrint.cpp
    g++ -Wall -Wextra -O2 -o additionTransform additionTransform.cpp

Utilisation :
    ./addition.sh <x> <y>
    avec <x> et <y> sont des entiers (positifs ou négatifs)
    Le programme affiche la somme de <x> et <y>.
Il y a aussi le script 
    ./addition_bench.sh <n> <x> <y>
    qui répète <n> fois l'appel à Tseitin qui résout le problème

Résultats :
    Une première remarque est que le calcul est correct !
    Le problème ne dépendant (presque) pas des entiers donnés en paramètre.
    Il est donc inutile de comparer les temps de calcul sur différentes entrées.
    De plus, la propagation est immédiate, et peut normalement être faite sans qu'aucun choix libre n'ai a être fait (le problème sat est simple à résoudre).
    Il est donc inutile de comparer différentes heuristiques entres-elles.

    Pour récupérer le fichier cnf généré, il suffit de faire :
    ./additionTransform 0 0 | ../Tseitin/tseitin
    (ce fichier à 385 variables et 1057 clauses)
    En le lançant sur minisat avec -verb=2, on remarque que minisat parvient à le résoudre en ne faisant qu'un seul choix libre, ce qui va dans le sens de notre hypothèse sur les choix libres nécessaires pour le résoudre.
    On peut cependant utiliser addition_bench.sh pour comparer plusieurs heuristiques.



===== Sudoku =====

Formalisation du problème :
    Ce problème ressemble fortement au carré latin 9x9 :
    On rajoute simplement une restriction sur le fait que les sous-carrés 3x3 contiennent une et une fois chaque nombre.

Compilation :
    cp Sudoku
    g++ -Wall -Wextra -O2 -o sudokuPrint sudokuPrint.cpp
    g++ -Wall -Wextra -O2 -o sudokuTransform sudokuTransform.cpp

Utilisation :
    ./suduku.sh <grille>
    avec <grille> un fichier décrivant une grille de sudoku
    (cf les fichiers grille_vide et grille_partielle pour des exemples)

Résultats :
    On mesure les temps pris pour remplire une grille vide de sudoku :
    (on ne compte que le temps de résolution, et pas de génération du problème,
     ni de la transformation de Tseitin) 
    Minisat      : 0.68s
    Smart   MOMS : 0.56s
    Smart   DLIS : >30s
    Watched MOMS : >30s
    Watched DLIS : >30s
    Sur ce problème, c'est Smart MOMS qui semble le mieux marcher, surpassant même minisat. Les autres sont beaucoup plus lents.



===== Coloration de graphe =====

Formalisation du problème :
    On utilise pour chaque noeud du graphe k variables, chacune indiquant si le noeud est colorié d'une certaine couleur.
    On rajoute des conditions nécessaires et suffisantes pour que ce soit une coloration du graphe :
    - chaque noeud est colorié par au moins une couleur
    - chaque noeud est colorié par au plus une couleur
    - pour chaque arête du graphe, les sommets aux extrémités de l'arête sont de couleurs différentes

Compilation :
    cd GrapheColoring
    g++ -Wall -Wextra -s -O2 -o coloringPrint coloringPrint.cpp
    g++ -Wall -Wextra -s -O2 -o coloringTransform coloringTransform.cpp

Utilisation :
    ./graphColoring.sh <k> <graphe>
    avec :
      <k> un entier >= 1.
      <graph> un fichier décrivant un graphe.
      (des exemples de graphe peuvent être trouvés dans colorTests/)
    Si le graphe est k-coloriable, un fichier colored.pdf est crée, montrant une coloration du graphe avec k couleurs
    Sinon, un message indiquant que le graphe n'est pas coloriable est affiché

Résultats :
    Le programme renvoit des résultats corrects sur de petits exemples résolus à la main.
    Il rend vite l'ame sur des exemples plus gros (sur queen11_11.col par exemple), lorsqu'on s'approche du nombre chromatique du graphe.



===== Transformation de Tseitin =====

Compilation :
    cd Tseitin
    make

Utilisation :
    Il y a deux manières de l'utiliser :
    soit sans aucun argument:
        la formule est lue sur l'entrée standart
        le problème cnf est renvoyé sur la sortie standart
    soit avec -r <nom de fichier>
        la formule est toujours lue sur l'entrée standart
        le problème cnf est écrit dans <nom de fichier>
        le programme ../../release est lancé sur ce problème
        le programme renvoit :
            soit "s UNSATISFIABLE"
            soit "s SATISFIABLE" suivit de l'assignation des variables ("v name" ou "v -name")
    Noter que la transformation admet un opérateur <=>, non demandé dans les spécifications.
    Il permet en pratique d'assigner une variable à une expression,
    ou encore de réaliser un ou-exclusif (<=> correspndant exactement à l'opposé d'un ou-exclusif)

Résultats :
    - Le programme à été concut de telle manière qu'il est capable de traiter de grandes expressions.
    Ainsi, si les premières versions dépassaient de la pile lorsqu'elle tentaient de traiter la formule du carré latin de taille 20 ou plus.
    Le parseur et la transformation en elle-même ont étés revus, et la version actuelle n'est limitée que par le temps qu'elle met à traiter les formules.
    Pour information, le problème cnf correspondant au carré latin de taille 40 fait 
    - Le temps de traitement reste très grand, de même que l'empreinte mémoire.
    Une solution à cela auraît été de concidérer que le fichier est une conjonction de formules, chaque formule étant sur une ligne distincte.
    Cela simplifierait également l'écriture de formules par des programmes anexes (plus besoin d'ajouter certaines parenthèses, ou de faire attention à ne pas écrire d'opérateur /\ à la fin du fichier)
    Cependant, cela va à l'encontre des spécifications données.




===== SMT =====
Formalisation du problème :
	On part d'une formule avec des termes construits à partir de variables et symboles de fonctions. On suppose ici que la formule est valide, dans le sens où elle est bien construite, que les symboles de fonctions ont une arité bien précise. Pour simplifier l'écriture des formules, on a introduit le symbole "!=" qui correspond à la négation de l'égalité.
	À partir du problème de départ, on crée une formule très similaire où l'on a remplacé chaque égalité par une variable de la forme c_i. On convertit cette nouvelle formule en problème CNF à l'aide de l'exécutable tseitin. Ensuite, on lance le solveur, et on vérifie à chaque fois qu'il renvoie une assignation qu'elle est valide en terme de cohérence logique (pas de "x != x"). Une fois que le solveur ne trouve pas de solution, ou que la solution qu'il renvoie est cohérente, on a terminé.
	
Implémentation :
	Pour le maintient des congruences (à chaque terme d'une classe d'équivalence on associe un unique représentant), on utilise la structure d'union find avec compression de chemins qui a de bonnes performances (en pratique linéaire en le nombre de find/union effectués). Pour parser le fichier (énumérer les égalités et tous les termes et sous-termes), on utilise la structure de pile pour faire correspondre les parenthèses ouvrantes et fermantes. Dans l'absolu cela permet de parser la formule en temps linéaire, mais concrètement ici on reparse plusieurs fois certaines parties, ce qui donne une complexité moins bonne.
	Pour vérifier une solution, on procède en deux étapes : d'abord on ajoute toutes les égalités, puis ensuite toutes les déségalités. Enfin, dès qu'on a une contradiction on s'arrête, et on peut construire alors une clause d'explication de conflit un petit peu moins naïve que celle consistant en la négation de l'assignation complète : on renvoie la négation de l'assignation des variables qu'on a considérées, sans inclure celle qu'on n'a pas examinées. De plus, si on se rend compte à un moment qu'une égalité n'ajoute pas d'information (les deux termes étaient déjà dans la même classe), on ne met pas non plus cette variable dans la clause d'explication du conflit. Pour ce qui est de la vérification, on commence par faire des unions sur toutes les égalités, ainsi que les égalités qu'elles impliquent s'il s'agit de symboles de fonctions (f(t1, ..., tn) = f(s1, ..., sn) ==> ti = si pour 1 <= i <= n). De plus, si on trouve une incohérence (f(t) = f(t,x) par exemple, ou bien f(bla) = g(blo)), on s'arrête. Ensuite on considère les diségalités : si on trouve u != v alors qu'on avait avant u = v, on s'arrête car c'est une contradiction. On n'oublie pas de plus le cas où l'on a quelque chose de la forme f(t1, ..., tn) != f(s1, ..., sn) alors qu'on avait ti = si pour 1<=i<=n.
	À chaque fois que l'assignation du solveur est incohérente, on ajoute la clause en modifiant directement le fichier cnf contenant le problème.
	
	Il y a néanmoins un problème qui fait que cette implémentation n'est pas complètement correcte : on ne crée pas de nouveaux termes, et si l'on apprend par exemple que h = f(x), on ne le propagera pas dans les termes où f(x) apparaît comme sous-terme. Ce bug est illustré dans le fichier test5.in.

Compilation :
    cd SMT
    make clean ; make

Utilisation :
    ./smt < <formule>
    avec <formule> un fichier contenant une formule "bien formée"


