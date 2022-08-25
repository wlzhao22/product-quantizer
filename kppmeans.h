#ifndef KPPMEANS_H
#define KPPMEANS_H

#include "abstractkmeans.h"
#include "nnitem.h"

#include <vector>

/*************************************************************************
It is an implementation of paper:

k-means++: The Advantages of Careful Seeding

by David Arthur and Sergei Vassilvitskii

@author:    Wan-Lei Zhao
@email:     stonescx@gmail.com
@date:      9/Jul/2014
@Institute: Xiamen University
**************************************************************************/

using namespace std;

class KppMeans: public AbstractKMeans
{
    private:
        static const unsigned int M_ITER, RN0;
        static const float EPS, Err0;
        double *arrayD, *tmparrayD;
        float  EXTRM_VAL0;

    public:
        KppMeans();
        bool init(const char *srcfn);
        bool init(float *mat, const int row, const int dim);
        bool refresh();

        double (KppMeans::*distf_func)(const float v1[], unsigned int s1,
                                       const float v2[], unsigned int s2, const unsigned int d0);
        double (KppMeans::*dist_func)(const float  v1[], unsigned int s1,
                                      const double v2[], unsigned int s2, const unsigned int d0);
        bool   (KppMeans::*comp_func)(const double v1, const double v2);
        bool   (KppMeans::*seed_func)(const unsigned int k, int rseeds[], const unsigned int bound);

        bool   config(const char *lg_first, const char *distfunc,
                      const char *crtrn,    int verbose);
        unsigned int clust(const unsigned int clust_num, const char *dstfn, const int verbose);

        bool   Kppseeds(const unsigned int k, int rseeds[], const unsigned int bound);
        bool   Rndseeds(const unsigned int k, int rseeds[], const unsigned int bound);
        bool   paraKppseeds(const unsigned int k, int rseeds[], const unsigned int bound);

        double l2(const float v1[], const unsigned int s1, const double v2[],
                   const unsigned int s2, const unsigned int dim0);
        double l2f(const float v1[], const unsigned int s1, const float v2[],
                   const unsigned int s2, const unsigned int dim0);

        double fvec_norm(const float *v, const long n, const double norm);
        bool   fvec_scale(float *v,      const long n, const double sc);
        void   saveCenters(const char *dstfn, bool append);
        int    fetchCenters(float *centers);

        static void test();
        virtual ~KppMeans();
};

#endif
