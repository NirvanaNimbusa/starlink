      SUBROUTINE CCDALIGN( PID, STATUS )
*+
*  Name:
*     CCDALIGN

*  Purpose:
*     Interactive procedure to aid the alignment of NDFs.

*  Language:
*     Starlink Fortran 77

*  Type of Module:
*     ADAM A-task

*  Invocation:
*     CALL CCDALIGN( STATUS )

*  Arguments:
*     PID = CHARACTER * ( * ) (Given)
*        A string to append to any task names used in this routine.
*        (this should be setup to allow attachment to existing monoliths).
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Description:
*     This procedure aids the registration of NDFs which are not
*     related by simple offsets (see FINDOFF and PAIRNDF if they are).
*     It also has the capability of dealing with groups of NDFs which
*     are almost registered (frames which have not been moved on the
*     sky) saving effort in re-identification of image features.
*
*     The basic method used is to access groups of NDFs (or a series
*     of single NDFs if all are moved between exposures) and an
*     optional reference NDF. The first NDF of the first group or the
*     reference NDF is then displayed and a cursor application is used
*     to record the positions of centroidable image features. The
*     first NDFs of all the other groups are then displayed and you
*     are invited to identify the image features in the order which
*     corresponds to that used for the reference NDF. Missing image
*     features are identified as off the currently displayed image (so
*     the centroid routine will fail to find them). The reference set
*     of image features may be extended by identification after the
*     last reference feature has been marked.
*
*     After centroiding you are then given the option to stop. If
*     you decide to then you will have labelled position lists to use
*     in the other CCDPACK routines (the labelled positions will be
*     called NDF_NAME.acc). If you chose the option to continue then
*     a full registration of the NDFs will be attempted. This may only
*     be performed for 'linear' transformations.
*
*     After choosing a transformation type the procedure will then go on
*     to calculate a transformation set between all the NDFs, this is
*     then used (with the extended reference set from REGISTER) to
*     approximate the position of all possible image features, these are
*     then located by centroiding and a final registration of all NDFs
*     is performed. The resultant NDFs then have associated lists of
*     labelled positions and TRANSFORM structures which may be used to
*     transform other position lists or when resampling the data.

*  Usage:
*     ccdalign [device]

*  ADAM Parameters:
*     CONTINUE = _LOGICAL (Read)
*        If TRUE then this command will proceed to also work
*        out the registrations of your images. Note that this is
*        only possible if you are intending to use linear
*        transformations (this is the usual case).
*        [FALSE]
*     DEVICE = _CHAR (Read)
*        The graphics device to use when displaying images.
*        [Current image display device]
*     HARDCOPY = _LOGICAL (Read)
*        If TRUE then a hardcopy of the reference NDF will be made.
*        [TRUE]
*     HARDDEV = _CHAR (Read)
*        A graphics device that can be used to draw a copy of the 
*        displayed image into a file that can be subsequently printed.
*        A postscript device such as "ps_p" or "ps_l" is normal.
*        [ps_p]
*     IN = LITERAL (Read)
*        A list of NDF names suitable to the current stage in
*        processing. The NDF names should be separated by commas
*        and may include wildcards.
*        [!]
*     LOGFILE = FILENAME (Read)
*        Name of the CCDPACK logfile.  If a null (!) value is given for
*        this parameter then no logfile will be written, regardless of
*        the value of the LOGTO parameter.
*
*        If the logging system has been initialised using CCDSETUP
*        then the value specified there will be used. Otherwise, the
*        default is 'CCDPACK.LOG'.
*        [CCDPACK.LOG]
*     LOGTO = LITERAL (Read)
*        Every CCDPACK application has the ability to log its output
*        for future reference as well as for display on the terminal.
*        This parameter controls this process, and may be set to any
*        unique abbreviation of the following:
*           -  TERMINAL  -- Send output to the terminal only
*           -  LOGFILE   -- Send output to the logfile only (see the
*                           LOGFILE parameter)
*           -  BOTH      -- Send output to both the terminal and the
*                           logfile
*           -  NEITHER   -- Produce no output at all
*
*        If the logging system has been initialised using CCDSETUP
*        then the value specified there will be used. Otherwise, the
*        default is 'BOTH'.
*        [BOTH]
*     MAXCANV = INTEGER (Read and Write)
*        A dimension in pixels for the maximum X or Y dimension of the
*        region in which the NDF is displayed.  Note this is the
*        scrolled region, and may be much bigger than the sizes given
*        by WINX and WINY, which limit the size of the window on the
*        X display.  It can be overridden during operation by zooming
*        in and out using the GUI controls, but it is intended to
*        limit the size for the case when ZOOM is large (perhaps
*        because the last image was quite small) and a large image
*        is going to be displayed, which otherwise might lead to
*        the program attempting to display an enormous viewing region.
*        If set to zero, then no limit is in effect.
*        [1280]
*     PERCENTILES( 2 ) = _DOUBLE (Read)
*        The low and high percentiles of the data range to use when
*        displaying the images; any pixels with a value lower than
*        the first value will have the same colour, and any with a value
*        higher than the second will have the same colour.  Must be in
*        the range 0 <= PERCENTILES( 1 ) <= PERCENTILES( 2 ) <= 100.
*        [2,98]
*     PRINTCMD = _CHAR (Read)
*        A command that will print the file "snapshot.ps". This file
*        is the result of printing a hardcopy of the image display
*        device. 
*        [lpr snapshot.ps]
*     WINX = INTEGER (Read and Write)
*        The width in pixels of the window to display the image and
*        associated controls in.  If the image is larger than the area
*        allocated for display, it can be scrolled around within the
*        window.  The window can be resized in the normal way using
*        the window manager while the program is running.
*        [200]
*     WINY = INTEGER (Read and Write)
*        The height in pixels of the window to display the image and
*        associated controls in.  If the image is larger than the area
*        allocated for display, it can be scrolled around within the
*        window.  The window can be resized in the normal way using
*        the window manager while the program is running.
*        [300]
*     ZOOM = DOUBLE (Read and Write)
*        A factor giving the initial level to zoom in to the image
*        displayed, that is the number of screen pixels to use for one
*        image pixel.  It will be rounded to one of the values
*        ... 3, 2, 1, 1/2, 1/3 ....  The zoom can be changed
*        interactively from within the program.  The initial value
*        may be limited by MAXCANV.
*        [1]

*  Examples:
*     ccdalign device=xw
*        This starts the CCDALIGN script and displays all images
*        in a GWM xwindows window.

*  Behaviour of parameters:
*     All parameters retain their current value as default. The
*     'current' value is the value assigned on the last run of the
*     application. If the application has not been run then the
*     'intrinsic' defaults, as shown in the parameter help, apply.
*
*     As this application is a script some of the parameters are used
*     repeatedly and cannot be sensibly set on the command-line, indeed
*     it is not designed for this use and in general all parameters
*     should be responded to interactively.
*
*     Certain parameters (LOGTO and LOGFILE) have global values. These
*     global values will always take precedence, except when an
*     assignment is made on the command line.  Global values may be set
*     and reset using the CCDSETUP and CCDCLEAR commands.
*
*     The DEVICE parameter also has a global association. This is not
*     controlled by the usual CCDPACK mechanisms, instead it works in
*     co-operation with KAPPA (SUN/95) image display/control routines.

*     Note - test it with NDFs in different directories.

*  Authors:
*     PDRAPER: Peter Draper (STARLINK - Durham University)
*     MBT: Mark Taylor (STARLINK)
*     {enter_new_authors_here}

*  History:
*     20-MAY-1997 (PDRAPER):
*        Original version.
*     16-OCT-1998 (PDRAPER):
*        Added call to SLV_RESET to work around problems with
*        KAPPA:DISPLAY dynamic parameters (X/YMAGN and CENTRE) not
*        updating. This is a problem when using images of differing
*        sizes.  
*     1-APR-1999 (MBT):
*        Modified to use WCS components.
*     19-MAY-2000 (MBT):
*        Added a call to IDI_ASSOC to ensure that no attempt is made to
*        use an unsupported visual.
*     25-AUG-2000 (MBT):
*        Removed the above IDI_ASSOC call since it causes obscure
*        problems on alpha_osf1 and sun4_solaris.
*     29-AUG-2000 (MBT):
*        Replaced use of CCDNDFAC A-task/routine, to try to fix weird 
*        platform-dependent parameter-related problems, with the normal
*        routine CCD1_NGLIS.  Turned out to be an NDG bug, but it's
*        cleaner this way anyway.
*     11-OCT-2000 (MBT):
*        Rewrote using Tcl instead of IDI.
*     {enter_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE             ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'         ! Standard SAE constants
      INCLUDE 'MSG_PAR'         ! Message system constants
      INCLUDE 'FIO_ERR'         ! FIO error codes
      INCLUDE 'FIO_PAR'         ! FIO system constants
      INCLUDE 'GRP_PAR'         ! Standard GRP system constants
      INCLUDE 'PSX_ERR'         ! PSX error codes

*  Arguments Given
      CHARACTER * ( * ) PID     ! Parent process id as string

*  Status:
      INTEGER STATUS            ! Global status

*  External References:
      EXTERNAL SLV_LOADW
      INTEGER SLV_LOADW

*  Local Constants:
      INTEGER TIMOUT
      PARAMETER ( TIMOUT = 30 ) ! Timeout when loading tasks
      INTEGER MAXPOS
      PARAMETER ( MAXPOS = 99 ) ! Maximum number of positions in list
      INTEGER MAXGRP
      PARAMETER ( MAXGRP = 20 ) ! Maximum number of NDF groups for alignment

*  External References:
      EXTERNAL CHR_LEN
      INTEGER CHR_LEN           ! Used length of string

*  Local Variables:
      CHARACTER * ( 1024 ) LINE ! Output message line (long)
      CHARACTER * ( 256 ) CMD   ! Command string
      CHARACTER * ( 30 ) DEVICE ! The display device
      CHARACTER * ( 30 ) HDEV   ! Hardcopy device
      CHARACTER * ( 5 ) NULL    ! Parameter NULL symbol
      CHARACTER * ( 30 ) PERC   ! Percentiles as string
      CHARACTER * ( 30 ) LISTID ! Identifier in filename for list
      CHARACTER * ( MSG__SZMSG ) NDFNAM ! Name of NDF
      CHARACTER * ( MSG__SZMSG ) REFNAM ! Name of reference NDF
      CHARACTER * ( FIO__SZFNM ) FNAME ! Name of position list file
      CHARACTER * ( FIO__SZFNM ) NAMLST ! File name list
      CHARACTER * ( 30 ) CCDREG ! Message system name for ccdpack_reg
      CHARACTER * ( 30 ) KAPVIE ! Message system name for kapview_mon
      CHARACTER * ( GRP__SZNAM ) NDFNMS( MAXGRP ) ! Names of lead NDFs in groups
      INTEGER CCDGID            ! Id for CCDPACK_REG monolith
      INTEGER CCDRID            ! Id for CCDPACK_RES monolith
      INTEGER FD                ! File identifier
      INTEGER FDOUT             ! File identifier
      INTEGER FITTYP            ! Transformation type
      INTEGER GRPOFF            ! Offset in NDF list group
      INTEGER I                 ! Loop variable
      INTEGER IGROUP            ! Index of the current group
      INTEGER INDEX( MAXPOS )   ! Identifiers for points in position list
      INTEGER IPIND             ! Pointer to identifiers for list positions
      INTEGER IPXPOS            ! Pointer to X coordinates of list positions
      INTEGER IPYPOS            ! Pointer to Y coordinates of list positions
      INTEGER KAPVID            ! Id for Kappa view monolith
      INTEGER LENG              ! Returned length of string
      INTEGER MAXCNV            ! Initial maximum dimension of display region
      INTEGER NDFLEN            ! length of NDF name
      INTEGER NGROUP            ! Number of file groups
      INTEGER NGNDF             ! Number of NDFs in a group
      INTEGER NL                ! Length of NULL string
      INTEGER NNDF              ! Number of NDFs passed to GUI script
      INTEGER NPOINT( MAXGRP )  ! Number of positions marked for each NDF
c     INTEGER NVAL              ! Dummy
      INTEGER OPLEN             ! String length
      INTEGER REFLEN            ! Length of reference NDF name
      INTEGER REFPOS            ! Position of the reference NDF in the list
      INTEGER WINDIM( 2 )       ! Window dimensions for display
      LOGICAL CONT              ! Continue processing
      LOGICAL EXISTS            ! File exists
      LOGICAL HAVREF            ! Have a reference NDF
      LOGICAL HCOPY             ! Print hardcopy
      LOGICAL OK                ! OK to loop
      DOUBLE PRECISION PERCNT( 2 ) ! The display percentiles
      DOUBLE PRECISION XPOS( MAXPOS ) ! X coordinates of positions in list
      DOUBLE PRECISION YPOS( MAXPOS ) ! Y coordinates of positions in list
      DOUBLE PRECISION ZOOM     ! Zoom factor for display

*.

*  Check inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Find out the environment we're running under. If it's IRAF then
*  INDEF is used instead of ! as a NULL symbol. 
      CALL CCD1_SETEX( NULL, NL, STATUS )
      CCDRID = -1
      CCDGID = -1
      KAPVID = -1

*  Start the application and introduce ourselves.
      CALL CCD1_START( 'CCDALIGN', STATUS )
      CALL CCD1_MSG( ' ', ' ', STATUS )
      CALL CCD1_MSG( ' ',
     :     '  An interactive aid for aligning groups of NDFs.', STATUS )
      CALL CCD1_MSG( ' ', ' ', STATUS )

*  Get the display device.
c     CALL MSG_BLANK( STATUS )
c     CALL MSG_OUT( ' ',
c    :     '  Give the name of an image display device', STATUS )
c     CALL MSG_BLANK( STATUS )
c     CALL PAR_GET0C( 'DEVICE', DEVICE, STATUS )
c     CMD = 'DEVICE='//DEVICE
c     CALL SLV_OBEYW( KAPVIE, 'idset', CMD, ' ', STATUS )
c     IF ( STATUS .NE. SAI__OK ) GO TO 99

*  Get a percentile range for displaying the images. Note we store these
*  as a string as KAPPA display tries to set up defaults and we
*  need to override this behaviour.
      CALL MSG_BLANK( STATUS )
      CALL MSG_OUT( ' ',
     :     '  What percentile range do you want to use when '//
     :     'displaying images?',
     :             STATUS )
      CALL MSG_BLANK( STATUS )
      CALL PAR_EXACD( 'PERCENTILES', 2, PERCNT, STATUS )
c     CALL MSG_SETR( 'LOW', PERCEN( 1 ) )
c     CALL MSG_SETR( 'HIGH', PERCEN( 2 ) )
c     CALL MSG_LOAD( ' ', '[^LOW,^HIGH]', PERC, OPLEN, STATUS )
c     IF ( STATUS .NE. SAI__OK ) GO TO 99

*  Write a message indicating that the user should return lists of
*  all the NDFs to process. Each group contains NDFs which have not
*  been moved on the sky.
      CALL MSG_BLANK( STATUS )
      LINE = 'Give the names of a series of groups of NDFs '//
     :       'whose positions have not been moved on the sky. '//
     :       'If all NDFs have been moved then give a single '//
     :       'NDF at each prompt. When all NDF groups/NDFs have '//
     :       'been given respond with a '//NULL(:NL)//' (null).'
      CALL CCD1_WRTPA( LINE, 72, 3, .FALSE., STATUS )
      CALL MSG_BLANK( STATUS )
      IF ( STATUS .NE. SAI__OK ) GO TO 99

*  Get the lists of NDFs. This continues until no output file list is
*  created.
      OK = .TRUE.
      NGROUP = 1
 1    CONTINUE
      IF ( OK .AND. STATUS .EQ. SAI__OK ) THEN
         CALL MSG_SETI( 'NGROUP', NGROUP )
         CALL MSG_LOAD( ' ', 'ccdalign_ndf^NGROUP.list', NAMLST,
     :                  OPLEN, STATUS )

*  Get a list of NDFs from the user which constitute this group.
         CALL PAR_CANCL( 'IN', STATUS )
         CALL CCD1_NGLIS( 'IN', NAMLST, 100, .TRUE., NGNDF, STATUS )

*  Bump the group number if the user gave some NDF names this time.
         IF ( STATUS .EQ. SAI__OK ) THEN
            IF ( NGNDF .GT. 0 ) THEN
               NGROUP = NGROUP + 1
            ELSE
               OK = .FALSE.
            END IF

*  Check if we have reached the limit of number of groups we can handle.
            IF ( NGROUP .EQ. MAXGRP .AND. OK ) THEN
               CALL CCD1_MSG( ' ', ' ', STATUS )
               CALL CCD1_MSG( ' ', 
     :         ' The maximum number of groups has been entered.', 
     :                        STATUS )
               CALL CCD1_MSG( ' ', ' ', STATUS )
               OK = .FALSE.
            END IF

*  Trap the condition when no groups have been given.
            IF ( NGROUP .EQ. 1 ) THEN
               STATUS = SAI__ERROR
               CALL ERR_REP( 'NOGROUPS', 'No groups of NDFs were given'
     :                     ,STATUS )
               GO TO 99
            END IF
         END IF

*  Return to the start of the loop to get some more NDFs if necessary.
         GO TO 1
      END IF

*  See if a reference NDF is available.
      CALL MSG_BLANK( STATUS )
      LINE = 'If you have a reference image to which the other NDFs '//
     :       'are to be aligned then give its name at the next '//
     :       'prompt. If no reference NDF is specified (signified '//
     :       'by a '//NULL(:NL)//' response) then the first NDF of '//
     :       'the first group will be used.'
      CALL CCD1_WRTPA( LINE, 72, 3, .FALSE., STATUS )
      CALL MSG_BLANK( STATUS )
      CALL PAR_CANCL( 'IN', STATUS )
      CALL CCD1_NGLIS( 'IN', 'ccdalign_ref.list', 100, .TRUE., NGNDF,
     :                 STATUS )
      IF ( STATUS .NE. SAI__OK ) GO TO 99
      HAVREF = ( NGNDF .GT. 0 )

*  Now display the reference image or the very first NDF.
c     IF ( HAVREF ) THEN
c        CALL CCD1_OPFIO( 'ccdalign_ref.list', 'READ', 'LIST', 0,
c    :                    FD, STATUS )
c     ELSE
c        CALL CCD1_OPFIO( 'ccdalign_ndf1.list', 'READ', 'LIST', 0,
c    :               FD, STATUS )
c     END IF
c     CALL FIO_READ( FD, REFNAM, REFLEN, STATUS )
c     CALL FIO_CLOSE( FD, STATUS )
c     CALL CCD1_OPLOG( STATUS )
c     CALL CCD1_MSG( ' ', ' ', STATUS )
c     CALL MSG_SETC( 'REFNDF', REFNAM( :REFLEN ) )
c     CALL CCD1_MSG( ' ', '  Using reference NDF ^REFNDF', STATUS )
c     CALL CCD1_MSG( ' ', ' ', STATUS )

*  Display this NDF.
c     CALL MSG_BLANK( STATUS )
c     CALL MSG_SETC( 'REFNDF', REFNAM( :REFLEN ) )
c     CALL MSG_OUT( ' ', '  Displaying NDF ^REFNDF', STATUS )
c     CALL MSG_BLANK( STATUS )
c     CMD = 'in='//REFNAM( :REFLEN )//' '//
c    :      'mode=percentiles '//
c    :      'percentiles='//PERC//' accept'
c     CALL SLV_OBEYW( KAPVIE, 'display', CMD, ' ', STATUS )
c     IF ( STATUS .NE. SAI__OK ) GO TO 99

*  Now use the cursor routine to read the image feature positions.
c     CALL MSG_BLANK( STATUS )
c     LINE = 'Use the cursor to mark the image features. Remember '//
c    :       'the order as this is important for later '//
c    :       'identifications.'
c     CALL CCD1_WRTPA( LINE, 72, 3, .FALSE., STATUS )
c     CALL MSG_BLANK( STATUS )

*  Activate the cursor routine. If this is the first NDF of the first group
*  then use all the NDF names of this group as the IN parameter. This will
*  associate this position list with all the NDFs.
c     IF ( HAVREF ) THEN
c        CMD = 'in='//REFNAM( :REFLEN)//' '//
c    :         'outlist='//REFNAM( :REFLEN)//'.fea accept reset'
c     ELSE
c        CMD = 'in=^ccdalign_ndf1.list'//' '//
c    :         'outlist='//REFNAM( :REFLEN)//'.fea accept reset'
c     END IF
c     CALL SLV_OBEYW( CCDREG, 'idicurs', CMD, ' ', STATUS )

*  Now plot the identifiers of the image features.
c     CMD = 'inlist='//REFNAM( :REFLEN)//'.fea '//
c    :      'mtype=-1 '//
c    :      'palnum=3 '//
c    :      'ndfnames=false accept'
c     CALL SLV_OBEYW( CCDREG, 'plotlist', CMD, ' ', STATUS )

*  See if user wants a hardcopy of the display.
c     CALL PAR_GET0L( 'HARDCOPY', HCOPY, STATUS )
c     IF ( STATUS .NE. SAI__OK ) GO TO 99

*  Do the hardcopy if asked.
c     IF ( HCOPY ) THEN
c        CALL MSG_BLANK( STATUS )
c        CALL MSG_OUT( ' ',
c    :        '  Give the name of a device that can be printed to.',
c    :                 STATUS )
c        CALL MSG_BLANK( STATUS )
c        CALL PAR_GET0C( 'HARDDEV', HDEV, STATUS )
c        IF ( STATUS .NE. SAI__OK ) GO TO 99

*  Get a snapshot.
c        CALL MSG_OUT( ' ',
c    :'  Capturing snapshot of display... Select portion of interest',
c    :                 STATUS )
c        INQUIRE( FILE='snapshot.ps', EXIST=EXISTS )
c        IF ( EXISTS ) THEN
c           CMD = 'rm snapshot.ps'
c           CALL CCD1_EXEC( CMD, STATUS )
c        END IF
c        CMD = 'odevice='//HDEV( :CHR_LEN(HDEV) )//';snapshot.ps '//
c    :         'whole=false '//
c    :         'negative=true accept'
c        CALL SLV_OBEYW( KAPVIE, 'snapshot', CMD, ' ', STATUS )

*  Need to print the output.
c2       CONTINUE
c        CALL PAR_GET0C( 'PRINTCMD', CMD, STATUS )
c        IF ( STATUS .NE. SAI__OK ) GO TO 99
c        IF ( CMD .EQ. ' ' ) THEN
c           CMD = 'lpr snapshot.ps'
c        END IF
c        IF ( INDEX( CMD, 'snapshot.ps' ) .EQ. 0 ) THEN 
c           CALL MSG_OUT( ' ', 
c    :'   You must give a command that will print the file snapshot.ps', 
c    :                    STATUS )
c           CALL PAR_CANCL( 'PRINTCMD', STATUS )
c           GO TO 2
c        END IF
c        CALL CCD1_EXEC( CMD, STATUS )
c     END IF

*  Get the names of the NDFs to be displayed (one from each group).
      DO 3 I = 1, NGROUP - 1
         CALL MSG_SETI( 'IGROUP', I ) 
         CALL MSG_LOAD( ' ', 'ccdalign_ndf^IGROUP.list', NAMLST,
     :                  OPLEN, STATUS )
         CALL CCD1_OPFIO( NAMLST, 'READ', 'LIST', 0, FD, STATUS )
         CALL FIO_READ( FD, NDFNMS( I ), NDFLEN, STATUS )
         CALL FIO_CLOSE( FD, STATUS )
 3    CONTINUE        

*  Get the name of the reference NDF if there is one.
      IF ( HAVREF ) THEN
         CALL CCD1_OPFIO( 'ccdalign_ref.list', 'READ', 'LIST', 0,
     :                    FD, STATUS )
         CALL FIO_READ( FD, REFNAM, REFLEN, STATUS )
         CALL FIO_CLOSE( FD, STATUS )
         CALL CCD1_MSG( ' ', ' ', STATUS )
         CALL MSG_SETC( 'REFNDF', REFNAM( :REFLEN ) )
         CALL CCD1_MSG( ' ', '  Using reference NDF ^REFNDF', STATUS )
         CALL CCD1_MSG( ' ', ' ', STATUS )
         NDFNMS( NGROUP ) = REFNAM
         REFPOS = NGROUP
         NNDF = NGROUP
      ELSE
         REFPOS = 1
         NNDF = NGROUP - 1
      END IF

*  Get values of display preferences from parameter system.
      CALL PAR_GET0D( 'ZOOM', ZOOM, STATUS )
      CALL PAR_GET0I( 'MAXCANV', MAXCNV, STATUS )
      CALL PAR_GET0I( 'WINX', WINDIM( 1 ), STATUS )
      CALL PAR_GET0I( 'WINY', WINDIM( 2 ), STATUS )

*  Allocate memory for coordinates of position lists.
      CALL CCD1_MALL( MAXPOS * NNDF, '_INTEGER', IPIND, STATUS )
      CALL CCD1_MALL( MAXPOS * NNDF, '_DOUBLE', IPXPOS, STATUS )
      CALL CCD1_MALL( MAXPOS * NNDF, '_DOUBLE', IPYPOS, STATUS )

*  Call the routine which will do the graphical user interaction to
*  obtain the position lists for each group of NDFs.
      CALL CCD1_ALGN( NDFNMS, NNDF, REFPOS, MAXPOS, PERCNT, ZOOM, 
     :                MAXCNV, WINDIM, NPOINT, %VAL( IPXPOS ),
     :                %VAL( IPYPOS ), %VAL( IPIND ), STATUS )

*  Write display preference parameters back to the parameter system.
      IF ( STATUS .NE. SAI__OK ) GO TO 99
      CALL PAR_PUT0D( 'ZOOM', ZOOM, STATUS )
      CALL PAR_PUT0I( 'MAXCANV', MAXCNV, STATUS )
      CALL PAR_PUT0I( 'WINX', WINDIM( 1 ), STATUS )
      CALL PAR_PUT0I( 'WINY', WINDIM( 2 ), STATUS )
      CALL PAR_PUT1D( 'PERCENTILES', PERCNT, 2, STATUS )

*  Start up monliths which are used later in this task.
*  CCDPACK_REG
      CALL PSX_GETENV( 'CCDPACK_DIR', CMD, STATUS )
      CMD = CMD( :CHR_LEN( CMD ) )//'/ccdpack_reg'
      CCDREG = 'ccdpack_reg'//PID
      CCDGID = SLV_LOADW( CCDREG, CMD, .TRUE., TIMOUT, STATUS )
      IF ( STATUS .NE. SAI__OK ) THEN
         STATUS = SAI__ERROR
         CALL ERR_REP( 'FAILED',
     :      'Sorry cannot proceed. Failed to load monolith '//
     :      '$CCDPACK_DIR/ccdpack_reg', STATUS )
         GO TO 99
      END IF

*  KAPVIEW_MON
c     CALL PSX_GETENV( 'KAPPA_DIR', CMD, STATUS )
c     CMD = CMD( :CHR_LEN( CMD ) )//'/kapview_mon'
c     KAPVIE = 'kapview_mon'//PID
c     KAPVID = SLV_LOADW( KAPVIE, CMD, .TRUE., TIMOUT, STATUS )
c     IF ( STATUS .NE. SAI__OK ) THEN
c        STATUS = SAI__ERROR
c        CALL ERR_REP( 'FAILED',
c    :'Sorry cannot proceed. Failed to load monolith '//
c    :'$KAPPA_DIR/kapview_mon', STATUS )
c        GO TO 99
c     END IF

*  Now write the position lists corresponding to the marked images, 
*  and associate the appropriate NDFs with those lists.
      DO I = 1, NNDF

*  Get the name of the position list file and of the file containing all
*  the NDFs in this group.
         IF ( HAVREF .AND. I .EQ. NNDF ) THEN
            CALL MSG_SETC( 'LISTID', 'ref' )
            CALL MSG_LOAD( ' ', '^LISTID', LISTID, LENG, STATUS )
         ELSE
            CALL MSG_SETI( 'LISTID', I )
            CALL MSG_LOAD( ' ', 'ndf^LISTID', LISTID, LENG, STATUS )
         END IF
         FNAME = 'ccdalign_' // LISTID( :LENG ) // '.fea'
         NAMLST = 'ccdalign_' // LISTID( :LENG ) // '.list'

*  Open the position list file.
         CALL CCD1_OPFIO( FNAME, 'WRITE', 'LIST', 0, FD, STATUS )

*  Write a header.
         CALL CCD1_FIOHD( FD, 'Output from CCDALIGN', STATUS )

*  Copy the data for this position list into workspace arrays so that it
*  can be accessed.
         CALL CCG1_COPSI( MAXPOS * ( I - 1 ) + 1, %VAL( IPIND ), 
     :                    NPOINT( I ), 1, INDEX, STATUS )
         CALL CCG1_COPSD( MAXPOS * ( I - 1 ) + 1, %VAL( IPXPOS ),
     :                    NPOINT( I ), 1, XPOS, STATUS )
         CALL CCG1_COPSD( MAXPOS * ( I - 1 ) + 1, %VAL( IPYPOS ),
     :                    NPOINT( I ), 1, YPOS, STATUS )

*  Write all the points in the position list to the file.
         CALL CCD1_WRIXY( FD, INDEX, XPOS, YPOS, NPOINT( I ), LINE, 
     :                    1024, STATUS )

*  Close the position list file.
         CALL FIO_CLOSE( FD, STATUS )

*  Associate the position list file with all the NDFs in this group.
         CMD = 'logto=n ' //
     :         'mode=alist ' //
     :         'in=^' // NAMLST( :CHR_LEN( NAMLST ) ) // ' ' //
     :         'inlist=' // FNAME( :CHR_LEN( FNAME ) ) // ' accept'
         CALL SLV_OBEYW( CCDREG, 'ccdedit', CMD, ' ', STATUS )
      END DO

*  Release memory used for position lists.
      CALL CCD1_MFREE( IPYPOS, STATUS )
      CALL CCD1_MFREE( IPXPOS, STATUS )
      CALL CCD1_MFREE( IPIND, STATUS )

*  Set the number of NDF groups we need to process.
c     IF ( HAVREF ) THEN 
c        GRPOFF = 1
c     ELSE
c        GRPOFF = 2
c     END IF

*  Introduction to next stage
c     CALL MSG_BLANK( STATUS )
c     LINE = 
c    :'Now the first member of each NDF group or each NDF will be '//
c    :'displayed. You will then be given the opportunity to use the '//
c    :'cursor to mark the image features which correspond to those '//
c    :'which you marked on the first (reference) NDF. The order in '//
c    :'which you identify the image features must be the same. If an '//
c    :'image feature does not exist mark a position off the frame. '//
c    :'You may extend the complete set of positions by indicating '//
c    :'image features after the last one in the reference set.'
c     CALL CCD1_WRTPA( LINE, 72, 3, .FALSE., STATUS )
c     CALL MSG_BLANK( STATUS )

*  Have used the first NDF group as reference set. Set start from next group.
c3    CONTINUE
c     IF ( GRPOFF .NE. NGROUP .AND. STATUS .EQ. SAI__OK ) THEN 

*  Extract the name of the first member of this group.
c        CALL MSG_SETI( 'NGROUP', GRPOFF ) 
c        CALL MSG_LOAD( ' ', 'ccdalign_ndf^NGROUP.list', NAMLST,
c    :                  OPLEN, STATUS )
c        CALL CCD1_OPFIO( NAMLST, 'READ', 'LIST', 0, FD, STATUS )
c        CALL FIO_READ( FD, NDFNAM, NDFLEN, STATUS )
c        CALL FIO_CLOSE( FD, STATUS )
         
*  Display this NDF. Note we need to reset the monolith parameters
*  as display centre & x/ymagn are not updated.
c        CALL MSG_SETC( 'NDFNAM', NDFNAM( :NDFLEN ) )
c        CALL CCD1_MSG( ' ',  '  Displaying NDF ^NDFNAM', STATUS )
c        CALL MSG_BLANK( STATUS )
c        CMD = 'in='//NDFNAM( :NDFLEN )//' '//
c    :         'mode=percentiles '//
c    :         'percentiles='//PERC//' accept'
c        CALL SLV_RESET( KAPVIE, STATUS )
c        CALL SLV_OBEYW( KAPVIE, 'display', CMD, ' ', STATUS )

*  Activate the cursor routine. Use all the NDF names of this group as the IN
*  parameter. This will associate this position list with all the NDFs.
c        CMD = 'outlist = '//NDFNAM( :NDFLEN )//'.fea '//
c    :         'in='//NDFNAM( :NDFLEN )//' accept reset'
c        CALL SLV_OBEYW( CCDREG, 'idicurs', CMD, ' ', STATUS )

*  Increment offset for next group.
c        GRPOFF = GRPOFF + 1
c        GO TO 3
c     END IF

*  Now centroid all the image feature positions to get accurate ones.
      CALL MSG_BLANK( STATUS )
      CALL MSG_OUT( ' ',  '  Centroiding the image feature positions.',
     :              STATUS )
      CALL MSG_BLANK( STATUS )
      GRPOFF = 1
 5    CONTINUE
      IF ( GRPOFF .NE. NGROUP .AND. STATUS .EQ. SAI__OK ) THEN 
         CALL MSG_SETI( 'NGROUP', GRPOFF ) 
         CALL MSG_LOAD( ' ', 'ccdalign_ndf^NGROUP.list', NAMLST,
     :                  OPLEN, STATUS )
         CMD = 'in=^'//NAMLST( :OPLEN )//' '//
     :         'ndfnames=true '//
     :         'outlist=*.acc '//
     :         'autoscale=true accept'
         CALL SLV_OBEYW( CCDREG, 'findcent', CMD, ' ', STATUS )
         GRPOFF = GRPOFF + 1
         GO TO 5
      END IF

*  See if the user wants to continue past this point.
      CALL MSG_BLANK( STATUS )
      LINE = 
     :'You may stop processing at this point if all you require are '//
     :'labelled position lists associated with NDFs. If you want to '//
     :'determine the NDF registrations, then this procedure will aid '//
     :'this but only for linear transformations.'
      CALL CCD1_WRTPA( LINE, 72, 3, .FALSE., STATUS )
      CALL MSG_BLANK( STATUS )
      CALL PAR_GET0L( 'CONTINUE', CONT, STATUS )
      IF ( .NOT. CONT .OR. STATUS .NE. SAI__OK ) GO TO 99

*  Will continue get the type of transformation to use.
      CALL MSG_BLANK( STATUS )
      CALL MSG_OUT( ' ', '  Ok will continue processing, which type'//
     :              ' of transformation do you require?', STATUS )
      CALL MSG_BLANK( STATUS )
      CALL MSG_OUT( ' ', '     1 = shift of origin only', STATUS )
      CALL MSG_OUT( ' ', '     2 = shift of origin and rotation',
     :              STATUS )
      CALL MSG_OUT( ' ', '     3 = shift of origin and magnification',
     :              STATUS )
      CALL MSG_OUT( ' ', '     4 = shift of origin rotation and'//
     :              ' magnification', STATUS )
      CALL MSG_OUT( ' ', '     5 = full six parameter fit', STATUS )
      CALL MSG_BLANK( STATUS )
      CALL PAR_GET0I( 'FITTYPE', FITTYP, STATUS )

*  Need a list of all the input NDFs. To get this we create a new file
*  ccdalign_ndfs.list and copy the contents of all the groups files
*  into it.
      CALL CCD1_OPFIO( 'ccdalign_ndfs.list', 'WRITE', 'LIST', 0, FDOUT, 
     :                 STATUS )
      DO 6 I = 1, NGROUP - 1
         CALL MSG_SETI( 'I', I ) 
         CALL MSG_LOAD( ' ', 'ccdalign_ndf^I.list', NAMLST,
     :                  OPLEN, STATUS )
         CALL CCD1_OPFIO( NAMLST, 'READ', 'LIST', 0, FD, STATUS )
         CALL ERR_MARK

*  While we can read this file, copy its contents into the total list.
 7       CONTINUE
         IF ( STATUS .EQ. SAI__OK ) THEN 
            CALL FIO_READ( FD, NDFNAM, NDFLEN, STATUS )
            CALL FIO_WRITE( FDOUT, NDFNAM, STATUS )
            GO TO 7
         END IF
         IF ( STATUS .EQ. FIO__EOF ) THEN 
            CALL ERR_ANNUL( STATUS )
         END IF
         CALL ERR_RLSE
         CALL FIO_CLOSE( FD, STATUS )
 6    CONTINUE
      CALL FIO_CLOSE( FDOUT, STATUS )

*  Find the initial transformations, saving the extended reference set
*  to look for new positions on frames.
      CALL MSG_BLANK( STATUS )
      CALL MSG_OUT( ' ',  '   Determining initial transformations.',
     :              STATUS )
      CALL MSG_BLANK( STATUS )
      CALL MSG_SETI( 'FITTYP', FITTYP ) 
      CALL MSG_LOAD( ' ', 'fittype=^FITTYP ', CMD, OPLEN, STATUS )
      CMD( OPLEN + 1: ) = ' ndfnames=true '//
     :      'inlist=^ccdalign_ndfs.list '//
     :      'refpos=1 '//
     :      'outref=ccdalign_ref.ext accept'
      CALL SLV_OBEYW( CCDREG, 'register', CMD, ' ', STATUS )

*  Now need to transform all the extended reference set to the coordinates of
*  all the other NDFs before re-centroiding to get accurate positions for any
*  image features beyond the initial reference set.
*  Associate extended reference set with all NDFs
      CMD = 'logto=n '//
     :      'mode=alist '//
     :      'in=^ccdalign_ndfs.list '//
     :      'inlist=ccdalign_ref.ext accept'
      CALL SLV_OBEYW( CCDREG, 'ccdedit', CMD, ' ', STATUS )
      CMD = 'logto=n '//
     :      'ndfnames=true '//
     :      'inlist=^ccdalign_ndfs.list '//
     :      'outlist=*.ext '//
     :      'inext=true '//
     :      'forward=false '//
     :      'trtype=wcs '//
     :      'framein=ccd_reg '//
     :      'frameout=pixel '//
     :      'accept'
      CALL SLV_OBEYW( CCDREG, 'tranlist', CMD, ' ', STATUS )
      CMD = 'logto=n '//
     :      'ndfnames=true '//
     :      'outlist=*.acc '//
     :      'in=^ccdalign_ndfs.list accept'
      CALL SLV_OBEYW( CCDREG, 'findcent', CMD, ' ', STATUS )

*  Now rework the transformations
      CALL MSG_BLANK( STATUS )
      CALL MSG_OUT( ' ',  '   Determining final transformations.',
     :              STATUS )
      CALL MSG_BLANK( STATUS )
      CALL MSG_SETI( 'FITTYP', FITTYP ) 
      CALL MSG_LOAD( ' ', 'fittype=^FITTYP ', CMD, OPLEN, STATUS )
      CMD( OPLEN + 1: ) = ' ndfnames=true '//
     :      'inlist=^ccdalign_ndfs.list '//
     :      'refpos=1 '//
     :      'outref=ccdalign_ref.ext '//
     :      'logto=both accept reset'
      CALL SLV_OBEYW( CCDREG, 'register', CMD, ' ', STATUS )

*  If an error occurred, then report a contextual message.
 99   CONTINUE

*  Free any memory which somehow has not been freed already.
      CALL CCD1_MFREE( -1, STATUS )

*  Stop any detached processes.
      IF ( CCDRID .GT. 0 ) CALL SLV_KILLW( CCDRID, STATUS )
      IF ( CCDGID .GT. 0 ) CALL SLV_KILLW( CCDGID, STATUS )
c     IF ( KAPVID .GT. 0 ) CALL SLV_KILLW( KAPVID, STATUS )
      IF ( STATUS .NE. SAI__OK ) THEN
          CALL ERR_REP( 'CCDALIGN_ERR',
     :                  'CCDALIGN: failed to align CCD frames.',
     :                  STATUS )
      END IF
      END
* $Id$
