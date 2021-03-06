*+  IEXCLUDE - removes pieces from image by setting QUALITY to IGNORE
      SUBROUTINE IEXCLUDE(STATUS)
*    Description :
*    Deficiencies :
*    Bugs :
*    Authors :
*     BHVAD::RJV
*    History :
*      28 Jan 93: V1.7-0 amalgamation of IZAP, IREMOVE, IPOLYFILL (RJV)
*       1 Jul 93: V1.7-1 GTR used (RJV)
*      25 Aug 94: V1.7-2 REGION option added (RJV)
*      16 Sep 94: V1.7-3 data min/max updated (RJV)
*      30 Sep 94: V1.7-4 slight rationalisation (RJV)
*       5 Jan 95: V1.8-0 new ARD (RJV)
*      12 Jan 95: V1.8-1 option to use current region (RJV)
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Status :
      INTEGER STATUS
*    Function declarations :
*    Local constants :
*    Local variables :
      CHARACTER*10 MODE
*    Version :
      CHARACTER*30 VERSION
      PARAMETER (VERSION = 'IEXCLUDE Version 2.2-0')
*-
      CALL USI_INIT()

      CALL MSG_PRNT(VERSION)

      IF (.NOT.I_OPEN) THEN
        CALL MSG_PRNT('AST_ERR: image processing system not active')
      ELSEIF (.NOT.I_DISP) THEN
        CALL MSG_PRNT('AST_ERR: no image being displayed')
      ELSE

*  ensure transformations are correct
        CALL GTR_RESTORE(STATUS)

        CALL USI_GET0C('MODE',MODE,STATUS)
        CALL CHR_UCASE(MODE)
        MODE=MODE(:3)

*  copy data to work area and create quality if necessary
        CALL IMG_COPY(.FALSE.,.TRUE.,STATUS)
        I_CAN_UNDO=.FALSE.
        IF (I_GUI) THEN
          CALL IMG_NBPUT0I('BUFFER',0,STATUS)
        ENDIF

        IF (MODE.EQ.'SOU') THEN
          CALL IEXCLUDE_SOURCE(STATUS)
        ELSEIF (MODE.EQ.'PIX') THEN
          CALL IEXCLUDE_PIXEL(STATUS)
        ELSEIF (MODE.EQ.'CIR') THEN
          CALL IEXCLUDE_CIRCLE(STATUS)
        ELSEIF (MODE.EQ.'POL') THEN
          CALL IEXCLUDE_POLY(STATUS)
        ELSEIF (MODE.EQ.'REG') THEN
          CALL IEXCLUDE_REGION(STATUS)
        ELSE
          CALL MSG_PRNT('AST_ERR: unknown mode')
          STATUS=SAI__ERROR
        ENDIF

        CALL IMG_SWAP(STATUS)
        CALL IMG_MINMAX(STATUS)

        IF (STATUS.EQ.SAI__OK) THEN
          I_CAN_UNDO=.TRUE.
          IF (I_GUI) THEN
            CALL IMG_NBPUT0I('BUFFER',1,STATUS)
          ENDIF
          I_PROC_COUNT=I_PROC_COUNT+1
          I_LAST_CMD='IEXCLUDE'
        ENDIF

      ENDIF

      CALL AST_ERR(STATUS)

      CALL USI_CLOSE()

      END


*+
      SUBROUTINE IEXCLUDE_SOURCE(STATUS)
*    Description :
*    Deficiencies :
*    Bugs :
*    Authors :
*     BHVAD::RJV
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'
      INCLUDE 'FIO_ERR'
      INCLUDE 'PAR_ERR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Status :
      INTEGER STATUS
*    Function declarations :
*    Local constants :
*    Local variables :
      CHARACTER*(DAT__SZLOC) RLOC
      CHARACTER*(DAT__SZLOC) DLOC
      CHARACTER*40 FILENAME
      CHARACTER*80 REC,RAS*20,DECS*20
      DOUBLE PRECISION RA,DEC
      REAL RAD
      REAL DVAL
      INTEGER NSRC
      INTEGER NPOS
      INTEGER RAPTR,DECPTR
      INTEGER LUN
      INTEGER SFID
      LOGICAL EXIST
      LOGICAL DAT
*-
      IF (STATUS.EQ.SAI__OK) THEN

*  get list of positions from file
        CALL USI_GET0C('LIST',FILENAME,STATUS)
*  get radius
        CALL USI_GET0R('RAD',RAD,STATUS)

*  are data to be altered
        CALL USI_GET0R('DVAL',DVAL,STATUS)
        IF (STATUS.EQ.PAR__NULL) THEN
          CALL ERR_ANNUL(STATUS)
          DAT=.FALSE.
        ELSE
          DAT=.TRUE.
        ENDIF

        IF (STATUS.EQ.SAI__OK) THEN
*  see if file exists in form given
          INQUIRE(FILE=FILENAME,EXIST=EXIST)
*  if it does - assume it to be text file
          IF (EXIST) THEN
            CALL FIO_OPEN(FILENAME,'READ','NONE',0,LUN,STATUS)
            DO WHILE (STATUS.EQ.SAI__OK)
              CALL FIO_READF(LUN,REC,STATUS)
              IF (STATUS.EQ.SAI__OK) THEN
*  ignore blank lines
                IF (REC.NE.' ') THEN
*  remove leading blanks
                  CALL CHR_LDBLK(REC)
*  split record into ra and dec
                  CALL CONV_SPLIT(REC,RAS,DECS,STATUS)
*  parse and plot
                  CALL CONV_RADEC(RAS,DECS,RA,DEC,STATUS)
                  CALL IEXCLUDE_SOURCE_REM(1,RA,DEC,RAD,DAT,DVAL,
     :                         %VAL(I_DPTR_W),%VAL(I_QPTR_W),STATUS)
                ENDIF
              ENDIF
            ENDDO
            IF (STATUS.EQ.FIO__EOF) THEN
              CALL ERR_ANNUL(STATUS)
            ENDIF
            CALL FIO_CLOSE(LUN,STATUS)
*  otherwise assume HDS file in PSS format
          ELSE
            CALL ADI_FOPEN( FILENAME, 'SSDSset|SSDS', 'READ', SFID,
     :              STATUS )

            IF (STATUS.EQ.SAI__OK) THEN

*  check if sources
              CALL ADI_CGET0I( SFID, 'NSRC', NSRC, STATUS )
              IF (NSRC.EQ.0 ) THEN
                CALL MSG_PRNT('AST_ERR: No sources in this SSDS')
              ELSE
*  get RA DEC of sources
                CALL SSI_MAPFLD( SFID, 'RA', '_DOUBLE', 'READ',
     :                                        RAPTR, STATUS )
                CALL SSI_MAPFLD( SFID, 'DEC', '_DOUBLE', 'READ',
     :                                        DECPTR, STATUS )
                CALL IEXCLUDE_SOURCE_REM(NSRC,%VAL(RAPTR),%VAL(DECPTR),
     :                                     RAD,DAT,DVAL,%VAL(I_DPTR_W),
     :                                           %VAL(I_QPTR_W),STATUS)

              ENDIF

              CALL SSI_RELEASE(SFID,STATUS)
              CALL ADI_FCLOSE(SFID,STATUS)

            ENDIF

          ENDIF

*  look for individual HDS arrays
        ELSEIF (STATUS.EQ.PAR__NULL) THEN
          CALL ERR_ANNUL(STATUS)
          CALL USI_DASSOC('RA','READ',RLOC,STATUS)
          CALL USI_DASSOC('DEC','READ',DLOC,STATUS)
*  get radius
          CALL USI_GET0R('RAD',RAD,STATUS)
          CALL DAT_SIZE(RLOC,NPOS,STATUS)
          CALL DYN_MAPD(1,NPOS,RAPTR,STATUS)
          CALL DYN_MAPD(1,NPOS,DECPTR,STATUS)
          CALL DAT_GET1D(RLOC,NPOS,%VAL(RAPTR),NPOS,STATUS)
          CALL DAT_GET1D(DLOC,NPOS,%VAL(DECPTR),NPOS,STATUS)
          CALL IEXCLUDE_SOURCE_REM(NPOS,%VAL(RAPTR),%VAL(DECPTR),RAD,
     :                  DAT,DVAL,%VAL(I_DPTR_W),%VAL(I_QPTR_W),STATUS)
          CALL DYN_UNMAP(RAPTR,STATUS)
          CALL DYN_UNMAP(DECPTR,STATUS)
          CALL DAT_ANNUL(RLOC,STATUS)
          CALL DAT_ANNUL(DLOC,STATUS)
        ENDIF

      ENDIF

      END


*+
      SUBROUTINE IEXCLUDE_SOURCE_REM(NSRC,RA,DEC,RAD,DAT,DVAL,D,Q,
     :                                                     STATUS)
*    Description :
*    Method :
*    Deficiencies :
*    Authors :
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'QUAL_PAR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Import :
      INTEGER NSRC
      DOUBLE PRECISION RA(NSRC),DEC(NSRC)
      REAL RAD
      LOGICAL DAT
      REAL DVAL
      REAL D(I_NX,I_NY)
      BYTE Q(I_NX,I_NY)
*    Import-Export :
*    Export :
*    Status :
      INTEGER STATUS
*    Function declarations :
*    Local constants :
*    Local variables :
      REAL X,Y,XP,YP
      INTEGER ISRC
      INTEGER IX,IY
      INTEGER IX1,IX2,IY1,IY2
      INTEGER IR,IRAD
      INTEGER I,J
*-
      IF (STATUS.EQ.SAI__OK) THEN

        DO ISRC=1,NSRC

*  get source centre
          CALL IMG_CELTOWORLD(RA(ISRC),DEC(ISRC),X,Y,STATUS)
          CALL IMG_WORLDTOPIX(X,Y,XP,YP,STATUS)
          IX=INT(XP+0.5)
          IY=INT(YP+0.5)

          IR=INT(ABS(RAD/I_XSCALE)+0.5)

*  check it falls within current region
          IF (IX.GE.I_IX1.AND.IX.LE.I_IX2.AND.
     :          IY.GE.I_IY1.AND.IY.LE.I_IY2) THEN
*  get extremes of source extent in pixels
            IX1=MAX(I_IX1,IX-IR)
            IX2=MIN(I_IX2,IX+IR)
            IY1=MAX(I_IY1,IY-IR)
            IY2=MIN(I_IY2,IY+IR)

            DO J=IY1,IY2
              DO I=IX1,IX2

*  check pixel falls within source circle
                IRAD=INT(SQRT(REAL(I-IX)**2 + REAL(J-IY)**2) +0.5)
                IF (IRAD.LE.IR) THEN

                  Q(I,J)=QUAL__IGNORE
                  I_BAD_W=.TRUE.
                  IF (DAT) THEN
                    D(I,J)=DVAL
                  ENDIF
                ENDIF

              ENDDO
            ENDDO

*  redisplay section of image
            I_PMIN=I_DMIN
            I_PMAX=I_DMAX
            CALL GFX_PIXELQ(I_WKPTR,I_NX,I_NY,IX1,IX2,IY1,IY2,.TRUE.,
     :                      %VAL(I_XPTR_W),%VAL(I_YPTR_W),0,0,
     :                        D,I_PMIN,I_PMAX,Q,I_MASK_W,STATUS)

          ENDIF

        ENDDO

      ENDIF

      END




*+
      SUBROUTINE IEXCLUDE_PIXEL(STATUS)
*    Description :
*    Deficiencies :
*    Bugs :
*    Authors :
*     BHVAD::RJV
*    History :
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'PAR_ERR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Status :
      INTEGER STATUS
*    Function declarations :
*    Local constants :
*    Local variables :
      REAL DVAL
      LOGICAL DAT
*-
      IF (STATUS.EQ.SAI__OK) THEN

*  are data to be altered
        CALL USI_GET0R('DVAL',DVAL,STATUS)
        IF (STATUS.EQ.PAR__NULL) THEN
          CALL ERR_ANNUL(STATUS)
          DAT=.FALSE.
        ELSE
          DAT=.TRUE.
        ENDIF

        CALL IEXCLUDE_PIXEL_REM(DAT,DVAL,%VAL(I_DPTR_W),%VAL(I_QPTR_W),
     :                                                          STATUS)


      ENDIF

      END


      SUBROUTINE IEXCLUDE_PIXEL_REM(DAT,DVAL,D,Q,STATUS)
*    Description :
*    Deficiencies :
*    Bugs :
*    Authors :
*     BHVAD::RJV
*    History :
*
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'QUAL_PAR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Import :
      LOGICAL DAT
      REAL DVAL
*    Import/export :
      REAL D(I_NX,I_NY)
      BYTE Q(I_NX,I_NY)
*    Status :
      INTEGER STATUS
*    Function declarations :
*    Local constants :
*    Local variables :
      CHARACTER*1 CH
      REAL X,Y
      REAL XP,YP
      INTEGER I,J
      LOGICAL LEFT,RIGHT
*-
      IF (STATUS.EQ.SAI__OK) THEN

        X=I_X
        Y=I_Y

        CALL MSG_PRNT(' ')
        CALL MSG_PRNT('Select pixels to remove - press X to eXit...')

        CH=CHAR(0)
        RIGHT=.FALSE.
        DO WHILE (CH.NE.'X'.AND..NOT.RIGHT.AND.STATUS.EQ.SAI__OK)

          CALL GFX_CURS(X,Y,LEFT,RIGHT,CH,STATUS)
          IF (CH.NE.'X'.AND..NOT.RIGHT.AND.STATUS.EQ.SAI__OK) THEN
            CALL IMG_WORLDTOPIX(X,Y,XP,YP,STATUS)
            I=INT(XP+0.5)
            J=INT(YP+0.5)
            Q(I,J)=QUAL__IGNORE
            I_BAD_W=.TRUE.
            IF (DAT) THEN
              D(I,J)=DVAL
            ENDIF

*  replot pixel
            I_PMIN=I_DMIN
            I_PMAX=I_DMAX
            CALL GFX_PIXELQ(I_WKPTR,I_NX,I_NY,I,I,J,J,.TRUE.,
     :                      %VAL(I_XPTR_W),%VAL(I_YPTR_W),0,0,
     :                        D,I_PMIN,I_PMAX,Q,I_MASK_W,STATUS)
          ENDIF

        ENDDO

      ENDIF

      END



*+
      SUBROUTINE IEXCLUDE_CIRCLE(STATUS)
*    Description :
*    Deficiencies :
*    Bugs :
*    Authors :
*     BHVAD::RJV
*    History :
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'PAR_ERR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Status :
      INTEGER STATUS
*    Function declarations :
*    Local constants :
*    Local variables :
      CHARACTER*1 CH
      REAL XC,YC,XR,YR,RAD
      LOGICAL DVAL
      LOGICAL OUTSIDE
      LOGICAL DAT
      LOGICAL RIGHT,LEFT
*-
      IF (STATUS.EQ.SAI__OK) THEN

*  are data to be altered
        CALL USI_GET0R('DVAL',DVAL,STATUS)
        IF (STATUS.EQ.PAR__NULL) THEN
          CALL ERR_ANNUL(STATUS)
          DAT=.FALSE.
        ELSE
          DAT=.TRUE.
        ENDIF

*  set QUALITY inside or outside circle
        CALL USI_GET0L('OUTSIDE',OUTSIDE,STATUS)

*  cursor mode
        IF (I_MODE.EQ.1) THEN
*  get centre
          XC=I_X
          YC=I_Y
          CALL MSG_PRNT(' ')
          CALL MSG_SETR('XC',XC)
          CALL MSG_SETR('YC',YC)
          CALL MSG_PRNT('Select centre/^XC,^YC/...')
          CALL GFX_CURS(XC,YC,LEFT,RIGHT,CH,STATUS)
          IF (CH.EQ.CHAR(13).OR.RIGHT) THEN
            XC=I_X
            YC=I_Y
          ENDIF
          CALL PGPOINT(1,XC,YC,2)

*  get radius
          CALL MSG_SETR('RAD',I_R)
          CALL MSG_PRNT('Select radius/^RAD/...')
          XR=XC
          YR=YC
          CALL GFX_CURS(XR,YR,LEFT,RIGHT,CH,STATUS)
          IF (CH.EQ.CHAR(13).OR.RIGHT) THEN
            RAD=I_R
            XR=XC+RAD
            YR=YC
          ELSE
            RAD=SQRT((XR-XC)**2 + (YR-YC)**2)
          ENDIF

*  keyboard mode
        ELSE
          CALL USI_DEF0R('XCENT',I_X,STATUS)
          CALL USI_GET0R('XCENT',XC,STATUS)
          CALL USI_DEF0R('YCENT',I_Y,STATUS)
          CALL USI_GET0R('YCENT',YC,STATUS)
          CALL USI_DEF0R('RAD',I_R,STATUS)
          CALL USI_GET0R('RAD',RAD,STATUS)
        ENDIF


*  plot circle
        CALL IMG_CIRCLE(XC,YC,RAD,STATUS)

        CALL IEXCLUDE_CIRCLE_REM(XC,YC,RAD,OUTSIDE,DAT,DVAL,
     :                      %VAL(I_DPTR_W),%VAL(I_QPTR_W),STATUS)

      ENDIF

      END


*+
      SUBROUTINE IEXCLUDE_CIRCLE_REM(X,Y,RAD,OUT,DAT,DVAL,D,Q,STATUS)
*    Description :
*    Method :
*    Deficiencies :
*    Authors :
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'QUAL_PAR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Import :
      REAL X,Y,RAD
      LOGICAL OUT
      LOGICAL DAT
      REAL DVAL
      REAL D(I_NX,I_NY)
      BYTE Q(I_NX,I_NY)
*    Import-Export :
*    Export :
*    Status :
      INTEGER STATUS
*    Function declarations :
      LOGICAL IMG_INCIRC
*    Local constants :
*    Local variables :
      INTEGER IX1,IX2,IY1,IY2
      INTEGER I,J
*-
      IF (STATUS.EQ.SAI__OK) THEN

*  get extremes of circle extent in pixels
        IF (OUT) THEN
          IX1=1
          IX2=I_NX
          IY1=1
          IY2=I_NY
        ELSE
          CALL IMG_CIRCTOBOX(X,Y,RAD,IX1,IX2,IY1,IY2,STATUS)
        ENDIF

        DO J=IY1,IY2
          DO I=IX1,IX2

*  check pixel falls within circle
            IF (IMG_INCIRC(I,J,X,Y,RAD)) THEN
              IF (.NOT.OUT) THEN
                Q(I,J)=QUAL__IGNORE
                I_BAD_W=.TRUE.
                IF (DAT) THEN
                  D(I,J)=DVAL
                ENDIF
              ENDIF
            ELSE
              IF (OUT) THEN
                Q(I,J)=QUAL__IGNORE
                I_BAD_W=.TRUE.
                IF (DAT) THEN
                  D(I,J)=DVAL
                ENDIF
              ENDIF
            ENDIF

          ENDDO
        ENDDO

*  redisplay section of image
        I_PMIN=I_DMIN
        I_PMAX=I_DMAX
        CALL GFX_PIXELQ(I_WKPTR,I_NX,I_NY,IX1,IX2,IY1,IY2,.TRUE.,
     :                      %VAL(I_XPTR_W),%VAL(I_YPTR_W),0,0,
     :                          D,I_PMIN,I_PMAX,Q,I_MASK_W,STATUS)

      ENDIF

      END





*+
      SUBROUTINE IEXCLUDE_POLY( STATUS )
*
*    Description :
*
*     The user selects polygonal areas on the current image using a cursor.
*     In ARD mode, the vertices of the polygon(s) are also written/appended
*     to an ARD file.
*
*    Method :
*
*
*
*    Deficiencies :
*
*     The GEO_POLYIN routine is not robust to highly convoluted polygons.
*
*    Bugs :
*    Authors :
*
*     David J. Allan (BHVAD::DJA)
*
*    History :
*
*     15 Mar 92 : V1.6-0  Original (DJA)
*     10 Sep 92 : V1.7-0  ARD file option added (DJA)
*     28 Jan 93 : incorporated into IEXCLUDE (RJV)
*      5 Jan 95 : new ARD (RJV)
*    Type Definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
      INCLUDE 'PAR_ERR'
      INCLUDE 'QUAL_PAR'
*
*    Status :
*
      INTEGER STATUS
*
*    Global Variables :
*
      INCLUDE 'IMG_CMN'
*
*    Functions :
*
      INTEGER        CHR_LEN
      LOGICAL IMG_INPOLY
*
*    Local Constants :
*
      INTEGER        MAXVERT                        ! Max no. of vertices
        PARAMETER    ( MAXVERT = 100 )
      INTEGER        OPEN_BOX                       ! Vertex symbol
        PARAMETER    ( OPEN_BOX = 6 )
*
*    Local variables :
*
      CHARACTER*132       ARDFILE                    ! ARD file name
      CHARACTER*132       TEXT                       ! O/p line for ARD file

      REAL                DVAL                       ! Data value to set
      REAL                XVERT(MAXVERT)             ! World X coord of vertices
      REAL                YVERT(MAXVERT)             ! World Y coord of vertices

      INTEGER             FID                        ! ARD FIO descriptor
      INTEGER             I,J
      INTEGER             I1,I2,J1,J2
      INTEGER             IV                         ! Loop over vertices
      INTEGER             NVERTEX                    ! # of vertices
      INTEGER             TLEN                       ! Length of TEXT?
      INTEGER             GRPID                      ! group id

      LOGICAL             FLAG
      LOGICAL             DAT                        ! change data
      LOGICAL		  THERE			     ! ARD already exists
      LOGICAL             APPEND                     ! Append to ARD file?
      LOGICAL             ARD                        ! Working in ARD file mode?
      LOGICAL             FIRST                      ! First row to be done yet?
      LOGICAL             LOOP                       ! Looping?
      LOGICAL             OUTSIDE                    ! Invert selection?
      LOGICAL             DOIT
*
*-
      IF (STATUS.EQ.SAI__OK) THEN

*      Initialise
        LOOP = .FALSE.
        FIRST = .TRUE.
        ARD = .FALSE.
        DAT=.FALSE.

*  are data to be altered
        CALL USI_GET0R('DVAL',DVAL,STATUS)
        IF (STATUS.EQ.PAR__NULL) THEN
          CALL ERR_ANNUL(STATUS)
          DAT=.FALSE.
        ELSE
          DAT=.TRUE.
        ENDIF

*      ARD file mode
        CALL USI_GET0L('ARD',ARD,STATUS)
        IF (ARD) THEN

*        Get file name
          CALL USI_GET0C('FILE',ARDFILE,STATUS)
          INQUIRE(FILE=ARDFILE,EXIST=THERE)

          IF (THERE) THEN

*          Open for APPEND access?
            CALL USI_GET0L( 'APPEND', APPEND, STATUS )

            IF (APPEND) THEN
              CALL FIO_OPEN(ARDFILE,'APPEND','LIST',0,FID,STATUS)
            ELSE
              CALL FIO_OPEN(ARDFILE,'UPDATE','LIST',0,FID,STATUS)
            ENDIF

          ELSE
            CALL FIO_OPEN(ARDFILE,'WRITE','LIST',0,FID,STATUS)

          ENDIF


*        Open group for storing AD text
          CALL ARX_OPEN('WRITE',GRPID,STATUS)

        ENDIF

*      Looping?
        CALL USI_GET0L( 'LOOP', LOOP, STATUS )

*      Work inside or outside polygon
        IF ( LOOP ) THEN
          OUTSIDE = .FALSE.
        ELSE
          CALL USI_GET0L( 'OUTSIDE', OUTSIDE, STATUS )
        END IF

        IF ( STATUS .NE. SAI__OK ) GOTO 99


        DOIT=.TRUE.
        DO WHILE (DOIT)

*      Select vertices
          CALL IMG_GETPOLY(MAXVERT,XVERT,YVERT,NVERTEX,STATUS)

*      ARD file mode?
          IF ( ARD ) THEN

*        Create description
            IF ( OUTSIDE ) THEN
              TEXT = ' .NOT.(POLYGON('
            ELSE
              TEXT = ' POLYGON('
            ENDIF
            TLEN = CHR_LEN(TEXT)
            CALL ARX_PUT(GRPID,0,TEXT(:TLEN),STATUS)
            TEXT = ' '
            TLEN = 1

*        Write each vertex allowing for line continuation
            DO IV = 1, NVERTEX
              CALL MSG_SETR( 'X', XVERT(IV) )
              CALL MSG_SETR( 'Y', YVERT(IV) )
              CALL MSG_MAKE( TEXT(:TLEN)//' ^X , ^Y ,', TEXT, TLEN )
              IF ( (TLEN .GT. 65) .AND. (IV.LT.NVERTEX) ) THEN
                CALL ARX_PUT(GRPID,0,TEXT(:TLEN),STATUS)
                TEXT = ' '
                TLEN = 1
              ENDIF
            ENDDO

*        Write remaining polygon descriptor line
            IF (OUTSIDE) THEN
              TEXT(TLEN:)='))'
              TLEN=TLEN+1
            ELSE
              TEXT(TLEN:TLEN)=')'
            ENDIF
            CALL ARX_PUT(GRPID,0,TEXT(:TLEN),STATUS)

          ENDIF

*        Define bounding rectangle in pixels. If OUTSIDE mode is selected,
*        then this is the area of the whole image
          IF ( OUTSIDE ) THEN
            I1 = 1
            I2 = I_NX
            J1 = 1
            J2 = I_NY
          ELSE
            CALL IMG_POLYTOBOX(NVERTEX,XVERT,YVERT,I1,I2,J1,J2,STATUS)
          ENDIF

*  check if ecah pixel is within the polygon
          DO J=J1,J2
            DO I=I1,I2

              FLAG=IMG_INPOLY(I,J,NVERTEX,XVERT,YVERT)

              IF ((FLAG.AND..NOT.OUTSIDE).OR.
     :                    (OUTSIDE.AND..NOT.FLAG)) THEN


*            Change quality values
                CALL IEXCLUDE_POLY_SETQ( I,J,
     :                       QUAL__IGNORE, %VAL(I_QPTR_W), STATUS )

*            Change data values?
                IF (DAT) THEN
                  CALL IEXCLUDE_POLY_SETD( I, J,
     :                             DVAL, %VAL(I_DPTR_W), STATUS )
                ENDIF

              ENDIF

            ENDDO
          ENDDO

*            Replot
          I_PMIN=I_DMIN
          I_PMAX=I_DMAX
          CALL GFX_PIXELQ(I_WKPTR,I_NX,I_NY,I1,I2,J1,J2,
     :                     .TRUE.,%VAL(I_XPTR_W),%VAL(I_YPTR_W),0,0,
     :                              %VAL(I_DPTR_W),I_PMIN,I_PMAX,
     :                               %VAL(I_QPTR_W),I_MASK_W,STATUS)



*      Looping?
          IF ( LOOP .AND. (STATUS.EQ.SAI__OK ) ) THEN
            DOIT=.TRUE.
          ELSE
            DOIT=.FALSE.
          ENDIF

        ENDDO

*      Terminate ARD file processing if necessary
        IF ( ARD ) THEN


*        Write ARD text to file and close group
          CALL ARX_WRITEF(GRPID,FID,STATUS)

          CALL FIO_CLOSE(FID,STATUS)

          CALL ARX_CLOSE(GRPID,STATUS)

        END IF

      END IF

*    Abort point
 99   CONTINUE

      END



*+
      SUBROUTINE IEXCLUDE_POLY_SETQ( I,J,QVAL,QUAL, STATUS )
*    Description :
*    Method :
*    Deficiencies :
*    Bugs :
*    Authors :
*
*
*    History :
*
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
*
*    Global Variables :
*
      INCLUDE 'IMG_CMN'
*
*    Import :
*
      INTEGER I,J
      BYTE          QVAL                         ! Quality value to insert
*
*    Import / Export :
*
      BYTE          QUAL(I_NX,I_NY)              ! Work quality array
*
*    Status :
*
      INTEGER STATUS
*
*    Local variables :
*
*-

*    Check status
      IF ( STATUS .NE. SAI__OK ) RETURN

          QUAL(I,J) = QVAL

          I_BAD_W = .TRUE.

      END



*+
      SUBROUTINE IEXCLUDE_POLY_SETD( I,J,DVAL,DATA, STATUS )
*    Description :
*    Method :
*    Deficiencies :
*    Bugs :
*    Authors :
*
*
*    History :
*
*    Type definitions :
*
      IMPLICIT NONE
*
*    Global constants :
*
      INCLUDE 'SAE_PAR'
*
*    Global Variables :
*
      INCLUDE 'IMG_CMN'
*
*    Import :
*
      INTEGER  I,J
      REAL          DVAL                         ! Data value to insert
*
*    Import / Export :
*
      REAL          DATA(I_NX,I_NY)              ! Work data array
*
*    Status :
*
      INTEGER STATUS
*
*    Local variables :
*
*-

*    Check status
      IF ( STATUS .NE. SAI__OK ) RETURN

          DATA(I,J) = DVAL

      END







*+
      SUBROUTINE IEXCLUDE_REGION(STATUS)
*    Description :
*    Deficiencies :
*    Bugs :
*    Authors :
*     BHVAD::RJV
*    History :
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'PAR_ERR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Status :
      INTEGER STATUS
*    Function declarations :
*    Local constants :
*    Local variables :
      CHARACTER*80 UNITS(2)
      REAL BASE(2),SCALE(2)
      REAL DVAL
      INTEGER GRPID
      INTEGER DIMS(2)
      INTEGER IPTR
      LOGICAL DAT
      LOGICAL CURR
*-
      IF (STATUS.EQ.SAI__OK) THEN

        CALL USI_GET0L('CURR',CURR,STATUS)

*  are data to be altered
        CALL USI_GET0R('DVAL',DVAL,STATUS)
        IF (STATUS.EQ.PAR__NULL) THEN
          CALL ERR_ANNUL(STATUS)
          DAT=.FALSE.
        ELSE
          DAT=.TRUE.
        ENDIF

*  use current region
        IF (CURR) THEN
          GRPID=I_ARD_ID
        ELSE
*  or get spatial description (ARD) file
          CALL ARX_OPEN('READ',GRPID,STATUS)
          CALL ARX_READ('FILE',GRPID,STATUS)
        ENDIF


        DIMS(1)=I_NX
        DIMS(2)=I_NY
        CALL DYN_MAPI(2,DIMS,IPTR,STATUS)
        CALL ARR_INIT1I(0,I_NX*I_NY,%val(IPTR),STATUS)

        BASE(1)=I_XBASE_W
        BASE(2)=I_YBASE_W
        SCALE(1)=I_XSCALE_W
        SCALE(2)=I_YSCALE_W
        UNITS(1)=I_XYUNITS
        UNITS(2)=I_XYUNITS
        CALL ARX_MASK(GRPID,DIMS,BASE,SCALE,UNITS,%val(IPTR),
     :                                                 STATUS)
        CALL IEXCLUDE_REGION_REM(%val(IPTR),DAT,DVAL,
     :                      %VAL(I_DPTR_W),%VAL(I_QPTR_W),STATUS)

        CALL DYN_UNMAP(IPTR,STATUS)

        IF (.NOT.CURR) THEN
          CALL ARX_CLOSE(GRPID,STATUS)
        ENDIF

      ENDIF

      END


*+
      SUBROUTINE IEXCLUDE_REGION_REM(INC,DAT,DVAL,D,Q,STATUS)
*    Description :
*    Method :
*    Deficiencies :
*    Authors :
*    Type definitions :
      IMPLICIT NONE
*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'QUAL_PAR'
*    Global variables :
      INCLUDE 'IMG_CMN'
*    Import :
      INTEGER INC(I_NX,I_NY)
      LOGICAL DAT
      REAL DVAL
      REAL D(I_NX,I_NY)
      BYTE Q(I_NX,I_NY)
*    Import-Export :
*    Export :
*    Status :
      INTEGER STATUS
*    Function declarations :
      BYTE BIT_ORUB
*    Local constants :
*    Local variables :
      INTEGER I,J
      INTEGER I1,I2,J1,J2
*-
      IF (STATUS.EQ.SAI__OK) THEN


        I1=I_NX
        I2=1
        J1=I_NY
        J2=1
        DO J=1,I_NY
          DO I=1,I_NX

            IF (INC(I,J).GT.0) THEN
              Q(I,J)=BIT_ORUB(Q(I,J),QUAL__IGNORE)
              I_BAD_W=.TRUE.
              IF (DAT) THEN
                D(I,J)=DVAL
              ENDIF
              I1=MIN(I1,I)
              I2=MAX(I2,I)
              J1=MIN(J1,J)
              J2=MAX(J2,J)
            ENDIF

          ENDDO
        ENDDO

*  redisplay
        IF (I1.LE.I2) THEN
          I_PMIN=I_DMIN
          I_PMAX=I_DMAX
          CALL GFX_PIXELQ(I_WKPTR,I_NX,I_NY,I1,I2,J1,J2,.TRUE.,
     :                        %VAL(I_XPTR_W),%VAL(I_YPTR_W),0,0,
     :                          D,I_PMIN,I_PMAX,Q,I_MASK_W,STATUS)
        ENDIF

      ENDIF

      END
