      SUBROUTINE KPG1_ASGET( INDF, NDIM, EXACT, TRIM, REQINV, SDIM, 
     :                       SLBND, SUBND, IWCS, STATUS )
*+
*  Name:
*     KPG1_ASGET

*  Purpose:
*     Get an AST FrameSet from the WCS component of an NDF.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL KPG1_ASGET( INDF, NDIM, EXACT, TRIM, REQINV, SDIM, SLBND, SUBND, 
*                      IWCS, STATUS )

*  Description:
*     This routine determines the axes to be used from an NDF and returns a
*     FrameSet representing the WCS information in the NDF. 
*
*     Each axis of the supplied NDF is checked to see if it is significant 
*     (i.e. has a size greater than 1).  The index of each significant axis 
*     is returned in SDIM, and the bounds of the axis are returned in SLBND 
*     and SUBND.  If EXACT is .TRUE., an error is reported if the number of 
*     significant axes is not exactly NDIM.  This mode is intended for case 
*     where (say) the user has supplied a single plane from a 3D data cube
*     to an application which requires a 2D array.
*
*     If EXACT is .FALSE. an error is only reported if the number of 
*     significant dimensions is higher than NDIM.  If there are less than 
*     NDIM significant dimensions then the insignificant dimensions are 
*     used (starting from the lowest) to ensure that the required number 
*     of dimensions are returned. This mode is intended for cases where (say)
*     the user supplies a 1D data stream to an application which requires a
*     2D array.
*
*     The GRID Frame (i.e. the Base Frame) obtained from the NDFs WCS 
*     component is modified so that it has NDIM axes corresponding to the 
*     axes returned in SDIM (the value 1.0 is used for the other axes). 
*
*     Likewise, the PIXEL Frame obtained from the NDFs WCS component is 
*     modified so that it has NDIM axes corresponding to the axes returned 
*     in SDIM (the lower pixel bound is used for the other axes). The
*     original PIXEL Frame is retained, but with Domain changed to
*     NDF_PIXEL.
*
*     If TRIM is .TRUE., then the Current Frame obtained from the NDFs WCS
*     component is also modified so that it has NDIM axes. If the original
*     Current Frame has more than NDIM axes, then the axes to use are 
*     obtained from the environment using parameter USEAXIS. A new Current 
*     Frame is then made by picking these axes from the original Current
*     Frame, assigning the value AST__BAD to the axes which have not been 
*     chosen.
*
*     If the original Current Frame has less than NDIM axes, then simple
*     axes are added into the new Current Frame to make up a total of
*     NDIM. These axes are given the value 1.0.

*  Parameters:
*     The name of the following environment parameter(s) are hard-wired 
*     into this subroutine in order to ensure conformity between application. 
*
*     USEAXIS = LITERAL (Read)
*        A set of NDIM axes to be selected from the Current Frame. Each 
*        axis can be specified either by giving its index within the Current 
*        Frame in the range 1 to the number of axes in the Frame, or by
*        giving its symbol. This parameter is only accessed if TRIM is
*        .TRUE. and the original Current Frame in the supplied NDF has
*        too many axes. The dynamic default selects the axes with the same
*        indices as the selected NDF axes. The value should be given as a
*        GRP group expression, with default control characters. []

*  Arguments:
*     INDF = INTEGER (Given)
*        The identifier for the NDF.
*     NDIM = INTEGER (Given)
*        The number of dimensions required by the application.
*     EXACT = LOGICAL (Given)
*        Must the NDF have exactly NDIM significant axes? Otherwise it is
*        acceptable for the NDIM axes to include some insignificant axes.
*     TRIM = LOGICAL (Given)
*        Should the returned FrameSet be trimmed to ensure that the
*        Current Frame has NDIM axes? Otherwise, the Current Frame read
*        from the NDF is not changed.
*     REQINV = LOGICAL (Given)
*        Is the inverse mapping (from Current Frame to Base Frame)
*        required? If it is, an error is reported if the inverse mapping
*        is not available. REQINV should be supplied .TRUE. in most cases.
*     SDIM( NDIM ) = INTEGER (Returned)
*        The indices of the significant dimensions.
*     SLBND( NDIM ) = INTEGER (Returned)
*        The lower pixel index bounds of the significant dimensions.  These 
*        are stored in the same order as the indices in SDIM.
*     SUBND( NDIM ) = INTEGER (Returned)
*        The upper pixel index bounds of the significant dimensions.  These
*        are stored in the same order as the indices in SDIM.
*     IWCS = INTEGER (Returned)
*        An AST pointer to the WCS FrameSet. Returned equal to AST__NULL
*        if an error occurs. The Base Frame is an NDIM-dimensional GRID 
*        Domain.
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Notes:
*     -  If the Current Frame in the returned FrameSet has no Title, then
*     the Title is set to the value of the NDF TITLE component (so long
*     as the NDF TITLE is not blank or undefined).

*  Authors:
*     DSB: David Berry (STARLINK)
*     {enter_new_authors_here}

*  History:
*     30-JUN-1998 (DSB):
*        Original version.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-
      
*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'NDF_PAR'          ! NDF constants
      INCLUDE 'AST_PAR'          ! AST constants
      INCLUDE 'PRM_PAR'          ! PRIMDAT constants

*  Arguments Given:
      INTEGER INDF
      INTEGER NDIM
      LOGICAL EXACT
      LOGICAL TRIM
      LOGICAL REQINV

*  Arguments Returned:
      INTEGER SDIM( NDIM )
      INTEGER SLBND( NDIM )
      INTEGER SUBND( NDIM )
      INTEGER IWCS

*  Status:
      INTEGER STATUS             ! Global status

*  External References:
      INTEGER CHR_LEN            ! Used length of a string
      LOGICAL CHR_SIMLR          ! Strings equal apart from case?

*  Local Constants:
      INTEGER SZFMT              ! Max. characters in formatted value
      PARAMETER ( SZFMT = 2 * VAL__SZD )

*  Local Variables:
      CHARACTER COSTR*( NDF__MXDIM * ( SZFMT + 1 ) + 1 ) 
                                   ! Formatted coordinate string
      CHARACTER PAXIS*( VAL__SZI ) ! Buffer for new axis number
      CHARACTER QAXIS*( VAL__SZI ) ! Buffer for original axis number
      CHARACTER TTL*80             ! NDF title
      DOUBLE PRECISION CONST( NDF__MXDIM )! Constant values for unassigned axes
      INTEGER FRM                  ! Pointer to a Frame in IWCS
      INTEGER I                    ! Axis index
      INTEGER IAXIS( NDF__MXDIM )  ! Axes to pick from the old Current Frame
      INTEGER IBASE                ! Index of original Base Frame in IWCS
      INTEGER ICURR                ! Index of original Current Frame in IWCS
      INTEGER INPRM( NDF__MXDIM )  ! Input axis permutation array
      INTEGER INDFS                ! NDF section identifier
      INTEGER IPIX                 ! Index of original PIXEL Frame in IWCS
      INTEGER IUP                  ! Highest significant axis index
      INTEGER J                    ! Axis index
      INTEGER LBND( NDF__MXDIM )   ! Original NDF bounds
      INTEGER NC                   ! No. of characters in text buffer
      INTEGER NCP                  ! No. of characters in PAXIS text buffer
      INTEGER NCQ                  ! No. of characters in QAXIS text buffer
      INTEGER NDIMS                ! No. of genuine axes in the NDF
      INTEGER NEWBAS               ! Pointer to the new Base Frame
      INTEGER NEWCUR               ! Pointer to the new Current Frame
      INTEGER NEWFS                ! Pointer to a FrameSet with new Base Frame
      INTEGER NEWPIX               ! Pointer to the new PIXEL Frame
      INTEGER NFC                  ! No. of axes in original Current Frame
      INTEGER OUTPRM( NDF__MXDIM ) ! Output axis permutation array
      INTEGER PMAP                 ! AST pointer to a PermMap
      INTEGER UBND( NDF__MXDIM )   ! Original NDF bounds
*.

*  Initialise.
      IWCS = AST__NULL

*  Check the inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Begin an AST context.
      CALL AST_BEGIN( STATUS )

*  Choose the NDF axes to be used by the application.
*  ==================================================

*  Find whether or not there are exactly the required number of
*  significant dimensions and which ones they are.
      IF ( EXACT ) THEN
         CALL KPG1_SGDIM( INDF, NDIM, SDIM, STATUS )

*  If insignificant dimensions can be used, pad out the returned
*  dimensions with insignificant dimensions if the required number
*  of significant dimensions is not present.
      ELSE
         CALL KPG1_SDIMP( INDF, NDIM, SDIM, STATUS )
      END IF

*  Obtain the bounds of the NDF.  
      CALL NDF_BOUND( INDF, NDF__MXDIM, LBND, UBND, NDIMS, STATUS )

*  Return the bounds of the chosen pixel axes. Also note the highest 
*  significant axis index.
      IUP = 0
      DO I = 1, NDIM
         SLBND( I ) = LBND( SDIM( I ) )
         SUBND( I ) = UBND( SDIM( I ) )
         IUP = MAX( IUP, SDIM( I ) )
      END DO
         
*  Get an NDF section. This will include or exclude any trailing
*  insignificant axes, as required.
      CALL NDF_SECT( INDF, IUP, LBND, UBND, INDFS, STATUS )

*  Get a pointer to the WCS FrameSet.
      CALL KPG1_GTWCS( INDFS, IWCS, STATUS )

*  Re-map the Base (GRID) Frame by selecting the chosen axes.
*  ==========================================================

*  If the Base Frame has the wrong number of axes, create a new Base
*  Frame by picking axes from the original.
      IF( IUP .NE. NDIM ) THEN

*  Create a new GRID Frame with NDIM axes.
         NEWBAS = AST_FRAME( NDIM, 'DOMAIN=GRID', STATUS )

*  Create a title for it, including the grid coordinates of the first
*  pixel, i.e. (1.0,1.0,...) 
         NC = 0
         CALL CHR_PUTC( '(', COSTR, NC )
         DO I = 1, NDIM
            IF ( I .GT. 1 ) CALL CHR_PUTC( ',', COSTR, NC )
            CALL CHR_PUTC( '1.0', COSTR, NC )
         END DO
         CALL CHR_PUTC( ')', COSTR, NC )

*  Store the title in the Frame.
         IF ( NDIM .EQ. 1 ) THEN
            CALL AST_SETC( NEWBAS, 'Title', 'Data grid index; first '//
     :                     'pixel at ' // COSTR( : NC ), STATUS )
         ELSE
            CALL AST_SETC( NEWBAS, 'Title', 'Data grid indices; '//
     :                     'first pixel at ' // COSTR( : NC ), STATUS ) 
         END IF

*  For each axis, set up a label, symbol and unit value. Use the original
*  NDF axis numbers.
         DO I = 1, NDIM
            NCP = 0
            NCQ = 0

            CALL CHR_PUTI( I, PAXIS, NCP )
            CALL CHR_PUTI( SDIM( I ), QAXIS, NCQ )

            CALL AST_SETC( NEWBAS, 'Label(' // PAXIS( : NCP ) // ')',
     :                     'Data grid index ' // QAXIS( : NCQ ), 
     :                     STATUS )
            CALL AST_SETC( NEWBAS, 'Symbol(' // PAXIS( : NCP ) // ')',
     :                     'g' // QAXIS( : NCQ ), STATUS )
            CALL AST_SETC( NEWBAS, 'Unit(' // PAXIS( : NCP ) // ')',
     :                     'pixel', STATUS )

         END DO

*  Create a PermMap which goes from this new NDIM-dimensional GRID Frame 
*  to the original IUP-dimensional GRID Frame. First, initialise the axis 
*  permutation arrays so that all input and output axes take the value of 
*  the first constant supplied to AST_PERMMAP (i.e. 1.0).
         DO I = 1, IUP
            OUTPRM( I ) = -1
         END DO

         DO I = 1, NDIM
            INPRM( I ) = -1
         END DO

*  Now over-write elements of the axis permutation arrays which correspond to 
*  genuine axes.
         DO I = 1, NDIM
            IF( SDIM( I ) .LE. IUP ) THEN
               INPRM( I ) = SDIM( I )
               OUTPRM( SDIM( I ) ) = I
            END IF 
         END DO

*  Create the PermMap.
         PMAP = AST_PERMMAP( NDIM, INPRM, IUP, OUTPRM, 1.0D0, ' ', 
     :                       STATUS ) 

*  Create a new FrameSet holding just the new GRID Frame.
         NEWFS = AST_FRAMESET( NEWBAS, ' ', STATUS )

*  Note the indices of the original Base and Current Frames.
         IBASE = AST_GETI( IWCS, 'BASE', STATUS )
         ICURR = AST_GETI( IWCS, 'CURRENT', STATUS )

*  Make the GRID Frame the Current Frame (AST_ADDFRAME uses the Current
*  Frame).
         CALL AST_SETI( IWCS, 'CURRENT', IBASE, STATUS )
         
*  Add in the FrameSet read from the NDF, using the PermMap created above
*  to connect the Base (GRID) Frame to the new FrameSet. 
         CALL AST_ADDFRAME( NEWFS, AST__BASE, PMAP, IWCS, STATUS )
         CALL AST_ANNUL( PMAP, STATUS )

*  Remove the original GRID Frame. Its index will have increased by 1
*  because of the single Frame which was already in NEWFS.
         CALL AST_REMOVEFRAME( NEWFS, IBASE + 1, STATUS )

*  Re-instate the original Current Frame.
         CALL AST_SETI( NEWFS, 'CURRENT', ICURR, STATUS )

*  Annul the original FrameSet and use the new one instead.
         CALL AST_ANNUL( IWCS, STATUS )
         IWCS = NEWFS

      END IF       

*  Re-map the PIXEL Frame by selecting the chosen axes.
*  ====================================================

*  If the PIXEL Frame has the wrong number of axes, create a new PIXEL
*  Frame by picking axes from the original.
      IF( IUP .NE. NDIM ) THEN

*  Create a new PIXEL Frame with NDIM axes.
         NEWPIX = AST_FRAME( NDIM, 'DOMAIN=PIXEL', STATUS )

*  Create a title for it, including the pixel coordinates of the first
*  pixel.
         NC = 0
         CALL CHR_PUTC( '(', COSTR, NC )
         DO I = 1, NDIM
            IF ( I .GT. 1 ) CALL CHR_PUTC( ',', COSTR, NC )
            CALL CHR_PUTR( REAL( SLBND( I ) ) - 0.5, COSTR, NC )
         END DO
         CALL CHR_PUTC( ')', COSTR, NC )

*  Store the title in the Frame.
         IF ( NDIM .EQ. 1 ) THEN
            CALL AST_SETC( NEWPIX, 'Title', 'Pixel coordinate; first '//
     :                     'pixel at ' // COSTR( : NC ), STATUS )
         ELSE
            CALL AST_SETC( NEWPIX, 'Title', 'Pixel coordinates; '//
     :                     'first pixel at ' // COSTR( : NC ), STATUS ) 
         END IF

*  For each axis, set up a label, symbol and unit value. Use the original
*  NDF axis numbers.
         DO I = 1, NDIM
            NCP = 0
            NCQ = 0

            CALL CHR_PUTI( I, PAXIS, NCP )
            CALL CHR_PUTI( SDIM( I ), QAXIS, NCQ )

            CALL AST_SETC( NEWPIX, 'Label(' // PAXIS( : NCP ) // ')',
     :                     'Pixel coordinate ' // QAXIS( : NCQ ), 
     :                     STATUS )
            CALL AST_SETC( NEWPIX, 'Symbol(' // PAXIS( : NCP ) // ')',
     :                     'p' // QAXIS( : NCQ ), STATUS )
            CALL AST_SETC( NEWPIX, 'Unit(' // PAXIS( : NCP ) // ')',
     :                     'pixel', STATUS )

         END DO

*  Store the pixel coordinate at the centre of the low bound pixel on each
*  axis. These are the constants used by AST_PERMMAP.
         DO I = 1, NDF__MXDIM
            CONST( I ) = DBLE( LBND( I ) ) - 0.5D0
         END DO

*  Create a PermMap which goes from this new NDIM-dimensional PIXEL Frame 
*  to the original IUP-dimensional PIXEL Frame. First, initialise the axis 
*  permutation arrays so that all input and output axes take the value of 
*  the corresponding axis lower bound. Nagative values index the array of
*  constants set up above.
         DO I = 1, IUP
            OUTPRM( I ) = -I
         END DO

         DO I = 1, NDIM
            INPRM( I ) = -SDIM( I )
         END DO

*  Now over-write elements of the axis permutation arrays which correspond to 
*  genuine axes.
         DO I = 1, NDIM
            IF( SDIM( I ) .LE. IUP ) THEN
               INPRM( I ) = SDIM( I )
               OUTPRM( SDIM( I ) ) = I
            END IF 
         END DO

*  Create the PermMap.
         PMAP = AST_PERMMAP( NDIM, INPRM, IUP, OUTPRM, CONST, ' ', 
     :                       STATUS ) 

*  Find the original PIXEL Frame, and change its Domain to NDF_PIXEL.
         IPIX = 0
         DO I = 1, AST_GETI( IWCS, 'NFRAME', STATUS )
            FRM = AST_GETFRAME( IWCS, I, STATUS )
            IF( AST_GETC( FRM, 'DOMAIN', STATUS ) .EQ. 'PIXEL' ) THEN
               CALL AST_SETC( FRM, 'DOMAIN', 'NDF_PIXEL', STATUS )
               CALL AST_ANNUL( FRM, STATUS )
               IPIX = I
               GO TO 10
            END IF
            CALL AST_ANNUL( FRM, STATUS )
         END DO
 10      CONTINUE

*  Report an error if no PIXEL Frame was found.
         IF( IPIX .EQ. 0 .AND. STATUS .EQ. SAI__OK ) THEN
            STATUS = SAI__ERROR
            CALL NDF_MSG( 'NDF', INDF )
            CALL ERR_REP( 'KPG1_ASGET_1', 'No PIXEL Frame found '//
     :                    'in WCS component of ''^NDF'' (possible '//
     :                    'programming error).', STATUS )      
         END IF

*  Invert the PermMap so that its forward mapping goes from the original 
*  PIXEL Frame to the new one. This is the direction required by
*  AST_ADDFRAME.
         CALL AST_INVERT( PMAP, STATUS )

*  Save the index of the Current Frame (changed by AST_ADDFRAME).
         ICURR = AST_GETI( IWCS, 'CURRENT', STATUS )
         
*  Add the new PIXEL Frame on to the end of the FrameSet, using the above
*  PermMap to connect it to the original PIXEL Frame. The new PIXEL Frame
*  becomes the Current Frame.
         CALL AST_ADDFRAME( IWCS, IPIX, PMAP, NEWPIX, STATUS )

*  Re-instate the original Current Frame, unless this was the original PIXEL 
*  Frame (in which case the new PIXEL Frame is left as the Current Frame).
         IF( ICURR .NE. IPIX ) CALL AST_SETI( IWCS, 'CURRENT', ICURR,
     :                                        STATUS ) 

      END IF       

*  Now modify the Current Frame if required to have exactly NDIM axes.
*  ===================================================================
      IF( TRIM ) THEN

*  Get the number of axes in the original Current Frame.
         NFC = MIN( NDF__MXDIM, AST_GETI( IWCS, 'NAXES', STATUS ) )

*  First deal with cases where the Current Frame has too many axes. 
*  At the moment a PermMap is used to select NDIM axes from those
*  available in the Current Frame. A value of AST__BAD is assigned to the
*  other axes by this PermMap. If the selected axes are not independant
*  of the other axes, then these AST__BAD values will result in bad values
*  on all axes when points are transformed from the trimmed Current Frame
*  to the GRID (or any other) Frame. Ideally, the value assigned to the
*  "non-slected" axes by the PermMap would vary in order to ensure that
*  points transformed into the GRID Frame reside in the slice selected
*  above. This may be possible with a future release of AST, which may
*  include a "SubMap" class for doing this. Until then, the best that we
*  can do is to assign AST__BAD values to the non-selected axes. At least
*  this will work in the common cases.
         IF( NFC .GT. NDIM ) THEN

*  Get the required number of axis selections. A resonable guess should
*  be to assume a one-to-one correspondance between Current and Base axes.
*  Therefore, use the significant axes selected above as the defaults to be
*  used if a null (!) parameter value is supplied.
            DO I = 1, NDIM
               IAXIS( I ) = SDIM( I )
            END DO

            CALL KPG1_GTAXI( 'USEAXIS', IWCS, NDIM, IAXIS, STATUS )

*  Create a new Frame by picking the selected axes from the original
*  Current Frame. This also returns a PermMap which goes from the 
*  original Frame to the new one, using AST__BAD values for the
*  un-selected axes.
            NEWCUR = AST_PICKAXES( IWCS, NDIM, IAXIS, PMAP, STATUS )

*  If the original Current Frame is a CmpFrame, the Frame created from
*  the above call to AST_PICKAXES may not have inherited its Title. If
*  the Frame created above has no Title, but the original Frame had, then
*  copy the original Frame's Title to the new Frame.
            IF( AST_TEST( IWCS, 'TITLE', STATUS ) .AND.
     :          .NOT. AST_TEST( NEWCUR, 'TITLE', STATUS ) ) THEN
               CALL AST_SETC( NEWCUR, 'TITLE', AST_GETC( IWCS, 'TITLE',
     :                                                STATUS ), STATUS )
            END IF

*  Add this new Frame into the FrameSet. It becomes the Current Frame.
            CALL AST_ADDFRAME( IWCS, AST__CURRENT, PMAP, NEWCUR, 
     :                         STATUS )

*  Now deal with cases where the original Current Frame has too few axes.
         ELSE IF( NFC .LT. NDIM ) THEN

*  Use zero to indicate the extra axes required.
            DO I = 1, NFC
               IAXIS( I ) = I
            END DO            

            DO I = NFC + 1, NDIM
               IAXIS( I ) = 0
            END DO            

*  Create a new Frame by adding the extra default axes to the original
*  Current Frame. This also returns a PermMap which goes from the 
*  original Frame to the new one, using AST__BAD values for the
*  new un-selected axes.
            NEWCUR = AST_PICKAXES( IWCS, NDIM, IAXIS, PMAP, STATUS )

*  Add this new Frame into the FrameSet. It becomes the Current Frame.
            CALL AST_ADDFRAME( IWCS, AST__CURRENT, PMAP, NEWCUR, 
     :                         STATUS )

         END IF

      END IF

*  If the Current Frame has no Title, use the Title from the NDF (if any).
      TTL = ' '
      CALL NDF_CGET( INDF, 'TITLE', TTL, STATUS )
      IF( TTL .NE. ' ' .AND. 
     :    .NOT. AST_TEST( IWCS, 'TITLE', STATUS ) ) THEN
         CALL AST_SETC( IWCS, 'TITLE', TTL, STATUS )
      END IF

*  Report an error if the inverse mapping is required, but is not
*  available.
      IF( REQINV .AND. .NOT. AST_GETL( IWCS, 'TRANINVERSE', STATUS ) 
     :    .AND. STATUS .EQ. SAI__OK ) THEN

         STATUS = SAI__ERROR
         CALL NDF_MSG( 'NDF', INDF ) 
         CALL ERR_REP( 'KPG1_ASGET_2', 'The mapping from the current '//
     :                 'co-ordinate Frame in ^NDF to the pixel '//
     :                 'co-ordinate Frame is not defined.', STATUS )

         CALL NDF_MSG( 'NDF', INDF ) 
         CALL ERR_REP( 'KPG1_ASGET_2', 'It may be possible to avoid '//
     :                 'this problem by changing the current '//
     :                 'co-ordinate Frame in ^NDF using WCSFRAME.', 
     :                  STATUS )

      END IF

*  Export the returned FrameSet from the current AST context so that it is
*  not annulled by the following call to AST_END.
      CALL AST_EXPORT( IWCS, STATUS )

*  End the AST context.
      CALL AST_END( STATUS )
    
      END
