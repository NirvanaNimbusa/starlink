      SUBROUTINE NDF_SQMF( QMF, INDF, STATUS )
*+
*  Name:
*     NDF_SQMF

*  Purpose:
*     Set a new logical value for an NDF's quality masking flag.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL NDF_SQMF( QMF, INDF, STATUS )

*  Description:
*     The routine sets a new logical value for an NDF's quality masking
*     flag.  This flag determines whether the NDF's quality component
*     (if present) will be used to generate "bad" pixel values for
*     automatic insertion into the data and variance arrays when these
*     are accessed in READ or UPDATE mode. If this flag is set to
*     .TRUE., then masking will occur, so that an application need not
*     consider the quality information explicitly.  If the flag is set
*     to .FALSE., then automatic masking will not occur, so that the
*     application can process the quality component by accessing it
*     directly.

*  Arguments:
*     QMF = LOGICAL (Given)
*        The logical value to be set for the quality masking flag.
*     INDF = INTEGER (Given)
*        NDF identifier.
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Notes:
*     -  A quality masking flag is associated with each NDF identifier
*     and is initially set to .TRUE..  Its value changes to .FALSE.
*     whenever the quality component is accessed directly (e.g. using
*     NDF_MAP or NDF_MAPQL) and reverts to .TRUE. when access is
*     relinquished (e.g. using NDF_UNMAP). This default behaviour may
*     also be over-ridden by calling NDF_SQMF to set its value
*     explicitly. The routine NDF_QMF allows the current value to be
*     determined.
*     -  The value of the quality masking flag is not propagated to new
*     identifiers.

*  Algorithm:
*     -  Import the NDF identifier.
*     -  Set a new quality masking flag value in the ACB.

*  Copyright:
*     Copyright (C) 1990 Science & Engineering Research Council.
*     All Rights Reserved.

*  Licence:
*     This program is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either version 2 of
*     the License, or (at your option) any later version.
*
*     This program is distributed in the hope that it will be
*     useful,but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE. See the GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with this program; if not, write to the Free Software
*     Foundation, Inc., 51 Franklin Street,Fifth Floor, Boston, MA
*     02110-1301, USA

*  Authors:
*     RFWS: R.F. Warren-Smith (STARLINK)
*     {enter_new_authors_here}

*  History:
*     5-FEB-1990 (RFWS):
*        Original version.
*     {enter_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'DAT_PAR'          ! DAT_ public constants
      INCLUDE 'NDF_PAR'          ! NDF_ public constants
      INCLUDE 'NDF_CONST'        ! NDF_ private constants

*  Global Variables:
      INCLUDE 'NDF_ACB'          ! NDF_ Access Control Block
*        ACB_QMF( NDF__MXACB ) = LOGICAL (Write)
*           Quality masking flag.

*  Arguments Given:
      LOGICAL QMF
      INTEGER INDF

*  Status:
      INTEGER STATUS             ! Global status

*  Local Variables:
      INTEGER IACB               ! Index to the NDF entry in the ACB

*.

*  Check inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Import the NDF identifier.
      CALL NDF1_IMPID( INDF, IACB, STATUS )
      IF ( STATUS .EQ. SAI__OK ) THEN

*  Set a new quality masking flag value in the ACB.
         ACB_QMF( IACB ) = QMF
      END IF

*  If an error occurred, then report context information and call the
*  error tracing routine.
      IF ( STATUS .NE. SAI__OK ) THEN
         CALL ERR_REP( 'NDF_SQMF_ERR',
     :   'NDF_SQMF: Error setting a new logical value for an NDF''s ' //
     :   'quality masking flag.', STATUS )
         CALL NDF1_TRACE( 'NDF_SQMF', STATUS )
      END IF

      END
