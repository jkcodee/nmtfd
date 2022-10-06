#LEGENDE
#unset key

set key spacing 1.5
set key box outside
set key right
unset colorbox

set size ratio -1

unset surface
set grid
set view map
unset ztics

set yrange [6:0]
set xrange [0:10]

set xlabel "x in m"
set ylabel "y in m"

#Zeichne Rohre in Plot
set obj 1 rect from 3.8,2 to 4,2.2 front lw 2
set obj 2 rect from 6,2 to 6.2,2.2 front lw 2

#set palette rgbformulae 33,13,10

set contour 
set cntrparam levels 30

set term pdf size 15, 9

set title "Druckverteilung Untergrund"
set output (outputPath."beleg_druck.pdf")
splot inputPath."beleg.data" using 3:4:5 w l t "p in bar" lw 3 palette


set title "u_x in m/s"
set output (outputPath."beleg_ux.pdf")
splot inputPath."beleg.data" using 3:4:6 w l t "u_x in m/s" lw 3 palette


set title "u_y in m/s"
set output (outputPath."beleg_uy.pdf")
splot inputPath."beleg.data" using 3:4:7 w l t "u_y in m/s" lw 3 palette


set term pdf size 12, 9


set title "Geschwindigkeitsvektoren"
set output (outputPath."beleg_geschwindigkeitsvektor.pdf")
set yrange [4:0]
set xrange [3:7]
unset key
d(dx,dy) = sqrt(dx*dx+dy*dy)/0.1
plot inputPath."geschwindigkeit.data" using 3:4:($5/d($5,$6)):($6/d($5,$6)) w vectors head size 0.1,20,60 filled
