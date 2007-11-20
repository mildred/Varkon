/*!******************************************************************/
/*  igjob.c                                                         */
/*  =======                                                         */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGgeneric();  Starts Varkon in generic mode                     */
/*  IGexplicit(); Starts Varkon in explicit mode                    */
/*  IGdljb();     Deletes jobs                                      */
/*  IGload();     Loads new job                                     */
/*  IGldmo();     Loads module                                      */
/*  IGsjpg();     Saves all                                         */
/*  IGsaln();     Saves all with new name                           */
/*  IGspmn();     Saves module with new name                        */
/*  IGsgmn();     Saves result with new name                        */
/*  IGsjbn();     Saves jobdata with new name                       */
/*  IGcatt();     Change module attribute                           */
/*  IGexit();     Exits                                             */
/*  IGexit_sn();  Exit without saving                               */
/*  IGexit_sa();  Exit with saving                                  */
/*  IGexsd();     Exit with saving and decompiling                  */
/*  IGnjsd();     Save, decompile and new job                       */
/*  IGnjsa();     Save and new job                                  */
/*  IGsave_all(); Save and continue                                 */
/*  IGnjsn();     New job without saving                            */
/*  IGselj();     Select job from list                              */
/*  IGchjn();     Change name of current job                        */
/*  IGgrst();     Returns resource value                            */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://varkon.sourceforge.net                             */
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

/* Namn på temporär .RIT-fil under körning. */

extern pm_ptr   actmod,pmstkp;
extern bool     tmpref,rstron,igxflg,igbflg,relpos,startup_complete;
extern char     jobnam[],jobdir[],mdffil[],actcnm[];
extern short    modtyp,modatt,sysmode,actfun,igmatt,pant,mant;
extern int      posmode;
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
static short load_jobdata();
static short igsvjb();
static short iginmo();
static short igsvmo();
static short igingm();
static short igldgm();
static short igsvgm();
static short init_macro();
static short newjob_macro();
static short exit_macro();
static short main_loop();

/********************************************************/

       short  IGgeneric()

/*     Start Varkon in generic mode.
*
*      (C)2007-11-07 J. Kjellander.
*
*******************************************************!*/

 {
/*
***Start system in generic mode.
*/
   sysmode = GENERIC;
/*
***Go.
*/
   return(main_loop());
 }

/******************************************************!*/
/*!******************************************************/

       short  IGexplicit()

/*     Start Varkon in explicit mode.
*
*      (C)2007-11-07 J. Kjellander.
*
*******************************************************!*/

 {
/*
***Start system in explicit mode.
*/
   sysmode = EXPLICIT;
/*
***Go.
*/
   return(main_loop());
 }

/******************************************************!*/
/*!******************************************************/

static short main_loop()

/*     Varkon main loop.
*
*      (C) microform ab 2/3/88  J. Kjellander.
*
*      8/5/89 igckjn(), J. Kjellander
*
*******************************************************!*/

 {
   short status,alt;

/*
***Init PM.
*/
   if ( pminit() < 0 ) return(EREXIT);
/*
***Load jobnam[].
*/
   pant = 0;
   mant = 0;

   if ( (status=IGload()) < 0 ) return(status);
/*
***System is now started.
*/
   startup_complete = TRUE;
/*
***Loop with IGexfu() and menu 0 = Main menu.
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

        short IGload()

/*      Loads a job from disc.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 5/11/85 J. Kjellander
 *
 *      12/10/88 iginmo först om ftabnr=4, J. Kjellander
 *      21/10/88 CGI, J. Kjellander
 *      28/2/92  omflag, J. Kjellander
 *      28/1/95  Multifönster, J. Kjellander
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
***EXPLICIT mode.
***Om ritmodulen aktiv, initiera PM först och ladda sedan
***jobbfilen. PM-initiering medför ju återställning av
***alla attribut. Om detta görs efter laddning av job-filen
***skrivs attribut lagrade i jobbfilen över av iginmo().
*/
    if ( sysmode == EXPLICIT )
      {
      if ( iginmo() < 0 ) goto errend;
      pmgstp(&pmstkp);
/*return(erpush("EX1862",filnam));
***Ladda jobbfil.
*/
      if ( (status=load_jobdata()) == -1 ) iginjb();
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
***GENERIC mode.
***Om basmodulen aktiv, gör tvärtom.
*/
    else
      {
      if ( (status=load_jobdata()) == -1 ) iginjb();
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
/*
***Ladda ev. resultatfil. Om resultatfil saknas men modulfil
***fanns kanske vi ska börja med att köra modulen.
*/
      if ( (status=igldgm()) == -1 )
        {
        if ( igingm() < 0 ) goto errend;
        if ( !newjob )
          {
          if ( igxflg || IGialt(118,67,68,TRUE) )
            {
            IGrun_active();
            if ( igbflg ) return(IGexit_sn());
            }
          }
        }
      else if ( status == 0  &&  igbflg )
        {
        IGrun_active();
        return(IGexit_sn());
        }
      else if ( status < 0 ) goto errend;
      }
/*
***Nu är det dags att köra ev. init_macro.
*/
   if ( init_macro() < 0 )
     {
     errmes();
     goto errend;
     }
/*
***Om det är ett helt nytt jobb ska vi kanske köra ett
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
***Rita om skärmen.
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
***Kör vi WIN32 måste vi ansluta rätt huvudmeny innan
***vi slutar.
*/
end:

#ifdef WIN32
    mssmmu((int)IGgmmu());
#endif

    return(0);
/*
***Felutgång.
*/
errend:
    WPclrg();
    return(EREXIT);
  }

/********************************************************/
/*!******************************************************/

       short  IGdljb()

/*     Delete job files.
 *
 *     Felkoder : IG0452 = Job %s is active.
 *
 *     (C) microform ab 28/9/95  J. Kjellander.
 *
 *     2007-11-14 2.0, J.Kjellander
 *
 *******************************************************!*/

 {
    char  job[JNLGTH+1];
    char  jobfil[V3PTHLEN+1];
    char  mbsfil[V3PTHLEN+1];
    char  mbofil[V3PTHLEN+1];
    char  resfil[V3PTHLEN+1];
    char  pltfil[V3PTHLEN+1];
    short status;

/*
***Get job name.
*/
    status = IGselj(job);
    if ( status < 0 ) return(status);
/*
***Check that it is not active.
*/
   if ( strcmp(job,jobnam) == 0 )
     {
     erpush("IG0452",job);
     errmes();
     return(0);
     }
/*
***Remove JOB-file ?
*/
   strcpy(jobfil,jobdir);
   strcat(jobfil,job);
   strcat(jobfil,JOBEXT);

   if ( IGftst(jobfil)  &&  IGialt(442,67,68,FALSE) ) IGfdel(jobfil);
/*
***Remove MBO-file ?
*/
   strcpy(mbofil,jobdir);
   strcat(mbofil,job);
   strcat(mbofil,MODEXT);

   if ( IGftst(mbofil)  &&  IGialt(443,67,68,FALSE)  &&
                            IGialt(444,67,68,FALSE)  ) IGfdel(mbofil);
/*
***Remove MBS-file ?
*/
   strcpy(mbsfil,jobdir);
   strcat(mbsfil,job);
   strcat(mbsfil,MBSEXT);

   if ( IGftst(mbsfil)  &&  IGialt(445,67,68,FALSE)  &&
                            IGialt(446,67,68,FALSE) ) IGfdel(mbsfil);
/*
***Remove RES-file ?
*/
   strcpy(resfil,jobdir);
   strcat(resfil,job);
   strcat(resfil,RESEXT);

   if ( IGftst(resfil)  &&  IGialt(447,67,68,FALSE) ) IGfdel(resfil);
/*
***Remove PLT-file ?
*/
   strcpy(pltfil,jobdir);
   strcat(pltfil,job);
   strcat(pltfil,PLTEXT);

   if ( IGftst(pltfil)  &&  IGialt(450,67,68,FALSE) ) IGfdel(pltfil);
/*
***The end.
*/
   return(0);
 }

/******************************************************!*/
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
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      16/11/86 V1.3, J. Kjellander
 *      17/19/88 Anrop av gemansam exerutin R. Svedin
 *
 ******************************************************!*/

  {
    char   filnam[V3PTHLEN+1];

/*
***Skapa filnamn och öppna filen.
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

/*      Creates a new module.
 *
 *      Return: IG0143 => Can't create module.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      29/10/85 Ny pmcmod, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      2/3/92   igmtyp/igmatt, J. Kjellander
 *      1996-02-26 Krypterat serienummer, J. Kjellander
 *      2007-11-07 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    int      y,m,d,h,min,s;
    PMMODULE modhed;

/*
***Init PM.
*/
    if ( pmclea() < 0 ) goto error;
    if ( incrts() < 0 ) goto error;
    inrdnp();
/*
***Create a default module head.
*/
    modhed.parlist = (pm_ptr)NULL;
    modhed.stlist  = (pm_ptr)NULL;
    modhed.idmax   = 0;
    modhed.ldsize  = 0;
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

    modhed.system.mpcode    = 0;
    modhed.system.ser_crypt = sydata.ser_crypt;
/*
***Module type and attribute. Module type is always _3D
***and module attribute is igmatt (LOCAL/GLOBAL/BASIC).
***Default for igmatt is GLOBAL but this may be changed
***by a command line argument.
*/
    modhed.mtype  = (char)_3D;
    modhed.mattri = (char)igmatt;
/*
***Create the module.
*/
    if (pmcmod(jobnam,&modhed,&actmod) < 0 ) goto error;
/*
***Largest sequence number at this stage is 0.
***No entities have been created yet.
*/
    snrmax = 0;
/*
***Init active coordinate system.
*/
  if ( sysmode & GENERIC )
    {
    lsyspk = NULL;
    lsysla = DBNULL;

    msyspk = NULL;
    msysla = DBNULL;

    EXmsini();
    strcpy(actcnm,IGgtts(223));
/*
***Message to user.
*/
    WPaddmess_mcwin(IGgtts(308),WP_MESSAGE);
    }
/*
***The end.
*/
    return(0);
/*
***Error exit.
*/
error:
    return(erpush("IG0143",""));
  }

/********************************************************/
/*!******************************************************/

        short IGldmo()

/*      Laddar modul från fil.
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
***Läs modulhuvud och sätt modtyp och modattr därefter.
*/
    pmrmod(&modhed);
    modtyp = modhed.mtype;
    modatt = modhed.mattri;
/*
***Initiera största sekvensnummer.
*/
    snrmax = modhed.idmax;
/*
***Interpretera modulens parameterlista.
*/
    IGevlp();
/*
***Modul inläst.
*/
    WPaddmess_mcwin(IGgtts(213),WP_MESSAGE);

    return(0);
/*
***Felutgång.
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
 *      29/11/88   Temporärfil, J. Kjellander
 *      1999-02-09 Ny gminit(), J.Kjellander
 *
 ******************************************************!*/

  {
    char filnam[V3PTHLEN+1],templ[JNLGTH+10];

    strcpy(filnam,jobdir);

    if ( sysmode == EXPLICIT )
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

/*      Load a RES-file.
 *
 *      Error:  0 => Ok.
 *             -1 => Can't load RES-file.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      14/11/85   Bug, J. Kjellander
 *      5/4/86     Ny felhantering, J. Kjellander
 *      29/11/88   Temporärfil, J. Kjellander
 *      1999-02-09 Ny felhantering, J.Kjellander
 *      2007-11-14 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    char  filnam[V3PTHLEN+1],templ[JNLGTH+10];
    short status;

/*
***Create a filename with path but no extension.
*/
    strcpy(filnam,jobdir);
    strcat(filnam,jobnam);
/*
***In explicit mode load a temporary copy.
*/
    if ( sysmode == EXPLICIT )
      {
      strcat(filnam,RESEXT);
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
          errmes();
          return(status);
          }
        else
          {
          WPaddmess_mcwin(IGgtts(140),WP_MESSAGE);
          return(0);
          }
        }
      else return(-1);
      }
/*
***In generic mode we load the RES-file itself.
***Status =  0 => File loaded.
***Status = -1 => File does not exist.
***Status < -1 => System error.
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
      else if ( status == -1 ) return(-1);
      else
        {
        errmes();
        return(-1);
        }
      }
  }

/********************************************************/
/*!******************************************************/

static short igsvgm()

/*      Save RES-file.
 *
 *      Error: IG0193 => Can't save file.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 ******************************************************!*/

  {
    char  filnam[V3PTHLEN+1];

/*
***Save the current DB.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));

/*
***A message to the user.
*/
    if ( sysmode & GENERIC ) WPaddmess_mcwin(IGgtts(217),WP_MESSAGE);
/*
***In explicit mode copy temporary file to .RES.
*/
    else
     {
     strcpy(filnam,jobdir);
     strcat(filnam,jobnam);
     strcat(filnam,RESEXT);
     IGfcpy(tmprit,filnam);
     IGfdel(tmprit);
     WPaddmess_mcwin(IGgtts(141),WP_MESSAGE);
     }
/*
***The end.
*/
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
    if ( sysmode & GENERIC  &&  igsvmo() < 0 ) errmes();
/*
***Lagra resultat.
*/
    if ( igsvgm() < 0 ) errmes();

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short IGsaln()

/*      Save everything as.
 *
 *      return: 0  = OK
 *          REJECT = Cancel
 *          GOMAIN = Main menu
 *
 *      Error: IG0342 = Illegal job name
 *             IG0422 = New name = current name
 *
 *      (C)microform ab 1998-09-16 J. Kjellander
 *
 *      2007-11-14 2.0, J.Kjellander
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
***Get new name.
*/
    status = IGssip(IGgtts(210),IGgtts(267),newnam,newnam,JNLGTH);
    if ( status < 0 ) return(status);
/*
***Is it valid ?
*/
   if ( igckjn(newnam) < 0 )
     {
     erpush("IG0342",newnam);
     errmes();
     goto exit;
     }
/*
***Is it different from the current ?
*/
    if ( strcmp(newnam,jobnam) == 0 )
      {
      erpush("IG0422",newnam);
      errmes();
      goto exit;
      }
/*
***Does a job with this name already exist ?
*/
    flag = FALSE;

    strcpy(path,jobdir);
    strcat(path,newnam);
    strcat(path,JOBEXT);
    if ( IGftst(path) ) flag = TRUE;

    if ( sysmode == GENERIC )
      {
      strcpy(path,jobdir);
      strcat(path,newnam);
      strcat(path,MODEXT);
      if ( IGftst(path) ) flag = TRUE;
      }

    strcpy(path,jobdir);
    strcat(path,newnam);
    strcat(path,RESEXT);
    if ( IGftst(path) ) flag = TRUE;

    if ( flag  &&  !IGialt(1626,67,68,TRUE) ) goto exit;
/*
***Save job file.
*/
    strcpy(tmpnam,jobnam);
    strcpy(jobnam,newnam);
    if ( igsvjb() < 0 ) errmes();
    strcpy(jobnam,tmpnam);
/*
***Save module.
*/
    if ( sysmode == GENERIC )
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
***Save DB.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));
/*
***Copy the saved DB file to a RES-file.
*/
    strcpy(resfil,jobdir);
    strcat(resfil,jobnam);
    strcat(resfil,RESEXT);

    strcpy(newfil,jobdir);
    strcat(newfil,newnam);
    strcat(newfil,RESEXT);

    if ( (status=IGfcpy(resfil,newfil)) < 0 ) return(status);
    else
      {
      if ( sysmode == EXPLICIT ) WPaddmess_mcwin(IGgtts(141),WP_MESSAGE);
      else                       WPaddmess_mcwin(IGgtts(217),WP_MESSAGE);
      }
/*
***Load DB again.
*/
    DBload(resfil,sysize.gm,
           DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
    WPaddmess_mcwin(IGgtts(196),WP_MESSAGE);
/*
***The end.
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
***Läs in nytt filnamn.
*/
    if ( (status=IGssip(IGgtts(349),IGgtts(267),newnam,jobnam,JNLGTH)) < 0 )
        goto exit;
/*
***Ändra namnet i modulhuvudet.
*/
    pmrmod(&modhed);
    strcpy(modhed.mname,newnam);
    pmumod(&modhed);
/*
***Lagra modul.
*/
    if ( igsvmo() < 0 ) errmes();
/*
***Ändra tillbaks namnet i modulhuvudet.
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

/*      Save DB as.
 *
 *      Return: 0  = OK
 *          REJECT = Cancel
 *          GOMAIN = Main menu
 *
 *      Error: IG0082 = New name = current
 *
 *      (C)microform ab 30/7/85 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      2/2/93   copy på VAX, J. Kjellander
 *      2007-11-14 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    char    resfil[V3PTHLEN+1];
    char    newfil[V3PTHLEN+1];
    char    newnam[JNLGTH+1];

/*
***Läs in nytt filnamn.
*/
loop:
    status = IGssip(IGgtts(279),IGgtts(267),newnam,"",JNLGTH);
    if ( status < 0 ) return(status);
/*
***Check that new name <> current.
*/
    if ( strcmp(newnam,jobnam) == 0 )
      {
      erpush("IG0082","");
      errmes();
      goto loop;
      }
/*
***Save DB.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));
/*
***Copy DB file to new name.
*/
    else
      {
      strcpy(resfil,jobdir);
      strcat(resfil,jobnam);
      strcat(resfil,RESEXT);

      strcpy(newfil,jobdir);
      strcat(newfil,newnam);
      strcat(newfil,RESEXT);

      if ( (status=IGfcpy(resfil,newfil)) < 0 )
        return(status);
      else
        {
        if ( sysmode == EXPLICIT ) WPaddmess_mcwin(IGgtts(141),WP_MESSAGE);
        else                       WPaddmess_mcwin(IGgtts(217),WP_MESSAGE);
        }
      }
/*
***Load DB again.
*/
    DBload(resfil,sysize.gm,
           DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
/*
***The end.
*/
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
***Läs in nytt filnamn.
*/
    if ( (status=IGssip(IGgtts(357),IGgtts(267),newnam,jobnam,JNLGTH)) < 0 )
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

/*      Ändra modulens attribut.
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
***Ta reda på nytt attribut.
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

      default:                           /* Okänt alt. */
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

/*      Avslutningsrutin. Används av toppnivån (futab4)
 *      och trap-rutiner för att återvända till OS.
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
***Stäng surpac.
*/
   suexit();
/*
***Här stänger vi fönsterhanteringssystemet.
*/
#ifdef UNIX
   WPexit();
#endif

#ifdef WIN32
   msexit();
#endif
/*
***Stäng ev. debug-filer.
*/
   dbgexi();
/*
***Återvänd till OS.
*/
#ifdef WIN32
   ExitProcess(0);
#else
   exit(V3EXOK);
#endif
/*
***För att slippa kompileringsvarning.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGexit_sn()

/*      Exit and save nothing.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      10/2/86  gpexit, J. Kjellander
 *      13/4/86  IGexit, J. Kjellander
 *      29/11/88 Temporär .RIT-fil, J. Kjellander
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

   if ( sysmode == EXPLICIT ) IGfdel(tmprit);
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

        short IGexit_sa()

/*      Exit and save all. f122.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      10/2/86  gpexit, J. Kjellander
 *      13/4/86  IGexit, J. Kjellander
 *      26/9/95  jnflag, J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *      2007-11-18 2.0, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Run optional exit_macro.
*/
   exit_macro();
/*
***Save all.
*/
   IGsjpg();
/*
***Clear graphics.
*/
   WPclrg();
/*
***The end.
*/
   return(EXIT);
  }

/********************************************************/
/*!******************************************************/

        short IGexsd()

/*      Sluta och lagra allt, även dekompilerad
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
    else return(IGexit_sa());
  }

/********************************************************/
/*!******************************************************/

        short IGnjsd()

/*      Nytt jobb och lagra allt, även dekompilerad
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

/*      Save all and start/load new job.
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
 *      2007-11-18 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    char   newnam[JNLGTH+1];
    char   oldnam[JNLGTH+1];

/*
***Get new job name.
*/
   status = IGselj(newnam);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***Run optional exit_macro.
*/
    exit_macro();
/*
***Save all.
*/
    IGsjpg();
/*
***Swap jobname.
*/
    strcpy(oldnam,jobnam);
    strcpy(jobnam,newnam);
/*
***Try to load the new job. If not successful,
***load old job again.
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
***New job loaded. If this function was called through the menu
***handler return(GOMAIN) is enough to display the main menu
***but if the function is called by a button GOMAIN
***will not be propagated back all the way (because buttons can't
***return that information) so to be sure we fix it by activating
***the main menu here.
*/
    {
    WPactivate_menu(&mnutab[IGgmmu()]);
    return(GOMAIN);
    }
  }

/********************************************************/
/*!******************************************************/

        short IGsave_all()

/*      Save all.
 *
 *      Return: 0  = OK
 *          REJECT = Cancel
 *          GOMAIN = Main menu
 *
 *      (C)microform ab 28/9/95 J. Kjellander
 *
 *      1998-02-06 WIN32, J.Kjellander
 *      2007-11-14 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
   char resfil[V3PTHLEN+1];

/*
***Save the JOB-file.
*/
   if ( igsvjb() < 0 ) errmes();
/*
***Optionally also a MBO-file.
*/
   if ( sysmode & GENERIC )
     {
     if ( igsvmo() < 0 ) errmes();
     }
/*
***In explicit mode also the RES-file.
*/
   if ( sysmode == EXPLICIT )
     {
     if ( DBexit() < 0 )
       {
       erpush("IG0193",jobnam);
       errmes();
       return(0);
       }

     strcpy(resfil,jobdir);
     strcat(resfil,jobnam);
     strcat(resfil,RESEXT);

     if ( IGfcpy(tmprit,resfil) < 0 )
       {
       errmes();
       return(0);
       }
/*
***Load DB again.
*/
     if ( DBload(tmprit,sysize.gm,
          DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL) < 0 ) errmes();
     }
   WPaddmess_mcwin(IGgtts(196),WP_MESSAGE);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGnjsn()

/*      Save nothing and load/start new job.
 *
 *      Return: 0  = OK
 *          REJECT = Operation cancelled
 *          GOMAIN = Main menu
 *
 *      (C)microform ab 6/10/86 J. Kjellander
 *
 *      8/5/87   Defaultsträng, J. Kjellander
 *      26/9/95  tmprit, J. Kjellander
 *      26/9/95  IGselj(), J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *      2007-11-18 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    char   newnam[JNLGTH+1];
    char   resfil[V3PTHLEN+1];

/*
***Get new jobname.
*/
   status = IGselj(newnam);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***Run optional exit_macro.
*/
    exit_macro();
/*
***Save nothing.
*/
   gmclpf();

   if ( sysmode & GENERIC )
     {
     strcpy(resfil,jobdir);
     strcat(resfil,jobnam);
     strcat(resfil,RESEXT);
     IGfdel(resfil);
     }
   else IGfdel(tmprit);
/*
***Update jobnam.
*/
    strcpy(jobnam,newnam);
/*
***Load new job.
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

/*      Select a new job (and jobdir).
 *
 *      Out: *newjob = New job name.
 *
 *      Return: 0      = Ok.
 *              REJECT = Cancel.
 *
 *      Error: IG0342 = Illegal job name.
 *
 *      (C)2007-11-20 J.Kjellander
 *
 ******************************************************!*/

  {
   short status,oldafu;
   char  typ[5],filter[6],newdir[V3PTHLEN];
   int   i;

/*
***Filter.
*/
   if ( sysmode & GENERIC ) strcpy(typ,MODEXT);
   else                     strcpy(typ,RESEXT);
/*
***Set active function.
*/
   oldafu = actfun;
   actfun = 1001;
/*
***Call the file selector. On return we have a filename with
***extension, XXX.MBO or XXX-RES and jobdir may have changed.
*/
    strcpy(newdir,jobdir);
    strcpy(filter,"*");
    strcat(filter,typ);
    status = WPfile_selector(IGgtts(210),newdir,FALSE,"",filter,FALSE,newjob);
    if ( status == 0 )
      {
      i = strlen(newjob) - 4;
      newjob[i] = '\0';
      if ( igckjn(newjob) < 0 ) return(erpush("IG0342",newjob));
      strcpy(jobdir,newdir);
      }
/*
***Reset active function.
*/
   actfun = oldafu;
/*
***The end.
*/
   return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGchjn(char *newnam)

/*      Change the name of the active job.
 *
 *      In: newnam = New jobname.
 *
 *      Error: IG0342 = Illegal jobname
 *             IG0422 = Job already exists
 *
 *      (C)microform ab 26/9/95 J. Kjellander
 *
 ******************************************************!*/

  {
    char       oldres[V3PTHLEN+1],newres[V3PTHLEN+1],
               filnam[V3PTHLEN+1],templ[JNLGTH+10];
    PMMODULE   modhed;

/*
***Check that the new name is valid.
*/
    if ( igckjn(newnam) < 0 ) return(erpush("IG0342",newnam));
/*
***Check that a job with this name does not exist.
*/
    if ( sysmode & GENERIC )
      {
      strcpy(filnam,jobdir);
      strcat(filnam,newnam);
      strcat(filnam,MODEXT);
      if ( IGftst(filnam) ) return(erpush("IG0422",newnam));
      }
    else
      {
      strcpy(filnam,jobdir);
      strcat(filnam,newnam);
      strcat(filnam,RESEXT);
      if ( IGftst(filnam) ) return(erpush("IG0422",newnam));
      }
/*
***Rename active module and RES-file.
*/
    if ( sysmode & GENERIC )
      {
      pmrmod(&modhed);
      strcpy(modhed.mname,newnam);
      pmumod(&modhed);

      strcpy(oldres,jobdir);
      strcat(oldres,jobnam);
      strcat(oldres,RESEXT);
      strcpy(newres,jobdir);
      strcat(newres,newnam);
      strcat(newres,RESEXT);
      IGfmov(oldres,newres);
      }
/*
***In explicit mode it's enough to rename the temp-file.
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
***Save the new job name.
*/
   strcpy(jobnam,newnam);
/*
***Update window border.
*/
   WPupwb(NULL);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool IGgrst(
        char *resurs,
        char *pvalue)

/*      OS-oberoende version av WPgrst/msgrst. Returnerar
 *      värdet på en resurs om den är definierad. 
 *
 *      In: resurs = Resursnamn utan "varkon."
 *          pvalue = Pekare till utdata eller NULL.
 *
 *      Ut: *pvalue = Resursvärde om tillgängligt.
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
***men med ordet varkon framför.
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
***Skall vi returnera resursvärde ?
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

/*      Kör macro när jobb laddas oavsett om det är
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

/*      Kör macro bara när nytt jobb skapas.
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

/*      Kör macro vid avslut.
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
 *      22/10/85 Släck ej def. nivåer, J. Kjellander
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      5/3/88   stalev, J. Kjellander
 *      15/11/88 posmod, J. Kjellander
 *      31/1/95  Multifönster, J. Kjellander
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
    relpos = FALSE;
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

static short load_jobdata()

/*      Load jobdata.
 *
 *      FV:   0 = Ok.
 *           -1 = Filen finns ej.
 *          < -1 = Status från EXldjb()
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      16/4/86  Bytt geo607 mot 612, J. Kjellander
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      7/11/86  V1.3, J. Kjellander
 *      26/12/86 hit och save, J. Kjellander
 *      15/11/88 EXldjb() och posmod, J. Kjellander
 *      30/1-95  Multifönster, J. Kjellander
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
***Kör vi X11 och inga fönster har skapats fanns det inga
***fönster i jobfilen. Då skapar vi default fönster enl.
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
***Jobbfil inläst.
*/
   WPaddmess_mcwin(IGgtts(212),WP_MESSAGE);

   return(0);
  }

/********************************************************/
