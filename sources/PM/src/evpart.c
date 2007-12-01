/**********************************************************************
*
*    evpart.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    short evpart()    evaluate PART 
*    
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"
#include "../include/indef.h"

/*
***standard types
*/
extern pm_ptr  stintp;
extern pm_ptr  stflop;
extern pm_ptr  stvecp;
extern pm_ptr  strefp;
extern pm_ptr  stfilp;
extern pm_ptr  ststrp;

extern pm_ptr  inttyla;
extern pm_ptr  flotyla;
extern pm_ptr  strtyla;
extern pm_ptr  vectyla;
extern pm_ptr  reftyla;

extern STTYTBL inttype;
extern STTYTBL flotype;
extern STTYTBL stype;
extern STTYTBL vectype;
extern STTYTBL reftype;

extern short modtyp;

/*!******************************************************/

       short evpart(
       V2REFVA *ident,
       PMPAST  *partp)

/*      Evaluate geometric procedure PART.
 *
 *      In:   *ident  =>    Geometric id
 *            *partp  =>    C-pointer to part statement node
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1986-10-22  �ndring av module-bas-adress hantering i evpart(), P-O A
 *      1986-12-26  Flyttat rutiner till pmmic, J. Kjellander
 *      1987-09-07  odtyp, J. Kjellander
 *      1993-08-15  uttryck i partnamn, J. Kjellander
 *      1996-03-02  Felhantering RTS, J. Kjellander
 *      1999-11-15  Rewritten, R. Svedin
 *      2001-05-02  Fixed inponp(void), J.Kjellander
 *      2007-04-22 Recursion test, J.Kjellander
 *
 ******************************************************!*/

  {
   pm_ptr exlist;   
   pm_ptr ppalist;          /* PM-pointer to part parameter list */
   pm_ptr nextlist;         /* PM-pointer to next list node */
   pm_ptr actlist;          /* PM-pointer to list node for actual parameter */ 
   pm_ptr actnext;          /* PM-pointer to next list node       - " -      */
   pm_ptr formlist;         /* PM-pointer to list node for formal parameter */ 
   pm_ptr formnext;         /* PM-pointer to next list node      - " -      */
   pm_ptr partbase;         /* PM base address for parted module */
   pm_ptr currbase;         /* PM base for current module */

   char  *modname;          /* C-pekare till modulnamn */
   char prtmna[V2SYNLEN+1]; /* name for parted module */
   char curmna[V2SYNLEN+1]; /* name for current module */
   bool actflg;             /* Flag to indicate if current module is active module */

   pm_ptr parala;           /* PM-pointer to module parameter node */
   pm_ptr exprla;           /* PM-pointer to expression node */
   PMMONO *np;              /* c-pointer to module node */
   char *currtsp;           /* RTS base pointer for current module */
   char *partrtsp;          /* RTS base pointer for parted module */
   DBint frsize;            /* size of new frame in RTS, parameter to inevpa() */
   PMMODULE moddata;        /* module data structure */

   short status,evstat;

   V2REFVA *refp;           /* pointer to optional parameter ref */

   PMLITVA partpval;        /* part parameter value */
   PMLITVA val;             /* parameter value structure */
   pm_ptr  tyla;            /* and it's type pointer */
   STTYTBL type;            /* type info struct */

   PMPANO *formpnp;         /* formal parameter node pointer */
   STVAR var;               /*    - " -         symbol info */

   int   errlev;            /* error level */
   char  errbuf[2*(V2SYNLEN+1)+15];         /* Insertstr�ng buffert */

   V2NAPA partnpbl;             /* Named parameter block for default in parted module */
                                                                   /* POA 850521 */
   DBPart prtrec;                /* GM record for Part */              /* POA 850514 */
   DBPdat prtdat;               /* GM record for Part */              /* POA 860310 */

   PMPATLOG ptyparr[ INPTLOG ];      /* and it's type info */
   int      npar;                    /* the number of parameters */
   short    olmtyp;                  /* Anroparens modtyp */
   char     valarr[V3STRLEN*V2MPMX]; /* array for parameter values to be logged in GM */
/*
***Spara anropad moduls namn i "prtmna".
***From. V1.12 kan namnet representeras p� tv� olika s�tt.
*/
   if ( partp->modname > 0 )
     {
     if ( (status=pmgstr(partp->modname,&modname)) < -1 ) return(status);
     strcpy(prtmna,modname);
     }
   else
     {
     exprla = -partp->modname;
     if ( (status=inevev(exprla,&partpval,&tyla)) < -1 ) return(status);
     strcpy(prtmna,partpval.lit.str_va);
     }
/*
***Finns REF-parameter f�r lokalt koordinatsystem?
*/
   exlist = partp->partpara;

   if ( exlist == (pm_ptr)NULL )
     { 
     ppalist = (pm_ptr)NULL;
     refp = NULL;
     }
/*
***Get list node in list for actual parameters to the part statement.
*/
   else
     {
    if ( ( status = pmgfli( exlist, &ppalist ) ) < 0 )
        return( status );
/*
***Get contents of list node.
*/
     if ( ( status = pmglin( ppalist, &nextlist, &exprla ) ) < -1 )
         return( status );
/*
***Evaluate expression value.
*/
    if ( ( status = inevev( exprla, &partpval, &tyla ) ) < -1 )
        return( status );
/* 
***Check type.
*/
    if ( ! ineqty( strefp, tyla ) )
        return( erpush( "IN3053", "PART" ) );

    refp = partpval.lit.ref_va;
    }
/*
***Evaluate Named parameters and prepere Named parameter block for 
***parted module.
*/
   if ( ( status = inevnl( partp->partacna, &partnpbl ) ) < -1 )
     return( status );
/*
***Get first list node in list for actual parameters to the parted module.
*/
   if ( ( status = pmgfli( partp->modpara, &actlist ) ) < 0 )
     return( status );
/*
***Set Run-Time environment.
***Check if current module is active module.
*/
   if  ( pmgbla() == pmgaba() ) actflg = TRUE;
   else                         actflg = FALSE;
/*
***Get name of current module.
*/
   pmgmod((pm_ptr)0,&np);
   strcpy(curmna,pmgadr(np->mona_));
/*
***Check that the current module is not calling itself recursively.
*/
   if ( strcmp(prtmna,curmna) == 0 ) return(erpush("IN3442",prtmna));
/*
***Get base address for Parted module.
*/
   if ( ( status = pmgeba( prtmna, &partbase ) ) < -1 )
     return( status );
/*
***Get base address for current module.
*/
  currbase = pmgbla();
/*
***Get RTS base pointer for current module.
*/
   currtsp = ingrtb();
/*
***Set PM base to start of Parted module.
*/
   if ( ( status = pmsbla( partbase ) ) < -1 )
     {
     errlev = 2;
     goto error;
     }
/*
***Put frame header on RTS.
*/
   pmrmod( &moddata );  /* read module info */
   if ( ( status = inpfrh( curmna, moddata.ldsize ) ) < -1 )
     {
     sprintf(errbuf,"%d",(int)moddata.ldsize);
     erpush("IN3312",errbuf);
     errlev = 3;
     goto error;
     }
/*
***Get RTS base pointer for parted module.
*/
   partrtsp = ingrtb();
/*
***Get pointer to and check if module node.
*/
   if ( ( status = pmgmod((pm_ptr)0, &np ) ) < -1 )
     {
     errlev = 2;
     goto error;
     }

   if ( np->monocl != MODULE )
     {
     erpush("IN2542","");
     errlev = 2;
     goto error;
     }
/*
***Evaluate parameters.
***Get first list node in list for formal  parameters.
*/
   if ( ( status = pmgfli( np->ppali_, &formlist ) ) < -1 )
     {
     errlev = 2;
     goto error;
     }

   while ( formlist != (pm_ptr)NULL )
     { 
/*
***Get contents of list node for formal parameters.
*/
     if ( ( status = pmglin( formlist, &formnext, &parala ) ) < -1 )
       {
       erpush("PM2112", "");
       errlev = 2;
       goto error;
       }
/*
***Get ST-info for formal parameter node, to be used for test
***of ST_RPAVA.
*/
      pmgpar( parala, &formpnp );
      strvar( formpnp->fopa_, &var );
/*
***Evaluate actual parameter value.
*/
      if ( actflg ) currbase = pmgaba();
      else pmgeba(curmna, &currbase);

      pmsbla( currbase );
/*
***Set RTS base pointer to currtsp.
*/
      if ( ( status = insrtb( currtsp ) ) < -1 )
        {
        errlev = 2;
        goto error;
        }
/*
***Check list.
*/
      if ( actlist == (pm_ptr)NULL )
        {
        erpush("IN3063","");
        errlev = 2;
        goto error;
        } /*  missmatch in parameters to parted module */
/*
***Get contents of list node, actual param.
*/
      if ( ( status = pmglin( actlist, &actnext, &exprla ) ) < -1 )
        { 
        errlev = 2;
        goto error;
        }
/*
***Check if formal parameter is ST_RPAVA, ( call by reference ).
*/
      if ( var.kind_va == ST_RPAVA )
        {  /* evaluate expression */
        if ( ( status = inevex( exprla, &val, &tyla ) ) < -1 )
          {
          errlev = 2;
          goto error;
          }
        }
      else
        {  /* evaluate expression value */
        if ( ( status = inevev( exprla, &val, &tyla ) ) < -1 )
          {
          errlev = 2;
          goto error;
          }
        }
/*
***Read type info for actual parameter.
*/
      strtyp( tyla, &type );
/*
***Next actual parameter.
*/
      actlist = actnext;
/*
***Set base to Parted module, RTS base pointer and evaluate parameters.
*/
      pmgeba(prtmna, &partbase);

      if ( ( status = pmsbla( partbase ) ) < -1 )
        { 
        errlev = 2;
        goto error;
        }
/*
***Set RTS base pointer to partrtsp.
*/
      if ( ( status = insrtb( partrtsp ) ) < -1 )
        {
        errlev = 2;
        goto error;
        }
/*
***Evaluate parameter.
*/
      frsize = partrtsp - currtsp;

      if ( ( status = inevpa( parala, &val, &type, frsize ) ) < -1 )
        {
        errlev = 2;
        goto error;
        }

      formlist = formnext;
      }
/*
***N�r namnparametern SAVE == 1, lagra partpost i GM.
*/
   if ( partnpbl.save == 1 )
     {
/*
***Read parameter values for parted module from RTS and fill in value 
***and type to be logged in GM.
*/
     if ( (status=ingpva( np->ppali_, valarr, ptyparr, &npar )) < -1 )
       {
       errlev = 2;           /* 861028, JK */
       goto error;
       }
/*
***Lagra �vriga partdata i prtrec och prtdat.
*/
     *( prtmna + JNLGTH ) = '\0';
     strcpy( prtrec.name_pt, prtmna );        /* POA 850913 */
     prtrec.its_pt = ( np->geidlev ) + 1;     /* POA 850530 */
     prtrec.hed_pt.level = partnpbl.level;    /* POA 850902 */
     prtrec.hed_pt.pen = partnpbl.pen;        /* POA 850902 */
     prtrec.hed_pt.blank = partnpbl.blank; 
     prtrec.hed_pt.hit = partnpbl.hit;        /* JK 861228 */ 
     prtdat.mtyp_pd = np->moty_;              /* POA 860310 */
     prtdat.matt_pd = np->moat_;              /*    - " -   */
     prtdat.npar_pd = npar;                   /*    - " -   */
/*
***�ppna part i GM.
*/
     if (( status = EXoppt( ident, refp, &prtrec, &prtdat,
                            ptyparr, valarr )) < -1 )
     {
     errlev = 2;
     goto error;
     }
  }
/*
***PUSH named parameter block and put "partnpbl" as default.
*/
   if ( ( status = inpunp( &partnpbl ) ) < -1 )
     {
     errlev = 1;
     goto error;
     }
/*
***Spara aktuell modtyp, 2D/3D och s�tt modtyp till
***den anropade modulens innan den b�rjar exekvera.
*/
   olmtyp = modtyp;
   modtyp = np->moty_;
/*
***K�r den anropade modulen och �terst�ll omedelbart modtyp.
***Status fr�n k�rningen = evstat. Om evstat = 4 => exit utan
***felmeddelande. Is�fall �r allt ok och status = 0 kan returneras.
*/
   evstat = inevsl(np->pstl_);
   modtyp = olmtyp;

   if ( evstat < -1 )
     {
     errlev = 1;
     goto error;
     }
   else if ( evstat == 4 ) evstat = 0;
/*
***POP named parameter block and put as default module.
*/
   if ( ( status = inponp() ) < -1 )
     {
     errlev = 1;
     goto error;
     }
/*
***Reset Run-Time environment.
***Get frame header from RTS.
*/
   if ( ( status = ingfrh( curmna ) ) < -1 )
     {
     errlev = 1;
     goto error;
     }
/*
***N�r namnparametern SAVE == 1, st�ng part i GM.
*/
   if ( partnpbl.save == 1 )
     {
     if ( ( status = EXclpt() ) < -1 )
       {
       errlev = 2;
       goto error;
       }
     }
/*
***Set PM base to start of previous module.
*/
   if ( actflg ) currbase = pmgaba();
   else pmgeba(curmna, &currbase);

   pmsbla( currbase );
/*
***Check lista for actual parameters to the parted module.
*/
   if ( actlist != (pm_ptr)NULL )
     {
     erpush( "IN1011",prtmna); /* to many parameters to %s */
     errlev = 4;
     goto error;
     }
/*
***Allt har g�tt normalt, returnera status fr�n k�rningen.
*/
   return(evstat);

/*
******************  Error ***************************
*/
error:

    switch ( errlev )
      {
      case 1:
           /* POP named parameter block */
           status = inponp();
           /* clear part in GM */
           if ( partnpbl.save == 1 ) status = EXclpt();

      case 2:
          /* Get frame header from RTS */
          status = ingfrh( curmna );

      case 3:
           /* Set PM base */
           if ( actflg )
               currbase = pmgaba();
           else
               pmgeba(curmna, &currbase);
           pmsbla( currbase );

      case 4:
           sprintf(errbuf,"%s%%#%d-%s",curmna,ident->seq_val,prtmna);
           return(erpush("IN2112",errbuf));
      }
/*
***Ett sista ej anv�ndt return(0) f�r att slippa
***kompileringsvarningar.
*/
   return(0);
  }

/*****************************************************/
