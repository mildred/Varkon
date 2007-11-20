/*!******************************************************************/
/*  igenvpath.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGfopr();     Open file with path =  path;path...               */
/*  IGtrfp();     Process $environment in path                      */
/* *IGgenv();     VARKON specific env-paths                         */
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
#include "../include/IG.h"
#include <time.h>
#include <string.h>
#include <fcntl.h>

#ifdef WIN32
#include <io.h>
#endif

/*!******************************************************/

        int   IGfopr(
        char *path,
        char *fil,
        char *ext)

/*      �ppnar en fil f�r l�sning. Path kan ges p� formen
 *      path1;path2 osv. i max 10 niv�er om max V3PTHLEN
 *      tecken.
 *
 *      In: path => Pekare till v�gbeskrivning.
 *          fil  => Pekare till filnamn.
 *          ext  => Pekare till extension.
 *
 *      Ut: Inget.
 *
 *      FV: Filedescriptor eller < 0 om �ppning misslyckats.
 *
 *      (C)microform ab 28/3/89 J. Kjellander
 *
 *      5/5/92   Plustecken p� VAX/VMS, J. Kjellander
 *      6/11/95  WIN32, J. Kjellander
 *      19/1/96  v3trfp(), J. Kjellander
 *      1997-01-08 :->;, J.Kjellander
 *
 ******************************************************!*/

  {
    char  *p1,*p2;
    char   fnam[V3PTHLEN+1];
    char   buf[10*V3PTHLEN+10];
    int    fd;

/*
***Lite initiering.
*/
    strcpy(buf,path);
    p1 = p2 = buf;
/*
***S�k upp '�0' eller semikolon i v�gbeskrivningen.
***Gamla PID-filer kan inneh�lla : i UNIX.
*/
loop:
#ifdef UNIX
    if ( *p2 == ';'  ||  *p2 == ':' )
#endif
#ifdef WIN32
    if ( *p2 == ';' )
#endif
      {
      *p2 = '\0';
      if ( *p1 != '\0' )
        {
        IGtrfp(p1,fnam);
#ifdef UNIX
        strcat(fnam,"/"); 
#endif
#ifdef WIN32
        strcat(fnam,"\\"); 
#endif
        strcat(fnam,fil); strcat(fnam,ext);
        }
      else
        {
        strcpy(fnam,fil);
        strcat(fnam,ext);
        }
#ifdef WIN32
      if ( (fd=open(fnam,O_BINARY | O_RDONLY)) < 0 ) 
#else
      if ( (fd=open(fnam,0)) < 0 ) 
#endif
        {
        ++p2;
        p1 = p2;
        goto loop;
        }
      else return(fd);
      }
/*
***Nu har vi kommit till sista alternativet.
*/
    else if ( *p2 == '\0' )
      {
      if ( *p1 != '\0' )
        {
        IGtrfp(p1,fnam);
#ifdef UNIX
        strcat(fnam,"/"); 
#endif
#ifdef WIN32
        strcat(fnam,"\\"); 
#endif
        strcat(fnam,fil);
        strcat(fnam,ext);
        }
      else
        {
        strcpy(fnam,fil);
        strcat(fnam,ext);
        }
#ifdef WIN32
      return(open(fnam,O_BINARY | O_RDONLY));
#else
      return(open(fnam,0));
#endif
      }
/*
***N�sta tecken.
*/
    else
      {
      ++p2;
      goto loop;
      }
  }

/********************************************************/
/*!******************************************************/

        void  IGtrfp(
        char *path1,
        char *path2)

/*      Translate file path. �vers�tter ev. $env i b�rjan
 *      p� en path till dess v�rde.
 *
 *      In:  path1  => Pekare till ej �versatt path.
 *
 *      Ut: *path2  => �versatt path.
 *
 *      (C)microform ab 23/3/95 J. Kjellander
 *
 *       2004-07-30 Slash/Backslash conversion, J.Kjellander
 *
 ******************************************************!*/

  {
   int   i,ntkn;
   char  envpar[V3STRLEN+1];


/*
***Orimliga indata tex. str�ngar med mindre �n 2 tecken
***bryr vi oss inte om.
*/
   ntkn = strlen(path1);

   if ( ntkn < 2 )
     {
     strcpy(path2,path1);
     return;
     }
/*
***Turn slashes/backslashes.
*/
#ifdef UNIX
     for ( i=0; i<ntkn; ++i ) if ( *(path1+i) == '\\' ) *(path1+i) = '/';
#endif
#ifdef WIN32
     for ( i=0; i<ntkn; ++i ) if ( *(path1+i) == '/' ) *(path1+i) = '\\';
#endif
/*
***Om pathen b�rjar med dollar plockar vi ut environment-
***parametern.
*/
   if ( *path1 == '$' )
     {
#ifdef UNIX
     for ( i=0; i<ntkn; ++i ) if ( *(path1+i) == '/' ) break;
#endif
#ifdef WIN32
     for ( i=0; i<ntkn; ++i ) if ( *(path1+i) == '\\' ) break;
#endif
     strncpy(envpar,path1+1,i-1);
     envpar[i-1] = '\0';
/*
***Sen provar vi att �vers�tta. Om parametern inte finns
***returnerar vi path1 som utdata. Annars den �versatta pathen.
*/
     if ( getenv(envpar) == NULL )
       {
       strcpy(path2,path1);
       }
     else
       {
       strcpy(path2,getenv(envpar));
       strcat(path2,(path1+i));
       }
     }
   else strcpy(path2,path1);
/*
***Slut.
*/
   return;
  }

/********************************************************/
/*!******************************************************/

        char *IGgenv(int envcode)

/*      Map Varkon environment code to actual text and
 *      add a trailing slash.
 *
 *      In: envcode = See IG/include/env.h
 *
 *      Return: NULL = No translation found.
 *              ptr  = C ptr to translation.
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      8/12/95    VARKON_TOL, J. Kjellander
 *      20/1/96    VARKON_SND, J. Kjellander
 *      1997-09-30 Ny defaulthantering, J.Kjellander
 *      2007-11-15 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
       char *envptr;
       int   ntkn;

static char  envbuf[V3PTHLEN+1];
static char *envtab[] = {"VARKON_ERM",
                         "VARKON_DOC",
                         "VARKON_PID",
                         "VARKON_MDF",
                         "VARKON_LIB",
                         "VARKON_TMP",
                         "VARKON_FNT",
                         "VARKON_ICO",
                         "VARKON_PLT",
                         "VARKON_TOL",
                         "VARKON_DJD",
                         "VARKON_SND"};

/*
***Check envcode validity.
*/
   if ( envcode < 0  || envcode > VARKON_SND ) return(NULL);
/*
***Translate.
*/
   if ( (envptr=getenv(envtab[envcode])) == NULL ) return(NULL);
/*
***Add trailing slash.
*/
   else
     {
     strcpy(envbuf,envptr);
     ntkn = strlen(envbuf);
#ifdef UNIX
     if ( envbuf[ntkn-1] != '/' ) strcat(envbuf,"/");
#endif

#ifdef WIN32
     if ( envbuf[ntkn-1] != '\\' ) strcat(envbuf,"\\");
#endif
/*
***The end.
*/
     return(envbuf);
     }
  }

/********************************************************/
