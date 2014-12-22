a.out: Reorder.o test.o Mandelbrot.o
	g++ -g  Reorder.o test.o Mandelbrot.o -lz3
Mandelbrot.o: Mandelbrot.cpp Mandelbrot.h
	g++ -g -c -Wall -std=c++11 Mandelbrot.cpp

Reorder.o: Reorder.cpp Reorder.h
	g++ -g -c -Wall -std=c++11 -mfma -O2 Reorder.cpp

test.o: test.cpp Reorder.h Mandelbrot.h
	g++ -g -c -Wall -std=c++11 -mfma -O2 -fexceptions  test.cpp -lz3

clean: 

