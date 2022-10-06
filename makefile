compile-gcc:
	mkdir -p build
	gcc src/beleg.c src/model.c src/numericalCalculation.c src/dataStructures.c -o build/beleg -lm -O2 -w

volumenstrom:
	cd build; rm -f volumenstrom_Rohr_Links.data; rm -f volumenstrom_Rohr_Rechts.data
	cd build; ./beleg -w 5 -h  4 -d 0.05 -o
	cd build; ./beleg -w 10 -h 6 -d 0.05 -o
	cd build; ./beleg -w 15 -h 8 -d 0.05 -o
	cd build; ./beleg -w 20 -h 10 -d 0.05 -o
	cd build; ./beleg -w 25 -h 15 -d 0.05 -o
	cd build; ./beleg -w 40 -h 20 -d 0.05 -o

plot:
	cd build; ./beleg -w 10 -h 6 -d 0.05
	gnuplot -e "outputPath='doc/';inputPath='build/'" src/beleg.gnu -p 
	
replot:
	gnuplot -e "outputPath='doc/';inputPath='build/'" src/beleg.gnu -p 
	
clean:
	rm -r build
