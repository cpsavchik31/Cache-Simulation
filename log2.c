#include <stdio.h>
#include <stdlib.h>

int log2(int n) {
	int r = 0;
	while (n >>= 1) r++;
	return r;
}