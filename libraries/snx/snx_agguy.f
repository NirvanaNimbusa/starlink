
      REAL FUNCTION snx_AGGUY (GY)

*+
*
*  - - - - - -
*   A G G U Y
*  - - - - - -
*
*  Convert grid Y-coordinate GY into user Y-coordinate.
*
*  Grid coordinates run (0.0,0.0)-(1.0,1.0) within
*  the grid window;  user coordinates are as originally
*  supplied to be plotted.
*
*  From COMMON:
*     LL from /IUTLCM/
*     YBUW,YTUW from /AGCONP/
*
*  P T Wallace   Starlink   April 1986
*  P C T Rees    Starlink   April 1992
*     Added trap to avoid logarithms of zero or negative numbers.
*
*+

      REAL GY

      REAL YB,Y

*  AUTOGRAPH variables
      INTEGER LL,MI,MX,MY,IU(96)
      COMMON /IUTLCM/ LL,MI,MX,MY,IU
      REAL QFRA,QSET,QROW,QIXY,QWND,QBAC,SVAL(2),
     :     XLGF,XRGF,YBGF,YTGF,XLGD,XRGD,YBGD,YTGD,SOGD,
     :     XMIN,XMAX,QLUX,QOVX,QCEX,XLOW,XHGH,
     :     YMIN,YMAX,QLUY,QOVY,QCEY,YLOW,YHGH,
     :     QDAX(4),QSPA(4),PING(4),PINU(4),FUNS(4),QBTD(4),
     :     BASD(4),QMJD(4),QJDP(4),WMJL(4),WMJR(4),QMND(4),
     :     QNDP(4),WMNL(4),WMNR(4),QLTD(4),QLED(4),QLFD(4),
     :     QLOF(4),QLOS(4),DNLA(4),WCLM(4),WCLE(4),
     :     QODP,QCDP,WOCD,WODQ,QDSH(26),
     :     QDLB,QBIM,FLLB(10,8),QBAN,
     :     QLLN,TCLN,QNIM,FLLN(6,16),QNAN,
     :     XLGW,XRGW,YBGW,YTGW,XLUW,XRUW,YBUW,YTUW,
     :     XLCW,XRCW,YBCW,YTCW,WCWP,HCWP,SCWP,
     :     XBGA(4),YBGA(4),UBGA(4),XNDA(4),YNDA(4),UNDA(4),
     :     QBTP(4),BASE(4),QMNT(4),QLTP(4),QLEX(4),QLFL(4),
     :     QCIM(4),QCIE(4),RFNL(4),WNLL(4),WNLR(4),WNLB(4),
     :     WNLE(4),QLUA(4),
     :     RBOX(6),DBOX(6,4),SBOX(6,4)
      COMMON /AGCONP/ QFRA,QSET,QROW,QIXY,QWND,QBAC,SVAL,
     :                XLGF,XRGF,YBGF,YTGF,XLGD,XRGD,YBGD,YTGD,SOGD,
     :                XMIN,XMAX,QLUX,QOVX,QCEX,XLOW,XHGH,
     :                YMIN,YMAX,QLUY,QOVY,QCEY,YLOW,YHGH,
     :                QDAX,QSPA,PING,PINU,FUNS,QBTD,
     :                BASD,QMJD,QJDP,WMJL,WMJR,QMND,
     :                QNDP,WMNL,WMNR,QLTD,QLED,QLFD,
     :                QLOF,QLOS,DNLA,WCLM,WCLE,
     :                QODP,QCDP,WOCD,WODQ,QDSH,
     :                     QDLB,QBIM,FLLB,QBAN,
     :                QLLN,TCLN,QNIM,FLLN,QNAN,
     :                XLGW,XRGW,YBGW,YTGW,XLUW,XRUW,YBUW,YTUW,
     :                XLCW,XRCW,YBCW,YTCW,WCWP,HCWP,SCWP,
     :                XBGA,YBGA,UBGA,XNDA,YNDA,UNDA,
     :                QBTP,BASE,QMNT,QLTP,QLEX,QLFL,
     :                QCIM,QCIE,RFNL,WNLL,WNLR,WNLB,
     :                WNLE,QLUA,
     :                RBOX,DBOX,SBOX



      IF (LL.EQ.2.OR.LL.EQ.4) THEN

*     Check validity of YBUW and YTUW.
         IF (YBUW.LE.0.0.OR.YTUW.LE.0.0) THEN

*        The value of either YBUW or YTUW is less than or equal to zero,
*        so abort.
            CALL SETER('SNX_AGGUY: UNABLE TO CONVERT GRID COORDINATES',
     :                 105,2)
            GO TO 999
         END IF

         YB=ALOG10(YBUW)
         Y=10**(YB+GY*(ALOG10(YTUW)-YB))
      ELSE
         YB=YBUW
         Y=YB+GY*(YTUW-YB)
      END IF

      snx_AGGUY=Y

 999  CONTINUE

      END
