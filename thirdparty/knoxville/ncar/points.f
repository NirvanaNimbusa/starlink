      SUBROUTINE POINTS (PX,PY,NP,IC,IL)
      DIMENSION PX(NP),PY(NP)
C
C Marks the points at positions in the user coordinate system defined
C by ((PX(I),PY(I)),I=1,NP).  If IC is zero, each point is marked with
C a simple point.  If IC is positive, each point is marked with the
C single character defined by the FORTRAN-77 function CHAR(IC).  If IC
C is negative, each point is marked with a GKS polymarker of type -IC.
C If IL is non-zero, a curve is also drawn, connecting the points.
C
C Define arrays to hold converted point coordinates when it becomes
C necessary to mark the points a few at a time.
C
      DIMENSION QX(10),QY(10)
C
C Define an array to hold the aspect source flags which may need to be
C retrieved from GKS.
C
      DIMENSION LA(13)
C
C If the number of points is zero or negative, there's nothing to do.
C
      IF (NP.LE.0) RETURN
C
C Otherwise, flush the PLOTIT buffer.
C
      CALL PLOTIT (0,0,2)
C
C Retrieve the parameters from the last SET call.
C
      CALL GETSET (F1,F2,F3,F4,F5,F6,F7,F8,LL)
C
C If a linear-linear, non-mirror-imaged, mapping is being done and the
C GKS polymarkers can be used, all the points can be marked with a
C single polymarker call and joined, if requested, by a single polyline
C call.
C
      IF (F5.LT.F6.AND.F7.LT.F8.AND.LL.EQ.1.AND.IC.LE.0) THEN
        CALL GQASF (IE,LA)
        IF (LA(4).EQ.0) THEN
          CALL GQPMI (IE,IN)
          CALL GSPMI (MAX0(-IC,1))
          CALL GPM (NP,PX,PY)
          CALL GSPMI (IN)
        ELSE
          CALL GQMK (IE,IN)
          CALL GSMK (MAX0(-IC,1))
          CALL GPM (NP,PX,PY)
          CALL GSMK (IN)
        END IF
        IF (IL.NE.0.AND.NP.GE.2) CALL GPL (NP,PX,PY)
C
C Otherwise, things get complicated.  We have to do batches of nine
C points at a time.  (Actually, we convert ten coordinates at a time,
C so that the curve joining the points, if any, won't have gaps in it.)
C
      ELSE
C
C Initially, we have to reset either the polymarker index or the text
C alignment, depending on how we're marking the points.
C
        IF (IC.LE.0) THEN
          CALL GQASF (IE,LA)
          IF (LA(4).EQ.0) THEN
            CALL GQPMI (IE,IN)
            CALL GSPMI (MAX0(-IC,1))
          ELSE
            CALL GQMK (IE,IN)
            CALL GSMK (MAX0(-IC,1))
          END IF
        ELSE
          CALL GQTXAL (IE,IH,IV)
          CALL GSTXAL (2,3)
        END IF
C
C Loop through the points by nines.
C
        DO 104 IP=1,NP,9
C
C Fill the little point coordinate arrays with up to ten values,
C converting them from the user system to the fractional system.
C
          NQ=MIN0(10,NP-IP+1)
          MQ=MIN0(9,NQ)
          DO 102 IQ=1,NQ
            QX(IQ)=CUFX(PX(IP+IQ-1))
            QY(IQ)=CUFY(PY(IP+IQ-1))
  102     CONTINUE
C
C Change the SET call to allow the use of fractional coordinates.
C
          CALL SET (F1,F2,F3,F4,F1,F2,F3,F4,1)
C
C Crank out either a polymarker or a set of characters.
C
          IF (IC.LE.0) THEN
            CALL GPM (MQ,QX,QY)
          ELSE
            DO 103 IQ=1,MQ
              CALL GTX (QX(IQ),QY(IQ),CHAR(IC))
  103       CONTINUE
          END IF
          IF (IL.NE.0.AND.NQ.GE.2) CALL GPL (NQ,QX,QY)
C
C Put the SET parameters back the way they were.
C
          CALL SET (F1,F2,F3,F4,F5,F6,F7,F8,LL)
C
  104   CONTINUE
C
C Finally, we put either the polymarker index or the text alignment
C back the way it was.
C
        IF (IC.LE.0) THEN
          IF (LA(4).EQ.0) THEN
            CALL GSPMI (IN)
          ELSE
            CALL GSMK (IN)
          END IF
        ELSE
          CALL GSTXAL (IH,IV)
        END IF
C
      END IF
C
C Update the pen position.
C
      CALL FRSTPT (PX(NP),PY(NP))
C
C Done.
C
      RETURN
C
      END
