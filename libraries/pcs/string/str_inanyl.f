*+  STRING_INANYL
      INTEGER FUNCTION STRING_INANYL ( STRING, CHOICE )
*    Description :
*     Finds the position of the first character in STRING which fails to 
*     match any of the characters in CHOICE.
*    Invocation :
*     POSITION = STRING_INANYL ( STRING, CHOICE )
*    Result :
*     POSITION = INTEGER
*           The value returned is the position in STRING at which the 
*           first character mis-match occurs.
*           If no mis-match found, then POSITION is set to zero.
*    Method :
*     Each character of STRING is compared with the characters in CHOICE 
*     until no match is found, or STRING is exhausted.
*    Deficiencies :
*     <description of any deficiencies>
*    Bugs :
*     <description of any "bugs" which have not been fixed>
*    Authors :
*     B.D.Kelly (REVAD::BDK)
*    History :
*     31.03.1984: original version (REVAD::BDK)
*    endhistory
*    Type Definitions :
      IMPLICIT NONE
*    Import :
      CHARACTER*(*) STRING,          ! character string to be searched
     :              CHOICE           ! set of matching characters
*    External references :
*     <declarations for external function references>
*    Local variables :
      INTEGER NUMSTRING,             ! number of characters in STRING
     :        NUMCHOICE,             ! number of characters in CHOICE
     :        POSITION,              ! current position in STRING
     :        MATCH                  ! position in CHOICE
      LOGICAL FOUND                  ! controller for search loop
*-

      NUMSTRING = LEN ( STRING )
      NUMCHOICE = LEN ( CHOICE )

      FOUND = .FALSE.
      POSITION = 1

      DO WHILE ( (.NOT.FOUND) .AND. (POSITION.LE.NUMSTRING) )
         MATCH = INDEX ( CHOICE, STRING(POSITION:POSITION) )
         IF ( MATCH .EQ. 0 ) THEN
            FOUND = .TRUE.
         ELSE
            POSITION = POSITION + 1
         ENDIF
      ENDDO

      IF ( FOUND ) THEN
         STRING_INANYL = POSITION
      ELSE
         STRING_INANYL = 0
      ENDIF

      END

