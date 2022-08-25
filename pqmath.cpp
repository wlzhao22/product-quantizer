#include "pqmath.h"

#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;

const float PQMath::smallVal0 = 0.000001f;

double PQMath::cos(const float v1[], const unsigned int s1,
                   const float v2[], const unsigned int s2, const unsigned int d0)
{
    double w1  = 0.0f, w2 = 0.0f, val = 0.0f;
    unsigned int loc1 = d0 * s1;
    unsigned int loc2 = d0 * s2;

    for(unsigned int i = 0; i < d0; i++)
    {
        w1  = w1  + v1[loc1+i]*v1[loc1+i];
        w2  = w2  + v2[loc2+i]*v2[loc2+i];
        val = val + v1[loc1+i]*v2[loc2+i];
    }

    w1 = sqrt(w1);
    w2 = sqrt(w2);

    if(w1 == 0 && w2 == 0)
    {
        return 1;
    }

    if(w1 == 0)
    {
        return fabs(1-w2/2);
    }
    else if(w2 == 0)
    {
        return fabs(1-w1/2);
    }
    else
    {
        val = val/(w1*w2);
        return val;
    }
    return val;
}

float PQMath::l2(const float v1[], const unsigned int s1,
                 const float v2[], const unsigned int s2, const unsigned int d0)
{
    float w1 = 0.0f, w2 = 0.0f;
    int loc1 = d0 * s1;
    int loc2 = d0 * s2;

    for(unsigned int i = 0; i < d0; i++)
    {
        w1  = v1[loc1+i] - v2[loc2+i];
        w2 += w1*w1;
    }

    w2 = sqrt(w2);

    return w2;
}

bool PQMath::l2_norm(float *vect1, const unsigned int dim)
{
    float  size = 0;
    for(unsigned int i = 0; i < dim; i++)
    {
        size += vect1[i]*vect1[i];
    }

    if(size == 0.0f)
    {
        return false;
    }
    size = sqrt(size);
    for(unsigned int i = 0; i < dim; i++)
    {
        vect1[i] = vect1[i]/size;
    }

    return true;
}

void PQMath::powerLaw(float *vect, const float alpha, const unsigned int d0)
{
    unsigned int i = 0;
    float tmpVal;
    for(i = 0; i < d0; i++)
    {
         tmpVal = fabs(vect[i]);
        vect[i] = PQMath::sign(vect[i])*pow(tmpVal, alpha);
    }
}

void PQMath::normVects(float *vects, const unsigned int d0,
                       const unsigned int n0, float *lens)
{
    assert(vects);
    assert(lens);
    unsigned int i = 0, j = 0, loc = 0;
    float len = 0;
    for(i = 0; i < n0; i++)
    {
        len = 0;
        for(j = 0; j < d0; j++)
        {
            len += vects[loc+j]*vects[loc+j];
        }
        len     = sqrt(len);
        lens[i] = len;
        if(len > PQMath::smallVal0)
        {
            for(j = 0; j < d0; j++)
            {
                vects[loc+j] = vects[loc+j]/len;
            }
        }else{
            lens[i] = 0;
        }
        loc += d0;
    }
    return ;
}


void PQMath::normVects(float *vects, const unsigned int d0,
                       const unsigned int n0)
{
    assert(vects);
    unsigned int i = 0, j = 0, loc = 0;
    float len = 0;

    for(i = 0; i < n0; i++)
    {
        len = 0;
        for(j = 0; j < d0; j++)
        {
            len += vects[loc+j]*vects[loc+j];
        }
        len  = sqrt(len);
        if(len > PQMath::smallVal0)
        {
            for(j = 0; j < d0; j++)
            {
                vects[loc+j] = vects[loc+j]/len;
            }
        }
        loc += d0;
    }
    return ;
}

void PQMath::sqrt_SIFT(float *feats, const unsigned int dim, const unsigned int numb)
{
    assert(feats);
    unsigned int i, j, loc;
    for(i = 0; i < numb; i++)
    {
        loc = i*dim;
        for(j = 0; j < dim; j++)
        {
            feats[loc+j] = sqrt(feats[loc+j]);
        }
    }
    return ;
}

double PQMath::fvec_norm(const float *v, const long n, const double norm)
{
    if(norm == 0)
        return n;

    long i;
    double s = 0;

    if(norm == 1)
    {
        for(i = 0 ; i < n ; i++)
            s += fabs(v[i]);
        return s;
    }

    if(norm == 2)
    {
        for(i = 0 ; i < n ; i++)
        {
            s += v[i]*v[i];
        }
        return sqrt(s);
    }

    if(norm == -1)
    {
        for(i = 0 ; i < n ; i++)
            if(fabs(v[i]) > s)
                s = fabs(v[i]);
        return s;
    }

    for (i = 0 ; i < n ; i++)
    {
        s += pow (v[i], norm);
    }

    return pow (s, 1 / norm);
}

bool PQMath::isZeros(const float *feat, const  unsigned int dim)
{
    unsigned int j;
    float len = 0;
    for(j = 0; j < dim; j++)
    {
        len += feat[j]*feat[j];
    }

    len = sqrt(len);
    if(len <= 0.0000)
    {
        return true;
    }
    else
        return false;
}

int PQMath::sign(const float val)
{
    if(val >=0 )
    {
        return 1;
    }else{
        return -1;
    }
}
