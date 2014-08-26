#ifndef REORDER_H_
#define REORDER_H_
float getMax(int round_mode, int op, float* fs, int size);
float getMin(int round_mode, int op, float* fs, int size);
float getMaxWithFMA(int round_mode, float* fs1, float* fs2, int size);
float getMinWithFMA(int round_mode, float* fs1, float* fs2, int size);
#endif
