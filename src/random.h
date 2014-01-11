#ifndef _GOC_RANDOM_H_
#define _GOC_RANDOM_H_

#include <stdlib.h>
#include <time.h>

#define goc_randomInit srand(time(NULL))
#define goc_random(_int_max) (int)((_int_max)*(rand()/(RAND_MAX+1.0)))
//#define goc_random(_int_min, _int_max) ( (_int_min) + ( (rand() / RAND_MAX + 1.0) * ((_int_max)-(_int_min)+1) )

#endif // ifdef _GOC_RANDOM_H_
