/*!******************************************************************/
/*  ighelp.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGhelp();    Interactive context sensitive help system          */
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
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern char  jobdir[],actpnm[];
extern short mstack[],mant;
extern int   actfunc;

/*
***Internal function prototypes.
*/
static short iglhlp(char *hlpnam);

/********************************************************/

        short IGhelp()

/*      Displays help on currently active function,
 *      menu or module. A help file name is formed
 *      from the menu number, module name or function
 *      number.
 *
 *      Global variable actfun = -1 => menu
 *                               -2 => module
 *                               >0 => function
 *
 *      (C)microform ab 13/10/86 J. Kjellander
 *
 *      1998-10-30 HTML, J.Kjellander
 *      2007-12-01 2.0, J.Kjellander
 *
 ********************************************************/

  {
    char  filnam[JNLGTH+5];
    short mnum;

/*
***A module is executing or being called, use module name.
*/
    if ( actfunc == -2 )
      {
      strcpy(filnam,actpnm);
      }
/*
***A menu is active, make name from menu number.
*/
    else if ( actfunc == -1 )
      {
      if ( (mnum=mstack[mant-1]) == 0 ) mnum = IGgmmu();
      filnam[0] = 'm';
      sprintf(&filnam[1],"%d",mnum);
      }
/*
***Function, make name from function number.
*/
    else
      {
      if      ( actfunc < 10  ) sprintf(filnam,"f00%d",actfunc);
      else if ( actfunc < 100 ) sprintf(filnam,"f0%d",actfunc);
      else                      sprintf(filnam,"f%d",actfunc);
      }
/*
***Display.
*/
    return(iglhlp(filnam));
  }

/********************************************************/
/********************************************************/

static short iglhlp(char *hlpnam)

/*      Display's a html help file.
 *
 *      In: hlpnam = File name.
 *                   partname, m+number or f+number
 *
 *      Felkoder: IG0202 = Can't find help file %s
 *
 *      (C)microform ab 13/10/86 J. Kjellander
 *
 *      5/11/86  GOMAIN, J. Kjellander
 *      15/2/95  VARKON_DOC, J. Kjellander
 *      1998-10-30 HTML, J.Kjellander
 *      1999-03-09 WIN32, J.Kjellander
 *      2007-12-01 2.0, J.Kjellander
 *
 ********************************************************/

  {
    char   filnam[V3PTHLEN+1];
    char   linbuf[V3STRLEN+1];
    char   htmcmd[V3STRLEN+1];
    char   oscmd[512];
    short  status;
    int    last;
    FILE  *hlpfil;

/*
***First of all check that the "html_viewer" resource
***is set. If not display the default help text file.
*/
      if ( !IGgrst("html_viewer",htmcmd) )
        {
        sprintf(filnam,"%sno_html_viewer.txt",IGgenv(VARKON_DOC));
        if ( IGfacc(filnam,'R') ) goto show;
        else return( erpush("IG0202",filnam));
        }
/*
***Global variable actfunc is used to determine what
***state the system is in. actfunc = 1 means that the
***system is starting and has not yet entered the main
***loop. During this time the WPselect_sysmode() dialog
***and the IGselect_job() dialog may be displayed and help
***requested from the user. This case is specially treated
***by displaying the top doc file $VARKON_DOC/man.htm.
***During startup jobdir is not yet defined.
*/
    if ( actfunc == 1 )
      {
      sprintf(filnam,"%sman.htm",IGgenv(VARKON_DOC));
      if ( IGfacc(filnam,'R') ) goto show;
      else return(erpush("IG0202",filnam));
      }
/*
***Where is the file ? First try on jobdir.
*/
    strcpy(filnam,jobdir);
    strcat(filnam,hlpnam);
    strcat(filnam,".htm");
    if ( IGfacc(filnam,'R') ) goto show;
    else                      erpush("IG0202",filnam);
/*
***The file was not found in jobdir.
***Try $VARKON_DOC.
*/
    sprintf(filnam,"%sv_man/%s.htm",IGgenv(VARKON_DOC),hlpnam);
    if ( IGfacc(filnam,'R') ) goto show;
    else                      erpush("IG0202",filnam);
/*
***Still no file found. Try with default filenames
***menudoc.htm/partdoc.htm on $VARKON_DOC
*/
    if ( actfunc < 0 )
      {
      if      ( actfunc == -1 ) strcpy(hlpnam,"menudoc");
      else if ( actfunc == -2 ) strcpy(hlpnam,"partdoc");

      sprintf(filnam,"%sv_man/%s.htm",IGgenv(VARKON_DOC),hlpnam);
      if ( IGfacc(filnam,'R') ) goto show;
      else                      erpush("IG0202",filnam);
      }
/*
***Nothing left to do but error message.
*/
    errmes();
    return(0);
/*
***A file is found. Display using html viewer if possible.
*/
show:
     erinit();

     if ( IGgrst("html_viewer",htmcmd) )
       {
       sprintf(oscmd,"(%s %s)&",htmcmd,filnam);
       EXos(oscmd,2);
       return(0);
       }
/*
***If htm viewer is not avalable, use a list window.
*/
    strcpy(linbuf,"Helpfile: ");
    strcat(linbuf,filnam);
    if ( (status=WPinla(linbuf)) < 0 ) goto end;
/*
***List the file. Strip newlines and returns.
*/
    hlpfil = fopen(filnam,"r"); 
    while(fgets(linbuf,V3STRLEN,hlpfil) != NULL)
      {
      last = strlen(linbuf) - 1;
      if ( linbuf[last] == '\n' )
        {
        linbuf[last] = '\0';
      --last;
        }
      if ( linbuf[last] == '\r' ) linbuf[last] = '\0';
      WPalla(linbuf,(short)1);
      }

    WPexla(TRUE);
/*
***The end.
*/
end:
    fclose(hlpfil);
    return(status);
  }

/********************************************************/
