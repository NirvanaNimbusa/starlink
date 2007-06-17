**==RINTEG.FOR
       FUNCTION RINTEG(X,Y,M,NDATA)
C   SIMPSON"S RULE VERSION OF RINTEG.
       INTEGER TWO
       DIMENSION X(M), Y(M)
       DATA THREE/3.0E0/, ONE/1.0E0/, TWO/2/, IZ/0/, HALF/0.5E0/, 
     :      FOUR/4.0E0/, ZERO/0.0E0/, IONE/1/, KTEST/0/
       KTEST = IZ
       H = ABS((X(2)-X(1))/THREE)
       IF (NDATA-((NDATA/TWO)*TWO).EQ.IZ) KTEST = TWO
       NPTS = NDATA - TWO
       IF (KTEST.NE.IZ) NPTS = NPTS - IONE
       RINTEG = ZERO
       DO 100 J = 1, NPTS, 2
          JJ = J + IONE
          JJJ = JJ + IONE
          RINTEG = RINTEG + (Y(J)+FOUR*Y(JJ)+Y(JJJ))
  100  CONTINUE
       RINTEG = RINTEG*H
       IF (KTEST.EQ.IZ) RETURN
       NURK = NDATA - IONE
       RINTEG = RINTEG + (Y(NURK)+Y(NDATA))*(ABS(X(NURK)-X(NDATA)))*HALF
       RETURN
       END
