*+REAL2BYTE	Converts REAL array to BYTE
	SUBROUTINE REAL2BYTE(NPTS, RDATA, BDATA)
	INTEGER NPTS		!input
	REAL RDATA(NPTS)	!input
	BYTE BDATA(NPTS)	!output
*-Author	Clive Page	1990-NOV-9
	INTEGER J
*
	DO J = 1,NPTS
	    BDATA(J) = NINT(RDATA(J))
	END DO
	END
