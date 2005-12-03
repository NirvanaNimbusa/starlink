#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include "f77.h"              /* F77 <-> C interface macros                  */
#include "cnf.h"              /* F77 <-> C string handling functions         */
#include "str.h"              /* String constants                            */
#include "hds1_types.h"
#include "hds_types.h"
#include "hds1.h"
#include "hds2.h"             /* for datWhere                                */
#include "hds.h"              /* HDS C interface			     */
#include "dat_par.h"          /* DAT__ constant definitions                  */
#include "ems.h"
#include "ems_par.h"

#include "hds_fortran.h"      /* Fortran import/export */

#define TRUE 1
#define FALSE 0
#define SAI__OK 0

F77_SUBROUTINE(dat_getc)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          CHARACTER(values),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(values)
			  );

F77_SUBROUTINE(dat_alter)( CHARACTER(locator),
                           F77_INTEGER_TYPE *ndim,
                           FORTRAN_INDEX_TYPE dims[],
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{
/*==================================*/
/* DAT_ALTER - Alter size of object */
/*==================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the locator string                  */

   dat1_import_floc( locator, locator_length, &locator_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];
      
/* Call pure C routine                                       */
   datAlter( &locator_c, *ndim, dims64, status );
#else
   datAlter( &locator_c, *ndim, dims, status );
#endif
}

F77_SUBROUTINE(dat_annul)( CHARACTER(locator),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{
/*===========================*/
/* DAT_ANNUL - Annul locator */
/*===========================*/
/* Local variable */
   
   HDSLoc *locator_c = NULL;

/* Enter routine.	*/

/* Import the locator string                  */
   
   datImportFloc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datAnnul( &locator_c, status);

/* Export returned locator */
   
   datExportFloc( &locator_c, 1, locator_length, locator, status );

}

F77_SUBROUTINE(dat_basic)( CHARACTER(locator),
                           CHARACTER(mode),
                           F77_POINTER_TYPE *pntr,
                           F77_INTEGER_TYPE *len,
                           F77_INTEGER_TYPE *status
	                   TRAIL(locator) 
                           TRAIL(mode) )
{

/*===============================================*/
/* DAT_BASIC - Map data (in basic machine units) */
/*===============================================*/

/* Local variables.     */
   char mode_c[DAT__SZMOD+1];
   unsigned char *cpntr = NULL; /* initialise in case of bad return status */
   HDSLoc locator_c;
   
/* Enter routine.	*/

/* Import the locator and mode strings  */

   dat1_import_floc( locator, locator_length, &locator_c, status );
   cnfImpn( mode, mode_length, DAT__SZMOD, mode_c);

/* Call pure C routine                                       */
   datBasic( &locator_c, mode_c, &cpntr, len, status);

/* Export the C pointer as a FORTRAN pointer */
   *pntr = cnfFptr( cpntr );
}

F77_SUBROUTINE(dat_ccopy)( CHARACTER(locator1),
                           CHARACTER(locator2),
                           CHARACTER(name),
                           CHARACTER(locator3),
                           F77_INTEGER_TYPE *status
 	                   TRAIL(locator1)
                           TRAIL(locator2)
                           TRAIL(name)
                           TRAIL(locator3) )
 {

/*======================================*/
/* DAT_CCOPY - copy one structure level */
/*======================================*/

/* Local variables.     */ 
   HDSLoc locator1_c;
   HDSLoc locator2_c;
   HDSLoc *locator3_c = NULL;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/

/* Import maxmimum length strings  */
   cnfImpn( name, name_length, DAT__SZNAM, name_c);

/* Import the input locator strings                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status );
   dat1_import_floc( locator2, locator2_length, &locator2_c, status );

/* Call pure C routine                                       */
   datCcopy( &locator1_c, &locator2_c, name_c, &locator3_c, status);

/* Export returned locator    */
   datExportFloc( &locator3_c, 1, locator3_length, locator3, status );
}

F77_SUBROUTINE(dat_cctyp)( INTEGER(size), CHARACTER(type)
                           TRAIL(type) )
{
/*============================================*/
/* DAT_CCTYP - Determine _CHAR*nnnn type      */
/*============================================*/

  char type_c[DAT__SZTYP+1];
  datCctyp( *size, type_c );
  cnfExprt( type_c, type, type_length );
}

F77_SUBROUTINE(dat_cell)( CHARACTER(locator1),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE subs[],
                          CHARACTER(locator2),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator1)
                          TRAIL(locator2) )
{

/*============================================*/
/* DAT_CELL - Locate a "cell" (array element) */
/*============================================*/

/* Local variables.     */
   HDSLoc locator1_c;
   HDSLoc *locator2_c = NULL;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG subs64[DAT__MXDIM];
   int i;
#endif

/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      subs64[i] = subs[i];
      
/* Call pure C routine                                       */
   datCell( &locator1_c, *ndim, subs64, &locator2_c, status);
#else
   datCell( &locator1_c, *ndim, subs, &locator2_c, status);
#endif

/* Export returned locator */
   datExportFloc( &locator2_c, 1, locator2_length, locator2, status ); 
}

F77_SUBROUTINE(dat_clen)( CHARACTER(locator),
                          F77_INTEGER_TYPE *clen,
                          F77_INTEGER_TYPE *status
	                  TRAIL(locator) )
{

/*===========================================*/
/* DAT_CLEN - Obtain character string length */
/*===========================================*/

/* Local variables */
   HDSLoc locator_c;
   size_t len_c;

/* Enter routine.	*/

/* Import the locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

/* Call pure C routine                                       */
   datClen( &locator_c, &len_c, status);
   *clen = (F77_INTEGER_TYPE)len_c;
}

F77_SUBROUTINE(dat_clone)( CHARACTER(locator1),
                           CHARACTER(locator2),
                           F77_INTEGER_TYPE *status
	                   TRAIL(locator1)
                           TRAIL(locator2) )
{

/*===========================*/
/* DAT_CLONE - clone locator */
/*===========================*/

/* Local variables */
   HDSLoc locator1_c;
   HDSLoc *locator2_c = NULL;

/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status );

/* Call pure C routine                                       */
   datClone( &locator1_c, &locator2_c, status);

/* Export the returned locator */
   datExportFloc( &locator2_c, 1, locator2_length, locator2, status );
}

F77_SUBROUTINE(dat_coerc)( CHARACTER(locator1),
                           F77_INTEGER_TYPE *ndim,
                           CHARACTER(locator2),
                           F77_INTEGER_TYPE *status
	                   TRAIL(locator1)
                           TRAIL(locator2) )
{

/*=================================*/
/* DAT_COERC - coerce object shape */
/*=================================*/

/* Local variables.     */
   HDSLoc locator1_c;
   HDSLoc *locator2_c = NULL;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status);

/* Call pure C routine                                       */
   datCoerc( &locator1_c, *ndim, &locator2_c, status);

/* Export the returned locator                      */
   datExportFloc( &locator2_c, 1, locator2_length, locator2, status );
}

F77_SUBROUTINE(dat_copy)( CHARACTER(locator1),
                          CHARACTER(locator2),
                          CHARACTER(name),
                          F77_INTEGER_TYPE *status
	                  TRAIL(locator1)
                          TRAIL(locator2)
                          TRAIL(name) )
{

/*========================*/
/* DAT_COPY - copy object */
/*========================*/

/* Local variables.     */
   HDSLoc locator1_c;
   HDSLoc locator2_c;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/

/* Import name string */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c);

/* Import the locator strings                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status);
   dat1_import_floc( locator2, locator2_length, &locator2_c, status);

/* Call pure C routine                                       */
   datCopy( &locator1_c, &locator2_c, name_c, status);
}

F77_SUBROUTINE(dat_drep)( CHARACTER(locator),
                          CHARACTER(format),
                          CHARACTER(order),
                          F77_INTEGER_TYPE *status
	                  TRAIL(locator)
                          TRAIL(format)
                          TRAIL(order) )
{

/*=============================================================*/
/* DAT_DREP - Obtain primitive data representation information */
/*=============================================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char *format_c;
   char *order_c;
   
/* Enter routine.	*/

/* Import the locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

/* Call pure C routine                                      */
   datDrep( &locator_c, &format_c, &order_c, status);

/* Export the returned C strings to the FORTRAN variables    */

   if( *status == SAI__OK )
   {
      cnfExprt( format_c, format, format_length );
      cnfExprt( order_c,  order,  order_length  );
   }
}

F77_SUBROUTINE(dat_erase)( CHARACTER(locator),
                           CHARACTER(name),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator)
                           TRAIL(name) )
{

/*=========================================*/
/* DAT_ERASE - Erase object                */
/*=========================================*/

/* Local variables.     */
   char name_c[DAT__SZNAM+1];
   HDSLoc locator_c;
   
/* Enter routine.	*/

/* Import name string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c);

/* Import the locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

/* Call pure C routine                                       */
   datErase( &locator_c, name_c, status);
}

F77_SUBROUTINE(dat_ermsg)( F77_INTEGER_TYPE *status,
			   F77_INTEGER_TYPE *length,
                           CHARACTER(msg)
                           TRAIL(msg) )
{

/*============================================================*/
/* DAT_ERMSG - Translate a status value into an error message */
/*============================================================*/

/* Local variables.     */
   char *msg_c;
   size_t length_c;

/* Enter routine.	*/

/* Call pure C routine                                       */
   datErmsg( *status, &length_c, &msg_c );

/* Export the returned C string to FORTRAN variable    */
   *length = (F77_INTEGER_TYPE)length_c;
   cnfExprt( msg_c, msg, msg_length );
}


F77_SUBROUTINE(dat_find)( CHARACTER(locator1),
                          CHARACTER(name),
                          CHARACTER(locator2),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator1)
                          TRAIL(name)
                          TRAIL(locator2) )
{

/*=================================*/
/* DAT_FIND - Find named component */
/*=================================*/

/* Local variables.     */
   char name_c[DAT__SZNAM+1];
   HDSLoc locator1_c;
   HDSLoc *locator2_c = NULL;
   

/* Enter routine.	*/

/* Import name  string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c);

/* Import the input locator string                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status );

/* Call pure C routine                                       */
   datFind( &locator1_c, name_c, &locator2_c, status);

/* Export the returned locator */
   datExportFloc( &locator2_c, 1, locator2_length, locator2, status );
}

F77_SUBROUTINE(dat_get)( CHARACTER(locator),
                         CHARACTER(type),
                         F77_INTEGER_TYPE *ndim,
                         FORTRAN_INDEX_TYPE dims[],
                         F77_BYTE_TYPE values[],
                         F77_INTEGER_TYPE *status
                         TRAIL(locator)
                         TRAIL(type)
			 TRAIL(values))
{

/*=============================*/
/* DAT_GET - Read primitive(s) */
/*=============================*/

/* Local variables.     */
   HDSLoc locator_c;
   char type_c[DAT__SZTYP+1];
   int ischar = 0;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import FORTRAN to C string  */
   cnfImpn( type, type_length, DAT__SZTYP,  type_c);

   /* Special case _CHAR since fortran is telling us the length */
   if (strncmp(type,"_CHAR",5) == 0) ischar = 1;

/* Import the locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   if (ischar) {
     datGetC( &locator_c, *ndim, dims64, (char*)values, values_length, status );
   } else {
     datGet( &locator_c, type_c, *ndim, dims, values, status);
   }
#else
   if (ischar) {
     datGetC( &locator_c, *ndim, dims, (char*)values, values_length, status );
   } else {
     datGet( &locator_c, type_c, *ndim, dims, values, status);
   }
#endif
}

F77_SUBROUTINE(dat_getc)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          CHARACTER(values),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(values)
			  )
{

/*====================================*/
/* DAT_GETC - Read _CHAR primitive(s) */
/*====================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datGetC( &locator_c, *ndim, dims64, values, values_length, status);
#else
   datGetC( &locator_c, *ndim, dims, values, values_length, status);
#endif
}

F77_SUBROUTINE(dat_getd)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_DOUBLE_TYPE values[],
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*======================================*/
/* DAT_GETD - Read _DOUBLE primitive(s) */
/*======================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datGetD( &locator_c, *ndim, dims64, values, status);
#else
   datGetD( &locator_c, *ndim, dims, values, status);
#endif
}

F77_SUBROUTINE(dat_geti)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_INTEGER_TYPE *values,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*=======================================*/
/* DAT_GETI - Read _INTEGER primitive(s) */
/*=======================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datGetI( &locator_c, *ndim, dims64, values, status);
#else
   datGetI( &locator_c, *ndim, dims, values, status);
#endif
}

F77_SUBROUTINE(dat_getl)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_LOGICAL_TYPE *values,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*=======================================*/
/* DAT_GETL - Read _LOGICAL primitive(s) */
/*=======================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datGetL( &locator_c, *ndim, dims64, values, status);
#else
   datGetL( &locator_c, *ndim, dims, values, status);
#endif
}

F77_SUBROUTINE(dat_getr)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_REAL_TYPE *values,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*====================================*/
/* DAT_GETR - Read _REAL primitive(s) */
/*====================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datGetR( &locator_c, *ndim, dims64, values, status);
#else
   datGetR( &locator_c, *ndim, dims, values, status);
#endif
}

/* ================================== */
/*  datGet0x                          */
/* ================================== */

F77_SUBROUTINE(dat_get0c)( CHARACTER(locator),
			   CHARACTER(value),
			   INTEGER(status)
			   TRAIL(locator) 
			   TRAIL(value) )
{
  HDSLoc locator_c;
  int cvalue_length;
  char * value_c;

  cvalue_length = value_length + 1;
  value_c = cnfCreat( cvalue_length );
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datGet0C( &locator_c, value_c, cvalue_length, status );
  cnfExprt( value_c, value, value_length );
  cnfFree(value_c);
}


F77_SUBROUTINE(dat_get0d)( CHARACTER(locator),
			   DOUBLE(value),
			   INTEGER(status)
			   TRAIL(locator) )
{
  HDSLoc locator_c;
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datGet0D( &locator_c, value, status );
}

F77_SUBROUTINE(dat_get0r)( CHARACTER(locator),
			   REAL(value),
			   INTEGER(status)
			   TRAIL(locator) )
{
  HDSLoc locator_c;
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datGet0R( &locator_c, value, status );
}

F77_SUBROUTINE(dat_get0i)( CHARACTER(locator),
			   INTEGER(value),
			   INTEGER(status)
			   TRAIL(locator) )
{
  HDSLoc locator_c;
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datGet0I( &locator_c, value, status );
}

F77_SUBROUTINE(dat_get0l)( CHARACTER(locator),
			   LOGICAL(value),
			   INTEGER(status)
			   TRAIL(locator) )
{
  HDSLoc locator_c;
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datGet0L( &locator_c, value, status );
}


F77_SUBROUTINE(dat_index)( CHARACTER(locator1),
                           F77_INTEGER_TYPE *index,
                           CHARACTER(locator2),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator1)
                           TRAIL(locator2) )
{

/*=======================================*/
/* DAT_INDEX - Index into component list */
/*=======================================*/

/* Local variables.     */
   HDSLoc locator1_c;
   HDSLoc *locator2_c = NULL;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status);

/* Call pure C routine                                       */
   datIndex( &locator1_c, *index, &locator2_c, status);

/*Export the returned locator */
   datExportFloc( &locator2_c, 1, locator2_length, locator2, status );
}

F77_SUBROUTINE(dat_len)( CHARACTER(locator),
                         F77_INTEGER_TYPE *len,
                         F77_INTEGER_TYPE *status
                         TRAIL(locator) )
{

/*====================================*/
/* DAT_LEN - Inquire primitive length */
/*====================================*/

/* Local variables */
   HDSLoc locator_c;
   size_t len_c;

/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

/* Call pure C routine                                       */
   datLen( &locator_c, &len_c, status );
   *len = (F77_INTEGER_TYPE)len_c;
}

F77_SUBROUTINE(dat_map)( CHARACTER(locator),
                         CHARACTER(type),
                         CHARACTER(mode),
                         F77_INTEGER_TYPE *ndim,
                         FORTRAN_INDEX_TYPE dims[],
                         F77_POINTER_TYPE *pntr,
                         F77_INTEGER_TYPE *status
                         TRAIL(locator)
                         TRAIL(type)
                         TRAIL(mode) )
{

/*============================*/
/* DAT_MAP - Map primitive(s) */
/*============================*/

/* Local variables.     */
   HDSLoc locator_c;
   char type_c[DAT__SZTYP+1];
   char mode_c[DAT__SZMOD+1];
   void *cpntr = NULL; /* initialise in case of bad return status */
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import FORTRAN to C strings  */
   cnfImpn( type, type_length, DAT__SZTYP,  type_c);
   cnfImpn( mode, mode_length, DAT__SZMOD,  mode_c);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datMap( &locator_c, type_c, mode_c, *ndim, dims64, &cpntr, status);
#else
   datMap( &locator_c, type_c, mode_c, *ndim, dims, &cpntr, status);
#endif

/* Export the C pointer as a FORTRAN POINTER */
   *pntr = cnfFptr( cpntr );
}

F77_SUBROUTINE(dat_mapc)( CHARACTER(locator),
                          CHARACTER(mode),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_POINTER_TYPE *pntr,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(mode) )
{

/*===================================*/
/* DAT_MAPC - Map _CHAR primitive(s) */
/*===================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char mode_c[DAT__SZMOD+1];
   unsigned char *cpntr = NULL; /* Initialise in case of bad return status */
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

/* Import mode string  */
   cnfImpn( mode, mode_length, DAT__SZMOD,  mode_c);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datMapC( &locator_c, mode_c, *ndim, dims64, &cpntr, status);
#else
   datMapC( &locator_c, mode_c, *ndim, dims, &cpntr, status);
#endif

/* Export the C pointer as a FORTRAN POINTER */
   *pntr = cnfFptr( cpntr );
}

F77_SUBROUTINE(dat_mapd)( CHARACTER(locator),
                          CHARACTER(mode),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_POINTER_TYPE *pntr,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(mode) )
{

/*=====================================*/
/* DAT_MAPD - Map _DOUBLE primitive(s) */
/*=====================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char mode_c[DAT__SZMOD+1];
   double *cpntr = NULL; /* initialise in case of bad return status */
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import mode string  */
   cnfImpn( mode, mode_length, DAT__SZMOD,  mode_c);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datMapD( &locator_c, mode_c, *ndim, dims64, &cpntr, status);
#else
   datMapD( &locator_c, mode_c, *ndim, dims, &cpntr, status);
#endif

/* Export the C pointer as a FORTRAN POINTER */
   *pntr = cnfFptr( cpntr );
}

F77_SUBROUTINE(dat_mapi)( CHARACTER(locator),
                          CHARACTER(mode),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_POINTER_TYPE *pntr,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(mode) )
{

/*======================================*/
/* DAT_MAPI - Map _INTEGER primitive(s) */
/*======================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char mode_c[DAT__SZMOD+1];
   int *cpntr = NULL; /* initialise in case of bad return status */
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import mode string  */
   cnfImpn( mode, mode_length, DAT__SZMOD,  mode_c);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datMapI( &locator_c, mode_c, *ndim, dims64, &cpntr, status);
#else
   datMapI( &locator_c, mode_c, *ndim, dims, &cpntr, status);
#endif

/* Export the C pointer as a FORTRAN POINTER */
   *pntr = cnfFptr( cpntr );
}

F77_SUBROUTINE(dat_mapl)( CHARACTER(locator),
                          CHARACTER(mode),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_POINTER_TYPE *pntr,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(mode) )
{

/*======================================*/
/* DAT_MAPL - Map _LOGICAL primitive(s) */
/*======================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char mode_c[DAT__SZMOD+1];
   int *cpntr = NULL; /* initialise in case of bad return status */
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import mode string  */
   cnfImpn( mode, mode_length, DAT__SZMOD,  mode_c);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datMapL( &locator_c, mode_c, *ndim, dims64, &cpntr, status);
#else
   datMapL( &locator_c, mode_c, *ndim, dims, &cpntr, status);
#endif

/* Export the C pointer as a FORTRAN POINTER */
   *pntr = cnfFptr( cpntr );
}

F77_SUBROUTINE(dat_mapr)( CHARACTER(locator),
                          CHARACTER(mode),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_POINTER_TYPE *pntr,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(mode) )
{

/*===================================*/
/* DAT_MAPR - Map _REAL primitive(s) */
/*===================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char mode_c[DAT__SZMOD+1];
   float *cpntr = NULL; /* initialise in case of bad return status */
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import mode string  */
   cnfImpn( mode, mode_length, DAT__SZMOD,  mode_c);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datMapR( &locator_c, mode_c, *ndim, dims64, &cpntr, status );
#else
   datMapR( &locator_c, mode_c, *ndim, dims, &cpntr, status );
#endif

/* Export the C pointer as a FORTRAN POINTER */
   *pntr = cnfFptr( cpntr );
}

F77_SUBROUTINE(dat_mapv)( CHARACTER(locator),
			  CHARACTER(type),
                          CHARACTER(mode),
                          F77_POINTER_TYPE *pntr,
			  F77_INTEGER_TYPE *actval,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(type)
                          TRAIL(mode) )
{

/*=====================================*/
/* DAT_MAPV - Map vectorized primitive(s) */
/*=====================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char mode_c[DAT__SZMOD+1];
   char type_c[DAT__SZTYP+1];
   void *cpntr = NULL; /* initialise in case of bad return status */
   size_t actval_c;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import mode and type string  */
   cnfImpn( mode, mode_length, DAT__SZMOD,  mode_c);
   cnfImpn( type, type_length, DAT__SZTYP,  type_c);

   datMapV( &locator_c, type_c, mode_c, &cpntr, &actval_c, status);

/* Export the C pointer as a FORTRAN POINTER */
   *pntr = cnfFptr( cpntr );
   *actval = (F77_INTEGER_TYPE) actval_c;
}

F77_SUBROUTINE(dat_mapn)( CHARACTER(locator),
                         CHARACTER(type),
                         CHARACTER(mode),
			 INTEGER(ndim),
                         F77_POINTER_TYPE *pntr,
                         FORTRAN_INDEX_TYPE dims[],
                         F77_INTEGER_TYPE *status
                         TRAIL(locator)
                         TRAIL(type)
                         TRAIL(mode) )
{

/*============================*/
/* DAT_MAP - Map primitive(s) */
/*============================*/

/* Local variables.     */
   HDSLoc locator_c;
   char type_c[DAT__SZTYP+1];
   char mode_c[DAT__SZMOD+1];
   void *cpntr = NULL; /* initialise in case of bad return status */
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import FORTRAN to C strings  */
   cnfImpn( type, type_length, DAT__SZTYP,  type_c);
   cnfImpn( mode, mode_length, DAT__SZMOD,  mode_c);

#if HDS_COPY_FORTRAN_DIMS
/* Call pure C routine                                       */
   datMapN( &locator_c, type_c, mode_c, *ndim, &cpntr, dims64, status);

   /* Copy the array back to fortran from HDS_PTYPE */
   for( i = 0; i<_min(*ndimx,*ndim); i++)
      dims[i] = dims64[i];

#else
   datMapN( &locator_c, type_c, mode_c, *ndim, &cpntr, dims, status);
#endif

/* Export the C pointer as a FORTRAN POINTER */
   *pntr = cnfFptr( cpntr );
}


F77_SUBROUTINE(dat_mould)( CHARACTER(locator),
                           F77_INTEGER_TYPE *ndim,
                           FORTRAN_INDEX_TYPE dims[],
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{
/*==================================*/
/* DAT_MOULD - Alter shape of object */
/*==================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datMould( &locator_c, *ndim, dims64, status );
#else
   datMould( &locator_c, *ndim, dims, status );
#endif
}

F77_SUBROUTINE(dat_move)( CHARACTER(locator1),
                          CHARACTER(locator2),
                          CHARACTER(name),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator1)
                          TRAIL(locator2)
                          TRAIL(name) )
{

/*========================*/
/* DAT_MOVE - Move object */
/*========================*/

/* Local variables.     */
   HDSLoc *locator1_c = NULL;
   HDSLoc locator2_c;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/

/* Import the first locator string                  */
   datImportFloc( locator1, locator1_length, &locator1_c, status );

/* Import the second locator string                  */
   dat1_import_floc( locator2, locator2_length, &locator2_c, status );

/* Import "name" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

/* Call pure C routine                                       */
   datMove( &locator1_c, &locator2_c, name_c, status );

/* Export returned locator - will be now DAT__NOLOC */
   datExportFloc( &locator1_c, 1, locator1_length, locator1, status );
}

F77_SUBROUTINE(dat_msg)( CHARACTER(token), CHARACTER(locator)
			 TRAIL(token) TRAIL(locator) )
{
  /*====================================================*/
  /* DAT_MSG - Associate a locator with a message token */
  /*====================================================*/

  char token_c[EMS__SZTOK+1];
  HDSLoc locator_c;
  int status = SAI__OK;

  /* Import the locator */
  dat1_import_floc( locator, locator_length, &locator_c, &status );
  if (status != SAI__OK) {
    emsAnnul(&status);
    return;
  }

  /* Import "token" to C string  */
  cnfImpn( token, token_length, EMS__SZTOK,  token_c );

  /* Call datMsg itself but only if status is good */
  datMsg( token_c, &locator_c );

}

F77_SUBROUTINE(dat_name)( CHARACTER(locator),
                          CHARACTER(name),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(name) )
{

/*================================*/
/* DAT_NAME - Enquire object name */
/*================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1]; 
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datName( &locator_c, name_c, status );

/* Export returned name to FORTRAN string                 */
   cnfExpn( name_c, DAT__SZNAM, name, name_length );
}

F77_SUBROUTINE(dat_ncomp)( CHARACTER(locator),
                           F77_INTEGER_TYPE *ncomp,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{

/*==========================================*/
/* DAT_NCOMP - Inquire number of components */
/*==========================================*/

/* Local variables */
   HDSLoc locator_c;

/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datNcomp( &locator_c, ncomp, status );
}

F77_SUBROUTINE(dat_new)( CHARACTER(locator),
                         CHARACTER(name),
                         CHARACTER(type),
                         F77_INTEGER_TYPE *ndim,
                         FORTRAN_INDEX_TYPE dims[],
                         F77_INTEGER_TYPE *status
                         TRAIL(locator)
                         TRAIL(name)
                         TRAIL(type) )
{

/*================================*/
/* DAT_NEW - Create new component */
/*================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
   char type_c[DAT__SZTYP+1];
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import FORTRAN to C strings  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c);
   cnfImpn( type, type_length, DAT__SZTYP,  type_c);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datNew( &locator_c, name_c, type_c, *ndim, dims64, status);
#else
   datNew( &locator_c, name_c, type_c, *ndim, dims, status);
#endif
}

F77_SUBROUTINE(dat_newc)( CHARACTER(locator),
                          CHARACTER(name),
                          F77_INTEGER_TYPE *len,
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(name) )
{

/*============================================*/
/* DAT_NEWC - Create new _CHAR type component */
/*============================================*/

/* Local variables */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datNewC( &locator_c, name_c, *len, *ndim, dims64, status );
#else
   datNewC( &locator_c, name_c, *len, *ndim, dims, status );
#endif
}

F77_SUBROUTINE(dat_new0)( CHARACTER(locator),
                          CHARACTER(name),
			  CHARACTER(type),
                          INTEGER(status)
                          TRAIL(locator)
                          TRAIL(name) 
			  TRAIL(type) )
{

/*========================================*/
/* DAT_NEW0 - Create new scalar component */
/*========================================*/

/* Local variables */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
   char type_c[DAT__SZTYP+1];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" and "type" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );
   cnfImpn( type, type_length, DAT__SZNAM,  type_c );

   datNew0( &locator_c, name_c, type_c, status );

}

F77_SUBROUTINE(dat_new0d)( CHARACTER(locator),
                          CHARACTER(name),
                          INTEGER(status)
                          TRAIL(locator)
			   TRAIL(name) )
{

/*========================================*/
/* DAT_NEW0D - Create new double scalar component */
/*========================================*/

/* Local variables */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" and "type" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

   datNew0D( &locator_c, name_c, status );

}

F77_SUBROUTINE(dat_new0i)( CHARACTER(locator),
                          CHARACTER(name),
                          INTEGER(status)
                          TRAIL(locator)
			   TRAIL(name) )
{

/*========================================*/
/* DAT_NEW0I - Create new integer scalar component */
/*========================================*/

/* Local variables */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" and "type" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

   datNew0I( &locator_c, name_c, status );

}

F77_SUBROUTINE(dat_new0r)( CHARACTER(locator),
                          CHARACTER(name),
                          INTEGER(status)
                          TRAIL(locator)
			  TRAIL(name) )
{

/*========================================*/
/* DAT_NEW0R - Create new real scalar component */
/*========================================*/

/* Local variables */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" and "type" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

   datNew0R( &locator_c, name_c, status );

}

F77_SUBROUTINE(dat_new0l)( CHARACTER(locator),
                          CHARACTER(name),
                          INTEGER(status)
                          TRAIL(locator)
			  TRAIL(name) )
{

/*========================================*/
/* DAT_NEW0L - Create new logical scalar component */
/*========================================*/

/* Local variables */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" and "type" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

   datNew0L( &locator_c, name_c, status );

}

F77_SUBROUTINE(dat_new0c)( CHARACTER(locator),
                          CHARACTER(name),
			  INTEGER(len),
                          INTEGER(status)
                          TRAIL(locator)
			  TRAIL(name) )
{

/*========================================*/
/* DAT_NEW0C - Create new _CHAR scalar component */
/*========================================*/

/* Local variables */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
   size_t len_c;

/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" and "type" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

   len_c = *len;
   datNew0C( &locator_c, name_c, len_c, status );

}

F77_SUBROUTINE(dat_paren)( CHARACTER(locator1),
                           CHARACTER(locator2),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator1)
                           TRAIL(locator2) )
{

/*=====================================*/
/* DAT_PAREN - Locate parent structure */
/*=====================================*/

/* Local variables.     */
   HDSLoc locator1_c;
   HDSLoc *locator2_c = NULL;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status );

/* Call pure C routine                                       */
   datParen( &locator1_c, &locator2_c, status );

/* Export returned locator to FORTRAN string                 */
   datExportFloc( &locator2_c, 1, locator2_length, locator2, status );
}

F77_SUBROUTINE(dat_prec)( CHARACTER(locator),
			  INTEGER(nbytes),
			  INTEGER(status)
			  TRAIL(locator) )

{
/*==========================================*/
/* DAT_PREC - Enquire object primitive size */
/*==========================================*/

#undef context_name
#undef context_message
#define context_name "DAT_PREC_ERR"
#define context_message\
        "DAT_PREC: Error enquiring the object primitive size."

/* Local variables.     */
   HDSLoc locator_c;
   size_t nbytes_c;
   
/* Enter routine.	*/
   if ( !_ok(*status) ) return;

/* Import the input locator string                  */
   _callv(dat1_import_floc( locator, locator_length, &locator_c, status ));

/* Call pure C routine */
   nbytes_c = (size_t)*nbytes;
   datPrec( &locator_c, &nbytes_c, status );
   *nbytes = nbytes_c;

}

F77_SUBROUTINE(dat_prim)( CHARACTER(locator),
                          F77_LOGICAL_TYPE *reply,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*=====================================*/
/* DAT_PRIM - Enquire object primitive */
/*=====================================*/

/* Local variables.     */
   HDSLoc locator_c;
   int reply_c;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datPrim( &locator_c, &reply_c, status );

/* Set FORTRAN locical return                                */
   if( reply_c )
      *reply = F77_TRUE;
   else
      *reply = F77_FALSE;   
}

F77_SUBROUTINE(dat_prmry)( F77_LOGICAL_TYPE *set,
                           CHARACTER(locator),
                           F77_LOGICAL_TYPE *prmry,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{

/*==========================================================*/
/* DAT_PRMRY - Set/Enquire primary/secondary locator status */
/*==========================================================*/

/* Local variables.     */
   HDSLoc *locator_c = NULL;
   int set_c;
   int primary_c;
   
/* Enter routine.	*/

   if( *set == F77_TRUE ) {
      set_c = TRUE;
      if( *prmry == F77_TRUE )
         primary_c = TRUE;
      else
         primary_c = FALSE;
   } else {
      set_c = FALSE;
      primary_c = FALSE;
   }

/* Import the locator string (given and returned)  */
   datImportFloc( locator, locator_length, &locator_c, status);

/* Call pure C routine                                       */
   datPrmry( set_c, &locator_c, &primary_c, status );

/* Export returned locator - may be set to DAT__NOLOC */
   datExportFloc( &locator_c, 1, locator_length, locator, status );

/* Set FORTRAN logical return                                */
   if( *set == F77_FALSE )
   {
      if (primary_c == TRUE )
         *prmry = F77_TRUE;
      else
         *prmry = F77_FALSE;
   }   
}

F77_SUBROUTINE(dat_putc)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          CHARACTER(values),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(values) )
{

/*==================================*/
/* DAT_PUTC - Write _CHAR primitive */
/*==================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];
      
/* Call C routine                                       */
   datPutC( &locator_c, *ndim, dims64, values, values_length, status);
#else
   datPutC( &locator_c, *ndim, dims, values, values_length, status);
#endif

}

F77_SUBROUTINE(dat_putd)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_DOUBLE_TYPE *values,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*=====================================*/
/* DAT_PUTD - Write _DOUBLE primitives */
/*=====================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];
      
/* Call pure C routine                                       */
   datPutD( &locator_c, *ndim, dims64, values, status );
#else
   datPutD( &locator_c, *ndim, dims, values, status );
#endif
}

F77_SUBROUTINE(dat_puti)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_INTEGER_TYPE *values,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*======================================*/
/* DAT_PUTI - Write _INTEGER primitives */
/*======================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];
      
/* Call pure C routine                                       */
   datPutI( &locator_c, *ndim, dims64, values, status );
#else
   datPutI( &locator_c, *ndim, dims, values, status );
#endif
}

F77_SUBROUTINE(dat_putr)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_REAL_TYPE *values,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*===================================*/
/* DAT_PUTR - Write _REAL primitives */
/*===================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];
      
/* Call pure C routine                                       */
   datPutR( &locator_c, *ndim, dims64, values, status );
#else
   datPutR( &locator_c, *ndim, dims, values, status );
#endif
}

F77_SUBROUTINE(dat_putl)( CHARACTER(locator),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          F77_LOGICAL_TYPE *values,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*======================================*/
/* DAT_PUTL - Write _LOGICAL primitives */
/*======================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];
      
/* Call pure C routine                                       */
   datPutL( &locator_c, *ndim, dims64, values, status );
#else
   datPutL( &locator_c, *ndim, dims, values, status );
#endif
}

F77_SUBROUTINE(dat_put)( CHARACTER(locator),
                         CHARACTER(type),
                         F77_INTEGER_TYPE *ndim,
                         FORTRAN_INDEX_TYPE dims[],
                         F77_BYTE_TYPE values[],
                         F77_INTEGER_TYPE *status
                         TRAIL(locator)
                         TRAIL(type)
			 TRAIL(values)
			 )
{

/*===========================*/
/* DAT_PUT - Write primitive */
/*===========================*/

/* Local variables.     */
   HDSLoc locator_c;
   char type_c[DAT__SZTYP+1];
   int ischar = 0;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "type" string  */
   cnfImpn( type, type_length, DAT__SZTYP,  type_c );

   /* Special case _CHAR since fortran is telling us the length */
   if (strncmp(type,"_CHAR",5) == 0) ischar = 1;

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];
      
/* Call pure C routine                                       */
   if (ischar) {
     datPutC( &locator_c, *ndim, dims64, (char*)values, values_length, status );
   } else {
     datPut( &locator_c, type_c, *ndim, dims64, values, status );
   }
#else
   if (ischar) {
     datPutC( &locator_c, *ndim, dims, (char *)values, values_length, status );
   } else {
     datPut( &locator_c, type_c, *ndim, dims, values, status );
   }
#endif
}

F77_SUBROUTINE(dat_put0c)( CHARACTER(locator),
			   CHARACTER(value),
			   INTEGER(status)
			   TRAIL(locator) 
			   TRAIL(value) )
{
  HDSLoc locator_c;
  char * value_c;
  
  value_c = cnfCreim( value, value_length );
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datPut0C( &locator_c, value_c, status );
  free(value_c);
}


F77_SUBROUTINE(dat_put0d)( CHARACTER(locator),
			   DOUBLE(value),
			   INTEGER(status)
			   TRAIL(locator) )
{
  HDSLoc locator_c;
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datPut0D( &locator_c, *value, status );
}

F77_SUBROUTINE(dat_put0r)( CHARACTER(locator),
			   REAL(value),
			   INTEGER(status)
			   TRAIL(locator) )
{
  HDSLoc locator_c;
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datPut0R( &locator_c, *value, status );
}

F77_SUBROUTINE(dat_put0i)( CHARACTER(locator),
			   INTEGER(value),
			   INTEGER(status)
			   TRAIL(locator) )
{
  HDSLoc locator_c;
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datPut0I( &locator_c, *value, status );
}

F77_SUBROUTINE(dat_put0l)( CHARACTER(locator),
			   LOGICAL(value),
			   INTEGER(status)
			   TRAIL(locator) )
{
  HDSLoc locator_c;
  dat1_import_floc( locator, locator_length, &locator_c, status );
  datPut0L( &locator_c, *value, status );
}


F77_SUBROUTINE(dat_refct)( CHARACTER(locator),
                           F77_INTEGER_TYPE *refct,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{

/*====================================================*/
/* DAT_REFCT - Enquire container file reference count */
/*====================================================*/

/* Local variables */
   HDSLoc locator_c;

/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datRefct( &locator_c, refct, status );
}

F77_SUBROUTINE(dat_renam)( CHARACTER(locator),
                           CHARACTER(name),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator)
                           TRAIL(name) )
{

/*==============================*/
/* DAT_RENAM - Rename an object */
/*==============================*/

/* Local variables.     */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1]; 
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

/* Call pure C routine                                       */
   datRenam( &locator_c, name_c, status );
}

F77_SUBROUTINE(dat_reset)( CHARACTER(locator),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{

/*================================*/
/* DAT_RESET - Reset object state */
/*================================*/

/* Local variables */
   HDSLoc locator_c;

/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datReset( &locator_c, status );
}

F77_SUBROUTINE(dat_retyp)( CHARACTER(locator),
                           CHARACTER(type),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator)
                           TRAIL(type) )
{

/*================================*/
/* DAT_RETYP - Change object type */
/*================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char type_c[DAT__SZTYP+1]; 
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "type" to C string  */
   cnfImpn( type, type_length, DAT__SZTYP,  type_c );

/* Call pure C routine                                       */
   datRetyp( &locator_c, type_c, status);
}

F77_SUBROUTINE(dat_shape)( CHARACTER(locator),
                           F77_INTEGER_TYPE *ndimx,
                           FORTRAN_INDEX_TYPE dims[],
                           F77_INTEGER_TYPE *ndim,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{
/*==================================*/
/* DAT_SHAPE - Enquire object shape */
/*==================================*/

/* Local variables.     */
   HDSLoc locator_c;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Call pure C routine                                       */
   datShape( &locator_c, *ndimx, dims64, ndim, status );

   /* Copy the array back to fortran from HDS_PTYPE */
   for( i = 0; i<_min(*ndimx,*ndim); i++)
      dims[i] = dims64[i];

#else
   datShape( &locator_c, *ndimx, dims, ndim, status );
#endif
}

F77_SUBROUTINE(dat_size)( CHARACTER(locator),
                          F77_INTEGER_TYPE *size,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator) )
{

/*================================*/
/* DAT_SIZE - Enquire object size */
/*================================*/

/* Local variables */
   HDSLoc locator_c;
   size_t size_c;

/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datSize( &locator_c, &size_c, status );
   *size = (F77_INTEGER_TYPE) size_c;
}

F77_SUBROUTINE(dat_slice)( CHARACTER(locator1),
                           F77_INTEGER_TYPE *ndim,
                           FORTRAN_INDEX_TYPE diml[],
                           FORTRAN_INDEX_TYPE dimu[],
                           CHARACTER(locator2),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator1)
                           TRAIL(locator2) )
{

/*=================================*/
/* DAT_SLICE - Locate object slice */
/*=================================*/

/* Local variables.     */
   HDSLoc locator1_c;
   HDSLoc *locator2_c = NULL;
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG diml64[DAT__MXDIM];
   INT_BIG dimu64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ ) {
      diml64[i] = diml[i];
      dimu64[i] = dimu[i];
   }

/* Call pure C routine                                       */
   datSlice( &locator1_c, *ndim, diml64, dimu64, &locator2_c, status );
#else
   datSlice( &locator1_c, *ndim, diml, dimu, &locator2_c, status );
#endif

/* Export returned locator */
   datExportFloc( &locator2_c, 1, locator2_length, locator2, status );
}

F77_SUBROUTINE(dat_state)( CHARACTER(locator),
                           F77_LOGICAL_TYPE *reply,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{

/*==================================*/
/* DAT_STATE - Enquire object state */
/*==================================*/

/* Local variables.     */
   HDSLoc locator_c;
   int reply_c;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datState( &locator_c, &reply_c, status );

/* Set FORTRAN locical return                                */
   if( reply_c )
      *reply = F77_TRUE;
   else
      *reply = F77_FALSE;   
}

F77_SUBROUTINE(dat_struc)( CHARACTER(locator),
                           F77_LOGICAL_TYPE *reply,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{

/*======================================*/
/* DAT_STRUC - Enquire object structure */
/*======================================*/

/* Local variables.     */
   HDSLoc locator_c;
   int reply_c = 0;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datStruc( &locator_c, &reply_c, status );

/* Set FORTRAN locical return                                */
   if( reply_c )
      *reply = F77_TRUE;
   else
      *reply = F77_FALSE;   
}

F77_SUBROUTINE(dat_temp)( CHARACTER(type),
                          F77_INTEGER_TYPE *ndim,
                          FORTRAN_INDEX_TYPE dims[],
                          CHARACTER(locator),
                          F77_INTEGER_TYPE *status
                          TRAIL(type)
                          TRAIL(locator) )
{

/*====================================*/
/* DAT_TEMP - Create temporary object */
/*====================================*/

/* Local variables.     */
   HDSLoc *locator_c = NULL;
   char type_c[DAT__SZTYP+1];
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import "type" to C string  */
   cnfImpn( type, type_length, DAT__SZTYP,  type_c );

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];

/* Call pure C routine                                       */
   datTemp( type_c, *ndim, dims64, &locator_c, status);
#else
   datTemp( type_c, *ndim, dims, &locator_c, status);
#endif

/* Export returned locator */
   datExportFloc( &locator_c, 1, locator_length, locator, status );
}

F77_SUBROUTINE(dat_there)( CHARACTER(locator),
                           CHARACTER(name),
                           F77_LOGICAL_TYPE *reply,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator)
                           TRAIL(name) )
{

/*=========================================*/
/* DAT_THERE - Enquire component existence */
/*=========================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char name_c[DAT__SZNAM+1];
   int reply_c = FALSE;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "name" string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c);

/* Call pure C routine                                       */
   datThere( &locator_c, name_c, &reply_c, status );

/* Set FORTRAN locical return                                */
   if( reply_c )
      *reply = F77_TRUE;
   else
      *reply = F77_FALSE;   
}

F77_SUBROUTINE(dat_type)( CHARACTER(locator),
                          CHARACTER(type),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(type) )
{

/*================================*/
/* DAT_TYPE - Enquire object type */
/*================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char type_c[DAT__SZTYP+1]; 
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datType( &locator_c, type_c, status );

/* Export returned type name to FORTRAN                 */
   cnfExpn( type_c, DAT__SZNAM, type, type_length );
}

F77_SUBROUTINE(dat_unmap)( CHARACTER(locator),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{
/*==========================*/
/* DAT_UNMAP - Unmap object */
/*==========================*/

/* Local variables.     */
   HDSLoc locator_c;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   datUnmap( &locator_c, status );
}

F77_SUBROUTINE(dat_valid)( CHARACTER(locator),
                           F77_LOGICAL_TYPE *reply,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{

/*===================================*/
/* DAT_VALID - Enquire locator valid */
/*===================================*/

/* Local variables.     */
   HDSLoc locator_c;
   int reply_c = FALSE;
   
/* Enter routine.	*/

/* Any errors triggered by this routine should be annulled
   and simply set "reply" to false intead */

   *reply = F77_FALSE;

   if ( *status != SAI__OK ) return;

   emsMark();

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

/* Call pure C routine                                       */
   datValid( &locator_c, &reply_c, status );

/* clear any bad status and set valid to FALSE */
   if ( *status != SAI__OK ) {
     reply_c = FALSE;
     emsAnnul( status );
   }

   emsRlse();

/* Set FORTRAN logical return                                */
   if( reply_c )
      *reply = F77_TRUE;
   else
      *reply = F77_FALSE;   
}

F77_SUBROUTINE(dat_vec)( CHARACTER(locator1),
                         CHARACTER(locator2),
                         F77_INTEGER_TYPE *status
                         TRAIL(locator1)
                         TRAIL(locator2) )
{

/*============================*/
/* DAT_VEC - Vectorise object */
/*============================*/

/* Local variables.     */
   HDSLoc locator1_c;
   HDSLoc *locator2_c = NULL;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status );

/* Call pure C routine                                       */
   datVec( &locator1_c, &locator2_c, status );

/* Export returned locator */
   datExportFloc( &locator2_c, 1, locator2_length, locator2, status );
}

F77_SUBROUTINE(dat_where)( CHARACTER(locator),
                           F77_INTEGER_TYPE *block,
                           F77_INTEGER_TYPE *offset,
                           F77_INTEGER_TYPE *status
                           TRAIL(locator) )
{

/*===============================================*/
/* DAT_WHERE Find primitive position in HDS file */
/*===============================================*/

/* Local variables.     */
   HDSLoc locator_c;
   INT_BIG bloc;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status);

/* Call pure C routine                                       */
   datWhere( &locator_c, &bloc, offset, status );

   *block = bloc;
}

F77_SUBROUTINE(hds_copy)( CHARACTER(locator),
                          CHARACTER(file),
                          CHARACTER(name),
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(file)
                          TRAIL(name) )
{

/*===================================================*/
/* HDS_COPY - Copy an object to a new container file */
/*===================================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char *file_c;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import FILE argument to C string */
   file_c = cnfCreim( file, file_length );

/* Import "name" to C string  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c );

/* Call pure C routine                                       */
   hdsCopy( &locator_c, file_c, name_c, status );

/* Free allocated string memory.                             */
   free( file_c );
}

F77_SUBROUTINE(hds_erase)( CHARACTER(locator),
                           F77_INTEGER_TYPE *status
	                   TRAIL(locator) )
{
/*==================================*/
/* HDS_ERASE - Erase container file */
/*==================================*/

/* Local variables.     */
   HDSLoc *locator_c = NULL;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   datImportFloc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   hdsErase( &locator_c, status );

/* Export the locator (will be DAT__NOLOC) */
   datExportFloc( &locator_c, 1, locator_length, locator, status );

}

F77_SUBROUTINE(hds_ewild) ( F77_INTEGER_TYPE *iwld,
                            F77_INTEGER_TYPE *status )
{
/*================================================================*/
/* HDS_EWILD - End a wild card search for HDS container files     */
/*================================================================*/

/* Enter routine.       */

/* Call pure C routine               */
   hdsEwild( iwld, status );
}

F77_SUBROUTINE(hds_flush)( CHARACTER(group),
                           F77_INTEGER_TYPE *status
                           TRAIL(group) )
{

/*=================================*/
/* HDS_FLUSH - Flush locator group */
/*==================================*/

/* Local variables.     */
   char group_c[DAT__SZGRP+1];
   
/* Enter routine.	*/

/* Import the maximum length strings  */

   cnfImpn( group, group_length,  DAT__SZGRP, group_c );

/* Call pure C routine                                       */
   hdsFlush( group_c, status );
}

F77_SUBROUTINE(hds_free)( CHARACTER(locator),
                          F77_INTEGER_TYPE *status
	                  TRAIL(locator) )
{

/*================================*/
/* HDS_FREE - Free container file */
/*================================*/

/* Local variables.     */
   HDSLoc locator_c;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   hdsFree( &locator_c, status );
}

F77_SUBROUTINE(hds_group)( CHARACTER(locator),
                           CHARACTER(group),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator)
                           TRAIL(group) )
{

/*===================================*/
/* HDS_GROUP - Enquire locator group */
/*===================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char group_c  [DAT__SZGRP+1];;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   hdsGroup( &locator_c, group_c, status );

/* Export returned group name to FORTRAN string                 */
   cnfExpn( group_c, DAT__SZGRP, group, group_length);
}

F77_SUBROUTINE(hds_gtune) ( CHARACTER(param_str),
                            F77_INTEGER_TYPE *value,
                            F77_INTEGER_TYPE *status
                            TRAIL(param_str) )
{
/*=========================================*/
/* HDS_GTUNE - Get HDS tuning parameter     */
/*=========================================*/

/* Local variables.     */
   char *param_str_c;
 
/* Enter routine.       */

/* Import PARAM argument to C string */

   param_str_c = cnfCreim( param_str, param_str_length );
   
/* Call pure C routine */
   hdsGtune( param_str_c, value, status );

/* Free allocated string memory.                             */
   free( param_str_c );
}

F77_SUBROUTINE(hds_link)( CHARACTER(locator),
                          CHARACTER(group),
                          F77_INTEGER_TYPE *status
	                  TRAIL(locator) 
                          TRAIL(group) )
{

/*==================================*/
/* HDS_LINK - Link locator to group */
/*==================================*/

/* Local variables.     */
   HDSLoc locator_c;
   char group_c[DAT__SZGRP+1];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import the group name string                  */
   cnfImpn( group, group_length, DAT__SZGRP, group_c );

/* Call pure C routine                                       */
   hdsLink( &locator_c, group_c, status );
}

F77_SUBROUTINE(hds_lock)( CHARACTER(locator),
                          F77_INTEGER_TYPE *status
	                  TRAIL(locator) )
{

/*================================*/
/* HDS_LOCK - Lock container file */
/*================================*/

/* Local variables.     */
   HDSLoc locator_c;
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   hdsLock( &locator_c, status );
}

F77_SUBROUTINE(hds_new)( CHARACTER(file),
                         CHARACTER(name),
                         CHARACTER(type),
                         F77_INTEGER_TYPE *ndim,
                         FORTRAN_INDEX_TYPE dims[],
                         CHARACTER(locator),
                         F77_INTEGER_TYPE *status
                         TRAIL(file)
                         TRAIL(name)
                         TRAIL(type)
	                 TRAIL(locator) )
{

/*=====================================*/
/* HDS_NEW - Create new container file */
/*=====================================*/

/* Local variables.     */
   HDSLoc * locator_c = NULL;
   char *file_c;
   char name_c[DAT__SZNAM + 1];
   char type_c[DAT__SZTYP + 1];
#if HDS_COPY_FORTRAN_DIMS
   INT_BIG dims64[DAT__MXDIM];
   int i;
#endif
   
/* Enter routine.	*/

/* Import FILE argument to C string */
   file_c = cnfCreim( file, file_length );

/* Import FORTRAN to C  strings  */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c);
   cnfImpn( type, type_length, DAT__SZTYP,  type_c);

#if HDS_COPY_FORTRAN_DIMS
/* Ensure that array subscripts are 64-bits         */
   for( i = 0; i<DAT__MXDIM; i++ )
      dims64[i] = dims[i];
      
/* Call pure C routine                                       */
   hdsNew( file_c, name_c, type_c, *ndim, dims64, &locator_c, status );
#else
   hdsNew( file_c, name_c, type_c, *ndim, dims, &locator_c, status );
#endif

/* Export returned locator */
   datExportFloc( &locator_c, 1, locator_length, locator, status );
   
/* Free allocated string memory.                             */
   free( file_c );
}

F77_SUBROUTINE(hds_open)( CHARACTER(file),
                          CHARACTER(mode),
                          CHARACTER(locator),
                          F77_INTEGER_TYPE *status
                          TRAIL(file)
                          TRAIL(mode)
	                  TRAIL(locator) )
{

/*=========================================*/
/* HDS_OPEN - Open existing container file */
/*=========================================*/

/* Local variables.     */
   HDSLoc * locator_c = NULL;
   char *file_c;
   char mode_c[DAT__SZMOD + 1];
   
/* Enter routine.	*/

/* Import FILE argument to C string */
   file_c = cnfCreim( file, file_length );

/* Import maxmimum length strings  */
   cnfImpn( mode, mode_length, DAT__SZMOD, mode_c );

/* Call pure C routine                                       */
   hdsOpen( file_c, mode_c, &locator_c, status );

/* Export returned locator */
   datExportFloc( &locator_c, 1, locator_length, locator, status );
   
/* Free allocated string memory.                             */
   free( file_c );
}

F77_SUBROUTINE(hds_show)( CHARACTER(topic),
                          F77_INTEGER_TYPE *status
                          TRAIL(topic) )
{

/*================================*/
/* HDS_SHOW - Show HDS statistics */
/*================================*/

/* Local variables.     */
   char *topic_c;
   
/* Enter routine.	*/

/* Import TOPIC argument to C string */
   topic_c = cnfCreim( topic, topic_length );

/* Call pure C routine                                       */
   hdsShow( topic_c, status);

/* Free allocated string memory.                             */
   free( topic_c );
}

F77_SUBROUTINE(hds_state) (int *state,
                           int *status )
{
/*================================================*/
/* HDS_STATE - Enquire the current state of HDS   */
/*================================================*/

/* Call pure C routine                                                     */
   hdsState( state, status );
}

F77_SUBROUTINE(hds_stop) ( F77_INTEGER_TYPE *status )
{
/*=============================*/
/* HDS_STOP - Close down HDS   */
/*=============================*/

/* Call pure C routine */
   hdsStop( status );
}

F77_SUBROUTINE(hds_trace)( CHARACTER(locator),
			   F77_INTEGER_TYPE *nlev,
                           CHARACTER(path),
                           CHARACTER(file),
                           F77_INTEGER_TYPE *status
                           TRAIL(locator)
                           TRAIL(path)
                           TRAIL(file) )
{

/*===============================*/
/* HDS_TRACE - Trace object path */
/*===============================*/

/* Local variables.     */
   HDSLoc locator_c;
   char path_c[STR_K_LENGTH];
   char file_c[STR_K_LENGTH];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   hdsTrace( &locator_c, nlev, path_c, file_c, status,
            STR_K_LENGTH, STR_K_LENGTH );

/* Export the returned C strings to the FORTRAN variables    */
   cnfExprt( path_c, path, path_length );
   cnfExprt( file_c, file, file_length );
}

F77_SUBROUTINE(hds_tune) ( CHARACTER(param_str),
                           F77_INTEGER_TYPE *value,
                           F77_INTEGER_TYPE *status
                           TRAIL(param_str) )
{
/*=========================================*/
/* HDS_TUNE - Set HDS tuning parameter     */
/*=========================================*/

/* Local variables.     */
   char *param_str_c;
 
/* Enter routine.       */

/* Import PARAM argument to C string */

   param_str_c = cnfCreim( param_str, param_str_length );
   
/* Call pure C routine  */
   hdsTune( param_str_c, value, status );
   
/* Free allocated string memory.                             */
   free( param_str_c );
}

F77_SUBROUTINE(hds_wild) ( CHARACTER(fspec),
                           CHARACTER(mode),
                           F77_INTEGER_TYPE *iwld,
                           CHARACTER(locator),
                           F77_INTEGER_TYPE *status
                           TRAIL(fspec)
                           TRAIL(mode)
                           TRAIL(locator) )
{
/*=================================================================*/
/* HDS_WILD - Perform a wild-card search for HDS container files   */
/*=================================================================*/

/* Local variables.     */
   HDSLoc * locator_c = NULL;
   char *fspec_c;
   char *mode_c;

/* Enter routine.       */

/* Import FSPEC argument to C string */
   fspec_c = cnfCreim( fspec, fspec_length );

/* Import FSPEC argument to C string */
   mode_c = cnfCreim( mode, mode_length );

/* Call pure C routine  */
   hdsWild( fspec_c, mode_c, iwld, &locator_c, status );

/* Export returned locator */
   datExportFloc( &locator_c, 1, locator_length, locator, status );
      
/* Free allocated string memory.                             */
   free( fspec_c );
   free( mode_c );
}

/*=================================================================*/
/*  Deprecated routines!                                           */   
/*=================================================================*/

F77_SUBROUTINE(dat_conv)( CHARACTER(locator),
                          CHARACTER(type),
                          F77_LOGICAL_TYPE *reply,
                          F77_INTEGER_TYPE *status
                          TRAIL(locator)
                          TRAIL(type))
{

/*=========================================*/
/* DAT_CONV - Enquire conversion possible? */
/*=========================================*/

/* Local variables.     */
   HDSLoc locator_c;
   int reply_c;
   char type_c[DAT__SZTYP+1];
   
/* Enter routine.	*/

/* Import the input locator string                  */
   dat1_import_floc( locator, locator_length, &locator_c, status );

/* Import "type" to C string  */
   cnfImpn( type, type_length, DAT__SZTYP,  type_c);

/* Call pure C routine                                       */
   datConv( &locator_c, type_c, &reply_c, status );

/* Set FORTRAN logical return                                */
   if( reply_c )
      *reply = F77_TRUE;
   else
      *reply = F77_FALSE;   
}

F77_SUBROUTINE(hds_close)( CHARACTER(locator),
                           F77_INTEGER_TYPE *status
	                   TRAIL(locator) )
{

/*======================================================*/
/* HDS_CLOSE - Close container file (Obselete routine!) */
/*======================================================*/

/* Local variables.     */
   HDSLoc *locator_c = NULL;

/* Enter routine.	*/

/* Import the input locator string                  */
   datImportFloc( locator, locator_length, &locator_c, status );

/* Call pure C routine                                       */
   hdsClose( &locator_c, status );

/* Export nulled locator */
   datExportFloc( &locator_c, 1, locator_length, locator, status );
}

/*==============================================*/
/* Obsolete routines that have no C counterpart */
/*==============================================*/

F77_SUBROUTINE(dat_rcera)( CHARACTER(locator), CHARACTER(cname), INTEGER(status)
                           TRAIL(locator) TRAIL(cname) )
{
/*=============================================*/
/* DAT_RCERA - recursive erase (use DAT_ERASE) */
/*=============================================*/ 
   HDSLoc locator_c;
   char cname_c[DAT__SZNAM+1];

   printf("DAT_RCERA() is deprecated. Please use DAT_ERASE instead\n");
   cnfImpn( cname, cname_length, DAT__SZNAM,  cname_c);
   dat1_import_floc( locator, locator_length, &locator_c, status );
   datErase( &locator_c, cname_c, status );
   return;
}


F77_SUBROUTINE(dat_tune) ( CHARACTER(name),
                           INTEGER(value),
                           INTEGER(status)
                           TRAIL(name) )
{
/*=========================================*/
/* DAT_TUNE - Set HDS tuning parameter     */
/*        Obsoleted by HDS_TUNE            */
/*=========================================*/

/* Local variables.     */
   char *name_c;
 
/* Enter routine.       */

   if (*status != SAI__OK ) return;

   printf("DAT_TUNE is obsoleted by HDS_TUNE\n");

/* Import NAME argument to C string */

   name_c = cnfCreim( name, name_length );
   
/* Call pure C routine  */
   if (strncmp(name_c, "NCOMP", 5) == 0 ) {
      hdsTune( name_c, value, status );
   } else {
      emsSetc( "NM", name_c );
      emsRep("DAT_TUNE_ERR", "Unrecognized tuning parameter (^NM)",
           status );
   }
   
/* Free allocated string memory.                             */
   cnfFree( name_c );
}

F77_SUBROUTINE(dat_rcopy)( CHARACTER(locator1),
                          CHARACTER(locator2),
                          CHARACTER(name),
                          INTEGER(status)
	                  TRAIL(locator1)
                          TRAIL(locator2)
                          TRAIL(name) )
{

/*===================================*/
/* DAT_RCOPY - Recursive copy object */
/*     Obsoleted by DAT_COPY         */
/*===================================*/

/* Local variables.     */
   HDSLoc locator1_c;
   HDSLoc locator2_c;
   char name_c[DAT__SZNAM+1];
   
/* Enter routine.	*/
   printf("DAT_RCOPY() is deprecated. Please use DAT_ERASE instead\n");

/* Import name string */
   cnfImpn( name, name_length, DAT__SZNAM,  name_c);

/* Import the locator strings                  */
   dat1_import_floc( locator1, locator1_length, &locator1_c, status);
   dat1_import_floc( locator2, locator2_length, &locator2_c, status);

/* Call pure C routine                                       */
   datCopy( &locator1_c, &locator2_c, name_c, status);
}


F77_SUBROUTINE(dat_ertxt)(CHARACTER(text), INTEGER(status) TRAIL(text) )
{
/*==================================================*/
/* DAT_ERTXT - Report Error                         */
/*     Obsolete as HDS now reports errors using HDS */
/*==================================================*/

   /* Local Variables: */
   char *msg;   /* Pointer to static buffer */
   size_t lmsg;    /* Length of error message */
   char *text_c; /* Input text */

   text_c = cnfCreim( text, text_length );

   /* Get textual translation of the error code */
   datErmsg( *status, &lmsg, &msg );

   /*  Mark the EMS error stack to prevent interaction with any message
    *  tokens already defined and define a token for the text and the error
    *  message. */
   emsMark();
   emsSetc( "TEXT", text );
   emsSetc( "MSG", msg );

   /*  Report the message and release the error stack. */
   emsRep( "HDS_ERROR", "^TEXT: ^MSG", status );
   emsRlse();
   cnfFree( text_c );
}


F77_SUBROUTINE(dat_erdsc)( CHARACTER(locator), INTEGER(status) TRAIL(locator) )
{
/*==================================================*/
/* DAT_ERDSC - Report Object Error                  */
/*     Obsolete as HDS now reports errors using HDS */
/*==================================================*/

/*    Local variables : */
  HDSLoc locator_c;
  char file[EMS__SZMSG+1];  /* Container file name */
  char *msg = NULL;         /* Error message */
  char name[EMS__SZMSG+1];  /* Object pathname */
  int  lstat;               /* Local status */
  size_t lmsg;              /* Length of error message */
  int  nlev;                /* Number of object levels */

  /*  Mark the EMS error stack to prevent interaction with any message
      tokens already defined. */
  emsMark();

  /* Import locator using local status */
  lstat = SAI__OK;
  dat1_import_floc( locator, locator_length, &locator_c, &lstat );

  /* obtain the object name. */
  hdsTrace( &locator_c, &nlev, name, file, &lstat, sizeof(name), sizeof(file) );

  /*  If the name could not be obtained, then annul the error and
      substitute an appropriate message. */
  if (lstat != SAI__OK ) {
    strcpy( name, "<Unknown object>" );
    emsAnnul( &lstat );
  }

  /*  Obtain a textual translation of the error code. */
  datErmsg( *status, &lmsg, &msg );

  /*  Define tokens for the object name and the error message. */
  emsSetc( "NAME", name );
  emsSetnc( "MSG", msg, lmsg );

  /*  Report the message and release the error stack. */
  emsRep( "HDS_ERROR", "^NAME: ^MSG", status );
  emsRlse();

}


F77_SUBROUTINE(dat_erdsn)( CHARACTER(locator), CHARACTER(cmp),
			   INTEGER(status) TRAIL(locator) TRAIL(cmp) )
{
/*==================================================*/
/* DAT_ERDSN - Report component Error               */
/*     Obsolete as HDS now reports errors using HDS */
/*==================================================*/

/*    Local variables : */
  HDSLoc locator_c;
  char cmp_c[DAT__SZNAM+1];
  char file[EMS__SZMSG+1];  /* Container file name */
  char *msg = NULL;         /* Error message */
  char name[EMS__SZMSG+1];  /* Object pathname */
  int  lstat;               /* Local status */
  size_t lmsg;              /* Length of error message */
  int  nlev;                /* Number of object levels */

  /* Import the component name */
  cnfImpn( cmp, cmp_length, DAT__SZNAM,  cmp_c);

  /*  Mark the EMS error stack to prevent interaction with any message
      tokens already defined. */
  emsMark();

  /* Import locator using local status */
  lstat = SAI__OK;
  dat1_import_floc( locator, locator_length, &locator_c, &lstat );

  /* obtain the object name. */
  hdsTrace( &locator_c, &nlev, name, file, &lstat, sizeof(name), sizeof(file) );

  /*  If the name could not be obtained, then annul the error and
      substitute an appropriate message. */
  if (lstat != SAI__OK ) {
    strcpy( name, "<Unknown structure>" );
    emsAnnul( &lstat );
  }

  /*  Obtain a textual translation of the error code. */
  datErmsg( *status, &lmsg, &msg );

  /*  Define tokens for the object name, component name and the error message. */
  emsSetc( "NAME", name );
  emsSetnc( "MSG", msg, lmsg );
  emsSetc("CMP", cmp_c );

  /*  Report the message and release the error stack. */
  emsRep( "HDS_ERROR", "^NAME.^CMP: ^MSG", status );
  emsRlse();


}
