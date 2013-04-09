# pour visualiser le dessin trace par ce script gnuplot, taper
# gnuplot -persist script-plot.p
#  (en s`assurant que le fichier comparaison.dat est present dans le repertoire)

reset

### decommenter les 2 lignes ci-dessous pour engendrer un fichier pdf
### plutot qu`un dessin a l`ecran
 set term pdfcairo
 set output "courbe.pdf" # le nom du fichier qui est engendre

set title "Comparaison de sat solvers, 50 variables, 2000 tests"
set xlabel "nbVar/nbClause"
set ylabel "Temps d'execution"
set xtics border ("2" 0, "3" 1,"3,5" 2,"3,8" 3,"4" 4,"4,3" 5, "4,5" 6, "4,8" 7, "5,5" 8, "6" 9, "7" 10, "8 11, "9" 12)

# Dessin en joignant des points
set style data linespoints

set pointsize 1   # la taille des points


# on trace deux courbes: avec les colonnes 1 et 2, avec les colonnes 1 et 3
# a chaque fois, le nom de la courbe est lu en tete de colonne
plot  "comparaison.dat" using 1:2 title columnheader(2), \
	  "comparaison.dat" using 1:3 title columnheader(3), \
      "comparaison.dat" using 1:4 title columnheader(4), \
      "comparaison.dat" using 1:5 title columnheader(5), \
      "comparaison.dat" using 1:6 title columnheader(6), \
      "comparaison.dat" using 1:7 title columnheader(7), \
      "comparaison.dat" using 1:8 title columnheader(8), \
      "comparaison.dat" using 1:9 title columnheader(9), \
	  "comparaison.dat" using 1:10 title columnheader(10)


