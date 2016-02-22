#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "dynamics.h"

void drift_init(struct Drift * b, size_t dx, size_t du,
                double * lbx, double * ubx,
                double * lbu, double * ubu)
{
    assert (b != NULL);
    b->dx = dx;
    b->du = du;
    b->lbx = lbx;
    b->ubx = ubx;
    b->lbu = lbu;
    b->ubu = ubu;
    b->b = NULL;
    b->bargs = NULL;
}

int drift_eval(struct Drift * b, double time, double * x, double * u,
               double * out)
{
    int res;
    res = b->b(time,x,u,out,b->bargs);
    return res;
}

void diff_init(struct Diff * s, size_t dw,
               size_t dx, size_t du,
               double * lbx, double * ubx,
               double * lbu, double * ubu)
{
    assert (s != NULL);
    s->dw = dw;
    s->dx = dx;
    s->du = du;
    s->lbx = lbx;
    s->ubx = ubx;
    s->lbu = lbu;
    s->ubu = ubu;
    s->s = NULL;
    s->sargs = NULL;
}

int diff_eval(struct Diff * b, double time, double * x, double * u, double * out)
{
    int res;
    res = b->s(time,x,u,out,b->sargs);
    return res;
}

int dyn_eval(struct Dyn * dyn, double time, double * x, double * u,
             double * drift, double * diff)
{
    int res;
    res = drift_eval(dyn->b,time,x,u,drift);
    if (res != 0){
        return res;
    }
    res = diff_eval(dyn->s,time,x,u,diff);
    return res;
}


