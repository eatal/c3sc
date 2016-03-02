#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "c3.h"

#include "util.h"
#include "simulate.h"
#include "control.h"
#include "cost.h"
#include "dp.h"
#include "tensmarkov.h"

///////////////////////////////////////////////////////////////
void dpih_init_ref(struct DPih * dp , struct Boundary * bound,
                   struct TensorMM * mm , struct Cost * cost,
                   struct Policy * pol, double beta,
                   int (*stage)(double,double*,double*,double*),
                   int (*boundc)(double,double*,double*))
{
    dp->bound = bound;
    dp->mm = mm;
    dp->cost = cost;
    dp->pol = pol;
    dp->beta = beta;
    dp->stagecost = stage;
    dp->boundcost = boundc;

    dp->trans = 0;
    dp->lt = NULL;
    dp->space = NULL;
}

/* double dpih_rhs(struct DPih * dp, double t, double * xin, */
/*                 double * u) */
/* { */
/*     double * x; */
/*     if (dp->trans == 0){ */
/*         x = xin; */
/*     } */
/*     else{ */
/*         lin_transform_eval(dp->lt,xin,dp->space); */
/*         x = dp->space; */
/*     } */

/*     // first check if x is in bounds */
/*     //int bound = boundary_ob(dp->bound,x); */
/*     int bound = 0; */
/*     /\* printf("check boundary for point "); *\/ */
/*     /\* dprint(dp->bound->d, x); *\/ */
/*     /\* printf("bound=%d\n",bound); *\/ */

/*     double out = 0.0; */
/*     if (bound == 0){ */
/*         // inbounds, can do standard kushner update */

/*         //NEED TO ADD SCALING */
/*         // stagecost */
/*         int res = dp->stagecost(t,x,u,&out); */
/*         assert (res == 0); */


/*         //printf("check boundary for point "); */
/*         //dprint(dp->bound->d, x); */
/*         //printf(" control is %G\n",u[0]); */

/*         // additional cost */
/*         double dt; */
/*         double probs[1000]; */
/*         // note xin here!! */
/*         double newval = tensor_mm_cost(dp->mm,t,xin,u,dp->cost, */
/*                                        &dt,probs); */
/*         //printf("dt = %G\n",dt); */
/*         //printf("stage cost =%G\n",dt*out); */
/*         out = dt*out + newval*exp(-dp->beta * dt); */
/*         //printf("otherwise out=%G\n",out); */
/*     } */
/*     else if (bound == 1){ */

/*         // unrecoverable */
/*         int res = dp->boundcost(t,x,&out); */
/*         assert (res == 0); */
/*         //printf("check boundary for point "); */
/*         //dprint(dp->bound->d, x); */
/*         //printf("in here out=%G\n",out); */
/*     } */
/*     else{ */
/*         // possibly recoverable */
/*         //int res = tensor_mm_dyn_eval(dp->tens,t,x,u); */
/*         //double drift * tensor_mm_drift_ref(dp->tens); */
/*         //assert (res == 0); */

/*         // check direction of movement */
/*         int dirin = 0; */
/*         if (dirin == 1) */
/*         { //direction of movement is into the domain so ok */
/*             fprintf(stderr, "movement away from boundary not yet implemented\n")  ; */
/*         } */
/*         else{ */
/*             // unrecoverable */
/*             int res = dp->boundcost(t,x,&out); */
/*             assert (res == 0); */
/*         } */
/*         //printf("out = %G\n",out); */
/*     } */

/*     return out; */
/* } */


/* double dpih_pi_rhs_approx(double * x, void * arg) */
/* { */
/*     struct DPih * dp = arg; */
/*     struct Control * u = NULL; */
/*     double t = 0.0; */
/*     int res = policy_eval(dp->pol,t,x,&u); */
/*     assert (res == 0); */

/*     double * uu = control_getu_ref(u); */
/*     double val = dpih_rhs(dp,t,x,uu); */
    
/*     control_free(u); u = NULL; */

/*     return val; */
/* } */


/* double dpih_pi_iter_approx(struct DPih * dp,int verbose) */
/* { */

/*     struct FunctionTrain * cost =  */
/*         cost_approx_new(dp->cost,dpih_pi_rhs_approx, */
/*                         dp,verbose); */

/*     double diff = function_train_norm2diff(cost, */
/*                                            dp->cost->cost); */
/*     cost_update_ref(dp->cost,cost); */

/*     return diff; */
/* } */
