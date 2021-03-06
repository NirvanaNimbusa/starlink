      SUBROUTINE CAP_GDPAR (STATUS)
*+
*  Name:
*     CAP_GDPAR
*  Purpose:
*     Display full details for all the parameters in the catalogue.
*  Language:
*     Fortran 77.
*  Invocation:
*     CALL CAP_GDPAR (STATUS)
*  Description:
*     Display full details for all the parameters in the catalogue.
*  Arguments:
*     STATUS  =  INTEGER (Given and Returned)
*        The global status.
*  Algorithm:
*     If the catalogue is open then
*       Display full details for the parameters.
*     else
*       Report warning: catalogue not open.
*     end if
*  Copyright:
*     Copyright (C) 1999 Central Laboratory of the Research Councils
*  Authors:
*     ACD: A C Davenhall (Leicester)
*  History:
*     27/9/94 (ACD): Original version.
*  Bugs:
*     None known
*-
*  Type Definitions:
      IMPLICIT NONE
*  Global Constants:
      INCLUDE 'SAE_PAR'           ! Standard Starlink constants.
      INCLUDE 'CAT_PAR'           ! CAT parametric constants.
      INCLUDE 'SGZ_PAR'           ! StarGaze parametric constants.
*  Global Variables:
      INCLUDE 'SGZ_CMN'           ! StarGaze common block.
*  Status:
      INTEGER STATUS             ! Global status
*.

      IF (STATUS .EQ. SAI__OK) THEN

*
*       Check if there is a catalogue open.

         IF (COPEN__SGZ) THEN

*
*          Display full details for the parameters.

            CALL CAP_LSTPR (CI__SGZ, 1, 0, GUI__SGZ, STATUS)

         ELSE
            CALL CAP_WARN (GUI__SGZ, ' ', 'There is no open catalogue.',
     :        STATUS)

         END IF

*
*       Report any error.

         IF (STATUS .NE. SAI__OK) THEN
            CALL ERR_REP ('CAP_GDPAR_ERR', 'Error displaying details '/
     :        /'for parameters.', STATUS)
         END IF

      END IF

      END
