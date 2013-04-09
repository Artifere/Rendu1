# pour visualiser le dessin trace par ce script gnuplot, taper
# gnuplot -persist script-plot.p
#  (en s`assurant que le fichier comparaison.dat est present dans le repertoire)

reset

### decommenter les 2 lignes ci-dessous pour engendrer un fichier pdf
### plutot qu`un dessin a l`ecran
# set term pdfcairo
# set output "courbe1.pdf" # le nom du fichier qui est engendre

set title "Comparaison de sat solvers"
set xlabel "Version"
set ylabel "Temps d'execution"
set xtics border ("30-60" 0, "30-90" 1,"30-120" 2,"30-150" 3,"30-210" 4,"30-300" 5, "30 400" 6)

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

