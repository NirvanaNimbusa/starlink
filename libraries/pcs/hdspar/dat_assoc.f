      SUBROUTINE DAT_ASSOC ( PARAM, ACCESS, LOC, STATUS )
*+
*  Name:
*     DAT_ASSOC

*  Purpose:
*     Return a locator associated with a parameter.

*  Language:
*     Fortran 77

*  Invocation:
*     CALL DAT_ASSOC ( PARAM, ACCESS, LOC, STATUS )

*  Description:
*     An HDS locator for the data object associated with the specified
*     parameter is returned. The parameter system will attempt to associate 
*     an object if one is not already associated. In the event of a failure,
*     an error message will be displayed and another attempt made (usually
*     by prompting the user). Up to five attempts will be made, after which
*     status PAR__NULL will be returned.
*
*     The object will be opened with the appropriate ACCESS mode. If ACCESS
*     is incompatible with the access mode specified for the parameter in 
*     the program's Interface File, status SUBPAR__ICACM will be returned.

*  Arguments:
*     PARAM=CHARACTER*(*) (given)
*        Name of program parameter
*     ACCESS=CHARACTER*(*) (given)
*        Access mode, 'READ', 'WRITE' or 'UPDATE'
*        (case insignificant)
*     LOC=CHARACTER*(*) (returned)
*        Locator to the associated data object
*     STATUS=INTEGER (given and returned)
*        Global status

*  Algorithm:
*     The named parameter is found and its identifying number obtained.
*     SUBPAR_ASSOC the does the work of obtaining a locator.

*  Authors:
*     BDK: B D Kelly (ROE)
*     AJC: A J Chipperfield (Starlink)
*     {enter_new_authors_here}

*  History:
*     24-SEP-1984 (BDK)
*        Original
*     30-JAN-1992 (AJC)
*        Inherit LOC size
*      1-SEP-1993 (AJC)
*        Correct "Arguments" section for above mod
*     16-JUN-1998 (AJC)
*        Re-format prologue
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE

*  Global Constants:
      INCLUDE 'SAE_PAR'

*  Arguments Given:
      CHARACTER*(*) PARAM          ! Name of program parameter

      CHARACTER*(*) ACCESS         ! Access mode, 'READ', 'WRITE'
                                   ! or 'UPDATE'

*  Arguments Returned:
      CHARACTER*(*) LOC            ! Locator to data object

*  Status:
      INTEGER STATUS

*  Local Variables:
      INTEGER NAMECODE                     ! pointer to storage for
                                           ! parameter
*.

      IF ( STATUS .NE. SAI__OK ) RETURN

      CALL SUBPAR_FINDPAR ( PARAM, NAMECODE, STATUS )

      CALL SUBPAR_ASSOC ( NAMECODE, ACCESS, LOC, STATUS )

      END
