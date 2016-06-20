#ifndef VALUE_H
#define VALUE_H

#include <stdlib.h>

#include "util.h"

struct ValueF;
void valuef_destroy(struct ValueF *);
double valuef_eval(struct ValueF *, const double *);


void 
cost_eval_fiber_ind_nn(struct ValueF *, const size_t *,
                           size_t, const size_t *, double *, 
                           double *);

struct ValueF * 
valuef_interp(size_t, double (*)(const double *,void*),void *,
              const size_t *, double **, double **,
              struct ApproxArgs *, int);
#endif
