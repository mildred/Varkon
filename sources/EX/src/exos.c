/*!*****************************************************
*
*    exos.c
*    ======
*
*    EXos();      Interface routine for OS
*
*    This file is part of the VARKON Execute Library.
*    URL:  http://www.varkon.com
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
/*#include "../../GP/include/GP.h"*/
#include <string.h>

#ifdef UNIX

#include "../../WP/include/WP.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <process.h>
#endif

#include "../include/EX.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

#ifdef UNIX
/*!******************************************************/

       short   EXos(
       char    oscmd[],
       DBshort mode)

/*      Proceduren OS i UNIX-version.
 *
 *      In: oscmd => Kommandostr�ng.
 *          mode  => 0 = Asynkront (Batch) med wait
 *                   1 = Interaktivt
 *                   2 = Asynkront utan wait
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 22/11/85 J. Kjellander
 *
 *      21/10/86   wait(), J. Kjellander
 *      16/11/88   CGI, J. Kjellander
 *      3/10/91    mode, J. Kjellander
 *      15/4/92    Flyttat v3cmos() hit, J. Kjellander
 *      1996-02-21 WPwton(), J. Kjellander
 *
 ******************************************************!*/

  {
    int   system_stat;


#ifdef DEBUG
    int wait_stat;

    if ( dbglev(EXEPAC) == 50 )
      {
      fprintf(dbgfil(EXEPAC),"***Start-EXos***\n");
      fprintf(dbgfil(EXEPAC),"oscmd=%s\n",oscmd);
      fprintf(dbgfil(EXEPAC),"mode=%hd\n",mode);
      fflush(dbgfil(EXEPAC));
      }
#endif
/*
***Batch-mode med eller utan wait..
*/
    if ( mode != 1 )
      {
/*
***UNIX, system() g�r alltid wait() sj�lv !
***F�r att subprocesser inte skall avbrytas av WINPAC-
***klockan tex. under k�r aktiv modul st�nger vi av den
***under OS-anropet.
*/
#ifdef UNIX
      if ( WPwton() ) WPlset(FALSE);
#endif
      system_stat = system(oscmd);
#ifdef UNIX
      if ( WPwton()  ) WPlset(TRUE);
#endif

#ifdef DEBUGJK
    if ( dbglev(EXEPAC) == 50 )
      {
      pid_t pid;

      fprintf(dbgfil(EXEPAC),"system_stat=%d\n",system_stat);
      if ( system_stat == -1 )
        fprintf(dbgfil(EXEPAC),"errno=%d\n",errno);
      pid = wait(&wait_stat);
      fprintf(dbgfil(EXEPAC),"pid=%d\n",pid);
      fprintf(dbgfil(EXEPAC),"wait_stat=%d\n",wait_stat);
      if ( pid == -1 )
        fprintf(dbgfil(EXEPAC),"errno=%d\n",errno);
      else
        {
        if ( WIFEXITED(wait_stat) )
          fprintf(dbgfil(EXEPAC),"WIFEXITED = TRUE\n");
        if ( WIFSIGNALED(wait_stat) )
          fprintf(dbgfil(EXEPAC),"WIFSIGNALED = TRUE\n");
        }
      fflush(dbgfil(EXEPAC));
      if ( system_stat == -1 ) return(erpush("EX1662",oscmd));
      }
#endif
      }
/*
***Interaktiv mode, IGcmos() fixar sk�rmen och anropar sedan
***EXos() igen med mode = 0, dvs. batch med wait.
*/
    else IGcmos(oscmd);

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 50 )
      {
      fprintf(dbgfil(EXEPAC),"***Slut-EXos***\n\n");
      fflush(dbgfil(EXEPAC));
      }
#endif

    return(0);
  }
  
/********************************************************/
#endif

#ifdef WIN32
/*!******************************************************/

       short EXos(
       char    oscmd[],
       DBshort mode)

/*      Proceduren OS i WIN32-version.
 *
 *      In: oscmd => Kommandostr�ng.
 *          mode  => 0 = Asynkront (Batch) med wait
 *                   1 = Interaktivt
 *                   2 = Asynkront utan wait
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX2092 = Fel fr�n os, cmd=%s
 *
 *      (C)microform ab 1996-02-21 J. Kjellander
 *
 *      1997-02-03 _flushall(), J.Kjellander
 *      1997-05-20 COMMAND.COM bort,mode 1, J.Kjellander
 *      1998-03-27 WaitForSingleObject(), J.Kjellander
 *
 ******************************************************!*/

  {
   DWORD               create,errnum;
   char                errbuf[80];
   STARTUPINFO         si;
   PROCESS_INFORMATION pi;

/*
DWORD excode anv�nds ej fn.;
*/
/*
***Initiera startupinfo och processinfo.
*/
   memset(&si,0,sizeof(si));
   si.cb = sizeof(si);
/*
***T�m alla filbuffrar.
*/
   _flushall();
/*
***Om mode = 1 skall DOS-f�nster skapas.
*/
   if ( mode == 1 ) create = CREATE_NEW_CONSOLE;
   else             create = DETACHED_PROCESS;
/*
***Starta processen.
*/
   if ( !CreateProcess(NULL,
                       oscmd,
                       NULL,
                       NULL,
                       FALSE,
                       create,
                       NULL,
                       NULL,
                      &si,
                      &pi) )
      {
      errnum = GetLastError();
      sprintf(errbuf,"%d%%%s",errnum,oscmd);
      return(erpush("EX2092",errbuf));
      }
/*
***Enligt dok. b�r man st�nga handtagen prim�ra
***tr�den s� fort som m�jligt om det inte beh�vs.
*/
    CloseHandle(pi.hThread);
/*
***Om mode = 0 eller 1 skall vi v�nta tills subprocessen �r
***klar. Detta verkar funka olika i NT och 95. C:s wait funkar
***inte ! Ej heller cwait(). Bytt till WaitForSingleObject()
***1998-03-27, JK.
*/
    if ( mode != 2 ) WaitForSingleObject(pi.hProcess,INFINITE);
/*
***Nu kan vi st�nga �ven handtaget till sj�lva processen.
*/
    CloseHandle(pi.hProcess);
/*
***Det ska g� att f� reda p� Exit-status men det test
***jag gjorde mot TakCAD funkade inte. VisualBasic inblandat !
*
loop:
    if ( GetExitCodeProcess(pi.hProcess,&excode) )
      {
      if ( excode == STILL_ACTIVE )
        {
        goto loop;
        }
      }
    else
      {
      errnum = GetLastError();
      sprintf(errbuf,"%d%%%s",errnum,oscmd);
      return(erpush("EX2092",errbuf));
      }
*/

 
    return(0);
  }
  
/********************************************************/
#endif
