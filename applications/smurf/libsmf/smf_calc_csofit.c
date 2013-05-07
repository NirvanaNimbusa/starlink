/*
*+
*  Name:
*     smf_calc_csofit

*  Purpose:
*     Calculate a tau for each time slice using CSO fit parameters

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Library routine

*  Invocation:
*     smf_calc_csofit ( const smfData * adata, AstKeyMap* extpars, double **tau,
*                       size_t *nframes, int *status );

*  Arguments:
*     adata = const smfData * (Given)
*        Data for which tau is calculated.
*     extpars = AstKeyMap * (Given)
*        Extinction parameters from the config file. Uses the
*        ext.csofit parameter to locate parameter file.
*     tau = double ** (Returned)
*        Pointer to array of double to receive the tau data.
*        Will be malloced by this routine and should be freed
*        by the caller.
*     nframes = size_t * (Returned)
*        Number of elements in "wvmtau" array.
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     Calculates the tau for each time slice using the CSO fit parameters that are
*     stored in the fit file.

*  Authors:
*     TIMJ: Tim Jenness (JAC, Hawaii)
*     {enter_new_authors_here}

*  Notes:
*     - Currently the CSOFIT routines use Unix epoch seconds but the RTS uses
*       MJD TAI. For efficiency we convert the first TAI time to epoch and then
*       offset from that. It would be easier if the fits were stored in TAI MJD.
*     - The full CSOFIT data file is opened and read every single time this routine
*       is called. This will be extremely inefficient.

*  History:
*     2013-04-04 (TIMJ):
*        Initial version
*     {enter_further_changes_here}

*  Copyright:
*     Copyright (C) 2013 Science and Technology Facilities Council.
*     All Rights Reserved.

*  Licence:
*     This program is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either version 3 of
*     the License, or (at your option) any later version.
*
*     This program is distributed in the hope that it will be
*     useful, but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE. See the GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public
*     License along with this program; if not, write to the Free
*     Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
*     MA 02110-1301, USA

*  Bugs:
*     {note_any_bugs_here}
*-
*/

#include "smf.h"
#include "csofit/csofit2.h"

#include "sae_par.h"
#include "mers.h"
#include "ast.h"

#include "star/one.h"

#include <time.h>


void smf_calc_csofit( const smfData * data, AstKeyMap* extpars, double **tau,
                      size_t *nframes, int *status ) {

  JCMTState *allState = NULL;       /* JCMTSTATE information */
  const char * csofit_entry = NULL; /* location of fit file from config file */
  char csofit_file[1024];           /* expanded path to fit file */
  csofit2_t * fits = NULL;          /* Struct containing fit parameters */
  csofit2_t * subset = NULL;        /* Relevant subset of fits */
  double endepoch = VAL__BADD;      /* Unix epoch at end of observation */
  double startmjd = VAL__BADD;      /* MJD TAI of start of observation */
  double startepoch = VAL__BADD;    /* Unix epoch at start of observation */
  double *taudata = NULL;           /* Expansion of CSO fit into time series */


  if (*status != SAI__OK) return;

  if (!tau) {
    *status = SAI__ERROR;
    errRep("", "Must supply a non-NULL pointer for tau argument"
           " (possible programming error)", status );
    return;
  }

  /* Ensure that we have a header */
  if (!data->hdr) {
    *status = SAI__ERROR;
    errRep( "", "smfData has no header so we can not attached CSO fit data",
            status );
    return;
  }

  /* Work out where the parameter file is */
  astMapGet0C( extpars, "CSOFIT", &csofit_entry );

  /* this string will probably include environment variables so we need to expand those. */
  one_wordexp_noglob( csofit_entry, csofit_file, sizeof(csofit_file), status );
  if (*status != SAI__OK) return;

  /* Before we allocate any resources we will first check to see if a fit is available
     for this time. This means we have to open the file. Ideally this should only happen once
     and the results be passed in to this routine. For now we are checking to see if it works. */

  fits = csofit2_open( csofit_file );
  if (!fits) {
    *status = SAI__ERROR;
    errRepf("", "Error opening CSO fit file '%s'", status, csofit_file );
    return;
  }

  /* How many time slices are there? */
  smf_get_dims( data, NULL, NULL, NULL, nframes, NULL, NULL, NULL, status );

  /* Now need to convert the start and end times of the RTS time stream to create a subset of the
     fits for further usage */
  allState = data->hdr->allState;
  startmjd = allState[0].rts_end;
  startepoch = smf_tai2unix( startmjd, status );
  endepoch = smf_tai2unix( allState[*nframes-1].rts_end, status );

  if (*status == SAI__OK) {

    subset = csofit2_subset( fits, startepoch, endepoch );
    if (!subset || subset->npolys == 0) {
      /* we did not have any fits for this period */
      if (*status == SAI__OK) {
        *status = SAI__ERROR;
        errRep("", "No CSO fit data available for this observation", status );
      }
    } else {
      size_t i;
      double curepoch;
      csofit2_poly_t *polys;

      polys = &(subset->polys[0]);

      /* Do we really need to do this at 200 Hz? */
      taudata = astCalloc( *nframes, sizeof(*taudata) );

      for (i = 0; i < *nframes; i++ ) {
        /* calculate the current time in unix epoch */
        curepoch = ( allState[i].rts_end - startmjd ) * SPD + startepoch;

        taudata[i] = csofit2_calc( subset, curepoch );
        /* printf("%zu %.*g %.*g\n", i, DBL_DIG, curepoch, DBL_DIG, taudata[i] ); */

        /* and convert this value to filter [this needs refactoring] */
        taudata[i] = smf_cso2filt_tau( data->hdr, taudata[i], extpars, status );

      }
    }

  }

  if (fits) free(fits);
  if (subset) free(subset);

  if (*status == SAI__OK) {
    *tau = taudata;
  } else {
    if (taudata) taudata = astFree(taudata);
  }

}
