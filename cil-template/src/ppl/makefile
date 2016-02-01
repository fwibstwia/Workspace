PPL_Power.o: 
           cc -g -c PPL_Power.c
           cc -g -c -fPIC -lmpfr -lppl -lgmpxx -lgmp PPL_Power.c
libpplpower.so.1.0.1: DP_Form.o PPL_Power.o
           cc -shared -Wl,-soname,libpplpower.so PPL_Power.o DP_Form.o -lc

DP_Form.o: PPL_Manager.h DP_Form.h DP_Form.c
           cc -g -c -fPIC -lmpfr -lppl -lgmpxx -lgmp DP_Form.c
