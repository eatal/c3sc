#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <getopt.h>

#include "c3.h"
#include "c3sc.h"

static char * program_name;

void print_code_usage (FILE *, int) __attribute__ ((noreturn));
void print_code_usage (FILE * stream, int exit_code)
{

    fprintf(stream, "Usage: %s options \n", program_name);
    fprintf(stream,
            " -h --help      Display this usage information.\n"
            " -d --directory Output directory (defaults to .)\n"
            " -v --verbose   Output words (default 0)\n"
        );
    exit (exit_code);
}

int f1(double t, double * x, double * u, double * out, void * args)
{
    (void)(t);
    (void)(args);
    
    out[0] = x[1] + sin(4.0*x[0]);
    out[1] = u[0];
    return 0;
}

int s1(double t,double * x,double * u,double * out,void * args)
{
    (void)(t);
    (void)(x);
    (void)(u);
    (void)(args);
    
    out[0] = sin(3.0*x[0]);
    out[1] = 0.0;
    out[2] = 0.0;
    out[3] = cos(8.0*x[1]);
    
    return 0;
}

int polfunc(double t, double * x, double * u)
{
    (void)(t);
    if (x[1] > 0){
        u[0] = -1.0;
    }
    else if (x[1] < 0){
        u[0] = 1.0;
    }
    else{
        u[0] = 0.0;
    }
//    u[0] = 0.0;
    return 0;
}

int outbounds(double * x, void * args)
{
    // two boundaries
    // outside of the box([-2,2]^2)
    // and the inside box [-0.1,0.1]^2
    (void)(args);
    
    for (size_t ii = 0; ii < 2; ii++){
        if (fabs(x[ii]) >= 2.0){
            return 1;
        }
    }

    if ( (fabs(x[0]) <= 1e-1) && (fabs(x[1]) <= 1e-1)){
        return 1;
    }
    
    return 0;
}


int stagecost(double t, double * x, double * u, double * out)
{
    (void)(t);
    *out = 0.0;
    *out += pow(x[0],2) + pow(x[1],2) + pow(u[0],2);
    return 0;
}

int boundcost(double t, double * x, double * out)
{

    (void)(t);
    *out = 0.0;

    if ( (fabs(x[0]) <= 1e-1) && (fabs(x[1]) <= 1e-1)){
        *out = 0.0;
    }
    else{
        *out = 5.0;
    }

    return 0;
}

double startcost(double * x, void * args)
{
    (void)(args);
    (void)(x);
    return 3.0;
}

int main(int argc, char * argv[])
{
    int next_option;
    const char * const short_options = "hd:v:";
    const struct option long_options[] = {
        { "help"     , 0, NULL, 'h' },
        { "directory", 1, NULL, 'd' },
        { "verbose"  , 1, NULL, 'v' },
        { NULL       , 0, NULL, 0   }
    };
    program_name = argv[0];

    char * dirout = ".";
    int verbose = 0;

    do {
        next_option = getopt_long (argc, argv, short_options, long_options, NULL);
        switch (next_option)
        {
            case 'h': 
                print_code_usage(stdout, 0);
            case 'd':
                dirout = optarg;
                break;
            case 'v':
                verbose = strtol(optarg,NULL,10);
                break;
            case '?': // The user specified an invalid option 
                print_code_usage (stderr, 1);
            case -1: // Done with options. 
                break;
            default: // Something unexpected
                abort();
        }
    } while (next_option != -1);

    struct Boundary bound;
    bound.bcheck = outbounds;
    bound.args = NULL;
    
    size_t dx = 2;
    size_t dw = 2;
    size_t du = 1;
    
    struct Drift drift;
    drift_init(&drift,dx,du,NULL,NULL,NULL,NULL);
    drift.b = f1;

    struct Diff diff;
    diff_init(&diff,dw,dx,du,NULL,NULL,NULL,NULL);
    diff.s = s1;

    //double lb = -1.0, ub = 1.0;
    double lb = -1.0, ub = 1.0;
    // slope from [-1,1] to (a,b)
    double slope[2] = {(ub-lb)/2, (ub-lb)/2};
    double offset[2] = {(ub+lb)/2, (ub+lb)/2};
    struct LinTransform lt = {2,slope,offset};
    double temp[2]; 
    printf("slope=(%G,%G)\n",slope[0],slope[1]);
    printf("offset=(%G,%G)\n",offset[0],offset[1]);
    struct Dyn dyn;
    dyn_init_ref(&dyn,&drift,&diff);
    dyn_add_transform_ref(&dyn,&lt,temp);

    struct TensorMM mm;
    size_t N = 10;
    double h = 2.0/((double)N-1.0);
    double spaces[10];
    tensor_mm_init_ref(&mm,dx,h,&dyn,spaces);

    double t3[2];
    struct Policy * pol = policy_alloc();
    policy_init(pol,dx,du,NULL,NULL);
    policy_add_feedback(pol,polfunc);
    policy_add_transform_ref(pol,&lt,t3);

    struct Cost * cost = cost_alloc();
    double lbx[2] = {lb,lb};
    double ubx[2] = {ub,ub};
    cost_init_discrete(cost,2,lbx,ubx,N,NULL);
    cost_approx(cost,startcost,NULL,1);
        
    struct DPih prob;
    prob.bound = &bound;
    prob.mm = &mm;
    prob.cost = cost;
    prob.pol = pol;
    prob.beta = 0.0;
    prob.stagecost = stagecost;
    prob.boundcost = boundcost;


    double delta = dpih_pi_iter(&prob);
    
    policy_free(pol);
    cost_free(cost);
    
    return 0;
}
