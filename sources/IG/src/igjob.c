/*!******************************************************************/
/*  igjob.c                                                         */
/*  =======                                                         */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGgene();     Starts Varkon in generic mode                     */
/*  IGexpl();     Starts Varkon in explicit mode                    */
/*  IGlsjb();     Lists jobs                                        */
/*  IGdljb();     Deletes jobs                                      */
/*  IGmvrr();     Copies RES-file to RIT-file                       */
/*  IGload();     Loads new job                                     */
/*  IGldmo();     Loads module                                      */
/*  IGsjpg();     Saves all                                         */
/*  IGsaln();     Saves all with new name                           */
/*  IGspmn();     Saves module with new name                        */
/*  IGsgmn();     Saves result with new name                        */
/*  IGsjbn();     Saves jobdata with new name                       */
/*  IGcatt();     Change module attribute                           */
/*  IGexit();     Exits                                             */
/*  IGexsn();     Exit without saving                               */
/*  IGexsa();     Exit with saving                                  */
/*  IGexsd();     Exit with saving and decompiling                  */
/*  IGnjsd();     Save, decompile and new job                       */
/*  IGnjsa();     Save and new job                                  */
/*  IGsjsa();     Save and continue                                 */
/*  IGnjsn();     New job without saving                            */
/*  IGselj();     Select job from list                              */
/*  IGchjn();     Change name of current job                        */
/*  IGgrst();     Returns resource value                            */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../../GE/include/GE.h"
#include "../include/IG.h"
#include "../include/debug.h"
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"
#include <string.h>

char   tmprit[V3PTHLEN+1];

/* Namn p� tempor�r .RIT-fil under k�rning. */

extern pm_ptr   actmod,pmstkp;
extern bool     tmpref,rstron,igxflg,igbflg,jnflag;
extern char     pidnam[],jobnam[],jobdir[],mdffil[],actcnm[];
extern short    modtyp,modatt,v3mode,actfun,
                rmarg,bmarg,igmatt,igmtyp,pant,mant;
extern short    posmode;
extern DBptr    msysla,lsysla;
extern DBseqnum snrmax;
extern V3MDAT   sydata;
extern V3MSIZ   sysize;
extern DBTmat  *lsyspk,*msyspk;
extern DBTmat   lklsys,lklsyi,modsys;
extern V2NAPA   defnap;
extern MNUDAT   mnutab[];

extern char *mktemp();

/*
***Prototypes for internal functions.
*/
static short iginjb();
static short igldjb();
static short igsvjb();
static short iginmo();
static short getmta(short *typ, short *att);
static short igsvmo();
static short igingm();
static short igldgm();
static short igsvgm();
static short init_macro();
static short newjob_macro();
static short exit_macro();
static short igebas();

/*!******************************************************/

       short  IGgene()

/*     Start Varkon in generic mode.
*
*      Felkoder : IG0252 = Kan ej starta Basmodulen
*
*      (C) microform ab 2/3/88  J. Kjellander.
*
*      12/12/94 v3mode, J. Kjellander
*
*******************************************************!*/

 {

/*
***Kolla att basmodulen ing�r.
*/
   if ( sydata.opmode != BAS_MOD )
     {
     erpush("IG0252","");
     return(EREXIT);
     }
/*
***Starta basmodulen med meny 0.
*/
   v3mode = BAS2_MOD;

   return(igebas());

 }

/******************************************************!*/
/*!******************************************************/

       short  IGexpl()

/*     Start Varkon in 2D explicit mode.
*
*      (C) microform ab 2/3/88  J. Kjellander.
*
*      12/12/94 v3mode, J. Kjellander
*
*******************************************************!*/

 {
   short status;

/*
***St�ll om opmode i sydata och anropa basmodulen
***med v3mode = RIT_MOD.
*/
   if ( sydata.opmode == BAS_MOD )
     {
     sydata.opmode = RIT_MOD;
     v3mode = RIT_MOD;
     status = igebas();
     sydata.opmode = BAS_MOD;
     }
   else
     {
     v3mode = RIT_MOD;
     status = igebas();
     }

   return(status);

 }

/******************************************************!*/
/*!******************************************************/

static short igebas()

/*     Huvudrutin f�r Basmodulen.
*
*      In : Inget.
*
*      Felkoder : IG0232 = Du har ej valt projekt
*                 IG0242 = Projektet finns ej
*                 IG0262 = Kan ej ladda menyer
*                 IG0342 = %s otill�tet jobnamn
*
*      (C) microform ab 2/3/88  J. Kjellander.
*
*      8/5/89 igckjn(), J. Kjellander
*
*******************************************************!*/

 {
   short status,alt;

/*
***Ladda menyerna. 
*/
   if ( IGinit(mdffil) < 0 )
     {
     erpush("IG0262",mdffil);
     return(EREXIT);
     }
/*
***Global initiering av PM.
*/
   if ( pminit() < 0 ) return(EREXIT);
/*
***Starta basmodulen.
*/
   pant = 0;
   mant = 0;

   if ( (status=IGload()) < 0 ) return(status);
/*
***Loopa med IGexfu() och meny 0 = Huvudmenyn.
*/
   for (;;)
     {
     actfun = -1;
     status = IGexfu(0,&alt);

     if ( status == GOMAIN )
       {
       pant = 0;
       WPclear_mcwin();
       mant = 0;
       }
     else if ( status == EXIT )
       {
       return(EXIT);
       }
     else if ( status == EREXIT )
       {
       return(EREXIT);
       }
     }

 }

/******************************************************!*/
/*!******************************************************/

       short  IGlsjb()

/*     Varkon-funktion f�r att lista job.
 *
 *     Felkoder:
 *
 *     (C) microform ab 28/9/95  J. Kjellander.
 *
 ******************************************************!*/

 {
   char buf[JNLGTH+1];

   IGselj(buf);

   return(0);
 }

/******************************************************!*/
/*!******************************************************/

       short  IGdljb()

/*     Varkon-funktion f�r att ta bort job.
 *
 *     Felkoder : IG0452 = Jobbet %s �r aktivt.
 *
 *     (C) microform ab 28/9/95  J. Kjellander.
 *
 *******************************************************!*/

 {
    char  job[JNLGTH+1];
    char  jobfil[V3PTHLEN+1];
    char  mbsfil[V3PTHLEN+1];
    char  mbofil[V3PTHLEN+1];
    char  resfil[V3PTHLEN+1];
    char  pltfil[V3PTHLEN+1];
    char  ritfil[V3PTHLEN+1];
    short status;

/*
***L�s in jobbnamn.
*/
    status = IGselj(job);
    if ( status < 0 ) return(status);
/*
***Kolla att det inte �r aktivt.
*/
   if ( strcmp(job,jobnam) == 0 )
     {
     erpush("IG0452",job);
     errmes();
     return(0);
     }
/*
***Ta bort ev. JOB-fil ?
*/
   strcpy(jobfil,jobdir);
   strcat(jobfil,job);
   strcat(jobfil,JOBEXT);

   if ( IGftst(jobfil)  &&  IGialt(442,67,68,FALSE) ) IGfdel(jobfil);
/*
***Ta bort ev. MBO-fil ?
*/
   strcpy(mbofil,jobdir);
   strcat(mbofil,job);
   strcat(mbofil,MODEXT);

   if ( IGftst(mbofil)  &&  IGialt(443,67,68,FALSE)  &&
                            IGialt(444,67,68,FALSE)  ) IGfdel(mbofil);
/*
***Ta bort ev. MBS-fil ?
*/
   strcpy(mbsfil,jobdir);
   strcat(mbsfil,job);
   strcat(mbsfil,MBSEXT);

   if ( IGftst(mbsfil)  &&  IGialt(445,67,68,FALSE)  &&
                            IGialt(446,67,68,FALSE) ) IGfdel(mbsfil);
/*
***Ta bort ev. RES-fil ?
*/
   strcpy(resfil,jobdir);
   strcat(resfil,job);
   strcat(resfil,RESEXT);

   if ( IGftst(resfil)  &&  IGialt(447,67,68,FALSE) ) IGfdel(resfil);
/*
***Ta bort ev. RIT-fil ?
*/
   strcpy(ritfil,jobdir);
   strcat(ritfil,job);
   strcat(ritfil,RITEXT);

   if ( IGftst(ritfil)  &&  IGialt(448,67,68,FALSE)  &&
                            IGialt(449,67,68,FALSE) ) IGfdel(ritfil);
/*
***Ta bort ev. PLT-fil ?
*/
   strcpy(pltfil,jobdir);
   strcat(pltfil,job);
   strcat(pltfil,PLTEXT);

   if ( IGftst(pltfil)  &&  IGialt(450,67,68,FALSE) ) IGfdel(pltfil);

   return(0);
 }

/******************************************************!*/
/*!******************************************************/

       short  IGmvrr()

/*     Varkon-funktion f�r att kopiera en RES-fil
 *     till en RIT-fil.
 *
 *     Felkoder : IG0232 = Du har ej valt projekt
 *                IG0242 = Projektet finns ej.
 *                IG0312 = Resultatfil finns ej
 *
 *     (C) microform ab 16/3/88  J. Kjellander.
 *
 *******************************************************!*/

 {
    char  job[JNLGTH+1];
    char  resfil[V3PTHLEN+1];
    char  ritfil[V3PTHLEN+1];
    short status;
    FILE  *fpk;

/*
***Kolla att pidnam finns.
*/
   if ( pidnam[0] == '\0' )
     {
     erpush("IG0232","");
     errmes();
     return(0);
     }
/*
***Ladda aktuell PID-fil.
*/
    if ( IGldpf(pidnam) < 0 )
      {
      erpush("IG0242",pidnam);
      errmes();
      return(0);
      }
/*
***L�s in jobnamn.
*/
    status = IGssip(IGgtts(400),job,"",JNLGTH);
    if ( status < 0 ) return(status);
/*
***Kolla om RES-filen finns.
*/
   strcpy(resfil,jobdir);
   strcat(resfil,job);
   strcat(resfil,RESEXT);

   if ( (fpk=fopen(resfil,"r")) == 0 )
     {
     erpush("IG0312",resfil);
     errmes();
     return(0);
     }
   fclose(fpk);
/*
***Kolla om RIT-filen finns.
*/
   strcpy(ritfil,jobdir);
   strcat(ritfil,job);
   strcat(ritfil,RITEXT);

   if ( (fpk=fopen(ritfil,"r")) != 0 )
     {
     fclose(fpk);
     if ( !IGialt(419,67,68,TRUE) ) return(0);
     }
/*
***Kopiera.
*/
   if ( IGfcpy(resfil,ritfil) < 0 ) errmes();

   return(0);
 }

/******************************************************!*/
/*!******************************************************/

        short IGload()

/*      Loads a job from disc.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 5/11/85 J. Kjellander
 *
 *      12/10/88 iginmo f�rst om ftabnr=4, J. Kjellander
 *      21/10/88 CGI, J. Kjellander
 *      28/2/92  omflag, J. Kjellander
 *      28/1/95  Multif�nster, J. Kjellander
 *      1997-03-11 IGupcs(), J.Kjellander
 *      1998-03-12 init/newjob_macro, J.Kjellander
 *      1999-04-23 Cray, J.Kjellander
 *      1999-06-05 igbflg, J.Kjellander
 *      2006-12-31 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    bool   newjob;
 
/*
***Init pen number.
*/
   WPspen(1);
/*
***Clear level name table.
*/
   EXclear_levels();
/*
***Om ritmodulen aktiv, initiera PM f�rst och ladda sedan
***jobbfilen. PM-initiering medf�r ju �terst�llning av
***alla attribut. Om detta g�rs efter laddning av job-filen
***skrivs attribut lagrade i jobbfilen �ver av iginmo().
*/
    if ( v3mode == RIT_MOD )
      {
      if ( iginmo() < 0 ) goto errend;
      pmgstp(&pmstkp);
/*return(erpush("EX1862",filnam));
***Ladda jobbfil.
*/
      if ( (status=igldjb()) == -1 ) iginjb();
      else if ( status < 0 ) goto errend;
/*
***Ladda ritfil.
*/
      if ( (status=igldgm()) == -1 )
        {
        if ( igingm() < 0 ) goto errend;
        newjob = TRUE;
        }
      else if ( status < 0 ) goto errend;
      else newjob = FALSE;
      }
/*
***Om basmodulen aktiv, g�r tv�rtom.
*/
    else
      {
      if ( (status=igldjb()) == -1 ) iginjb();
      else if ( status < 0 ) goto errend;

      if ( (status=IGldmo()) == -1 )
        {
        status = iginmo();
        if      ( status == REJECT ) return(REJECT);
        else if ( status == GOMAIN ) return(GOMAIN);
        else if ( status < 0 ) goto errend;
        newjob = TRUE;
        }
      else
        {
        if (status < 0 ) goto errend;
        newjob = FALSE;
        }

      if ( v3mode & BAS_MOD )
        {
        if ( modtyp == 2 ) v3mode = BAS2_MOD;
        else v3mode = BAS3_MOD;
        }
/*
***Ladda ev. resultatfil. Om resultatfil saknas men modulfil
***fanns kanske vi ska b�rja med att k�ra modulen.
*/
      if ( (status=igldgm()) == -1 )
        {
        if ( igingm() < 0 ) goto errend;
        if ( !newjob )
          {
          if ( igxflg || IGialt(118,67,68,TRUE) )
            {
            IGramo();
            if ( igbflg ) return(IGexsn());
            }
          }
        }
      else if ( status == 0  &&  igbflg )
        {
        IGramo();
        return(IGexsn());
        }
      else if ( status < 0 ) goto errend;
      }
/*
***Nu �r det dags att k�ra ev. init_macro.
*/
   if ( init_macro() < 0 )
     {
     errmes();
     goto errend;
     }
/*
***Om det �r ett helt nytt jobb ska vi kanske k�ra ett
***newjob_macro.
*/
   if ( newjob )
     {
     if ( IGgrst("newjob_macro",NULL) )
       {
       if ( newjob_macro() < 0 )
         {
         errmes();
         goto errend;
         }
       goto end;
       }
     }
/*
***Rita om sk�rmen.
*/
#ifdef UNIX
    WPreload_view(GWIN_ALL);
    WPrepaint_GWIN(GWIN_ALL);
    WPrepaint_RWIN(RWIN_ALL,TRUE);
#endif

#ifdef WIN32
    msrepa(GWIN_ALL);
    if ( rstron ) WPdrrs();
    IGupcs(lsysla,V3_CS_ACTIVE);
#endif
/*
***K�r vi WIN32 m�ste vi ansluta r�tt huvudmeny innan
***vi slutar.
*/
end:

#ifdef WIN32
    mssmmu((int)IGgmmu());
#endif

    return(0);
/*
***Felutg�ng.
*/
errend:
    WPclrg();
    return(EREXIT);
  }

/********************************************************/
/*!******************************************************/

static short igsvjb()

/*      Lagrar ett jobb.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0133 => Kan ej lagra jobbfil.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      30/9/86  Ny niv�hantering, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      16/11/86 V1.3, J. Kjellander
 *      17/19/88 Anrop av gemansam exerutin R. Svedin
 *
 ******************************************************!*/

  {
    char   filnam[V3PTHLEN+1];

/*
***Skapa filnamn och �ppna filen.
*/
    strcpy(filnam,jobdir);
    strcat(filnam,jobnam);
    strcat(filnam,JOBEXT);
/*
***Skriv ut aktiva jobb-data till fil.
*/
    if ( EXsave_job(filnam) < 0 ) return(erpush("IG0133",filnam));
/*
***Jobbfil lagrad.
*/
    WPaddmess_mcwin(IGgtts(218),WP_MESSAGE);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

static short iginmo()

/*      Initierar (skapar) ny modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0143 => Kan ej skapa modul
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      29/10/85 Ny pmcmod, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      2/3/92   igmtyp/igmatt, J. Kjellander
 *      1996-02-26 Krypterat serienummer, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      y,m,d,h,min,s;
    PMMODULE modhed;

/*
***Ta reda p� den blivande modulens typ och attribut.
*/
    if ( (status=getmta(&modtyp,&modatt)) < 0 ) return(status);
/*
***Nollst�ll och initiera PM.
*/
    if ( pmclea() < 0 ) goto error;
    if ( incrts() < 0 ) goto error;
    inrdnp();
/*
***Initiera ett modulhuvud.
*/
    modhed.parlist = (pm_ptr)NULL;
    modhed.stlist = (pm_ptr)NULL;
    modhed.idmax = 0;
    modhed.ldsize = 0;
    modhed.system.sernr = sydata.sernr;
    modhed.system.vernr = sydata.vernr;
    modhed.system.revnr = sydata.revnr;
    modhed.system.level = sydata.level;

    EXtime(&y,&m,&d,&h,&min,&s);

    modhed.system.year_c = y;
    modhed.system.mon_c  = m;
    modhed.system.day_c  = d;
    modhed.system.hour_c = h;
    modhed.system.min_c  = min;

    modhed.system.year_u = 0;
    modhed.system.mon_u  = 0;
    modhed.system.day_u  = 0;
    modhed.system.hour_u = 0;
    modhed.system.min_u  = 0;

    strcpy(modhed.system.sysname,sydata.sysname);
    strcpy(modhed.system.release,sydata.release);
    strcpy(modhed.system.version,sydata.version);

    modhed.system.mpcode = 0;
    modhed.system.ser_crypt = sydata.ser_crypt;
/*
***Skapa modulen.
*/
    modhed.mtype = (char)modtyp;
    modhed.mattri = (char)modatt;
    if (pmcmod(jobnam,&modhed,&actmod) < 0 ) goto error;
/*
***Initiera st�rsta sekvensnummer.
*/
    snrmax = 0;
/*
***Initiera koordinatsystem. Modulens system = BASIC och inget
***loaklt aktivt.
*/
  if ( v3mode & BAS_MOD )
    {
    lsyspk = NULL;
    lsysla = DBNULL;

    msyspk = NULL;
    msysla = DBNULL;

    EXmsini();
    strcpy(actcnm,IGgtts(223));
/*
***Modul skapad.
*/
    WPaddmess_mcwin(IGgtts(308),WP_MESSAGE);
    }

    return(0);
/*
***Felutg�ng.
*/
error:
    return(erpush("IG0143",""));
  }

/********************************************************/
/*!******************************************************/

 static short getmta(
        short *typ,
        short *att)

/*      Tar reda p� vilken typ och vilket attribut
 *      en ny modul skall ha.
 *
 *      In: typ = Pekare till utdata.
 *          att = Pekare till utdata.
 *
 *      Ut: *typ = _2D eller _3D.
 *          *att = LOCAL, GLOBAL eller BASIC
 *
 *      FV: 0, REJECT eller GOMAIN.
 *
 *      (C)microform ab 8/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    short alt;

/*
***I ritmodulen �r alla "moduler" DRAWING och BASIC.
*/
  if ( v3mode == RIT_MOD )
    {
   *typ = _2D;
   *att = BASIC;
    }
/*
***I basmodulen kan en modul ha typen DRAWING (_2D) eller
***GEOMETRY (_3D).
*/
  else
    {
#ifdef WIN32
    if ( igmtyp == IGUNDEF  ||  igmatt == IGUNDEF )
      return(msdl01(typ,att));
    else
      {
     *att = igmatt;
     *typ = igmtyp;
      return(0);
      }
#endif
    if ( igmtyp == IGUNDEF )
      {
      IGexfu( 144, &alt);
      switch ( alt )
        {
        case 1:
       *typ = _3D;
        break;

        case 2:
       *typ = _2D;
        break;

        case REJECT:
        return(REJECT);

        case GOMAIN:
        return(GOMAIN);

        default:
        return(-1);
        }
      }
    else *typ = igmtyp;
/*
***Attribut kan vara LOCAL, GLOBAL eller BASIC..
*/
    if ( igmatt == IGUNDEF )
      {
      IGexfu( 145, &alt);
      switch ( alt )
        {
        case 1:
       *att = LOCAL;
        break;
 
        case 2:
       *att = GLOBAL;
        break;

        case 3:
       *att = BASIC;
        break;

        case REJECT:
        return(REJECT);

        case GOMAIN:
        return(GOMAIN);

        default:
        return(-1);
        }
      }
    else *att = igmatt;
    }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGldmo()

/*      Laddar modul fr�n fil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *              -1 => Filen finns ej.
 *          IG0153 => Kan ej ladda modulen
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 ******************************************************!*/

  {
    PMMODULE modhed;

/*
***Laddning av modul.
*/
    if ( pmclea() < 0 ) goto error;
    if ( incrts() < 0 ) goto error;

    if ( pmload(jobnam, &actmod) < 0 )
      {
      if ( erlerr() == 201 )
        {
        erinit();
        return(-1);
        }
      else goto error;
      }
/*
***L�s modulhuvud och s�tt modtyp och modattr d�refter.
*/
    pmrmod(&modhed);
    modtyp = modhed.mtype;
    modatt = modhed.mattri;
/*
***Initiera st�rsta sekvensnummer.
*/
    snrmax = modhed.idmax;
/*
***Interpretera modulens parameterlista.
*/
    IGevlp();
/*
***Modul inl�st.
*/
    WPaddmess_mcwin(IGgtts(213),WP_MESSAGE);

    return(0);
/*
***Felutg�ng.
*/
error:
    return(erpush("IG0153",jobnam));
  }

/********************************************************/
/*!******************************************************/

static short igsvmo()

/*      Lagra PM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG0173 = Systemfel vid lagring.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      7/10/86  Time, J. Kjellander
 *
 ******************************************************!*/

  {
    int      y,m,d,h,min,s;
    PMMODULE modhed;

/*
***Uppdatera modulhuvudet.
*/
    pmrmod(&modhed);

    EXtime(&y,&m,&d,&h,&min,&s);
    modhed.system.year_u = y;
    modhed.system.mon_u  = m;
    modhed.system.day_u  = d;
    modhed.system.hour_u = h;
    modhed.system.min_u  = min;

    pmumod(&modhed);
/*
***Lagra modulfil.
*/
    if ( pmsave(actmod) < 0 ) return(erpush("IG0173",jobnam));
/*
***Modulfil lagrad.
*/
    WPaddmess_mcwin(IGgtts(216),WP_MESSAGE);

    return(0);
  }

/********************************************************/
/*!******************************************************/

static short igingm()

/*      Skapar resultafil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0183 => Kan ej skapa resultatfil
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      29/11/88   Tempor�rfil, J. Kjellander
 *      1999-02-09 Ny gminit(), J.Kjellander
 *
 ******************************************************!*/

  {
    char filnam[V3PTHLEN+1],templ[JNLGTH+10];

    strcpy(filnam,jobdir);

    if ( v3mode == RIT_MOD )
      {
      strcpy(templ,jobnam);
      strcat(templ,".XXXXXX");
      strcat(filnam,mktemp(templ));
      strcpy(tmprit,filnam);
      }
    else
      {
      strcat(filnam,jobnam);
      strcat(filnam,RESEXT);
      }

    if ( DBinit(filnam,sysize.gm,
                DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL) < 0 )
                                      return(erpush("IG0183",filnam));
/*
***Resultatfil skapad.
*/
    WPaddmess_mcwin(IGgtts(309),WP_MESSAGE);

    return(0);
  }

/********************************************************/
/*!******************************************************/

static short igldgm()

/*      Laddar resultafil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder:  0 => Ok.
 *                -1 => Kan ej ladda resultat.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      14/11/85   Bug, J. Kjellander
 *      5/4/86     Ny felhantering, J. Kjellander
 *      29/11/88   Tempor�rfil, J. Kjellander
 *      1999-02-09 Ny felhantering, J.Kjellander
 *
 ******************************************************!*/

  {
    char  filnam[V3PTHLEN+1],templ[JNLGTH+10];
    short status;

/*
***Bilda filnamn utan extension.
*/
    strcpy(filnam,jobdir);
    strcat(filnam,jobnam);
/*
***Ritpaketet, kolla om filen finns. Finns den,
***kopieras den till en tempor�rfil och tempor�rfilen
***laddas. Finns den inte returneras -1.
*/
    if ( v3mode == RIT_MOD )
      {
      strcat(filnam,RITEXT);
      if ( IGftst(filnam) )
        {
        strcpy(tmprit,jobdir);
        strcpy(templ,jobnam);
        strcat(templ,".XXXXXX");
        strcat(tmprit,mktemp(templ));
        IGfcpy(filnam,tmprit);
        status = DBload(tmprit,sysize.gm,
                    DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
        if ( status < 0 )
          {
          errmes();       /* Allvarligare fel */
          return(status);
          }
        else
          {
          WPaddmess_mcwin(IGgtts(140),WP_MESSAGE);
          return(0);
          }
        }
      else return(-1);    /* Filen finns ej */
      }
/*
***Basmodulen ! Prova att ladda gammal resultatfil.
***Status =  0 => Filen har laddats.
***Status = -1 => Filen finns ej.
***Status < -1 => Allvarligare fel, tex. filen tom.
*/
    else
      {
      strcat(filnam,RESEXT);
      status = DBload(filnam,sysize.gm,
                      DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);

      if ( status == 0 )
        {
        WPaddmess_mcwin(IGgtts(214),WP_MESSAGE);
        return(0);
        }
      else if ( status == -1 ) return(-1);  /* Filen finns ej */
      else
        {
        errmes();                           /* Allvarligare fel */
        return(-1);
        }
      }
  }

/********************************************************/
/*!******************************************************/

static short igsvgm()

/*      Lagra resultatfil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0193 => Kan ej lagra resultatfilen
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 ******************************************************!*/

  {
    char  filnam[V3PTHLEN+1];

/*
***Lagra resultatfil.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));

/*
***Resultatfil lagrad.
*/
    if ( v3mode & BAS_MOD ) WPaddmess_mcwin(IGgtts(217),WP_MESSAGE);
/*
***Om ritpaketet, kopiera den tempor�ra arbetsfilen
***till en .RIT-fil.
*/
    else
     {
     strcpy(filnam,jobdir);
     strcat(filnam,jobnam);
     strcat(filnam,RITEXT);
     IGfcpy(tmprit,filnam);
     IGfdel(tmprit);
     WPaddmess_mcwin(IGgtts(141),WP_MESSAGE);
     }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGsjpg()

/*      Lagra jobb, PM och GM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Lagra jobbet.
*/
    if ( igsvjb() < 0 ) errmes();
/*
***Lagra modul.
*/
    if ( v3mode & BAS_MOD  &&  igsvmo() < 0 ) errmes();
/*
***Lagra resultat.
*/
    if ( igsvgm() < 0 ) errmes();

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short IGsaln()

/*      Lagrar allt med nytt namn.
 *
 *      FV: 0      = OK
 *          REJECT = Avsluta
 *          GOMAIN = Huvudmenyn
 *
 *      Felkod: IG0342 = Otill�tet jobnamn
 *              IG0422 = Nytt namn = aktuellt namn
 *
 *      (C)microform ab 1998-09-16 J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    char     resfil[V3PTHLEN+1];
    char     newfil[V3PTHLEN+1];
    char     tmpnam[JNLGTH+1];
    char     path[V3PTHLEN+1];
    bool     flag;
    PMMODULE modhed;

    static char newnam[JNLGTH+1] = "";

/*
***L�s in nytt jobbnamn.
*/
    IGptma(210,IG_INP);     
    status = IGssip(IGgtts(267),newnam,newnam,JNLGTH);
    if ( status < 0 ) return(status);
/*
***Kolla att namnet �r ok.
*/
   if ( igckjn(newnam) < 0 )
     {
     erpush("IG0342",newnam);
     errmes();
     goto exit;
     }
/*
***Kolla att det nya namnet inte �r lika med aktuellt jobbnamn.
*/
    if ( strcmp(newnam,jobnam) == 0 )
      {
      erpush("IG0422",newnam);
      errmes();
      goto exit;
      }
/*
***Kolla om det redan finns ett jobb med det angivna namnet.
*/
    flag = FALSE;

    strcpy(path,jobdir);
    strcat(path,newnam);
    strcat(path,JOBEXT);
    if ( IGftst(path) ) flag = TRUE;

    if ( v3mode == !RIT_MOD )
      {
      strcpy(path,jobdir);
      strcat(path,newnam);
      strcat(path,MODEXT);
      if ( IGftst(path) ) flag = TRUE;

      strcpy(path,jobdir);
      strcat(path,newnam);
      strcat(path,RESEXT);
      if ( IGftst(path) ) flag = TRUE;
      }
   else
     {
      strcpy(path,jobdir);
      strcat(path,newnam);
      strcat(path,RITEXT);
      if ( IGftst(path) ) flag = TRUE;
      }

    if ( flag  &&  !IGialt(1626,67,68,TRUE) ) goto exit;
/*
***Lagra jobfil.
*/
    strcpy(tmpnam,jobnam);
    strcpy(jobnam,newnam);
    if ( igsvjb() < 0 ) errmes();
    strcpy(jobnam,tmpnam);
/*
***Lagra ev. modul. �ndra namnet i modulhuvudet tillf�lligt
***och skriv ut.
*/
    if ( v3mode != RIT_MOD )
      {
      pmrmod(&modhed);
      strcpy(modhed.mname,newnam);
      pmumod(&modhed);

      if ( igsvmo() < 0 ) errmes();

      pmrmod(&modhed);
      strcpy(modhed.mname,jobnam);
      pmumod(&modhed);
      }
/*
***Lagra GM.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));
/*
***Kopiera den lagrade pagefilen till en fil med det nya namnet.
*/
    if ( v3mode == RIT_MOD ) strcpy(resfil,tmprit);
    else
      {
      strcpy(resfil,jobdir);
      strcat(resfil,jobnam);
      strcat(resfil,RESEXT);
      } 
 
    strcpy(newfil,jobdir);
    strcat(newfil,newnam);
    if ( v3mode == RIT_MOD ) strcat(newfil,RITEXT);
    else strcat(newfil,RESEXT);
/*
***Kopiera filen.
*/
    if ( (status=IGfcpy(resfil,newfil)) < 0 )
      return(status);
    else
      {
      if ( v3mode == RIT_MOD ) WPaddmess_mcwin(IGgtts(141),WP_MESSAGE);
      else                     WPaddmess_mcwin(IGgtts(217),WP_MESSAGE);
      }
/*
***�ppna GM igen.
*/
    DBload(resfil,sysize.gm,
           DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
    IGptma(196,IG_MESS);     
/*
***Slut.
*/
exit:
    return(0);
  }

/********************************************************/
/********************************************************/

        short IGspmn()

/*      Lagra modul med nytt namn.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 24/11/85 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      10/10/86 default, J. Kjellander
 *
 ******************************************************!*/

  {
    short      status;
    char       newnam[JNLGTH+1];
    PMMODULE   modhed;

/*
***L�s in nytt filnamn.
*/
    IGptma(349,IG_INP);
    if ( (status=IGssip(IGgtts(267),newnam,jobnam,JNLGTH)) < 0 )
        goto exit;
/*
***�ndra namnet i modulhuvudet.
*/
    pmrmod(&modhed);
    strcpy(modhed.mname,newnam);
    pmumod(&modhed);
/*
***Lagra modul.
*/
    if ( igsvmo() < 0 ) errmes();
/*
***�ndra tillbaks namnet i modulhuvudet.
*/
    pmrmod(&modhed);
    strcpy(modhed.mname,jobnam);
    pmumod(&modhed);

exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGsgmn()

/*      Lagra GM med nytt namn.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG0082 = Resultatfilens namn = jobbnamn
 *
 *      (C)microform ab 30/7/85 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      2/2/93   copy p� VAX, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    char    resfil[V3PTHLEN+1];
    char    newfil[V3PTHLEN+1];
    char    newnam[JNLGTH+1];
#ifdef VMS
    char    oscmd[2*(V3PTHLEN+1)+20];
#endif

/*
***L�s in nytt filnamn.
*/
loop:
    IGptma(279,IG_INP);     
    status = IGssip(IGgtts(267),newnam,"",JNLGTH);
    if ( status < 0 ) return(status);
/*
***Kolla att det nya namnet inte �r lika med aktuellt jobbnamn.
*/
    if ( strcmp(newnam,jobnam) == 0 )
      {
      erpush("IG0082","");
      errmes();
      goto loop;
      }
/*
***Lagra GM.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));
/*
***Kopiera den lagrade pagefilen till en fil med det nya namnet.
*/
    else
      {
      if ( v3mode == RIT_MOD ) strcpy(resfil,tmprit);
      else
        {
        strcpy(resfil,jobdir);
        strcat(resfil,jobnam);
        strcat(resfil,RESEXT);
        } 
 
      strcpy(newfil,jobdir);
      strcat(newfil,newnam);
      if ( v3mode == RIT_MOD ) strcat(newfil,RITEXT);
      else strcat(newfil,RESEXT);
/*
***Kopiera filen, p� VAXEN kan inte IGfcpy() anv�ndas.
*/
#ifdef UNIX
      if ( (status=IGfcpy(resfil,newfil)) < 0 )
        return(status);
      else
        {
        if ( v3mode == RIT_MOD ) WPaddmess_mcwin(IGgtts(141),WP_MESSAGE);
        else                     WPaddmess_mcwin(IGgtts(217),WP_MESSAGE);
        }
      }
#endif

#ifdef WIN32
      if ( (status=IGfcpy(resfil,newfil)) < 0 )
        return(status);
      else
        {
        if ( v3mode == RIT_MOD ) WPaddmess_mcwin(IGgtts(141),WP_MESSAGE);
        else                     WPaddmess_mcwin(IGgtts(217),WP_MESSAGE);
        }
      }
#endif
/*
***�ppna GM igen.
*/
    DBload(resfil,sysize.gm,
           DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGsjbn()

/*      Lagra jobbfil med nytt namn.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 11/10/86 J. Kjellander
 *
 ******************************************************!*/

  {
    short      status;
    char       newnam[JNLGTH+1];
    char       tmpnam[JNLGTH+1];

/*
***L�s in nytt filnamn.
*/
    IGptma(357,IG_INP);
    if ( (status=IGssip(IGgtts(267),newnam,jobnam,JNLGTH)) < 0 )
        goto exit;
/*
***Lagra jobb.
*/
    strcpy(tmpnam,jobnam);
    strcpy(jobnam,newnam);
    if ( igsvjb() < 0 ) errmes();
    strcpy(jobnam,tmpnam);

exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGcatt()

/*      �ndra modulens attribut.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 24/11/85 J. Kjellander
 *
 *      9/10/86  GOMAIN, J. Kjellander
 *      1/3/94   Snabbval, J. Kjellander
 *
 ******************************************************!*/

  {
    short     alt=-1,alttyp;
    MNUALT   *pmualt;
    PMMODULE  modhed;

/*
***Ta reda p� nytt attribut.
*/
#ifdef WIN32
    msshmu(145);
#else
    IGaamu(145);
#endif

l1:
    IGgalt(&pmualt,&alttyp);

#ifdef WIN32
    mshdmu();
#endif

    if ( pmualt == NULL )
      {
      switch ( alttyp )
        {
        case SMBRETURN:
        IGsamu();
        return(REJECT);

        case SMBMAIN:
        return(GOMAIN);
        }
      }
    else alt = pmualt->actnum;

    switch ( alt )
      {
      case 1:
      modatt = LOCAL;
      break;

      case 2:
      modatt = GLOBAL;
      break;

      case 3:
      modatt = BASIC;
      break;

      default:                           /* Ok�nt alt. */
      erpush("IG0103","");
      errmes();
      goto l1;
      }
/*
***Uppdatera modulhuvudet.
*/
    pmrmod(&modhed);
    modhed.mattri = (char)modatt;
    pmumod(&modhed);
/*
***Uppdatera statusarean.
*/
    IGsamu();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGexit()

/*      Avslutningsrutin. Anv�nds av toppniv�n (futab4)
 *      och trap-rutiner f�r att �terv�nda till OS.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 13/4/86 J. Kjellander
 *
 *      25/9/95  X11, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***St�ng surpac.
*/
   suexit();
/*
***H�r st�nger vi f�nsterhanteringssystemet.
*/
#ifdef UNIX
   WPexit();
#endif

#ifdef WIN32
   msexit();
#endif
/*
***St�ng ev. debug-filer.
*/
   dbgexi();
/*
***�terv�nd till OS.
*/
#ifdef WIN32
   ExitProcess(0);
#else
   exit(V3EXOK);
#endif
/*
***F�r att slippa kompileringsvarning.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGexsn()

/*      Exit without saving.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      10/2/86  gpexit, J. Kjellander
 *      13/4/86  IGexit, J. Kjellander
 *      29/11/88 Tempor�r .RIT-fil, J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *      2007-03-26 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   char resfil[V3PTHLEN+1];

/*
***In interactive mode (not batch mode) this
***requires an ok from the user.
*/
   if ( !igbflg )
     {
     if ( !IGialt(69,67,68,TRUE) ) return(0);
     }
/*
***Execute optional exit_macro.
*/
   exit_macro();
/*
***Close DB and delete RES- or temp. RIT-file.
*/
   gmclpf();

   if ( v3mode == RIT_MOD ) IGfdel(tmprit);
   else
     {
     strcpy(resfil,jobdir);
     strcat(resfil,jobnam);
     strcat(resfil,RESEXT);
     IGfdel(resfil);
     }
/*
***Close down.
*/
   WPclrg();
/*
***The end.
*/
   return(EXIT);
  }

/********************************************************/
/*!******************************************************/

        short IGexsa()

/*      Sluta och lagra allt. f122.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      10/2/86  gpexit, J. Kjellander
 *      13/4/86  IGexit, J. Kjellander
 *      26/9/95  jnflag, J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *
 ******************************************************!*/

  {
   short status;
   char  newnam[JNLGTH+1];

/*
***Om inget riktigt jobnamn �nnu har definierats
***fr�gar vi om detta nu och byter namn p� jobbet.
*/
   if ( !jnflag )
     {
     IGptma(193,IG_INP);
     if ( (status=IGssip(IGgtts(400),newnam,"",JNLGTH)) < 0 )
        return(status);

     if ( IGchjn(newnam) < 0 )
       {
       errmes();
       return(0);
       }
     }
/*
***K�r ev. exit_macro.
*/
   exit_macro();
/*
***Lagra allt.
*/
   IGsjpg();
/*
***Avsluta.
*/
   WPclrg();
   return(EXIT);
  }

/********************************************************/
/*!******************************************************/

        short IGexsd()

/*      Sluta och lagra allt, �ven dekompilerad
 *      version av aktiv modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 19/4/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Dekompilera.
*/
    if ( IGprtm() < 0 ) return(0);
/*
***Sluta och lagra.
*/
    else return(IGexsa());
  }

/********************************************************/
/*!******************************************************/

        short IGnjsd()

/*      Nytt jobb och lagra allt, �ven dekompilerad
 *      version av aktiv modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 9/8/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Dekompilera.
*/
    if ( IGprtm() < 0 ) return(0);
/*
***Sluta och lagra.
*/
    else return(IGnjsa());
  }

/********************************************************/
/*!******************************************************/

        short IGnjsa()

/*      Lagra allt och nytt jobb.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      26/9/95  IGselj(), J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    char   newnam[JNLGTH+1];
    char   oldnam[JNLGTH+1];

/*
***Om inget riktigt jobnamn �nnu har definierats
***fr�gar vi om detta nu och byter namn p� jobbet.
*/
   if ( !jnflag )
     {
     IGptma(193,IG_INP);
     if ( (status=IGssip(IGgtts(400),newnam,"",JNLGTH)) < 0 )
        return(status);

     if ( IGchjn(newnam) < 0 )
       {
       errmes();
       return(0);
       }
     }
/*
***L�s in nytt jobnamn.
*/
   status = IGselj(newnam);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***K�r ev. exit_macro.
*/
    exit_macro();
/*
***Lagra allt.
*/
    IGsjpg();
/*
***Lagra nya namnet men spara f�rst det gamla s� att
***vi kan byta tillbaks om det inte g�r att ladda det
***nya jobbet.
*/
    strcpy(oldnam,jobnam);
    strcpy(jobnam,newnam);
/*
***Ladda/skapa nytt jobb, ny modul och nytt resultat.
***Om det inte g�r eller avbryts av anv�ndaren laddar
***vi tillbaks det gamla jobbet igen.
*/
    WPclrg();

    status = IGload();

    if ( status < 0 )
      {
      if ( status != REJECT  &&  status != GOMAIN ) errmes();
      strcpy(jobnam,oldnam);
      WPclrg();
      if ( IGload() < 0 ) return(EREXIT);
      else return(status);
      }
    else
/*
***New job loaded. v3mode could now have changed and a new
***main menu might be needed, ie. 2D->3D etc. If this function
***was called through the menu handler return(GOMAIN) is enough
***to fix this but if the function is called by a button GOMAIN
***will not be propagated back all the way (because buttons can't
***return that information) so to be sure we fix it by updating
***the mein menu here.
*/
    {
    WPactivate_menu(&mnutab[IGgmmu()]);
    return(GOMAIN);
    }
  }

/********************************************************/
/*!******************************************************/

        short IGsjsa()

/*      Lagra allt och forts�tt.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 28/9/95 J. Kjellander
 *
 *      1998-02-06 WIN32, J.Kjellander
 *
 ******************************************************!*/

  {
   short  status;
   char   newnam[JNLGTH+1],ritfil[V3PTHLEN+1];

/*
***Om inget riktigt jobnamn �nnu har definierats
***fr�gar vi om detta nu och byter namn p� jobbet.
*/
   if ( !jnflag )
     {
     IGptma(193,IG_INP);
     if ( (status=IGssip(IGgtts(400),newnam,"",JNLGTH)) < 0 )
        return(status);

     if ( IGchjn(newnam) < 0 )
       {
       errmes();
       return(0);
       }
     }
/*
***Lagra JOB-fil.
*/
   if ( igsvjb() < 0 ) errmes();
/*
***Kanske �ven MBO-fil.
*/
   if ( v3mode & BAS_MOD )
     {
     if ( igsvmo() < 0 ) errmes();
     }
/*
***Och kanske �ven RIT-fil.
*/
   if ( v3mode == RIT_MOD )
     {
     if ( DBexit() < 0 )
       {
       erpush("IG0193",jobnam);
       errmes();
       return(0);
       }

     strcpy(ritfil,jobdir);
     strcat(ritfil,jobnam);
     strcat(ritfil,RITEXT);

#ifdef UNIX
     if ( IGfcpy(tmprit,ritfil) < 0 )
       {
       errmes();
       return(0);
       }
#endif

#ifdef WIN32
     if ( IGfcpy(tmprit,ritfil) < 0 )
       {
       errmes();
       return(0);
       }
#endif
/*
***�ppna GM igen.
*/
     if ( DBload(tmprit,sysize.gm,
          DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL) < 0 ) errmes();
     }
/*
***� s� ett litet meddelande.
*/
   WPaddmess_mcwin(IGgtts(196),WP_MESSAGE);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGnjsn()

/*      Lagra inget och nytt jobb.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 6/10/86 J. Kjellander
 *
 *      8/5/87   Defaultstr�ng, J. Kjellander
 *      26/9/95  tmprit, J. Kjellander
 *      26/9/95  IGselj(), J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    char   newnam[JNLGTH+1];
    char   resfil[V3PTHLEN+1];

/*
***L�s in nytt jobnamn.
*/
   status = IGselj(newnam);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***K�r ev. exit_macro.
*/
    exit_macro();
/*
***Lagra inget.
*/
   gmclpf();

   if ( v3mode & BAS_MOD )
     {
     strcpy(resfil,jobdir);
     strcat(resfil,jobnam);
     strcat(resfil,RESEXT);
     IGfdel(resfil);
     }
   else IGfdel(tmprit);
/*
***Lagra nya namnet. Eftersom detta namn �r givet av anv�ndaren
***s�tter vi nu jnflaggan till true.
*/
    strcpy(jobnam,newnam);
    jnflag = TRUE;
/*
***Ladda/skapa nytt jobb, ny modul och nytt resultat.
*/
    WPclrg();

    status = IGload();

    if (      status == REJECT ) return(EXIT);
    else if ( status == GOMAIN ) return(EXIT);
    else if ( status < 0 ) return(EREXIT);
/*
***New job loaded. Update the main menu, see IGnjsa().
*/
    else
    {
    WPactivate_menu(&mnutab[IGgmmu()]);
    return(GOMAIN);
    }
  }

/********************************************************/
/*!******************************************************/

        short IGselj(char *newjob)

/*      Interaktiv funktion f�r att v�lja jobb.
 *
 *      In: newjob = Pekare till utdata.
 *
 *      Ut: *newjob = Jobbnamn eller odefinierat.
 *
 *      FV:  0      = Ok.
 *           REJECT = Avbryt.
 *          -1      = Kan ej skapa pipe till "ls".
 *
 *      Felkoder: IG0342 = %s �r ett otill�tet jobbnamn.
 *                IG0442 = Kan ej �ppna pipe %s
 *
 *      (C)microform ab 25/9/95 J. Kjellander
 *
 *      1998-11-03 actfun, J.Kjellander
 *
 ******************************************************!*/

  {
   short status,oldafu;
   char *pekarr[1000],strarr[20000];
   char  typ[5],mesbuf[V3STRLEN+1];
   int   i,nstr,actalt;

/*
***Skapa filf�rteckning.
*/
   if ( v3mode & BAS_MOD ) strcpy(typ,MODEXT);
   else                    strcpy(typ,RITEXT);

   IGdir(jobdir,typ,1000,20000,pekarr,strarr,&nstr);
/*
***Vilket av dem �r aktivt ?
*/
   for ( i=0; i<nstr; ++i ) if ( strcmp(pekarr[i],jobnam) == 0 ) break;

   if ( i < nstr ) actalt =  i;
   else            actalt = -1;
/*
***Aktiv funktion, specialare f�r hj�lp-systemet.
*/
   oldafu = actfun;
   actfun = 1001;
/*
***L�t anv�ndaren v�lja.
*/
   sprintf(mesbuf,"%s - %s ",pidnam,IGgtts(210));

   if ( nstr > 0 )
     {
#ifdef UNIX
     status = WPilse(mesbuf,"",pekarr,actalt,nstr,newjob);
#endif

#ifdef WIN32
     status = msilse(mesbuf,"",pekarr,actalt,nstr,newjob);
#endif
     }
/*
***Om det inte finns n�gra jobb att v�lja mellan r�cker det
***med en enkel fr�ga.
*/
   else
     {
     IGplma(mesbuf,IG_INP);
     status=IGssip(IGgtts(210),newjob,"",JNLGTH);
     }

   actfun = oldafu;

   if ( status <  0 ) return(status);
/*
***Han kan ha matat in ett JOB-namn fr�n tangentbordet s� det �r
***b�st att kolla att det f�ljer reglerna.
*/
   if ( igckjn(newjob) < 0 ) return(erpush("IG0342",newjob));
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGchjn(char *newnam)

/*      �ndrar namn p� aktivt jobb.
 *
 *      In: newnam = Nytt jobbnamn.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0342 = Jobnamnet %s �r ej till�tet
 *                IG0422 = Jobbet finns redan
 *
 *      (C)microform ab 26/9/95 J. Kjellander
 *
 ******************************************************!*/

  {
    char       oldres[V3PTHLEN+1],newres[V3PTHLEN+1],
               filnam[V3PTHLEN+1],templ[JNLGTH+10];
    PMMODULE   modhed;

/*
***Kolla att det �r ett till�tet namn.
*/
    if ( igckjn(newnam) < 0 ) return(erpush("IG0342",newnam));
/*
***Kolla at inte ett jobb med detta namn finns redan.
*/
    strcpy(filnam,jobdir);
    strcat(filnam,newnam);
    strcat(filnam,JOBEXT);
    if ( IGftst(filnam) ) return(erpush("IG0422",newnam));

    if ( v3mode & BAS_MOD )
      {
      strcpy(filnam,jobdir);
      strcat(filnam,newnam);
      strcat(filnam,MODEXT);
      if ( IGftst(filnam) ) return(erpush("IG0422",newnam));

      strcpy(filnam,jobdir);
      strcat(filnam,newnam);
      strcat(filnam,RESEXT);
      if ( IGftst(filnam) ) return(erpush("IG0422",newnam));
      }
    else
      {
      strcpy(filnam,jobdir);
      strcat(filnam,newnam);
      strcat(filnam,RITEXT);
      if ( IGftst(filnam) ) return(erpush("IG0422",newnam));
      }
/*
***K�r vi basmodulen skall vi �ndra namnet i modulhuvudet....
*/
    if ( v3mode & BAS_MOD )
      {
      pmrmod(&modhed);
      strcpy(modhed.mname,newnam);
      pmumod(&modhed);
/*
***samt byta namn p� resultatfilen.
*/
      strcpy(oldres,jobdir);
      strcat(oldres,jobnam);
      strcat(oldres,RESEXT);
      strcpy(newres,jobdir);
      strcat(newres,newnam);
      strcat(newres,RESEXT);
      IGfmov(oldres,newres);
      }
/*
***K�r vi ritmodulen r�cker det att byta namn p� tempor�rfilen.
*/
    else
      {
      strcpy(templ,newnam);
      strcat(templ,".XXXXXX");
      mktemp(templ);

      strcpy(newres,jobdir);
      strcat(newres,templ);
      IGfmov(tmprit,newres);
      strcpy(tmprit,newres);
      }
/*
***Sist av allt byter vi aktivt jobnamn.
*/
   strcpy(jobnam,newnam);
   jnflag = TRUE;
/*
***Och uppdaterar f�nsterramen.
*/
#ifdef UNIX
   WPupwb(NULL);
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool IGgrst(
        char *resurs,
        char *pvalue)

/*      OS-oberoende version av WPgrst/msgrst. Returnerar
 *      v�rdet p� en resurs om den �r definierad. 
 *
 *      In: resurs = Resursnamn utan "varkon."
 *          pvalue = Pekare till utdata eller NULL.
 *
 *      Ut: *pvalue = Resursv�rde om tillg�ngligt.
 *
 *      FV: TRUE  = Resursen finns.
 *          FALSE = Resursen finns ej.
 *
 *      (C)microform ab 1998-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
   char value[1000];
   bool status;

/*
***X-resurser heter samma sak som WIN32-resurser
***men med ordet varkon framf�r.
*/
#ifdef UNIX
   char xrmnam[V3STRLEN];

   strcpy(xrmnam,"varkon.");
   strcat(xrmnam,resurs);
   status = WPgrst(xrmnam,value);
#endif

/*
***WIN32-resurser heter samma sak som i igepac.
*/
#ifdef WIN32
   status = msgrst(resurs,value);
#endif
/*
***Skall vi returnera resursv�rde ?
*/
   if ( status )
     {
     if ( pvalue != NULL ) strcpy(pvalue,value);
     }
/*
***Slut.
*/
   return(status);
  }

/********************************************************/
/*!******************************************************/

 static short init_macro()

/*      K�r macro n�r jobb laddas oavsett om det �r
 *      ett nytt jobb som skapas eller ett gammalt
 *      som laddas.
 *
 *      FV:  0 = OK.
 *          <0 = Fel.
 *
 *      (C)microform ab 1998-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
   char name[V3PTHLEN];

   if ( IGgrst("init_macro",name) )
     {
     name[JNLGTH] = '\0';
     return(IGcall_macro(name));
     }
   else return(0);
   }

/********************************************************/
/*!******************************************************/

 static short newjob_macro()

/*      K�r macro bara n�r nytt jobb skapas.
 *
 *      FV:  0 = OK.
 *          <0 = Fel.
 *
 *      (C)microform ab 1998-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
   char name[V3PTHLEN];

   if ( IGgrst("newjob_macro",name) )
     {
     name[JNLGTH] = '\0';
     return(IGcall_macro(name));
     }
   else return(0);
   }

/********************************************************/
/*!******************************************************/

 static short exit_macro()

/*      K�r macro vid avslut.
 *
 *      FV:  0 = OK.
 *          <0 = Fel.
 *
 *      (C)microform ab 1998-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
   short status;
   char  name[V3PTHLEN];

   if ( IGgrst("exit_macro",name) )
     {
     name[JNLGTH] = '\0';
     status = IGcall_macro(name);
     if ( status < 0 )
       {
       errmes();
       IGialt(457,458,458,TRUE);
       }
     return(status);
     }
   else return(0);
   }

/********************************************************/
/*!******************************************************/

static short iginjb()

/*      Init's (creates default) job data.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      22/10/85 Sl�ck ej def. niv�er, J. Kjellander
 *      30/9/86  Ny niv�hantering, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      5/3/88   stalev, J. Kjellander
 *      15/11/88 posmod, J. Kjellander
 *      31/1/95  Multif�nster, J. Kjellander
 *      2007-01-01 Removed nivtb1[], J.Kjellander
 *      2007-02-10 Views, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;
    int   y,m,d,h,min,s;

/*
***Initiera "jobb skapat datum" i sydata.
*/
    EXtime(&y,&m,&d,&h,&min,&s);
    sydata.year_c = y;
    sydata.mon_c  = m;
    sydata.day_c  = d;
    sydata.hour_c = h;
    sydata.min_c  = min;
/*
***Init views.
*/
    WPinit_views();
/*
***Create the default graphical window.
*/
#ifdef UNIX
     if ( (status=WPcgws()) < 0 ) return(status);
#endif
#ifdef WIN32
     if ( (status=(short)mscdgw(TRUE)) < 0 ) return(status);
#endif
/*
***Initiera diverse flaggor.
*/
    tmpref  = FALSE;
    posmode = 2;
/*
***Initiera koordinatsystem. Modulens system = GLOBAL och
***inget lokalt system aktivt.
*/
    msyspk = NULL;
    msysla = DBNULL;

    lsyspk = NULL;
    lsysla = DBNULL;

    EXmsini();                     /* Stackpekaren */
    strcpy(actcnm,"GLOBAL");
/*
***A "New job created !" message.
*/
    WPaddmess_mcwin(IGgtts(211),WP_MESSAGE);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

static short igldjb()

/*      Load a .JOB-file from disc.
 *
 *      FV:   0 = Ok.
 *           -1 = Filen finns ej.
 *          < -1 = Status fr�n EXldjb()
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      16/4/86  Bytt geo607 mot 612, J. Kjellander
 *      30/9/86  Ny niv�hantering, J. Kjellander
 *      7/11/86  V1.3, J. Kjellander
 *      26/12/86 hit och save, J. Kjellander
 *      15/11/88 EXldjb() och posmod, J. Kjellander
 *      30/1-95  Multif�nster, J. Kjellander
 *
 ******************************************************!*/

  {
   short  status;
   char   filnam[V3PTHLEN+1];

/*
***Bilda filnamn och prova att ladda.
*/
   strcpy(filnam,jobdir);
   strcat(filnam,jobnam);
   strcat(filnam,JOBEXT);
   status = EXload_job(filnam);
/*
***Om status < 0 och felkod = EX1862 finns filen inte.
*/
   if ( status < 0  &&  erlerr() == 186 )
     {
     erinit();
     return(-1);
     }
/*
***Annars om status < 0, felmeddelande.
*/
   else if ( status < 0 ) return(status);
/*
***K�r vi X11 och inga f�nster har skapats fanns det inga
***f�nster i jobfilen. D� skapar vi default f�nster enl.
***resursfil nu.
*/
#ifdef UNIX
     if ( WPngws() == 0 )
       if ( (status=WPcgws()) < 0 ) return(status);
#endif
#ifdef WIN32
     if ( msngws() == 0 )
       {
       if ( (status=(short)mscdgw(FALSE)) < 0 ) return(status);
       }
#endif
/*
***Initiera koordinatsystem.
*/
   if ( msyspk != NULL ) msyspk = &modsys;

   if ( lsyspk != NULL )
     {
     lsyspk = &lklsys;
     GEtform_inv(&lklsys,&lklsyi);
     }

   EXmsini();
/*
***Jobbfil inl�st.
*/
   WPaddmess_mcwin(IGgtts(212),WP_MESSAGE);

   return(0);
  }

/********************************************************/
