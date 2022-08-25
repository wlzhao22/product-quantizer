#ifndef PQMATH_H
#define PQMATH_H

/**
Distribute tasks to different classes, that are
responsible. It is designed for the sake of clarity of
the code.

@author: Wanlei Zhao
@email:  stonescx@gmail.com
@date:   Mar-28-2015

**/

class PQMath
{
    private:
        static const float smallVal0;
    public:
        static double cos(const float v1[], const unsigned int s1,
                          const float v2[], const unsigned int s2, const unsigned int d0);
        static float  l2(const float v1[], const unsigned int s1,
                         const float v2[], const unsigned int s2,
                         const unsigned int dim0);
        static bool   l2_norm(float *vect1,  const unsigned int dim);
        static double fvec_norm(const float *v, const long n, const double norm);
        static void   normVects(float *vects, const unsigned int d0,
                                 const unsigned int n0, float *lens);
        static void   normVects(float *vects, const unsigned int d0,
                                 const unsigned int n0);
        static void   sqrt_SIFT(float *vects, const unsigned int d0, const unsigned int numb);
        static bool   isZeros(const float *feat, const  unsigned int dim);
        static int    sign(const float val);
        static void   powerLaw(float *vect, const float alpha, const unsigned int d0);
    public:
        PQMath(){}
        virtual ~PQMath(){}
};

#endif
