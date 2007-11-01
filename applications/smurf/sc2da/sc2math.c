/* sc2math - SCUBA-2 mathematics library

   Authors :
    B.D.Kelly (bdk@roe.ac.uk)
   History :
    22Feb2005 : original adapted from SCUBA transputer system (bdk)
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Dits_Err.h"
#include "Ers.h"
#include "sae_par.h"
#include "prm_par.h"
#include "dat_par.h"
#include "sc2math.h"
#include "dream_par.h"

#define EPS 1.0e-20                      /* near-zero trap */

static char errmess[132];          /* error string */

/*+ sc2math_choles - Factorize symmetric positive definite matrix */

void sc2math_choles 
( 
int n,         /* Dimension of the matrix (given) */
double a[],    /* Square matrix of NxN values.
                  Only the lower-left triangle is given and returned.
                  (given and returned) */
int *loc,      /* Diagonal index in A giving the problem or the lowest value.
                  (returned) */
double *dmin,  /* Lowest value or problem value (returned) */
int *ierr      /* Error code
                   0 : Normal return.
                   1 : There is loss of significance.
                  -1 : Matrix not positive definite.
                  (returned) */
)

/* Description :
   Factorize a symmetric positive definite matrix according to the
   Cholesky method.

   Authors :
    W.N. Brouw  (wbrouw@csiro.au)

   History :
     ? : Original version
    1997 : adopted for SCUBA work (GREVE)
    06Dec2002 : C translation (bdk)
    15Mar2006:  copied from lsq.c into sc2math (agg)
*/

{
   double sum;
   int ib;
   int ind;
   int k;
   int kl;
   int l;


/* preset error indicator */

   *ierr = 0;

/* initialize row loop */

   *dmin = 1.0e20;
   ind = 0;
   ib  = 1;

/* execute loop over all rows */

   for ( k=1; k<=n; k++ )
   {
      kl = 0;
      for ( ; ; )
      {
         sum = 0.0;
         if ( ib <= ind )
         {

/* calculate scalar product */

            for ( l=ib; l<=ind; l++ )
            {
               kl++;
               sum = sum + a[l-1] * a[kl-1];
            }
         }

         kl++;
         ind++;
         sum = a[ind-1] - sum;

/* is a[ind] on diagonal */

         if ( ind <= kl ) break;
         a[ind-1] = sum / a[kl-1];
      }

/* test sign of radicand */

      if ( sum <= 0.0 )
      {

/* matrix not positive definite */

         *ierr = -1;
         *loc  = (int)( ( sqrt((double)ind*8.0+1.0) - 1.0 ) / 2.0 );
         *dmin = sum;
         break;

      }

      if ( sum <= fabs ( 1.0e-16 * a[ind-1] ) )
      {

/* serious loss of significance */

         *ierr = 1;
         *loc  = (int)( ( sqrt ( (double)ind * 8.0 + 1.0 ) - 1.0 ) / 2.0 );
         *dmin = sqrt(sum);
         break;
      }

      a[ind-1] = sqrt(sum);
      if ( a[ind-1] < *dmin )
      {
         *loc = (int)( ( sqrt ( (double)ind*8.0+1.0 ) - 1.0 ) / 2.0 );
         *dmin = a[ind-1];
      }

      ib = ib + k;
   }

}


/*+ sc2math_cholesky - Invert matrix according to the Cholesky method */

void sc2math_cholesky 
(
int nunkno,     /* The number of unknowns (given) */
double lmat[],  /* The lower left triangle of the equation matrix.
                   This is a 1 dimensional array of dimension
                   NUNKNO*(NUNKNO+1)/2 which must be filled before the
                   call of this routine, and which is changed into the
                   inverse matrix (given and returned) */
int *loc,       /* Index nr in LMAT giving the lowest value.
                   This must be a diagonal element (returned) */
double *dmin,   /* Lowest value or problem value of the diagonal element
                   with index nr LOC (returned) */
int *err        /* Possible error code in factorizing the matrix (returned) */
)

/* Description :
    Factorize and invert matrix according to the Cholesky method.
    It is assumed for this method that the matrix is symmetric
    and positive definite, else this inversion method will not work.
    If the original matrix in LMAT is given as {aij}. then this routine
    returns in LMAT the inverse matrix of {aij}, e.g. D/{aij}, where D
    is the Determinant of matrix {aij}.

    First factorize the matrix in CHOLES.
    If the matrix is not a proper positive definite matrix, this becomes 
    apparent in CHOLES during the factorization process.
    Then do the inversion on the transformed matrix in INVPDM.

   Authors :
    W.N. Brouw  (wbrouw@csiro.au)

   History :
     ? : Original version
    1997 : adopted for SCUBA work (GREVE)
    06Dec2002 : C translation (bdk)
    15Mar2006:  copied from lsq.c into sc2math (agg)
*/

{
   int i;
   int j;
   int ofs;
   int nlist;                      /* Nr of lines to list */


   *err = 0;
   sc2math_choles ( nunkno, lmat, loc, dmin, err );
   if ( *err == 1 || *err == -1 ) return;

   if ( sc2math_trace(3) )
   {
      nlist = nunkno;
      if ( nunkno > 9 ) nlist = 9;
      printf ( 
        "sc2math_cholesky: Top %d low left triangle of the factorized matrix\n",
        nlist );
      ofs = 1;
      for ( i=1; i<=nlist; i++ )
      {
         for ( j=0; j<i; j++ )
         {
            printf ( " %e", lmat[ofs+j-1] );
         }
         printf ( "\n" );
         ofs = ofs + i;
      }
   }

/* Invert matrix. */

   sc2math_invpdm ( nunkno, lmat );

}

/*+ sc2math_conval - Calculate a value of the convolution function */

double sc2math_conval 
(
int conv_shape,     /* Code for convolution function (given) */
double conv_sig,    /* Convolution function parameter (given) */
double dx,          /* Distance from the centre in X (given) */
double dy,          /* Distance from the centre in Y (given) */
int *status         /* global status (given and returned) */
)

/* Description :
    Calculate the response value of the convolution function as a function
    of the used function characteristics and the distance.

   Authors :
    H.W. van Someren Greve (greve@astron.nl)

   History :
    08-09-2001 : Original version (GREVE)
    06Dec2002 : C translation (bdk)
    31Mar2003 : give dx and dy as arguments instead of radial distance r,
                add sinc(dx).sinc(dy) option (bdk)
    06Aug2003 : add sinc(dx).sinc(dy) tapered (bdk)
    12Aug2003 : add bessel function (bdk)
    15Mar2006:  copied from weight.c into sc2math (agg)
*/

{
   double s;
   double s1;
   double s2;
   double w;


   if ( !StatusOkP(status) ) return 0.0;

   if ( conv_shape == CONV__GAUS )               /* Gaussian */
   {
      s = 1.0 / ( 2.0 * conv_sig * conv_sig );
      w = exp( -s*(dx*dx + dy*dy) );
   }
   else if ( conv_shape == CONV__SINC )          /* sinc(dx).sinc(dy) */
   {
      if ( fabs(dx) > 1.0e-9 )
      {
         s = M_PI * dx / conv_sig;
         s1 = ( sin(s) / s );
      }
      else
      {
         s1 = 1.0;
      }
      if ( fabs(dy) > 1.0e-9 )
      {
         s = M_PI * dy / conv_sig;
         s2 = ( sin(s) / s );
      }
      else
      {
         s2 = 1.0;
      }
      w = s1 * s2;
   }
   else if ( conv_shape == CONV__SINCTAP )          /* sinc(dx).sinc(dy) tapered */
   {
      if ( fabs(dx) > 3.0 )
      {
         s1 = 0.0;
      }
      else if ( fabs(dx) > 1.0e-9 )
      {
         s = M_PI * dx / conv_sig;
         s1 = ( 1.0 - 0.333 * fabs(dx) ) * ( sin(s) / s );
      }
      else
      {
         s1 = 1.0;
      }
      if ( fabs(dy) > 3.0 )
      {
         s2 = 0.0;
      }
      else if ( fabs(dy) > 1.0e-9 )
      {
         s = M_PI * dy / conv_sig;
         s2 = ( 1.0 - 0.333 * fabs(dy) ) * ( sin(s) / s );
      }
      else
      {
         s2 = 1.0;
      }
      w = s1 * s2;
   }
   else if ( conv_shape == CONV__SINCDEL )   /* sinc(dx).sinc(dy) delay tapered */
   {
      if ( fabs(dx) > 3.0 )
      {
         s1 = 0.0;
      }
      else if ( fabs(dx) > 1.0 )
      {
         s = M_PI * dx / conv_sig;
         s1 = ( 1.0 - 0.5 * ( fabs(dx) - 1.0 ) ) * ( sin(s) / s );
      }
      else if ( fabs(dx) > 1.0e-9 )
      {
         s = M_PI * dx / conv_sig;
         s1 = sin(s) / s;
      }
      else
      {
         s1 = 1.0;
      }
      if ( fabs(dy) > 3.0 )
      {
         s2 = 0.0;
      }
      else if ( fabs(dy) > 1.0 )
      {
         s = M_PI * dy / conv_sig;
         s2 = ( 1.0 - 0.5 * ( fabs(dy) - 1.0 ) ) * ( sin(s) / s );
      }
      else if ( fabs(dy) > 1.0e-9 )
      {
         s = M_PI * dy / conv_sig;
         s2 = sin(s) / s;
      }
      else
      {
         s2 = 1.0;
      }
      w = s1 * s2;
   }
     /* bessel tapered */
   /*   else if ( conv_shape == 4 ) 
   {

      s = sqrt ( dx*dx + dy*dy );
      if ( s > 3.0 )
      {
         w = 0.0;
      }
      else if ( s > 1.0e-9 )
      {
         w = gsl_sf_bessel_J1 ( M_PI * s ) / ( 2.0 * s);
         w = ( 1.0 - 0.333 * s ) * w;
      }
      else
      {
         w = 0.25 * M_PI;
      }
      }*/
   else
   {
      printf ( "Convolution function %d not implemented\n", conv_shape );
      *status = DITS__APP_ERROR;
      w = 0.0;
   }
   return w;
}

/*+  sc2math_convolve - convolve a dataset with a filter */

void sc2math_convolve
(
int filtlen,      /* number of elements in filter (given) */
int filtmid,      /* index of filter centre (given) */
double *filter,     /* convolving filter (given) */
int datalen,      /* length of dataset (given) */
double *input,      /* input dataset (given) */
double *output,     /* convolved dataset (returned) */
int *status       /* global status (given and returned) */
)

/*   Method :
      Put into each output value the sum of the input values weighted by 
      the filter.
     Author :
      B.D.Kelly (REVAD::BDK)
     History :
      30.06.1993: original, based loosely on JCMTDR_CONVOLVE 
                  (REVAD::BDK)
*/

{
   int endfilt;      /* convolution limit */
   int j;            /* index to output array */
   int joff;         /* position in input array corresponding to start 
                         of filter */
   int k;            /* index to filter */
   int startfilt;    /* convolution limit */

   double sum;         /* temporary accumulator */


   if ( !StatusOkP(status) ) return;

   for ( j=0; j<datalen; j++ )
   {
      sum = (double) 0.0;

/*   Set range of convolution to prevent k and inpos from going outside 
     their array bounds */

      startfilt = filtmid - j;
      if ( startfilt < 0 )
      {
         startfilt = 0;
      }

      endfilt = filtmid - j + datalen;
      if ( endfilt > filtlen )
      {
         endfilt = filtlen;
      }

/*   Calculating joff is a small optimisation not done by the compiler 
     */

      joff = j - filtmid;
      for ( k=startfilt; k<endfilt; k++ )
      {
         sum += input[joff+k] * filter[k];
      }
      output[j] = sum;
   }

}



/*+ sc2math_cubfit - fit a set of values with a cubic */

void sc2math_cubfit 
( 
int npts,                 /* number of data points (given) */
double *x,                /* observed values (given) */
double *y,                /* observed values (given) */
double *coeffs,           /* coefficients of fit (returned) */
double *variances,        /* variances of fit (returned) */
int *status               /* global status (given and returned) */
)

/*   Method :
      Fit a cubic polynomial to a dataset.
     Authors :
      B.D.Kelly (bdk@roe.ac.uk)
     History :
      19.05.2005: original (bdk)
      20.05.2005: pass-in both x and y (bdk)
     endhistory
*/

{


   int j;
   int despike;            /* flag for spike removal */
   int nterms;             /* number of combined waveforms */
   static double standard_waves[1024*3];   /* values of standard waveforms */
   double standard_weights[1024]; /* if y[j] is not to be used, 
                            standard_weights[j] = 0.0, otherwise
                            standard_weights[j] = 1.0 */
   double used_weights[1024];  /* if y[j] was not used, 
                                 used_weights[j] = 0.0, otherwise 
                                 used_weights[j] = 1.0  */
   double fitted_y[1024];  /* combined waveform computed from the fit */
   int nbad;              /* number of points rejected as suspected
                            "spikes" */



   if ( !StatusOkP(status) ) return;

   despike = 0;
   nterms = 3;

   for ( j=0; j<npts; j++ )
   {
      standard_waves[j] = x[j];
      standard_waves[j+npts] = x[j] * x[j];
      standard_waves[j+npts*2] = x[j] * x[j] * x[j];
      standard_weights[j] = 1.0;
   }

   sc2math_recurfit ( despike, npts, nterms, standard_waves,
     standard_weights, y, used_weights, fitted_y, coeffs,
     variances, &nbad, status );

}

/*+ sc2math_eq0  - Make equation matrix */

void sc2math_eq0 
(
int nunkno,        /* The number of unknowns (given) */
double lcoef[],    /* The known values (given) */
double lmat[]      /* equation matrix (given and returned) */
)

/* Description :
    This routine makes the square equation matrix with the sum 
    coefficients of a least squares solution.
    The routine is called in a loop per observed point with the
    NUNKNO coefficients for that point.
    There should be >= observed points than NUNKNO.
    The routine is called in a loop per observed point.
    This routine differs only from LSQ_EQU because of a special check if
    the coefficient in LCOEFF is zero.
    For that reason it is much faster than LSQ_EQU, for big matrices,
    especially if the LCOEF vector contains mostly zeroes.

    lcoef contains the known coefficients of the
    least square equation, for example for a polynomial the vector 
    contains the values : 1.0, X, X**2, X**3, etc.

    Only the lower left part of lmat (the equation matrix) is made
    because the equation matrix is symmetrical.
    So this is a 1 dimensional array of dimension NUNKNO*(NUNKNO+1)/2 
    which must be cleared before the first call of this routine.
 
   Authors :
    W.N. Brouw  (wbrouw@csiro.au)

   History :
     ? : Original version.
    1997 : adopted for SCUBA work (GREVE)
    04Dec2002 : C translation (bdk)
    15Mar2006:  copied from lsq.c into sc2math (agg)
*/

{
   int i;
   int ik;
   int j;
   double s;


/* Summation of lower triangle part of the equation matrix in row-wise
   order */

   ik = -1;
   for ( j=1; j<=nunkno; j++ )
   {
      s = lcoef[j-1];
      if ( s == 0.0 )                  /* This should be mostly true */
      {
         ik = ik + j;                  /* No contribution */
      }
      else                             /* Add contribution */
      {
         for ( i=0; i<j; i++ )
         {
            ik = ik + 1;
            lmat[ik] = lmat[ik] + s * lcoef[i];
         }
      }
   }

}

/*+ sc2math_fitsky - fit a sky baseline for each bolometer */

void sc2math_fitsky
(
int cliptype,       /* type of sigma clipping (given) */
int nboll,          /* number of bolometers (given) */
int nframes,        /* number of frames in scan (given) */
int ncoeff,         /* number of coefficients (given) */
double *inptr,      /* measurement values (given) */
double *coptr,      /* coefficients of fit (returned) */
int *status         /* global status (given and returned) */
)

/* Description :
    For each bolometer, determine an estimated sky level by
    making a linear fit to timestream with sigma clipping.
   Authors :
    B.D.Kelly (bdk@roe.ac.uk)

   History :
    25Feb2005 : original (bdk)
    14Jul2005 : add cliptype argument (bdk)
    13Mar2006 : copied from map.c (agg)
*/

{
   double cons;        /* offset for straight-line fit */
   double grad;        /* gradient of staright-line fit */
   int i;              /* loop counter */
   int j;              /* loop counter */
   double *pos;        /* positions in scan for single bolometer */
   double *scan;       /* copy of scan for single bolometer */
   double *wt;         /* weights for single bolometer */


   if ( !StatusOkP(status) ) return;

/* provide default values */

   for ( j=0; j<nboll; j++ )
   {
      for ( i=0; i<ncoeff; i++ )
      {
         coptr[j+i*nboll] = 0.0;
      }
   }

   if ( nframes > 10 )
   {
      scan = calloc ( nframes, sizeof(double) );
      pos = calloc ( nframes, sizeof(double) );
      wt = calloc ( nframes, sizeof(double) );

      for ( i=0; i<nframes; i++ )
      {
         pos[i] = (double)i;
      }

      for ( j=0; j<nboll; j++ )
      {

/* extract the values for one bolometer */

         for ( i=0; i<nframes; i++ )
         {
            scan[i] = inptr[nboll*i+j];
         }

/* fit a straight line with clipping */
	 sc2math_sigmaclip ( cliptype, nframes, pos, scan, wt, &grad, &cons,
			     status );
         coptr[j] = cons;
         coptr[j+nboll] = grad;
      }

      free ( scan );
      free ( wt );
      free ( pos );
   }
}


/*+ sc2math_flatten - apply flat field correction to set of frames */

void sc2math_flatten
(
int nboll,                /* number of bolometers (given) */
int nframes,              /* number of frames in scan (given) */
const char *flatname,     /* name of flatfield algorithm (given) */
int nflat,                /* number of flatfield parameters (given) */
const double *fcal,       /* calibration coefficients (given) */
const double *fpar,       /* calibration parameters (given) */
double *inptr,            /* measurement values (given and returned) */
int *status               /* global status (given and returned) */
)

/* Description :
    Apply the calibration corrections indicated by the given algorithm
    name. Currently two are supported.

    "POLYNOMIAL" requires six coefficients per bolometer.

    "TABLE" means use the fcal values for any bolometer as an
     interpolation table to determine the corresponding fpar value.

   Authors :
    B.D.Kelly (bdk@roe.ac.uk)

   History :
    12Jul2005 : original (bdk)
    05Oct2005 : generalise by adding flatname, nflat and fpar arguments (bdk)
    30Jul2007 : allow "NULL" flatfield - do nothing (bdk)
    03Sep2007 : allow for bad values in linear coeffs (bdk)
    31Oct2007:  use const for appropriate arguments (bdk)
*/

{
   int i;              /* loop counter */
   int j;              /* loop counter */
   double lincal[2];   /* interpolation coeffs for a bolometer */
   double t;           /* intermediate result */
   double *temp;       /* pointer to storage for a single bolometer */


   if ( !StatusOkP(status) ) return;

   if ( strcmp ( "POLYNOMIAL", flatname ) == 0 )
   {
      for ( j=0; j<nframes; j++ )
      {

/* apply flatfield to each bolometer in frame */

         for ( i=0; i<nboll; i++ )
         {
	   t = inptr[j*nboll+i] - fcal[i+nboll];
	   inptr[j*nboll+i] = fcal[i] 
	     + fcal[i+2*nboll] 
	     + fcal[i+3*nboll] * t 
	     + fcal[i+4*nboll] * t * t
	     + fcal[i+5*nboll] * t * t * t;
         }
      }
   }
   else if ( strcmp ( "TABLE", flatname ) == 0 )
   {
      temp = calloc ( nframes, sizeof(double) );

      for ( i=0; i<nboll; i++ )
      {

/* Get the time series for this bolometer and determine the interpolation
   formula for its intensity conversion */

         for ( j=0; j<nframes; j++ )
         {
            temp[j] = inptr[j*nboll+i];
         }

/* Tailor the flatfield calibration coefficients */

         sc2math_setcal ( nboll, i, nframes, temp, nflat, fpar, fcal,
           lincal, status );

/* apply flatfield for this bolometer in each frame */

         for ( j=0; j<nframes; j++ )
         {
	    if ( ( lincal[0] == VAL__BADD ) || ( lincal[1] == VAL__BADD ) )
	    {
	       inptr[j*nboll+i] = VAL__BADD;
	    }
	    else
	    {
               inptr[j*nboll+i] = inptr[j*nboll+i] * lincal[1] + lincal[0];
	    }
         }
      }
      free ( temp );
   }
   else if ( strcmp ( "NULL", flatname ) == 0 )
   {
/* Do nothing - just return the data unchanged */
   }
   else
   {
      *status = DITS__APP_ERROR;
      printf ( "map_flatten: invalid flatfield name - %s\n", flatname );
   }

}

/*+ sc2math_get_cycle - Return data for a single measurement cycle */

void sc2math_get_cycle 
(
int cur_cycle,    /* current cycle number (given) */
int nsam_cycle,   /* number of samples per cycle (given) */
int ncycle,       /* total number of cycles (given) */
int r_nbol,       /* number of bolometers (given) */
double drdata[],  /* full raw data set (given) */
double sbuf[],    /* data for the current cycle (returned) */
int *status       /* global status (given and returned) */
)

/* Description :
    Extract the data for all the bolometers for a single cycle.

   Authors :
    B.D.Kelly (bdk@roe.ac.uk)

   History :
    17Jan2003 : original (bdk)
    15Mar2006:  copied from dsolve.c into sc2math (agg)
*/

{

   int bol;                 /* bolometer counter */
   int sample;              /* sample counter */
   int cycle_off;           /* offset of current cycle */
   int bol_off;             /* offset of current bolometer */


   if ( !StatusOkP(status) ) return;

   cycle_off = cur_cycle * nsam_cycle;

   for ( bol=0; bol<r_nbol; bol++ )
   {
      bol_off = bol * nsam_cycle;
      for ( sample=0; sample<nsam_cycle; sample++ )
      {
         sbuf[bol_off+sample] =
           drdata[r_nbol*cycle_off+r_nbol*sample+bol];
      }
   }

}

/*+ sc2math_gridext - Calculate the extent of the sky grid */

void sc2math_gridext
(
int ngrid,           /* Number of grid positions within the jiggle area
                        (given) */
int gridpts[][2],    /* Table with relative grid offsets for a single
                        bolometer (given) */
int *xmin,           /* Grid limit in X-dir (returned) */
int *xmax,           /* Grid limit in X-dir (returned) */
int *ymin,           /* Grid limit in Y-dir (returned) */
int *ymax,           /* Grid limit in Y-dir (returned) */
int *status          /* global status (given and returned) */
)

/* Description :
    Search for the minimum and maximum grid offsets in X and Y.
 
   Authors :
    B.D.Kelly (bdk@roe.ac.uk)

   History :
    04Aug2003 : Original (bdk)
    15Mar2006:  copied from map.c into sc2math (agg)
*/

{
   int j;                     /* Bolometer location */
      

   if ( !StatusOkP(status) ) return;

/* Find the limits of the sky grid offsets */

   *xmin = gridpts[0][0];
   *xmax = *xmin;
   *ymin = gridpts[0][1];
   *ymax = *ymin;

   for ( j=0; j<ngrid; j++ )
   {
        if (gridpts[j][0] < *xmin) *xmin = gridpts[j][0];
        if (gridpts[j][0] > *xmax) *xmax = gridpts[j][0];
        if (gridpts[j][1] < *ymin) *ymin = gridpts[j][1];
        if (gridpts[j][1] > *ymax) *ymax = gridpts[j][1];
   }


}

/*+ sc2math_interpwt - Calculate the weight matrix for spatial interpolation */

void sc2math_interpwt 
(
int npath,           /* Nr of rows (given) */
int ngrid,            /* Nr of columns (given) */
int conv_shape,      /* Code for convolution function (given) */
double conv_sig,     /* Convolution function parameter (given) */
double calc_t,       /* Time per path point in millisec (given) */
double tbol,         /* Bolometer time constant in millisec (given) */
double jigpath[][2], /* Table with jiggle path pos (given) */
int jigpts[][2],     /* Table with grid positions (given) */
double b[],          /* the weight factors (returned) */
int *status          /* (given and returned) */
)


/* Description :
    This is the routine that makes the coefficient matrix with weight
    factors for the solution of all defined grid positions affected by a
    single detector.


          +-------------------------------------------------------+
          | 0 | 1 | 2 | 3 | 4 | ..........................|ngrid-1|
          +-------------------------------------------------------+
     0    | This row contains ngrid weights for point 0.          |
     1    | This row contains ngrid weights for point 1.          | 
     .    |                                                       |
     .    |                                                       |
     i    | This row contains ngrid weights for point i.          |
     .    |                                                       |
  npath-1 | This row contains ngrid weights for the last point.   |
          +-------------------------------------------------------+
      

    Make the weight coefficients for each sample and for each grid point.
    depending on the SMU position, the distances to each grid point, and
    the convolution function.
    Then apply the convolution with the impulse-response function on the
    weight coefficients in time, where the impulse response function
    depends on the electronics (time constant)
    Normalize the coefficients in such a way that for each sample the sum
    of all grid coefficients is equal to 1.0.

   Authors :
    H.W. van Someren Greve (greve@astron.nl)

   History :
    17-05-1996 : Original version (GREVE)
    30-07-1997 : Changed for the final programs.
    20-08-2000 : Add parameter conv_sig, and call wt_pixel with conv_sig.
                 Change continuation print line.
    05Dec2002 : C translation (bdk)
    20Jun2003 : change constant name to DREAM__MXGRID (bdk)
    05Aug2003 : interpwt version of weight_pixwt (bdk)
    15Mar2006:  copied from map.c into sc2math (agg)
*/
{
   int nrv;                       /* Nr of values in b(*) */
   int nirf;                      /* Nr of values in irf function */
   int i;                         /* Loop variable (rows) */
   int j;                         /* Loop variable (col) */
   int n;                         /* Loop variable (iter) */
   int k;                         /* Index in wtpix */
   double rwt;                       /* Convolution value */
   double sumirf;                    /* Sum all impulse response val. */
   double fb;                        /* 1/Bolometer time const in KHz */
   double y[DREAM__MXGRID];
   double x;

/* Large work arrays - declared static to avoid going on the stack */

   static double wtpix[DREAM__MXGRID*DREAM__MXSIM];
   static double irf[DREAM__MXIRF]; /* Impulse response function */
   static double wtcol[DREAM__MXGRID*DREAM__MXSIM+DREAM__MXIRF];


   if ( !StatusOkP(status) ) return;

   fb = 1.0 / tbol;
   nrv = npath * ngrid;

   for ( i=0; i<nrv; i++ )
   {
      b[i] = 0.0;
   }

/* Calculate the coefficients of the pixel points as a function
   of the points in the SMU path, and the convolution function, for each
   point in the path. */

   if ( sc2math_trace(3) )
   {
      printf ( "sc2math_interpwt: calling sc2math_pathwts\n" );
   }
   sc2math_pathwts ( conv_shape, conv_sig, npath, jigpath, ngrid, jigpts,
     wtpix, status );

   if ( sc2math_trace(3) )
   {
      printf ( "Unnormalized coefficient matrix without impulse response\n" );
      printf ( "  row    0     1     2     3     4     5     6     7     8\n" );
      for ( j=0; j<npath; j++ )
      {
         i = j * ngrid;
         printf ( "  %d", i/ngrid );
         for ( n=0; n<ngrid; n++ )
         {
            printf ( " %.2e", wtpix[i+n] );
         }
         printf ( "\n" );
      }
   }

/* Calculate the Impulse Response function */

   nirf = DREAM__MXIRF;
   sumirf = 0.0;
   x = 0.0;
   for ( i=0; i<nirf; i++ )
   {
      irf[i] = fb * exp(-x*fb);
      sumirf = sumirf + irf[i];
      x = x + calc_t;
   }

   if ( sc2math_trace(4) )
   {
      printf ( "Impulse response function :\n" );
      for ( i=0; i<10; i++ )
      {
         for ( n=0; n<10; n++ )
         {
            printf ( "  %.2e", irf[i*10+n] );
         }
         printf ( "\n" );
      }
      printf ( "The sum of all values : %e\n", sumirf );
   }

/* Start the loop over all the pixels in the pattern */

   for ( j=0; j<ngrid; j++ )
   {

/* Get all pixel weights of pixel j (column) in array wtcol
   for Njiggle cycles. These are stored in wtcol, in such a way that
   the 1st nirf values (length of the Impulse response function) 
   remain empty. */

      k = j;                          /* Column for pixel j in wtpix */
      for ( i=0; i<npath; i++ )
      {
         wtcol[i+nirf] = wtpix[k];
         k = k + ngrid;
      }

/* Now fill the 1st nirf values in wtcol with the last nirf values of
   the pixel weights. These values are necessary in the response
   function when we calculate the convolution values for the 1st nirf
   points. */

      k = npath;                     /* Offset to start of last nirf points */
      for ( i=0; i<nirf; i++ )
      {
         wtcol[i] = wtcol[i+k];
      }

/* Make the response function as a convolution of the pixel weights
   with the impulse response function, and save the convolved normalized
   response values in its proper place in output array b. */

      n = nirf;                       /* Start point for convolution */
      for ( i=0; i<npath; i++ )
      {
         sc2math_response ( wtcol, irf, nirf, n, &rwt );
         rwt = rwt / sumirf;            /* Normalize */
         b[i*ngrid+j] = rwt;             /* Save convolved value */
         n++;                           /* Next convolution point */
      }

   }

/* Normalize all the rows in such a way that the sum of all coefficients
   is always 1.0. */

   for ( j=0; j<npath; j++ )
   {
      x = 0.0;
      i = j * ngrid;
      for ( n=0; n<ngrid; n++ )
      {
         x = x + b[i+n];
      }
      for ( n=0; n<ngrid; n++ )
      {
         b[i+n] = b[i+n] / x;
      }
   }

   if ( sc2math_trace(2) )
   {
      printf ( "Normalized coefficient matrix with Impulse Response (output)\n" );
      printf ( 
        " row    0     1     2     3     4     5     6     7     8     SUM\n" );
      for ( n=0; n<ngrid; n++ )
      {
          y[n] = 0.0;
      }
      for ( j=0; j<npath; j++ )
      {
         x = 0.0;
         i = j * ngrid;                    
         for ( n=0; n<ngrid; n++ )
         {
            x = x + b[i+n];
            y[n] = y[n] + b[i+n];
         }
         printf ( " %d", i/ngrid );
         for ( n=0; n<ngrid; n++ )
         {
            printf ( "  %.2e", b[i+n] );
         }
         printf ( " %e\n", x );
      }
      printf ( 
        "    ------------------------------------------------------------\n" );
      printf ( "   SUM" );
      for ( n=0; n<ngrid; n++ )
      {
         printf ( " %.2e", y[n] );
      }
      printf ( "\n" );
   }

}

/*+ sc2math_invpdm - Invert positive definite matrix */

void sc2math_invpdm 
(
int n,       /* Dimension of the matrix (given) */
double a[]   /* Square Matrix with NxN points.
                Only the lower-left triangle is given.
                (given and returned) */
)

/* Description :
    Invert symmetric positive definite matrix.

   Authors :
    W.N. Brouw  (wbrouw@csiro.au)

   History :
     ? : Original version
    1997 : adopted for SCUBA work (GREVE)
    06Dec2002 : C translation (bdk)
    15Mar2006:  copied from lsq.c into sc2math (agg)
*/

{

   int icol;
   int ipiv;
   int irow;
   int j;
   int k;
   int m;
   int ln;
   int lmn;
   int lk;
   double sum;
   double piv;



/* invert triangular matrix */

   ln = n;
   j = 0;

/* perform loop over all rows */

   lmn = ln - 1;
   for ( k=0; k<=lmn; k++ )
   {
      ipiv = 0;
      j++;
      piv = a[j+k-1];
      piv = 1.0 / piv;
      a[j+k-1] = piv;
      if ( k != 0)
      {
         for ( lk=1; lk<=k; lk++ )
         {
            sum = 0.0;
            irow = j;
            ipiv += lk;
            icol = ipiv;
            for ( m=lk; m<=k; m++ )
            {
               sum = sum + a[irow-1] * a[icol-1];
               icol = icol + m;
               irow = irow + 1;
            }
            a[j-1] = -sum * piv;
            j++;
         }
      }
   }

/* multiply by transpose */

   j = 0;

/* perform loop over all rows */

   for ( k=1; k<=ln; k++ )
   {
      irow = k;

/* execute loop within k-th row */

      for ( lk=1; lk<=k; lk++ )
      {
         sum = 0.0;
         j++;
         icol = j;
         irow = irow - 1;

/* calculate scalar products */

         for ( m=k; m<=ln; m++ )
         {
            sum = sum + a[icol-1] * a[icol+irow-1];
            icol = icol + m;
         }
         a[j-1] = sum;
      }
   }

}

/*+ sc2math_linfit - straight line fit */

void sc2math_linfit
(
int np,               /* number of points (given) */
double x[],           /* X data (given) */
double y[],           /* Y data (given) */
double wt[],          /* weights (given) */
double *grad,         /* slope (returned) */
double *cons,         /* offset (returned) */
int *status           /* global status (given and returned) */
)                                       
/* Description :
    Simple least-squares fit of a straight line. In general the weights
    are expected to be 1.0 or 0.0, allowing points to be ignored.

   History :
    20Oct2004 : original (bdk)
    13Mar2006 : copied from map.c (agg)
    24Aug2007 : trap for noiseless data (bdk)
*/

{
   double xm;           /* X mean */
   double ym;           /* Y mean */
   double rp;           /* number of values used (total weight) */
   double rnum;         /* XY factor */
   double rden;         /* X squared factor */
   double ximxm;        /* X differences */
   int i;

   if ( !StatusOkP(status) ) return;

   xm = 0.0;
   ym = 0.0;
   rp = 0.0;

   for ( i=0; i<np; i++ )
   {
      xm += x[i] * wt[i];
      ym += y[i] * wt[i];
      rp += wt[i];
   }
   xm = xm / rp;
   ym = ym / rp;

   rnum = 0.0;
   rden = 0.0;

   for ( i=0; i<np; i++ )
   {
      ximxm = ( x[i] - xm ) * wt[i];
      rnum += ximxm * y[i];
      rden += ximxm * ximxm;
   }

   if ( fabs(rden) < 1.0e-10 )
   {
      *grad = 0.0;
   }
   else
   {
      *grad = rnum / rden;
   }
   *cons = ym - (*grad) * xm;
}


/*+  sc2math_martin - spike removal from chop-scan data */

void sc2math_martin
(
double period,       /* chop period in samples (given) */
int maxlen,        /* length of signal (given) */
double *signal,      /* signal to be cleaned (given and returned) */
char *badscan,      /* record of spikes removed (returned) */
int *nbad,         /* number of points removed (returned) */
int *status        /* global status (given and returned) */
)

/*   Method :
     Detect spikes using a Martin filter run at a single scale length,
     the scale corresponding to the chop period.
     Authors :
      B.D.Kelly (REVAD::BDK)
     History :
      15.06.1993: original (REVAD::BDK)
      28.09.1993: add badscan (REVAD::BDK)
*/

{
   double diff;                 /* data difference */
   double fnum;                 /* number of points used */
   int roundper;              /* rounded version of period */
   double sumdiff;              /* sum of data differences */
   double sumdiffsq;            /* sum of squares of data differences */
   double sigma;                /* sigma of data differences */
   double worstval;             /* most discrepant value */
   int finished;              /* loop controller */
   int j;                     /* loop counter */
   int worstpos;              /* position of worstval */


   if ( !StatusOkP(status) ) return;


/*  Initialise bad value marker */

   memset ( badscan, 0, (size_t) maxlen );

/*  The data ends are assumed to be padded with zeros */

   *nbad = 0;
   fnum = (double) maxlen - (double) 2.0 * period;
   roundper = (int) ( period + (double) 0.5 );
   finished = 0;
   while ( finished == 0 )
   {
      worstval = (double) 0.0;
      worstpos = (int) 0;
      sumdiff = (double) 0.0;
      sumdiffsq = (double) 0.0;
      for ( j=roundper; j<maxlen-roundper; j++ )
      {
         diff = signal[j] - 
           ( signal[j-roundper] + signal[j+roundper] ) / (double)2.0;
         sumdiff += diff;
         sumdiffsq += diff * diff;
         if ( diff < 0 )
         {
            diff = (-diff);
         }
         if ( diff > worstval )
         {
            worstval = diff;
            worstpos = j;
         }
      }
      sigma = sumdiffsq - sumdiff * sumdiff / fnum;
      sigma = (double) sqrt ( (double) sigma / (double) fnum );
      if ( worstval > ( (double)4.0 * sigma ) )
      {
         signal[worstpos] = 
           ( signal[worstpos-roundper] + signal[worstpos+roundper] ) / 
           (double)2.0;
         (*nbad)++;
         badscan[worstpos] = (char) 1;
      }
      else
      {
         finished = 1;
      }
   }
}

/*+  sc2math_matinv - invert a symmetric matrix */

void sc2math_matinv 
( 
int norder,          /* degree of matrix (given) */
double array[10][10],  /* given matrix, its inverse is returned 
                         (given and returned) */
double *det,           /* determinant of ARRAY (returned) */
int *status          /* global status (given and returned) */
)

/*   Method :
      Invert a symmetric matrix and calculate its determinant.
      Taken from "Data reduction and error analysis for the physical 
      sciences", P.R.Bevington.
      example: the inverse of 
                   1  2  3
                   2  1  1
                   3  1  2

      is
                 -0.25  0.25  0.25
                  0.26  1.75 -1.25
                  0.25 -1.25  0.75
     Authors :
      B.D.Kelly (REVAD::BDK)
     History :
      30.11.1988:  original (REVAD::BDK)
      02.02.1990:  rationalise names (REVAD::BDK)
      01.04.1992:  version for SCUSP library (REVAD::BDK)
      11.10.1993:  use the "+="-style construction, and test on a VAX 
                   against the example given in Method (REVAD::BDK)
     endhistory
*/

{
   double armax;
   double store;
   int ik[10];
   int jk[10];
   int i;
   int j;
   int k;

   if ( !StatusOkP(status) ) return;


   *det = (double) 1.0;

   for ( k=0; k<norder; k++ )
   {

/*   Find largest element array[j][i] in rest of matrix */

      armax = (double) 0.0;

      for ( i=k; i<norder; i++ )
      {
         for ( j=k; j<norder; j++ )
         {
            if ( fabs(armax) <= fabs(array[j][i]) ) 
            {
               armax = array[j][i];
               ik[k] = i;
               jk[k] = j;
            }
         }
      }

/*   Interchange rows and columns to put armax in array[k][k] */

      if ( armax == 0.0 ) 
      {
         *det = (double) 0.0;
         return;
      }

      i = ik[k];

      if ( i > k ) 
      {
         for ( j=0; j<norder; j++ )
         {
            store = array[j][k];
            array[j][k] = array[j][i];
            array[j][i] = -store;
         }
      }

      j = jk[k];

      if ( j > k ) 
      {
         for ( i=0; i<norder; i++ )
         {
            store = array[k][i];
            array[k][i] = array[j][i];
            array[j][i] = -store;
         }
      }

/*   Accumulate elements of inverse matrix */

      for ( i=0; i<norder; i++ )
      {
         if ( i != k) 
         {
            array[k][i] = -array[k][i] / armax;
         }
      }

      for ( i=0; i<norder; i++ )
      {
         if ( i != k ) 
         {
            for ( j=0; j<norder; j++ )
            {
               if ( j != k ) 
               {
                  array[j][i] += array[k][i] * array[j][k];
               }
            }
         }
      }

      for ( j=0; j<norder; j++ )
      {
         if ( j != k ) 
         {
            array[j][k] /= armax;
         }
      }

      array[k][k] = (double) 1.0 / armax;
      *det = (*det) * armax;

   }

/*   Restore ordering of matrix */

   for ( k=norder-1; k>=0; k-- )
   {
      j = ik[k];

      if ( j > k ) 
      {
         for ( i=0; i<norder; i++ )
         {
            store = array[k][i];
            array[k][i] = -array[j][i];
            array[j][i] = store;
         }
      }

      i = jk[k];

      if ( i > k ) 
      {
         for ( j=0; j<norder; j++ )
         {
            store = array[j][k];
            array[j][k] = - array[j][i];
            array[j][i] = store;
         }
      }

   }
 
}

/*+ sc2math_msv - Multiply matrix with column vector */

void sc2math_msv 
(
int m,        /* Number of unknowns (given) */
double s[],   /* Lower left triangle of a symmetric matrix of MxM (given) */
double v[],   /* Vector with dimension M (given) */
double x[]    /* Vector with dimension M with the result (returned) */
)

/* Description :
    Multiplication of a symmetric matrix (S) in compressed form,
    stored row-wise, by a column vector (V)
    Result is stored in vector (X).

   Authors :
    W.N. Brouw  (wbrouw@csiro.au)

   History :
    1997 : adopted for SCUBA work (GREVE)
    04Dec2002 : C translation (bdk)
    15Mar2006:  copied from lsq.c into sc2math (agg)
*/

{
   int i;
   int j;
   int k;
   int ki;
   double t;


   ki = 1;

   for ( i=1; i<=m; i++ )
   {
      k = ki;
      t = 0.0;

      for ( j=1; j<=m; j++ )
      {
         t = t + s[k-1] * v[j-1];
         if ( j < i )
         {
            k++;
         }
         else
         {
            k = k + j;
         }
      }

      x[i-1] = t;
      ki = ki + i;
   }
}

/*+ sc2math_pathwts - Pixel weights for SMU path */

void sc2math_pathwts 
(
int conv_shape,       /* Code for convolution function (given) */
double conv_sig,      /* Convolution function parameter (given) */
int npath,            /* Nr of rows in wtpix (given) */
double jigpath[][2],  /* Table with jiggle path pos. (given) */
int ncol,             /* Nr of columns in wtpix (given) */
int gridpts[][2],     /* Table with grid positions (given) */
double wtpix[],       /* Matrix with pixel weights (returned) */
int *status           /* global status (given and returned) */
)

/* Description :
    Make weight table for each grid position covered by the SMU path
    when the SMU moves from pixel to pixel in the Jiggle pattern.

   Authors :
    H.W. van Someren Greve (greve@astron.nl)

   History :
     09-09-2001 : Original version. (GREVE)
     05Dec2002 : C translation (bdk)
     15Mar2006:  copied from weight.c into sc2math (agg)
*/

{
   int i;                          /* loop over rows */
   int p;                          /* loop over columns */
   double xpath;                   /* Current X value */
   double ypath;                   /* Current Y value */
   double r;                       /* radial distance */
   double dx;                      /* X and Y differences */
   double dy;                      /* X and Y differences */
   double v;                       /* Convolution value */


   if ( !StatusOkP(status) ) return;

   for ( i=0; i<npath*ncol; i++ )
   {
      wtpix[i] = 0.0;                          /* Clear all coefficients */
   }

   if ( conv_sig > 0.0 )
   {

/* Normal case, use PSF model for calculating all the contributions. */

      for ( i=0; i<npath; i++ )                 /* Loop over the path */
      {
         xpath = jigpath[i][0];                 /* Relative X coordinate */
         ypath = jigpath[i][1];                 /* Relative Y coordinate */
         for ( p=0; p<ncol; p++ )               /* Loop over grid pixels */
         {
            dx = (double)gridpts[p][0] - xpath;
            dy = (double)gridpts[p][1] - ypath;
            v = sc2math_conval ( conv_shape, conv_sig, dx, dy, status );
            wtpix[i*ncol+p] = wtpix[i*ncol+p] + v;
         }
      }
   }
   else
   {

/* Special case, don't use a PSF, but use a contribution of 1.0 only
   when we are on the grid point, else use a contribution of zero. */

      for ( i=0; i<npath; i++ )                 /* Loop over the path */
      {
         xpath = jigpath[i][0];                /* Relative x coordinate */
         ypath = jigpath[i][1];                /* Relative Y coordinate */
         for ( p=0; p<ncol; p++ )              /* Loop over grid pixels */
         {
            dx = (double)gridpts[p][0] - xpath;
            dy = (double)gridpts[p][1] - ypath;
            r = sqrt(dx*dx + dy*dy);
            if ( r < 1.0e-20 )
            {
              wtpix[ncol+p] = wtpix[i*ncol+p] + 1.0;
            }
         }
      }

   }

}

/*+ sc2math_recurfit - fit a set of values with outlier rejection */

void sc2math_recurfit 
( 
int despike,            /* flag for spike removal (given) */
int npts,               /* number of data points (given) */
int nterms,             /* number of combined waveforms (given) */
double *standard_waves,   /* values of standard waveforms (given) */
double *standard_weights, /* if volts[j] is not to be used, 
                            standard_weights[j] = 0.0, otherwise
                            standard_weights[j] = 1.0 (given) */
double *volts,            /* observed values (given) */
double *used_weights,     /* if volts[j] was not used, 
                            used_weights[j] = 0.0, otherwise 
                            used_weights[j] = 1.0 (returned) */
double *fitted_volts,     /* combined waveform computed from the fit 
                            (returned) */
double *coeffs,           /* coefficients of fit (returned) */
double *variances,        /* variances of fit (returned) */
int *nbad,              /* number of points rejected as suspected
                            "spikes" (returned) */
int *status             /* status must be 0 on entry. 
                            If no valid fit was found, SAI__ERROR is
                            returned (given and returned) */
)

/*   Method :
      The observed waveform is assumed to be a linear combination of the 
      standard waveforms :
        volts[i] = coeffs[0] + coeffs[1] * standard_waves[0][i] + 
                   coeffs[2] * standard_waves[1][i] + .....
      The problem is solved by multiple linear regression. It is assumed
      that volts may contain bad values - ie "spikes", so after the fit
      a search is made for discrepant points which are rejected before
      the fit is repeated.

      Note there are (nterms+1) coefficients but nterms standard_waves,
      the extra term being the constant offset.
     Authors :
      B.D.Kelly (bdk@roe.ac.uk)
     History :
      01.12.1988: original (REVAD::BDK)
      02.02.1990: rationalise names (REVAD::BDK)
      01.04.1992: return CALIB_AMP and CHOP_AMP separately, separate 
                  import and export WEIGHTS arrays (REVAD::BDK)
      07.10.1992: return CALIB_VAR and CHOP_VAR (REVAD::BDK)
      11.05.1993: add despike, change threshold to 3.5 sigma 
                  (REVAD::BDK)
      16.10.1995: make despike ignore zero-weight points (BDK)
      28.02.2005: generalise SCUBA demodulation algorithm for SCUBA-2
                  function fitting (bdk)
     endhistory
*/

{
   int i;                 /* loop counter */
   int fitcnt;            /* counter for number of attempted fits */
   int done;              /* loop controller */
   double a0;               /* constant term in fit */
   double a[10];            /* other terms in fit */
   double noise;            /* rms noise in the overall fit */
   double sigma0;           /* sigma in constant term */
   double sigmaa[10];       /* sigma for other terms */
   double r[10];            /* linear correlation coefficients */
   double rmul;             /* multiple linear correlation coefficient */
   double chisqr;           /* reduced chi square for fit */
   double ftest;            /* value of "F" for test of fit */


   if ( !StatusOkP(status) ) return;


/*   Initialise weights */

   for ( i=0; i<npts; i++ )
   {
      used_weights[i] = standard_weights[i];
   }

/*   Repeatedly fit the data until no more outlying values are rejected 
     or the data seem too bad. */

   done = 0;
   *nbad = 0;
   fitcnt = 0;
   while ( ( done == 0 ) && ( fitcnt < 10 ) )
   {

/*   Perform fit */

      fitcnt++;
      sc2math_regres ( npts, nterms, standard_waves, volts, 
        used_weights, fitted_volts, &a0, a, &sigma0, sigmaa, r,
        &rmul, &chisqr, &ftest, &noise, status );

      if ( StatusOkP(status) )
      {

         done = (int) 1;
         if ( despike == 1 )
         {

/*   Search for bad values */

            for ( i=0; i<npts; i++ )
            {
               if ( used_weights[i] != (double)0.0 )
               {
                  if ( fabs ( (double)volts[i] - (double)fitted_volts[i] ) 
                    > ( 3.5 * noise ) )
                  {
                     used_weights[i] = (double) 0.0;
                     *nbad += (int) 1;
                     done = 0;
                  }
               }
            }
         }
      }
      else
      {

/*   fit failed completely */

         fitcnt = 10;
      }

   }

   if ( done == 1 )
   {

/*   A successful fit, return computed amplitudes */

      coeffs[0] = a0;
      variances[0] = sigma0 * sigma0;
      for ( i=0; i<nterms; i++ )
      {
         coeffs[i+1] = a[i];
         variances[i+1] = sigmaa[i] * sigmaa[i];
      }
   }
   else
   {

/*   Fit failed */

      *nbad = (int) npts;
      coeffs[0] = 0.0;
      variances[0] = 0.0;
      for ( i=0; i<nterms; i++ )
      {
         coeffs[i+1] = 0.0;
         variances[i+1] = 0.0;
      }
   }
}



/*+  sc2math_regres - multiple linear regression fit */

void sc2math_regres 
( 
int npts,      /* number of data points (given) */
int nterms,    /* number of combined waveforms (given) */
double *x,       /* values of standard waveforms (given) */
double *y,       /* observed values (given) */
double *weight,  /* weight for each observed value (given) */
double *yfit,    /* values of Y computed from the fit (returned) */
double *a0,      /* constant term in fit (returned) */
double *a,       /* coefficients of fit (returned) */
double *sigma0,  /* standard deviation of A0 (returned) */
double *sigmaa,  /* array of standard deviations for coefficients
                   (returned) */
double *r,       /* array of linear correlation coefficients 
                   (returned) */
double *rmul,    /* multiple linear correlation coefficient (returned) */
double *chisqr,  /* reduced chi square for fit (returned) */
double *ftest,   /* value of F for test of fit (returned) */
double *perr,    /* probable error in deviation of a single point from
                   the fit (returned) */
int *status    /* status must be OK on entry
                   on exit, STATUS = OK => fit ok
                   STATUS = DITS__APP_ERROR => exact fit (no noise) 
                   (given and returned) */
)

/*   Method :
      Make a multiple linear regression fit to data with a specified 
      function which is linear in coefficients. It is assumed that all 
      weights are 1.0 or 0.0, points with weight 0.0 being totally 
      ignored.
      Taken from "Data reduction and error analysis for the physical 
      sciences", P.R.Bevington.
     Authors :
      B.D.Kelly (REVAD::BDK)
     History :
      29.11.1988: original (REVAD::BDK)
      02.02.1990: rationalise names (REVAD::BDK)
      01.04.1992: use SAI errors, make part of SCUSP (REVAD::BDK)
      12.10.1993: fix various one-off errors, use += -style syntax 
                  (REVAD::BDK)
     endhistory
*/

{

   double array[10][10]; /* normal equations */
   double sigmax[10];    /* */
   double xmean[10];     /* weighted means of standard waveforms */
   double sum;           /* sum of weights */
   double ymean;         /* weighted mean of data to be fitted */
   double sigma;         /* */
   double chisq;         /* */
   double free1;         /* */
   double freej;         /* */
   double freen;         /* */
   double det;           /* determinant of normal equations */
   int i;              /* loop controller */
   int j;              /* loop controller */
   int k;              /* loop controller */



   if ( *status != 0 ) return;


/*   Initialise sums and arrays */

   sum = (double) 0.0;
   ymean = (double) 0.0;
   sigma = (double) 0.0;
   chisq = (double) 0.0;
   *rmul = (double) 0.0;

   for ( i=0; i<npts; i++ )
   {
      yfit[i] = (double) 0.0;
   }

   for ( j=0; j<nterms; j++ )
   {
      xmean[j] = (double) 0.0;
      sigmax[j] = (double) 0.0;
      r[j] = (double) 0.0;
      a[j] = (double) 0.0;
      sigmaa[j] = (double) 0.0;
      for ( k=0; k<nterms; k++ )
      {
         array[k][j] = (double) 0.0;
      }
   }

/*   Accumulate weighted sums */

   for ( i=0; i<npts; i++ )
   {
      sum += weight[i];
      ymean += weight[i]*y[i];
      for ( j=0; j<nterms; j++ )
      {
         xmean[j] += weight[i] * x[j*npts+i];
      }
   }

   ymean /= sum;

   for ( j=0; j<nterms; j++ )
   {
      xmean[j] /= sum;
   }

/*   Accumulate matrices R and ARRAY */

   for ( i=0; i<npts; i++ )
   {
      sigma += weight[i] * ( y[i] - ymean ) * ( y[i] - ymean );
      for ( j=0; j<nterms; j++ )
      {
         sigmax[j] += weight[i] * 
           ( x[j*npts+i] - xmean[j] ) * ( x[j*npts+i] - xmean[j] );
         r[j] += weight[i] * ( x[j*npts+i] - xmean[j] ) * 
           ( y[i] - ymean );
         for ( k=0; k<=j; k++ )
         {
            array[k][j] += weight[i] * 
              ( x[j*npts+i] - xmean[j] ) * ( x[k*npts+i] - xmean[k] );
         }
      }
   }

   free1 = sum - (double) 1.0;
   sigma = (double) sqrt ( (double) sigma / (double) free1 );

   for ( j=0; j<nterms; j++ )
   {
      sigmax[j] = 
        (double) sqrt ( (double) sigmax[j] / (double) free1 );
      r[j] /= ( free1 * sigmax[j] * sigma );
      for ( k=0; k<=j; k++ )
      {
         array[k][j] /= ( free1 * sigmax[j] * sigmax[k] );
         array[j][k] = array[k][j];
      }
   }

/*   Invert symmetric matrix */

   sc2math_matinv ( nterms, array, &det, status );

   if ( fabs ( (double) det ) < EPS ) 
   {
      *a0 = (double) 0.0;
      *sigma0 = (double) 0.0;
      *rmul = (double) 0.0;
      *chisqr = (double) 0.0;
      *ftest = (double) 0.0;
      *status = DITS__APP_ERROR;
   }
   else
   {

/*   Calculate coefficients, fit and chi square */

      *a0 = ymean;
      for ( j=0; j<nterms; j++ )
      {
         for ( k=0; k<nterms; k++ )
         {
            a[j] += r[k] * array[k][j];
         }
         a[j] *= sigma / sigmax[j];
         *a0 -= a[j] * xmean[j];
         for ( i=0; i<npts; i++ )
         {
            yfit[i] += a[j] * x[j*npts+i];
         }
      }

      for ( i=0; i<npts; i++ )
      {
         yfit[i] += (*a0);
         chisq += weight[i] * ( y[i] - yfit[i] ) * 
           ( y[i] - yfit[i] );
      }

      *perr = (double) sqrt ( (double) chisq / (double) sum );
      freen = sum - (double) nterms - (double) 1.0;
      *chisqr = chisq / freen;

/*   Calculate uncertainties */

      for ( j=0; j<nterms; j++ )
      {
         sigmaa[j] = array[j][j] * (*chisqr) / 
           ( free1 * sigmax[j] * sigmax[j] );
         sigmaa[j] = (double) sqrt ( (double) sigmaa[j] );
         *rmul += a[j] * r[j] * sigmax[j] / sigma;
      }

      freej = (double) nterms;
      *ftest = ( (*rmul) / freej ) / ( ( (double) 1.0 - (*rmul) ) 
        / freen );
      *rmul = (double) sqrt ( (double) *rmul );
      *sigma0 = (double) 1.0 / sum;
      for ( j=0; j<nterms; j++ )
      {
         for ( k=0; k<nterms; k++ )
         {
            *sigma0 += (*chisqr) * xmean[j] * xmean[k]
              * array[k][j] / ( free1 * sigmax[j] * sigmax[k] );
         }
      }

      *sigma0 = (double) sqrt ( (double) *sigma0 );

   }

}

/*+  sc2math_remsine - remove sine wave from scan data */

void sc2math_remsine
(
int phase,        /* position in scan corresponding to zero phase 
                      of the superimposed sine (given) */
double period,      /* period in samples of the sine wave (given) */
int scanlen,      /* length of scan (given) */
double *scan,       /* the time series of measurements (given and 
                      returned) */
double *amplitude,  /* amplitude of the sine wave (returned) */
int *status       /* global status (given and returned) */
)

/*  Method :
     Given the phase and period of the sine wave, multiply the scan 
     by the matching sine of unit amplitude and sum the result. This 
     gives the amplitude of the sine in the data. Subtract the 
     calculated sine from the data.
    Author :
     B.D.Kelly (REVAD::BDK)
    History :
     16.06.1993: original (REVAD::BDK)
*/

{
   int j;       /* loop counter */
   double rad;   /* conversion to radians */

   if ( !StatusOkP(status) ) return;

/*   Calculate amplitude of sine wave */

   *amplitude = (double) 0.0;
   rad = (double) ( 2.0 * 3.14159 ) / (double)period;
   for ( j=0; j<scanlen; j++ )
   {
     *amplitude += scan[j] * 
        (double) sin ( ( (double) j - (double) phase ) * rad );
   }
   *amplitude = (double) 2.0 * ( *amplitude / (double)scanlen );

/*   Subtract the sine wave from the scan */

   for ( j=0; j<scanlen; j++ )
   {
     scan[j] -= *amplitude * 
       (double) sin ( ( (double) j - (double) phase ) * rad );
   }

}

/*+ sc2math_response - Calculate the response value */

void sc2math_response
(
double f[],   /* Input function (given) */
double w[],   /* Impulse response function (given) */
int wdim,     /* Dimension of W (given) */
int ix,       /* Index corresponding to response (given) */
double *r     /* Response value (returned) */
)

/* Description :
    This routine calculates the response value (R) of input function F
    with impulse response function W for index Ix.
    Calculated is :

         R = sum[F(Ix-i).W(i)]   for i=0, 1, Wdim-1

    Ix=        0123456.....Wdim......................................
    Input      FFFFFFF.....F   FFFF..FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
    Ix=Wdim    WWWWWWW.....W 
    Ix=Wdim+1  .WWWWWWW.....W 
    Ix=Wdim+2  ..WWWWWWW.....W 
    Ix=Wdim+3  ...WWWWWWW.....W 
    ..........
    Ix=Wdim+n  .........................................WWWWWWW.....W 
           
    The minimum value of IX must be >= Wdim-1, else the index in the F 
    function will be negative at the start. 
    This must be checked in the calling routine.
    The maximum value of IX is equal to the dimension of F (Fdim) plus Wdim.
    Fdim does'nt have to be known in this routine.
    Usually this function is called for Fdim successive values, ranging
    from Wdim to Wdim+Fdim.
 
    The array W must have at least a size of Wdim.
 
    The response value should be normalized in the calling routine !


   Authors :
    H.W. van Someren Greve (greve@astron.nl)

   History :
     25-06-2000 : Original version (GREVE)
     05Dec2002 : C translation (bdk)
     15Mar2006:  copied from weight.c into sc2math (agg)
*/

{
   int i;                   /* index */
   int j;                   /* index */
   double sum;              /* Summation variable */


   sum = 0.0;
   for ( i=0; i<wdim; i++ )
   {
      j = ix - i;
      sum = sum + w[i] * f[j];
   }
   *r = sum;

}

/*+ sc2math_setcal - set flatfield calibration for a bolometer */

void sc2math_setcal
( 
int nboll,               /* total number of bolometers (given) */
int bol,                 /* number of current bolometer (given) */
int numsamples,          /* number of data samples (given) */
const double values[],   /* measurements by bolometer (given) */
int ncal,                /* number of calibration measurements (given) */
const double heat[],     /* calibration heater settings (given) */
const double calval[],   /* calibration measurements for all bolometers (given) */
double lincal[2],        /* calibration parameters (returned) */
int *status              /* global status (given and returned) */
)

/*  Description :
     Given calibration measurements plus actual measurements, select the
     section of the calibration which matches the actual measurements and
     determine a suitable interpolation formula for calibrating the
     actual measurents.

     The approach is to find three calibration measurements which bracket
     the mean of the actual measurements and determine coefficients for a
     straight-line interpolation.

    Authors :
     B.D.Kelly (ROE)
    History :
     12Aug2005:  original (bdk@roe.ac.uk)
     03Sep2007:  allow for increasing and decreasing data numbers (bdk)
     31Oct2007:  use const for appropriate arguments (bdk)
*/

{
   int j;
   double mean;

   if ( !StatusOkP(status) ) return;

/* calculate mean of current measurements */

   mean = 0.0;

   for ( j=0; j<numsamples; j++ )
   {
      mean += values[j];
   }
   mean /= (double)numsamples;

/* Choose interpolation point from calibration tables */

   if ( calval[bol+nboll] > calval[bol] )
   {
      sc2math_setcalinc ( nboll, bol, mean, ncal, heat, calval, lincal, 
        status );
   }
   else if ( calval[bol+nboll] < calval[bol] )
   {
      sc2math_setcaldec ( nboll, bol, mean, ncal, heat, calval, lincal, 
        status );
   }
   else
   {
      lincal[0] = VAL__BADD;
      lincal[1] = VAL__BADD;
   }
}


/*+ sc2math_setcaldec - set decreasing flatfield calibration */

void sc2math_setcaldec
( 
int nboll,               /* total number of bolometers (given) */
int bol,                 /* number of current bolometer (given) */
const double dvalue,     /* representative data number (given) */
int ncal,                /* number of calibration measurements (given) */
const double heat[],     /* calibration heater settings (given) */
const double calval[],   /* calibration measurements for all bolometers (given) */
double lincal[2],        /* calibration parameters (returned) */
int *status              /* global status (given and returned) */
)

/*  Description :
     Given calibration measurements plus a representative data number, 
     select the section of the calibration which matches the data number and
     determine a suitable interpolation formula for calibrating the
     actual measurents.

     The approach is to find two calibration measurements which bracket
     the representative data number and determine coefficients for a
     straight-line interpolation.

    Authors :
     B.D.Kelly (ROE)
    History :
     12Aug2005:  original (bdk@roe.ac.uk)
     03Sep2007:  provide for increasing and decreasing data numbers (bdk)
     31Oct2007:  use const for appropriate arguments (bdk)
*/

{
   int j;

   if ( !StatusOkP(status) ) return;


/* Choose interpolation point from calibration tables 
   Note the tables are assumed to be in increasing order of power,
   decreasing order of data number */

   if ( dvalue >= calval[bol] )
   {
      lincal[1] = ( heat[1] - heat[0] ) / 
        ( calval[bol+nboll] - calval[bol] );
      lincal[0] = heat[1] - lincal[1] * calval[bol+nboll];
   }
   else if ( dvalue <= calval[bol+(ncal-1)*nboll] )
   {
      lincal[1] = ( heat[ncal-1] - heat[ncal-2] ) / 
        ( calval[bol+(ncal-1)*nboll] - calval[bol+(ncal-2)*nboll] );
      lincal[0] = heat[ncal-1] - lincal[1] * calval[bol+(ncal-1)*nboll];
   }
   else
   {
      for ( j=1; j<ncal; j++ )
      {
         if ( calval[bol+j*nboll] < dvalue )
         {
            lincal[1] = ( heat[j] - heat[j-1] ) / 
              ( calval[bol+j*nboll] - calval[bol+(j-1)*nboll] );
            lincal[0] = heat[j] - lincal[1] * calval[bol+j*nboll];
            break;
         }
      }
   }
}



/*+ sc2math_setcalinc - set increasing flatfield calibration */

void sc2math_setcalinc
( 
int nboll,               /* total number of bolometers (given) */
int bol,                 /* number of current bolometer (given) */
const double dvalue,     /* representative data number (given) */
int ncal,                /* number of calibration measurements (given) */
const double heat[],     /* calibration heater settings (given) */
const double calval[],   /* calibration measurements for all bolometers (given) */
double lincal[2],        /* calibration parameters (returned) */
int *status              /* global status (given and returned) */
)

/*  Description :
     Given calibration measurements plus a representative data number, 
     select the section of the calibration which matches the data number and
     determine a suitable interpolation formula for calibrating the
     actual measurents.

     The approach is to find two calibration measurements which bracket
     the representative data number and determine coefficients for a
     straight-line interpolation.

    Authors :
     B.D.Kelly (ROE)
    History :
     12Aug2005:  original (bdk@roe.ac.uk)
     03Sep2007:  provide for increasing and decreasing data numbers (bdk)
     31Oct2007:  use const for appropriate arguments (bdk)
*/

{
   int j;

   if ( !StatusOkP(status) ) return;


/* Choose interpolation point from calibration tables 
   Note the tables are assumed to be in increasing order of power,
   increasing order of data number */

   if ( dvalue <= calval[bol] )
   {
      lincal[1] = ( heat[1] - heat[0] ) / 
        ( calval[bol+nboll] - calval[bol] );
      lincal[0] = heat[1] - lincal[1] * calval[bol+nboll];
   }
   else if ( dvalue >= calval[bol+(ncal-1)*nboll] )
   {
      lincal[1] = ( heat[ncal-1] - heat[ncal-2] ) / 
        ( calval[bol+(ncal-1)*nboll] - calval[bol+(ncal-2)*nboll] );
      lincal[0] = heat[ncal-1] - lincal[1] * calval[bol+(ncal-1)*nboll];
   }
   else
   {
      for ( j=1; j<ncal; j++ )
      {
         if ( calval[bol+j*nboll] > dvalue )
         {
            lincal[1] = ( heat[j] - heat[j-1] ) / 
              ( calval[bol+j*nboll] - calval[bol+(j-1)*nboll] );
            lincal[0] = heat[j] - lincal[1] * calval[bol+j*nboll];
            break;
         }
      }
   }
}







/*+ sc2math_sigmaclip - do sigma clipping on a straight-line fit */

void sc2math_sigmaclip
(
int type,             /* 0 for double sided clip, 
                        >0 positive clip, 
                        <0 negative clip (given) */
int np,               /* number of points (given) */
double x[],           /* X data (given) */
double y[],           /* Y data (given) */
double wt[],          /* weights (given) */
double *grad,         /* slope (returned) */
double *cons,         /* offset (returned) */
int *status           /* global status (given and returned) */
)
/* Description :
    Repeatedly fit a straight line to data, locate outliers from the fit
    and zero-weight them for the next attempt. Stop when no more outliers
    or down to 70% of original data.

   History :
    20Oct2004 : original (bdk)
    14Jul2005 : add type argument (bdk)
    13Mar2006 : copied from map.c (agg)
*/

{
   int found;          /* flag for outlier located */
   int j;              /* loop counter */
   double sigma;       /* standard deviation */
   double rnum;        /* number of values used (total weight) */
   double diff;        /* residual at a point */


   if ( !StatusOkP(status) ) return;


/* Initial weighting selects all the points */

   for ( j=0; j<np; j++ )
   {
      wt[j] = 1.0;
   }

/* repeatedly fit removing outliers */
   for ( ; ; )
   {
      sc2math_linfit ( np, x, y, wt, grad, cons, status );
/* calculate scatter in y */

      sigma = 0.0;
      rnum = 0.0;
      for ( j=0; j<np; j++ )
      {
         diff = y[j] - ( (*cons) + (*grad) * x[j] );
         sigma += wt[j] * diff * diff;
         rnum += wt[j];
      }
      sigma = sqrt ( sigma / rnum );

      if ( rnum > 0.7 * (double)np )
      {

/* search for any outliers */

         found = 0;

         if ( type > 0 )
         {
            for ( j=0; j<np; j++ )
            {
               diff = wt[j] * ( y[j] - ( (*cons) + (*grad) * x[j] ) );
               if ( diff > 3.5*sigma )
               {
                  found = 1;
                  wt[j] = 0.0;
               }
            }
         }
         else if ( type == 0 )
         {
            for ( j=0; j<np; j++ )
            {
               diff = wt[j] * ( y[j] - ( (*cons) + (*grad) * x[j] ) );
               if ( fabs(diff) > 3.5*sigma )
               {
                  found = 1;
                  wt[j] = 0.0;
               }
            }
         }
         else
         {
            for ( j=0; j<np; j++ )
            {
               diff = wt[j] * ( y[j] - ( (*cons) + (*grad) * x[j] ) );
               if ( (-diff) > 3.5*sigma )
               {
                  found = 1;
                  wt[j] = 0.0;
               }
            }
         }

         if ( found == 0 ) break;
      }
      else
      {
         break;
      }
   }
}


/*+  sc2math_sinedemod - sine wave demodulate a signal */

void sc2math_sinedemod 
( 
int length,          /* length of the signal array (given) */
double *sine,          /* sine wave (given) */
double *cosine,        /* cosine wave (given) */
double *signal,        /* signal (given) */
double *amplitude,     /* modulation amplitude (returned) */
double *phase,         /* phase of signal (returned) */
int *status          /* global status (given and returned) */
)

/*   Method :
      Demodulate by multiplying the given signal by sine and cosine and 
      summing.
     Authors :
      B.D.Kelly (REVAD::BDK)
     History :
      15.10.1993: original (REVAXD::BDK)
     endhistory
*/

{
   double avsig;                 /* average value of signal */
   int j;                      /* loop counter */
   double sumcos;                /* total of cosine multiplication */
   double sumsine;               /* total of sine multiplication */

   if ( !StatusOkP(status) ) return;


   avsig = (double)0.0;
   for ( j=0; j<length; j++ )
   {
      avsig += signal[j];
   }
   avsig /= (double)length;

   sumsine = (double)0.0;
   sumcos = (double)0.0;
   for ( j=0; j<length; j++ )
   {
      sumsine += ( signal[j] - avsig ) * sine[j];
      sumcos += ( signal[j] - avsig ) * cosine[j];
   }
   sumsine = (double)2.0 * sumsine / (double)length;
   sumcos = (double)2.0 * sumcos / (double)length;
   *amplitude = (double) sqrt ( ( (double)sumsine*(double)sumsine 
     + (double)sumcos*(double)sumcos ) );
   *phase = (double) atan2 ( (double) sumcos, (double) sumsine );
}

/*+ sc2math_smupath - Calculate the path positions of the SMU */

void sc2math_smupath 
(
int nvert,           /* number of vertices in the jiggle pattern, 
                        implemented are :
                        =1 : No visit of points.
                        At the moment a circle but that does not work !
                        =4 : Visit 4 points on a square.
                        =5 : Visit 5 points on a '+'
                        =8 : Visit 8 points on a star. (This is the best)
                        (given) */
double vertex_t,     /* Time for movement between vertices in msec (given) */
int jig_vert[][2],   /* Table with relative vertex coordinates in time
                        (given) */
double jig_stepx,    /* The step size in -X- direction between Jiggle
                        positions in arcsec (given) */
double jig_stepy,    /* The step size in -Y- direction between Jiggle
                        positions in arcsec (given) */
int movecode,        /* The code for the SMU waveform that determines the
                        SMU motion (given) */
int nppp,            /* The number of calculated coordinates in the path
                        between 2 successive vertices (given) */
double sample_t,     /* time between samples in msec (given) */
double smu_offset,   /* smu timing offset in msec (given) */
int pathsz,          /* maximum no of path points (given) */
double jigpath[][2], /* Buffer containing the X and Y coordinates of each
                        point in the path of the SMU during the Jiggle in
                        units of arcsec (returned) */
int *status          /* global status (given and returned) */
)

/* Description :
    This routine calculates all the SMU positions during the cycle
    This depends on the Jiggle Pattern, the SMU waveform, and the 
    overdimensioning factor.

   Authors :
    H.W. van Someren Greve (greve@astron.nl)

   History :
     07-09-2001 : Original version (GREVE)
     31Oct2002  : C translation (bdk)
     13Feb2003  : change to use dream_smupos and to allow smu_offset (bdk)
     20Jun2003  : pass-in pathsz, change constant name to DREAM__MXVERT (bdk)
     15Mar2006:  copied from dream.c into sc2math (agg)
*/

{
   int np;                          /* nr of calculated points */
   int j;                           /* Loop variable */
   double anginc;                   /* Angle increment */
   double t;                        /* time at a point in the path */      
   double vertxy[DREAM__MXVERT][2]; /* coordinates of jiggle vertices */


   if ( !StatusOkP(status) ) return;

/* Make the number of positions in the Jiggle Path, and check the size */

   np = nppp * nvert;

   if ( np > pathsz )
   {
      *status = DITS__APP_ERROR;
      sprintf ( errmess,
        "%d points in the Jiggle path requested, but only %d points allowed",
        np, pathsz );
      ErsRep ( 0, status, errmess );
   }


   if ( nvert > 1)
   {


/* Calculate all the positions. */

      if ( sc2math_trace(0) )
      {
         printf ( "dream_smupath: vertext offsets are\n" );
      }
      for ( j=0; j<nvert; j++ )
      {
         vertxy[j][0] = jig_stepx * (double)jig_vert[j][0];
         vertxy[j][1] = jig_stepy * (double)jig_vert[j][1];
         if ( sc2math_trace(3) )
         {
            printf ( "dream_smupath: %e   %e\n", vertxy[j][0], vertxy[j][1] );
         }
      }

      for ( j=0; j<np; j++ )
      {
         t = (double)j * sample_t + smu_offset;
         sc2math_smupos ( t, vertex_t, movecode, nvert, vertxy,
           &(jigpath[j][0]), &(jigpath[j][1]), status );
      }
   }
   else if ( nvert == 1)
   {
      anginc = 2.0 * M_PI / (double)np;
      for ( j=0; j<np; j++ )
      {
         jigpath[j][0] = 1.2 * jig_stepx * cos ( (double)j * anginc );
         jigpath[j][1] = 0.9 * jig_stepy * sin ( (double)j * anginc );
      }

   }


   if ( sc2math_trace(2) )
   {
      printf ( "dream_smupath : jigpath with %d positions\n", np );

      printf ( "Position    -X-       -Y-\n" );
      for ( j=0; j<np; j++ )
      {
         printf ( "  %d    %e  %e\n", j, jigpath[j][0], jigpath[j][1] );
      }
   }

}



/*+ sc2math_smupos - Calculate the SMU position at an instant */

void sc2math_smupos
(
double t,           /* Time from start of pattern in msec (given) */
double vertex_t,    /* Time for movement between vertices in msec (given) */
int movecode,       /* The code for the SMU waveform that determines the
                       SMU motion (given) */
int nvert,          /* number of vertices in the DREAM pattern (given) */
double vertxy[][2], /* Table of vertex offsets in arcsec (given) */
double *x,          /* calculated X-position (returned) */
double *y,          /* calculated Y-position (returned) */
int *status         /* global status (given and returned) */
)

/* Description :
   This routine calculates the SMU waveform between 2 Jiggle positions.

   Authors :
    H.W. van Someren Greve (greve@astron.nl)

   History :
     07-09-2001 : Original version (GREVE)
     01Nov2002 : C translation (bdk)
     12Feb2003 : adapted from original routine dsim_smuwave (bdk)
     15Mar2006:  copied from dream.c into sc2math (agg)
*/

{
   int jstart;                /* index of vertex before sample point */
   int jend;                  /* index of vertex after sample point */
   double mxv;                /* Maximum value */
   double theta;              /* Fractional time between vertices in radians */
   double frac;               /* fractional distance between vertices */
   double offset;             /* time after passing first vertex in msec */

   if ( !StatusOkP(status) ) return;

/* Get vertex coordinates bracketing the measurement instant */

   if ( t < 0.0 )
   {

/* Add a cycle */

      jstart = ( (int)( (t + (double)nvert * vertex_t ) / vertex_t) ) 
        % nvert;
      offset = t + (double)nvert * vertex_t - (double)jstart * vertex_t;
   }
   else
   {
      jstart = ( (int)(t / vertex_t) ) % nvert;
      offset = t - (double)jstart * vertex_t;
   }
   jend = ( jstart + 1 ) % nvert;   


   if ( movecode == 0 )
   {

/* Block wave. This is unrealistic, but is added for compatibility
   with old code. */

      *x = vertxy[jend][0];
      *y = vertxy[jend][1];

   }
   else if ( movecode == 1 )
   {

/* 2 term not damped. */

      mxv = sin ( M_PI / 4.0 ) + sin ( 3.0 * M_PI / 4.0 ) / 3.0;
      theta = ( -M_PI / 4.0 ) + offset * M_PI / vertex_t;
      frac = 1.104 * ( ( sin(theta) + sin ( 3.0 * theta ) / 3.0 ) / ( 2.0 * mxv ) 
           + 0.5 );
      *x = frac * ( vertxy[jend][0] - vertxy[jstart][0] ) + vertxy[jstart][0];
      *y = frac * ( vertxy[jend][1] - vertxy[jstart][1] ) + vertxy[jstart][1];
   }
   else if ( movecode == 2 )
   {

/* 3 term not damped. */

      mxv = sin ( M_PI / 6.0 ) + sin ( 3.0 * M_PI / 6.0 ) / 3.0 + 
        sin ( 5.0 *M_PI / 6.0 ) / 5.0; 

      theta = ( -M_PI / 6.0 ) + offset * M_PI / vertex_t;
      frac = 1.104 * ( ( sin(theta) + sin ( 3.0 * theta ) / 3.0 + 
           sin ( 5.0 * theta ) / 5.0 ) / ( 2.0 * mxv ) + 0.5 );
      *x = frac * ( vertxy[jend][0] - vertxy[jstart][0] ) + vertxy[jstart][0];
      *y = frac * ( vertxy[jend][1] - vertxy[jstart][1] ) + vertxy[jstart][1];

   }
   else if ( movecode == 3 )
   {

/* 4 term not damped. */

      mxv = sin ( M_PI / 8.0 ) + sin ( 3.0 * M_PI / 8.0 ) / 3.0 + 
        sin ( 5.0 * M_PI / 8.0 ) / 5.0 + sin ( 7.0 * M_PI / 8.0 ) / 7.0; 
      theta = (-M_PI/8.0) + offset * M_PI / vertex_t;
      frac = 1.104 * ( ( sin(theta) + sin ( 3.0 * theta ) / 3.0 + 
        sin ( 5.0 * theta ) / 5.0 + sin ( 7.0 * theta ) / 7.0 ) 
        / ( 2.0 * mxv ) + 0.5 );
      *x = frac * ( vertxy[jend][0] - vertxy[jstart][0] ) + vertxy[jstart][0];
      *y = frac * ( vertxy[jend][1] - vertxy[jstart][1] ) + vertxy[jstart][1];

   }
   else if ( movecode == 4 )
   {

/* 2 term flat end. */

      mxv = sin(M_PI/4.0) + sin(3.0*M_PI/4.0) / 3.0;
      theta = (-M_PI/4.0) + offset * M_PI / vertex_t;
      if ( theta < M_PI/4.0 )
      {
         frac =1.02 * ( ( sin(theta) + sin(3.0*theta) / 3.0 ) / (2.0*mxv) + 0.5 );
      }
      else 
      {
         frac = 1.0;
      }
      *x = frac * ( vertxy[jend][0] - vertxy[jstart][0] ) + vertxy[jstart][0];
      *y = frac * ( vertxy[jend][1] - vertxy[jstart][1] ) + vertxy[jstart][1];

   }
   else if ( movecode == 5 )
   {

/* 3 term flat end. */

      mxv = sin(M_PI/6.0) + sin(3.0*M_PI/6.0) / 3.0 + sin(5.0*M_PI/6.0) / 5.0;
      theta = (-M_PI/6.0) + offset * M_PI / vertex_t;
      if ( theta < M_PI/6.0 )
      {
         frac = ( sin(theta) + sin(3.0*theta) / 3.0 +  sin(5.0*theta) / 5.0 ) / 
           (2.0*mxv) + 0.5;
      }
      else 
      {
         frac = 1.0;
      }
      *x = frac * ( vertxy[jend][0] - vertxy[jstart][0] ) + vertxy[jstart][0];
      *y = frac * ( vertxy[jend][1] - vertxy[jstart][1] ) + vertxy[jstart][1];

   }
   else if ( movecode == 6 )
   {

/* 4 term flat end. */

      mxv = sin(M_PI/8.0) + sin(3.0*M_PI/8.0) / 3.0 + 
        sin(5.0*M_PI/8.0) / 5.0 + sin(7.0*M_PI/8.0) / 7.0;
      theta = -M_PI/8.0 + offset * M_PI / vertex_t;
      if ( theta < M_PI/8.0 )
      {
         frac = ( sin(theta) + sin(3.0*theta) / 3.0 + sin(5.0*theta) / 5.0 + 
           sin(7.0*theta) / 7.0 ) / (2.0*mxv) + 0.5;
      }
      else 
      {
         frac = 1.0;
      }
      *x = frac * ( vertxy[jend][0] - vertxy[jstart][0] ) + vertxy[jstart][0];
      *y = frac * ( vertxy[jend][1] - vertxy[jstart][1] ) + vertxy[jstart][1];

   }
   else if ( movecode == 7 )
   {

/* ScubaWave. After 1 Ms 0.098. After 8 Ms 0.913. After 9 Ms 1.000.
   popepi points is equivalent with 64 Ms. */

      if ( offset >= 9.0 )
      {
         frac = 1.0;
      }
      else if ( offset >= 8.0 )
      {
         frac = 0.913 + 0.087 * ( offset - 8.0 );
      }
      else if ( offset >= 1.0 )
      {
         frac = 0.098 + 0.116428571 * ( offset - 1.0 );
      }
      else 
      {
         frac = 0.098 * offset;
      }
      *x = frac * ( vertxy[jend][0] - vertxy[jstart][0] ) + vertxy[jstart][0];
      *y = frac * ( vertxy[jend][1] - vertxy[jstart][1] ) + vertxy[jstart][1];

   }
   else if ( movecode == 8 )
   {

/* Experimental. 
   This is an experimental wave form, which may change often..
   Now it is a cosine waveform from 0 to 1 in the full time. */

      frac = 0.5 * ( 1.0 - cos ( M_PI * offset / vertex_t ) );
      *x = frac * ( vertxy[jend][0] - vertxy[jstart][0] ) + vertxy[jstart][0];
      *y = frac * ( vertxy[jend][1] - vertxy[jstart][1] ) + vertxy[jstart][1];

   }

}

/*+ sc2math_sol - Solution of least square fit */

void sc2math_sol 
(
int nunkno,      /* The number of unknowns (given) */
int nequ,        /* The number of observed points (given) */
double lmat[],   /* lower left triangle of inverted equation matrix
                    (given) */
double lvec[],   /* array containing the known vector (given) */
double lssum,    /* The sum of the square of known terms (given) */
double *lme,     /* Quality measure of the Least Square Solution (returned) */
double lmex[],   /* rms errors of the vector of solutions (returned) */
double lsol[]    /* solved parameters of the Least Square Solution
                    (returned) */
)

/* Description :
    Solution routine of the Least Square fit.
    This routine is called after the loop over all observed points
    in which the known vector LVEC has been made.

    lmat is the lower left triangle of the inverted equation matrix.
    This is a 1 dimensional array of dimension NUNKNO*(NUNKNO+1)/2 
    which must be filled before the call of this routine.

    lvec is an array containing the known vector, i.e. the vector 
    containing sum terms with the observed values.
    The array must be filled before the call of this routine.

    lssum is the sum of the square of known terms.
    The value must be filled before the call of this routine. 
    The sum is used for determining the total quality of the least 
    square solution.
     
    The solution vector (LSOL) is simply the matrix (LMAT) multiplication 
    with the vector (LVEC), done in MSV.
    NEQU and LSSUM are only needed for calculating the rms values.
    These rms values can be calculated because of the least square fit
    aspect of the solution, when we have more equations than unknowns.

   Authors :
    W.N. Brouw  (wbrouw@csiro.au)

   History :
    1997 : adopted for SCUBA work (GREVE)
    04Dec2002 : C translation (bdk)
    15Mar2006:  copied from lsq.c into sc2math (agg)
*/

{

   int i;
   int m;
   int ir1;
   double w1;


/* Multiply matrix LMAT with vector LVEC giving vector LSOL */

   sc2math_msv ( nunkno, lmat, lvec, lsol );
   w1 = lssum;
   for ( i=0; i<nunkno; i++ )
   {
      w1 = w1 - lsol[i] * lvec[i];
   }

   ir1 = nequ - nunkno;
   if (ir1 <= 0) ir1 = 1;

/* RMS error per observation squared */

   *lme = w1 / ir1;
   for ( m=1; m<=nunkno; m++ )
   {
      i = (m*m + m) / 2;

/* RMS error solution */

      w1 = fabs ( lmat[i-1] * (*lme) );
      lmex[m-1] = sqrt ( w1 );
   }

/* RMS error observation */

   *lme = sqrt ( fabs(*lme) );

}

/*+ sc2math_trace - provide a flag for debugging level */

int sc2math_trace
( 
int value       /* trace level (given) */
)

/*  Description :
     On first entry store the given trace level. On subsequent entries
     compare the new given value with the stored value.
     If new value <= stored value, return 1, else return 0.
    Authors :
     B.D.Kelly (ROE)
    History :
     04Sep2002:  original (bdk@roe.ac.uk)
     15Mar2006:  copied from dream.c into sc2math (agg)
*/

{
   static int localtrace = -1;            /* stored level */
   int ans;                               /* returned value */

   if ( localtrace == -1 )
   {
      localtrace = value;
      ans = 0;
   }
   else if ( value <= localtrace )
   {
      ans = 1;
   }
   else
   {
      ans = 0;
   }

   /* Force zero for now (agg) */
   ans = 0;

   return ans;
}

/*+ sc2math_vec - Make known vector */

void sc2math_vec 
(
int nunkno,        /* The number of unknowns (given) */
double lcoef[],    /* The known values (given) */
double lknow,      /* The measured point, or observed value (given) */
double lvec[],     /* array to contain the known vector 
                      (given and returned) */ 
double *lssum      /* sum of the square of known terms (returned) */
)

/* Description :
    This routine makes a sum of the known values for a least 
    squares solution.
    The routine is called in a loop per observed point.
    There should be >= observed points than NUNKNO.
    This routine differs only from LSQ_WVEC because all WEIGHT
    factors are implicitly equal to 1.0.

     lcoef contains the known coefficients of the
     least square equation, for example for a polynomial the vector 
     contains the values : 1.0, X, X**2, X**3, etc.

     lvec is an array containing the known vector, i.e. the vector 
     containing sum terms with LKNOW.
     The array must be cleared before the first call of this routine.

     lssum is the sum of the square of known terms.
     The value must be cleared before the first call of this routine.
     The sum is used later for determining the total quality of the 
     least square solution.

   Authors :
    W.N. Brouw  (wbrouw@csiro.au)

   History :
    1997 : adopted for SCUBA work (GREVE)
    04Dec2002 : C translation (bdk)
    15Mar2006:  copied from lsq.c into sc2math (agg)
*/

{
   int j;


/* Summation of terms of the known vector with known values */

   for ( j=0; j<nunkno; j++ )
   {
     lvec[j] = lvec[j] + lcoef[j] * lknow;
   }

   *lssum = *lssum + lknow*lknow;

}

/*+  sc2math_wavegen - generate sine and cosine signals */

void sc2math_wavegen 
( 
double period,                 /* period in array elements (given) */
int length,                  /* length of arrays (given) */
double *sine,                  /* generated sine wave (returned) */
double *cosine,                /* generated cosine wave (returned) */
int *status                  /* global status (given and returned) */
)

/*   Method :
      Calculate sine and cosine waves, zero phase occuring at the first 
      element, of the given period.
     Authors :
      B.D.Kelly (REVAD::BDK)
     History :
      15.10.1993: original (REVAD::BDK)
     endhistory
*/

{

   int j;                        /* loop counter */
   double twopi;                  /* ( 2 * pi ) */

   if ( !StatusOkP(status) ) return;

   twopi = 2.0 * 3.14159;

   for ( j=0; j<length; j++ )
   {
      sine[j] = (double) sin ( twopi * (double)j / (double)period );
      cosine[j] = (double) cos ( twopi * (double)j / (double)period );
   }
}
