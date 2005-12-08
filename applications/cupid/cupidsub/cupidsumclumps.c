#include "sae_par.h"
#include "ndf.h"
#include "cupid.h"
#include "prm_par.h"
#include "star/hds.h"
#include <string.h>

/* Local Constants: */
#define CLUMPFIND   1
#define GAUSSCLUMPS 2
#define UNKNOWN     3

void cupidSumClumps( int dtype, int ndim, int *lbnd, int *ubnd, int nel, 
                     HDSLoc **clist, int nclump, double bg, 
                     float *rmask, void *out, char *method ){
/*
*  Name:
*     cupidSumClumps

*  Purpose:
*     Create an image holding the sum of the supplied clumps, and another
*     holding a mask.

*  Synopsis:
*     void cupidSumClumps( int dtype, int ndim, int *lbnd, int *ubnd, int nel, 
*                          HDSLoc **clist, int nclump, double bg, 
*                          float *rmask, void *out, char *method )

*  Description:
*     This function stores an image of the sum of all the found clumps
*     in "out", and stores a mask in "rmask" identifying the pixels which
*     are inside a clump.

*  Parameters:
*     dtype
*        The data type of the input NDF, CUPID__DOUBLE or CUPID__FLOAT.
*     ndim
*        The number of pixel axes in the "out" array.
*     lbnd
*        The lower bounds on the pixel axes of "out".
*     ubnd
*        The upper bounds on the pixel axes of "out".
*     nel
*        Number of pixels in each of "out" and "rmask".
*     clist
*        A pointer to an array of "nclump" HDS locators. These locators will 
*        be annulled before returning.
*     nclump
*        The number of locators in "clist".
*     bg
*        The background level to be added to "out".
*     rmask
*        The array to receive the mask. A value of 1.0 will be stored in
*        every pixel which inside a clump. All other pixels will be set
*        to VAL__BADR. The supplied array must be the same size and shape as 
*        "out" and the pixels are assumed to be in fortran order. May be NULL.
*     out
*        The array to receive the sum of the clump intensities plus the
*        background given by "bg". The supplied array must be the same size 
*        as the user supplied NDF and the pixels are assumed to be in 
*        fortran order. May be NULL.
*     method
*        The name of the algorithm being used (e.g. "CLUMPFIND",
*        "GAUSSCLUMPS", etc)

*  Authors:
*     DSB: David S. Berry
*     {enter_new_authors_here}

*  History:
*     15-NOV-2005 (DSB):
*        Original version.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}
*/      

/* Local Variables: */
   double *ipd;         /* Pointer to data array in clump NDF */
   double *m;           /* Pointer to next clump data value */
   double *pd;          /* Pointer to next element of the "out" array */
   float *pf;           /* Pointer to next element of the "out" array */
   float *r;            /* Pointer to next element of the "rmask" array */
   int *pi;             /* Pointer to next element of the "out" array */
   int alg;             /* Specifies the algorithm */
   int cdim[ 3 ];       /* Pixel axis dimensions within clump NDF */
   int clbnd[ 3 ];      /* Lower pixel bounds of clump NDF */
   int cndim;           /* Number of pixel axes in clump NDF */
   int cubnd[ 3 ];      /* Upper pixel bounds of clump NDF */
   int el;              /* Number of pixels in clump NDF */
   int i;               /* 1D vector index loop count */
   int ii;              /* 1D vector index of current "out" pixel */
   int indf;            /* NDF identifier for clump image */
   int j;               /* Clump loop count */
   int k;               /* Axis loop count */
   int place;           /* Unused NDF placeholder */
   int step[ 3 ];       /* 1D step between adjacent pixels on each axis */
   int yy[ 3 ];         /* nD pixel coords of current "out" pixel */

/* Abort if an error has already occurred. */
   if( *status != SAI__OK ) return;

/* Identify the algorithm. */
   if( !strcmp( method, "CLUMPFIND" ) ) {
      alg = CLUMPFIND;

   } else if( !strcmp( method, "GAUSSCLUMPS" ) ) {
      alg = GAUSSCLUMPS;

   } else {
      alg = UNKNOWN;
   }

/* If supplied, fill the "out" array with the background value. */
   if( out ) {
      if( alg == GAUSSCLUMPS ) {
         if( dtype == CUPID__DOUBLE ) {
            pd = (double *) out;
            for( i = 0; i < nel; i++ ) *(pd++) = bg;      
         } else {
            pf = (float *) out;
            for( i = 0; i < nel; i++ ) *(pf++) = bg;      
         }

      } else if( alg == CLUMPFIND ) {
         pi = (int *) out;
         for( i = 0; i < nel; i++ ) *(pi++) = VAL__BADI;
      }
   }

/* If supplied, fill the "rmask" array with bad values. */
   if( rmask ) {
      r = rmask;
      for( i = 0; i < nel; i++ ) *(r++) = VAL__BADR;
   }

/* Set up the 1D vector step size for stepping between adjacent pixels on each 
   axis of "out/rmask". */
   step[ 0 ] = 1;   
   for( k = 1; k < ndim; k++ ) {
      step[ k ] = step[ k - 1 ]*( ubnd[ k - 1 ] - lbnd[ k - 1 ] + 1 );
   }

/* Store a pointer of the relevant type to the output array. */
   if( alg == GAUSSCLUMPS ) {
      if( dtype == CUPID__DOUBLE ) {
         pd = (double *) out;
      } else {
         pf = (float *) out;
      }

   } else if( alg == CLUMPFIND ) {
      pi = (int *) out;
   }

/* Loop round all non-NULL clumps. */
   for( j = 0; j < nclump; j++ ) {
      if( !clist[ j ] ) continue;

/* The component named MODEL contains an NDF holding the clump model values. 
   Get an NDF identifier for it. */
      ndfOpen( clist[ j ], "MODEL", "READ", "OLD", &indf, &place, status );

/* Get its bounds and its dimensions. */
      ndfBound( indf, 3, clbnd, cubnd, &cndim, status );
      for( k = 0; k < ndim; k++ ) cdim[ k ] = cubnd[ k ] - clbnd[ k ] + 1;

/* Map its DATA component. */
      ndfMap( indf, "DATA", "_DOUBLE", "READ", (void *) &ipd, &el, status );
      if( ipd ) {

/* Initialise the n-D pixel coords of the first pixel in "ipd". Also, set 
   the 1D vector index within "out/rmask" of the first pixel in 
   "ipd". This assumes fortran pixel ordering. */
         ii = 0;
         for( k = 0; k < ndim; k++ ) {
            yy[ k ] = clbnd[ k ];
            ii += ( clbnd[ k ] - lbnd[ k ] )*step[ k ];
         }

/* Loop round every pixel in the clump image. */
         m = ipd;
         for( i = 0; i < el; i++, m++ ) {

/* Skip bad pixels */
            if( *m != VAL__BADD ) {

/* If required, increment the "out" array by the values in the NDFs Data
   array, or store the clump index. */
               if( out ) {

                  if( alg == GAUSSCLUMPS ) {
                     if( dtype == CUPID__DOUBLE ) {
                        pd[ ii ] += *m;
                     } else {
                        pf[ ii ] += *m;
                     }

                  } else if( alg == CLUMPFIND ) {
                     pi[ ii ] = j + 1;
                  }               
               }
         
/* If required, store 1.0 in the "rmask" array. */
               if( rmask ) rmask[ ii ] = 1.0;
            }

/* Move on to the next pixel. */
            k = 0;
            yy[ 0 ]++;
            ii++;
            while( yy[ k ] > cubnd[ k ] ) {
               yy[ k ] = clbnd[ k ];
               ii -= cdim[ k ]*step[ k ];
               k++;
               if( k == ndim ) break;
               yy[ k ]++;
               ii += step[ k ];
            }
         }
      }

/* Annul the NDF identifier. */
      ndfAnnul( &indf, status );

   }

}

/* Undefine Local Constants: */
#undef CLUMPFIND
#undef GAUSSCLUMPS
#undef UNKNOWN

