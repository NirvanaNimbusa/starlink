*+  USI_CLOSE - global tidy up for USI
      SUBROUTINE USI_CLOSE()
*    Description :
*    History :
*    Type definitions :
      IMPLICIT NONE
      INCLUDE  'SAE_PAR'
      INCLUDE  'ADI_PAR'
      INCLUDE  'DAT_PAR'
      INCLUDE  'USI_CMN'
*    Local constants :
      CHARACTER*1 BLANK
      PARAMETER (BLANK=' ')
*    Local variables :
      INTEGER N
      LOGICAL VALID
      INTEGER ISTAT
*-

      CALL USI0_EXIT()

      ISTAT=SAI__OK
      DO N=1,USI__NMAX
        IF (DS(N).USED) THEN
          IF ( DS(N).ADIFPN ) THEN
            CALL ADI_FCLOSE( DS(N).ADI_ID, ISTAT )

          ELSE IF ( DS(N).LOC .NE. DAT__NOLOC ) THEN
            CALL DAT_VALID(DS(N).LOC,VALID,ISTAT)
            IF (VALID) THEN
              CALL DAT_ANNUL(DS(N).LOC,ISTAT)
            ENDIF
          ENDIF

          DS(N).ADI_ID = ADI__NULLID
          DS(N).LOC=DAT__NOLOC
          DS(N).USED=.FALSE.
          DS(N).IO=BLANK

        ENDIF
      ENDDO
      USI_SYINIT = .FALSE.
      END
