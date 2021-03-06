      SUBROUTINE KPG1_ASRGG( STATUS )
*+
*  Name:
*     KPG1_ASRGG

*  Purpose:
*     Registers all graphical AST IntraMaps known by KAPPA.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL KPG1_ASRGG( STATUS )

*  Description:

*     This routine registers all graphical AST IntraMaps known to
*     KAPPA. It should be called before any AST routine which may use an
*     IntraMap (such as a transformation routine, read/write routine,
*     etc.).

*  Arguments:
*     STATUS = INTEGER (Given)
*        Global status value.

*  Copyright:
*     Copyright (C) 1998 Central Laboratory of the Research Councils.
*     Copyright (C) 2005 Particle Physics & Astronomy Research Council.
*     All Rights Reserved.

*  Licence:
*     This program is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either Version 2 of
*     the License, or (at your option) any later version.
*
*     This program is distributed in the hope that it will be
*     useful, but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE. See the GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with this program; if not, write to the Free Software
*     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
*     02110-1301, USA.

*  Authors:
*     DSB: David S. Berry (STARLINK)
*     TIMJ: Tim Jenness (JAC, Hawaii)
*     {enter_new_authors_here}

*  History:
*     21-SEP-1998 (DSB):
*        Original version.
*     12-SEP-2005 (TIMJ):
*        Factor out from KPG1_ASREG
*     {enter_further_changes_here}

*  Notes:
*     Use KPG1_ASREG to register all IntraMaps (graphical and non-graphical).
*     Use KPG1_ASRGN to register just non-graphical IntraMaps.

*  Bugs:
*     {note_new_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'AST_PAR'          ! AST constants
      INCLUDE 'KPG_PAR'          ! KPG constants

*  Status:
      INTEGER STATUS             ! Global status

*  External References:
      EXTERNAL KPG1_ASAGD

*  Local Variables:
      CHARACTER PURPOSE*80
*.

*  Check the global inherited status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  "ASAGD" - Encapsulates a TRANSFORM structure stored with an AGI database
*  picture. The transformation routine uses the TRANSFORM structure
*  associated with the current AGI picture.
      PURPOSE = 'Converts AGI World co-ords to AGI DATA co-ords'
      CALL AST_INTRAREG( 'ASAGD', 2, 2, KPG1_ASAGD,
     :                   AST__SIMPIF + AST__SIMPFI, PURPOSE( : 46 ),
     :                   KPG_AUTHOR, KPG_CONTACT, STATUS )

      END
