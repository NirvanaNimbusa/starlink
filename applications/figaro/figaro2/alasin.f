C+
      SUBROUTINE ALASIN
C
C     A L A S I N
C
C     ALASIN reads a 'Computed Profile File' generated by ALAS
C     (Absorption Line Analysis Software) and creates an
C     equivalent FIGARO spectrum.
C        The ALAS 'Computed Profile' is an ASCII file with each line
C     containing a radial velocity and the corresponding residual
C     intensity value (ie. normalised by the continuum).  This is
C     converted to a standard Figaro spectrum, except that the X values
C     are velocities rather than wavelengths.
C
C     Command Parameters
C
C     ALASFILE    Name (including type) of the input ALAS format file
C     SPECTRUM    The output FIGARO format spectrum
C
C                                 J.G. Robertson  September 1985
C     Modified:
C
C     9th Oct 1985   KS / AAO.  Minor change to input sequence to remove
C                    possibility of infinite loop in batch mode.
C                    Test for irregularly spaced data added.
C     2nd Sep 1987   DJA/ AAO.  Revised DSA_ routines - some specs
C                    changed.  Now uses DYN routines for dynamic-memory
C                    handling. Added logical unit as parameter to
C                    TRANSFER subroutine.
C     28th Sep 1992  HME / UoE, Starlink.  INCLUDE changed, TABs
C                    removed. Lowercase structure definition SPECTRUM.
C     18th Jul 1996  MJCL / Starlink, UCL.  ALAS set to 132 chars.
C     24th Jul 1996  MJCL / Starlink, UCL.  Corrected abuse of STATUS
C                    (was being loaded with LOGICAL value).
C     29th Jul 1996  MJCL / Starlink, UCL.  PAR_ABORT checking.
C     1996 Nov 22nd  MJC / Starlink  Corrected end-of-file check.
C                    Removed unused variables.
C     2005 June 10   MJC / Starlink  Use CNF_PVAL for pointers to
C                    mapped data.
C+
      IMPLICIT NONE

      INCLUDE 'CNF_PAR'          ! For CNF_PVAL function
C
C     Functions
C
      LOGICAL FIG_SCRCHK
      LOGICAL PAR_ABORT          ! (F)PAR abort flag
      CHARACTER ICH_CI*5         ! Only needs to be 5 as maximum length
C                                ! of the output spectrum is 65536
C                                ! pixels
C
      INTEGER  EOF
      PARAMETER (EOF=(-1))
C
C     Local variables
C
      CHARACTER    ALAS*132      ! The name of the ALAS input file
      LOGICAL      ALOPEN        ! TRUE if the input was opened OK
      REAL         D             !
      DOUBLE PRECISION DUMMY     ! Dummy magnitude flag
      LOGICAL      FAULT         ! TRUE if an error occurred
      INTEGER      IGNORE        ! Used to pass ignorable status
      INTEGER      LU            ! Logical unit number of input file
      INTEGER      NX            ! Size of 1st dimension
      INTEGER      OPTR          ! Dynamic-memory pointer to output data
                                 ! array
      INTEGER      OSLOT         ! Map slot number for output data array
      INTEGER      STATUS        ! Running status for DSA_ routines
      CHARACTER    STRING*80     ! Output message text
      CHARACTER    STRINGS(2)*80 ! Unit and label information for output
      REAL         X             !
      REAL         X1            !
      REAL         X2            !
      INTEGER      XPTR          ! Dynamic-memory pointer to output
                                 ! axis array
      INTEGER      XSLOT         ! Map slot number for output axis array
C
C     Initialisation of DSA_ routines
C
      STATUS=0
      CALL DSA_OPEN(STATUS)
      IF (STATUS.NE.0) GO TO 500
C
C     Initial values
C
      LU=2
      ALOPEN=.FALSE.
      FAULT=.FALSE.
C
C     Get input file
C
      CALL PAR_RDCHAR('ALASFILE',' ',ALAS)
      IF ( PAR_ABORT() ) GO TO 500
      OPEN(FILE=ALAS,UNIT=LU,STATUS='OLD',IOSTAT=STATUS)
      IF (STATUS.NE.0) THEN
         CALL GEN_FORTERR(STATUS,.FALSE.,STRING)
         CALL PAR_WRUSER('Unable to open ALAS input file',IGNORE)
         CALL PAR_WRUSER(STRING,IGNORE)
         FAULT=.TRUE.
         GO TO 500
      END IF
      ALOPEN=.TRUE.
C
C     Find number of channels in input
C
      NX=-1
      DO WHILE (STATUS.EQ.0)
         READ(LU,*,IOSTAT=STATUS) X,D
         NX=NX+1
      END DO
      IF (STATUS.EQ.EOF) STATUS = 0
      IF (STATUS.NE.0) THEN
         CALL PAR_WRUSER('Error while reading input file',IGNORE)
         GOTO 500
      END IF
C
C     Create name of and create FIGARO spectrum
C
      CALL DSA_READ_STRUCT_DEF('spectrum',STATUS)
      CALL DSA_SET_STRUCT_VAR('NX',ICH_CI(NX),STATUS)
      CALL DSA_CREATE_STRUCTURE('SPECT','SPECTRUM','SPECTRUM',STATUS)
      IF (STATUS.NE.0) GOTO 500
C
C     Map output spectrum
C
      CALL DSA_MAP_DATA('SPECT','WRITE','FLOAT',OPTR,OSLOT,STATUS)
      IF (STATUS.NE.0) GOTO 500
C
      CALL DSA_MAP_AXIS_DATA('SPECT',1,'WRITE','FLOAT',XPTR,XSLOT,
     :                       STATUS)
      IF (STATUS.NE.0) GOTO 500
C
C     Read and transfer data.  Report first and last X values, and
C     total number of channels. Warn user about non-linear X scales.
C
      CALL TRANSFER(LU,%VAL(CNF_PVAL(XPTR)),%VAL(CNF_PVAL(OPTR)),
     :              NX,X1,X2)
      WRITE(STRING,6) NX
    6 FORMAT('Number of channels transferred =',I6)
      CALL PAR_WRUSER(STRING,IGNORE)
      WRITE(STRING,7) X1,X2
    7 FORMAT('First X value = ',F9.3,'  Last X value = ',F9.3)
      CALL PAR_WRUSER(STRING,IGNORE)
      IF (.NOT.FIG_SCRCHK(NX,%VAL(CNF_PVAL(XPTR)))) THEN
         CALL PAR_WRUSER(
     :               '*** WARNING *** ALAS output file does not have'
     :                //' channels spaced exactly equally.',IGNORE)
         CALL PAR_WRUSER('Check whether a rescrunch is needed.',IGNORE)
      END IF
C
C     Set units and labels
C
      STRINGS(1)='km/s'
      STRINGS(2)='Velocity'
      CALL DSA_SET_AXIS_INFO('SPECT',1,2,STRINGS,0,DUMMY,STATUS)
      IF (STATUS.NE.0) GOTO 500
      STRINGS(1)=' '
      STRINGS(2)='Residual intensity'
      CALL DSA_SET_DATA_INFO('SPECT',2,STRINGS,0,DUMMY,STATUS)
C
C     Terminate
C
  500 CONTINUE
C
      CALL DSA_CLOSE(STATUS)
C
      IF (ALOPEN) THEN
         CLOSE(UNIT=LU,IOSTAT=STATUS)
      END IF
C
      IF (FAULT) CALL FIG_SETERR
C
      END

      SUBROUTINE TRANSFER(LU,XVALS,DVALS,NX,X1,X2)
C
C     Read ALAS data from ASCII file (unit LU) and write to (data
C     mapped) arrays
C
      IMPLICIT NONE
      INTEGER NX,N,LU
      REAL XVALS(NX), DVALS(NX), X, D, X1, X2
C
      REWIND LU
      N=0
    1 READ(LU,*,END=2)X,D
      N=N+1
      XVALS(N)=X
      DVALS(N)=D
      GO TO 1
C
    2 IF (N.NE.NX) STOP ' Unexpected error #1 . S/R TRANSFER'
      X1=XVALS(1)
      X2=XVALS(NX)
      END
