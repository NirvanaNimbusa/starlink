      SUBROUTINE SKYFRAME( STATUS )
*+
*  Name:
*     SKYFRAME

*  Purpose:
*     Create a SkyFrame.

*  Language:
*     Starlink Fortran 77

*  Type of Module:
*     ADAM A-task

*  Invocation:
*     CALL SKYFRAME( STATUS )

*  Arguments:
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Description:
*     This application creates a new SkyFrame and optionally initialises
*     its attributes. A SkyFrame is a specialised form of Frame which
*     describes celestial longitude/latitude coordinate systems. The
*     particular celestial coordinate system to be represented is 
*     specified by setting the SkyFrame's System attribute (currently, 
*     the default is FK5) qualified, as necessary, by a mean Equinox 
*     value and/or an Epoch. 
*
*     All the coordinate values used by a SkyFrame are in radians.

*  Usage:
*     skyframe options result

*  ADAM Parameters:
*     OPTIONS = LITERAL (Read)
*        A string containing an optional comma-separated list of attribute 
*        assignments to be used for initialising the new SkyFrame. 
*     RESULT = LITERAL (Read)
*        A text file to receive the new SkyFrame.

*  Authors:
*     DSB: David Berry (STARLINK)
*     {enter_new_authors_here}

*  History:
*     12-JAN-2001 (DSB):
*        Original version.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-
*  Type Definitions:
      IMPLICIT NONE              ! no default typing allowed

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'AST_PAR'          ! AST constants and function declarations

*  Status:
      INTEGER STATUS

*  Local Variables:
      INTEGER RESULT
*.

*  Check inherited status.      
      IF( STATUS .NE. SAI__OK ) RETURN

*  Begin an AST context.
      CALL AST_BEGIN( STATUS )

*  Create the required SkyFrame.
      RESULT = AST_SKYFRAME( ' ', STATUS )

*  Store the required attribute values.
      CALL ATL1_SETOP( 'OPTIONS', RESULT, STATUS )

*  Write the results out to a text file.
      CALL ATL1_PTOBJ( 'RESULT', ' ', RESULT, STATUS )

 999  CONTINUE

*  End the AST context.
      CALL AST_END( STATUS )

*  Give a context message if anything went wrong.
      IF( STATUS .NE. SAI__OK ) THEN
         CALL ERR_REP( 'SKYFRAME_ERR', 'Error creating a new SkyFrame.',
     :                 STATUS )
      END IF

      END
