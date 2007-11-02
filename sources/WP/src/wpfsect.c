/**********************************************************************
*
*    wpfsect.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPfile_selector(); Get filename selector dialog
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
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"
#include "../include/WP.h"
#include <math.h>

#define N_LINES              10    /* Number of lines in list area */
#define MAX_COLS            500    /* Max number of columns */
#define MAX_FILES          5000    /* Max number of files */
#define FNBUF_SIZE 20*MAX_FILES    /* Size of filenamebuffer */

/*
***Static variables (used by callback).
*/
static char  **altlst;
static int     ncols_tot,nbuts,list_x,list_y,
               list_dx,list_dy,air1,air2,air3,altlen,alth,
               n_alts,butsiz,colwdts[MAX_COLS],
               colw_tot;
static bool    sbar;
static DBint   iwin_id,alt_id[WP_IWSMAX];
static WPSBAR *sbptr;

/*
***Prototypes for internal functions.
*/
static bool fssb_callback(WPIWIN *iwinpt);
static void listarea_layout();
static bool clip_textbutton(int *pos,int lim1,int lim2,char *text);

/********************************************************/

     short WPfile_selector(
     char *title,
     char *def_path,
     bool  path_edit,
     char *def_file,
     char *def_filter,
     bool  filter_edit,
     char *outfile)

/*   A file selector under development. Currently
 *   used as a replacement for WPilse() only.
 *
 *   In:   title       = Window title/prompt
 *         def_path    = Initial path
 *         path_edit   = True if path may be edited
 *         def_file    = Default filename or ""
 *         def_filter  = Initial filter
 *         filter_edit = True if filter may be edited
 *
 *   Out: *outfile = The file name chosen (or entered).
 *
 *   Return:  0 = Ok.
 *       REJECT = Cancel.
 *
 *   (C)2007-10-31 J.Kjellander
 *
 *******************************************************!*/

  {
   char     file[81],filett[81],filter[81],filtertt[81],
            okey[81],okeytt[81],reject[81],rejecttt[81],
            help[81],helptt[81],pathtt[81];
   char    *fnptrs[MAX_FILES],fnbuf[FNBUF_SIZE];
   int      iwin_x,iwin_y,iwin_dx,iwin_dy,i,scr_width,
            scr_height,pmtlen,butlen,buth,edtlen,edth,
            alt_x,alt_y,x1,y1,x2,y2;
   short    status;
   DBint    but_id,okey_id,help_id,reject_id,path_id,file_id,
            filter_id,pmt_id,sb_id,up_id;
   unsigned int dum1,dum2;
   char    *type[20];
   XrmValue value;
   WPIWIN  *iwinpt;
   WPBUTT  *butptr;
   WPEDIT  *edtptr;

/*
***Init static variables.
*/
   altlst = fnptrs;
/*
***Get initial file list from def_path.
*/
   EXdirl(def_path,def_filter,MAX_FILES,FNBUF_SIZE,fnptrs,fnbuf,&n_alts);
/*
***Texts from the ini-file.
*/
   if ( !WPgrst("varkon.input.path.tooltip",pathtt) )     strcpy(pathtt,"pathjk");
   if ( !WPgrst("varkon.input.file",file) )               strcpy(file,"File:");
   if ( !WPgrst("varkon.input.file.tooltip",filett) )     strcpy(filett,"filejk");
   if ( !WPgrst("varkon.input.filter",filter) )           strcpy(filter,"Filter:");
   if ( !WPgrst("varkon.input.filter.tooltip",filtertt) ) strcpy(filtertt,"filterjk");
   if ( !WPgrst("varkon.input.okey",okey) )               strcpy(okey,"Okej");
   if ( !WPgrst("varkon.input.okey.tooltip",okeytt) )     strcpy(okeytt,"");
   if ( !WPgrst("varkon.input.reject",reject) )           strcpy(reject,"Avbryt");
   if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) ) strcpy(rejecttt,"");
   if ( !WPgrst("varkon.input.help",help) )               strcpy(help,"Hjälp");
   if ( !WPgrst("varkon.input.help.tooltip",helptt) )     strcpy(helptt,"");
/*
***Window position.
*/
    iwin_x = iwin_y = 20;

    if ( XrmGetResource(xresDB,"varkon.input.geometry",
                               "Varkon.input.Geometry",type,&value) )
      XParseGeometry((char *)value.addr,&iwin_x,&iwin_y,&dum1,&dum2);
/*
***Air1 = Outer air.
***Air2 = Inner air.
***Air3 = List area air.
*/
   air1 = 0.6*WPstrh();
   air2 = 1.0*WPstrh();
   air3 = 0.3*WPstrh();
/*
***Edit length and height.
*/
   edtlen = 40*WPstrl("A");
   edth   = 1.6*WPstrh();
/*
***Calculate column positions in list area etc.
*/
   listarea_layout();
/*
***Height of list area (without optional slidebar).
*/
   alth    = WPstrh();
   list_dy = N_LINES*(alth + air3) + air3;
/*
***How long is the longest of the two propmts (File:/Filter:) ?
*/
   pmtlen = 0;
   if ( WPstrl(file)   > pmtlen ) pmtlen = WPstrl(file);
   if ( WPstrl(filter) > pmtlen ) pmtlen = WPstrl(filter);
/*
***How long is the longest of Ok, Reject and Help (butlen) ?
*/
   butlen = 0;
   if ( WPstrl(okey)   > butlen ) butlen = WPstrl(okey);
   if ( WPstrl(reject) > butlen ) butlen = WPstrl(reject);
   if ( WPstrl(help)   > butlen ) butlen = WPstrl(help);
   butlen *= 1.8;
   buth    = 2*WPstrh();
/*
***The size of the screen.
*/
   scr_width  = DisplayWidth(xdisp,xscr);
   scr_height = DisplayHeight(xdisp,xscr);
/*
***WPIWIN width, iwin_dx, the longest of a prompt + an edit
***or the three buttons Ok + Cancel + Help.
*/
   iwin_dx = air1 + pmtlen + air1 + edtlen + air1;
   if ( iwin_dx < (3*butlen + 4*air1) ) iwin_dx = 3*butlen + 4*air1;
/*
***Now that we know the width of the WPIWIN we can
***calculate the width of list area.
*/
   list_dx = iwin_dx - 2*air1;
/*
***Is a slidebar needed ?
*/
   if ( colw_tot > list_dx ) sbar = TRUE;
   else                      sbar = FALSE;

   if ( sbar ) list_dy += WPstrh();
/*
***Now that we know if a slidebar is needed
***we can calculate the height of the WPIWIN, iwin_dy.
*/
   iwin_dy = air1 +         /* Air */
             edth +         /* Path edit */
             air2 +         /* Air */
             list_dy +      /* List alternatives */
             air2 +         /* Air */
             edth +         /* File name edit */
             air2 +         /* Air */
             edth +         /* Filter edit */
             air2 +         /* Air over line */
             air2 +         /* Air under line */
             buth +         /* Ok, Cancel and Help */
             air1;          /* Air */
/*
***Create the WPIWIN window.
*/
   WPposw(iwin_x,iwin_y,iwin_dx+10,iwin_dy+25,&iwin_x,&iwin_y);

   WPwciw((short)iwin_x,(short)iwin_y,(short)iwin_dx,(short)iwin_dy,title,&iwin_id);
   iwinpt = (WPIWIN *)wpwtab[(wpw_id)iwin_id].ptr;
/*
***The up button.
*/
   alt_x  = air1;
   alt_y  = air1;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(" Up "),buth,(short)2,
                           "Up","Up","",WP_BGND2,WP_FGND,&up_id);
/*
***The path edit.
*/
   alt_x  = air1 + pmtlen + air1;
   alt_y  = air1;
   WPmced((wpw_id)iwin_id,alt_x,alt_y,edtlen,edth,(short)1,
                   def_path,V3PTHLEN,&path_id);
   edtptr = (WPEDIT *)iwinpt->wintab[path_id].ptr;
   strcpy(edtptr->tt_str,pathtt);
/*
***The list area outline.
*/
   list_x = alt_x = air1;
   list_y = alt_y += edth + air2;

   x1 = alt_x;
   y1 = alt_y;
   x2 = alt_x + list_dx;
   y2 = y1;
   WPcreate_3Dline(iwin_id,x1,y1,x2,y2);

   y1 = alt_y + list_dy;
   y2 = y1;
   WPcreate_3Dline(iwin_id,x1,y1,x2,y2);

   x2 = x1;
   y2 = alt_y;
   WPcreate_3Dline(iwin_id,x1,y1,x2,y2);

   x1 = alt_x + list_dx;
   x2 = x1;
   WPcreate_3Dline(iwin_id,x1,y1,x2,y2);
/*
***Optional list area slidebar.
*/
   if ( sbar )
     {
     alt_x = air1 + 1;
     butsiz = ((double)(list_dx)/(double)(colw_tot)*list_dx);
     WPcreate_slidebar(iwin_id,alt_x,alt_y+list_dy-WPstrh(),list_dx-2,WPstrh(),
                       0,butsiz,WP_SBARH,&sbptr);
     sb_id = sbptr->id.w_id;
     sbptr->cback = fssb_callback;
     }
/*
***List area contents.
*/
   nbuts = 0;
   fssb_callback(iwinpt);
/*
***The file edit with prompt.
*/
   alt_x  = air1;
   alt_y += list_dy + air2;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(file),edth,file,&pmt_id);

   alt_x  = air1 + pmtlen + air1;
   WPmced((wpw_id)iwin_id,alt_x,alt_y,edtlen,edth,(short)1,
                   def_file,JNLGTH,&file_id);
   edtptr = (WPEDIT *)iwinpt->wintab[file_id].ptr;
   strcpy(edtptr->tt_str,filett);
/*
***The filter edit with prompt.
*/
   alt_x  = air1;
   alt_y += edth + air2;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(filter),edth,filter,&pmt_id);

   alt_x  = air1 + pmtlen + air1;
   WPmced((wpw_id)iwin_id,alt_x,alt_y,edtlen,edth,(short)1,
                   def_filter,JNLGTH,&filter_id);
   edtptr = (WPEDIT *)iwinpt->wintab[filter_id].ptr;
   strcpy(edtptr->tt_str,filtertt);
/*
***A 3D line.
*/
   alt_x  = iwin_dx/8;
   alt_y += edth + air2;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,7*iwin_dx/8,alt_y);
/*
***Ok, reject and help.
*/
   alt_x  = air1;
   alt_y += air2;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen,buth,(short)2,
                           okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   alt_x  = air1 + butlen + air1;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen,buth,(short)2,
                           reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   alt_x  = iwin_dx - air1 - butlen;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen,buth,(short)2,
                           help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Display.
*/
   WPwshw(iwin_id);
/*
***Wait for action.
*/
   status = 0;
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Using the up button is only allowed if
***path_edit = TRUE.
*/
   if ( but_id == up_id )
     {
     if ( path_edit )
       {
       goto loop;
       }
     else
       {
       WPbell();
       goto loop;
       }
     }
/*
***Action in the path edit is only allowed if
***path_edit = TRUE.
*/
   else if ( but_id == path_id )
     {
     if ( path_edit )
       {
       goto loop;
       }
     else
       {
       WPbell();
       goto loop;
       }
     }
/*
***Action in the file edit. We don't like an empty edit.
*/
   else if ( but_id == file_id )
     {
     WPgted(iwin_id,file_id,outfile);
     if ( outfile[0] == '\0' )
       {
       XBell(xdisp,100);
       goto loop;
       }
     else goto exit;
     }
/*
***Action in the filter edit is only allowed if
***filter_edit = TRUE.
*/
   else if ( but_id == filter_id )
     {
     if ( filter_edit )
       {
       goto loop;
       }
     else
       {
       WPbell();
       goto loop;
       }
     }
/*
***ButtonRelease in a slidebar. All slidebar actions
***are currently served by the callback.
*/
   else if ( sbar && (but_id == sb_id) )
     {
     goto loop;
     }
/*
***Okey, return contents of file edit.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,file_id,outfile);
     if ( outfile[0] == '\0' )
       {
       XBell(xdisp,100);
       goto loop;
       }
     else goto exit;
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     status = REJECT;
     goto exit;
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***An alternative selected.
*/
   else
     {
     for ( i=0; i<nbuts; ++i )
       {
       if ( but_id == alt_id[i] )
         {
         butptr = (WPBUTT *)iwinpt->wintab[but_id].ptr;
         strcpy(outfile,butptr->stron);
         goto exit;
         }
       }
     goto loop;
     }
/*
***The end.
*/
exit:
   WPwdel(iwin_id);
   return(status);
 }

/********************************************************/
/********************************************************/

 static bool fssb_callback(WPIWIN *iwinpt)

/*   Callback for the file selector slidebar. Updates the
 *   contents of the list area.
 *
 *   In:   iwinpt = C ptr to the file selector WPIWIN.
 *
 *   Return: Always TRUE.
 *
 *   (C)2007-10-31 J.Kjellander
 *
 *********************************************************/

  {
   int     i,j,x,y,tmpx,scroll,alt;
   char    tmpstr[V3STRLEN];
   WPBUTT *butptr;

/*
***Delete all currently existing text buttons.
*/
   for ( i=0; i<nbuts; ++i )
     {
     butptr = (WPBUTT *)iwinpt->wintab[alt_id[i]].ptr;
     XDestroyWindow(xdisp,butptr->id.x_id);
     WPdlbu(butptr);
     iwinpt->wintab[alt_id[i]].ptr = NULL;
     iwinpt->wintab[alt_id[i]].typ = TYP_UNDEF;
     }
/*
***Calculate how much to scroll left in pixels.
***
*/
   if ( sbar )
     {
     butsiz = sbptr->butend - sbptr->butstart;
     scroll = ((double)sbptr->butstart/((double)(sbptr->geo.dx - butsiz)))*
               (colw_tot - list_dx);
     }
   else scroll = 0;
/*
***Create new buttons.
*/
   nbuts = 0;

   for ( i=0; i<ncols_tot; i++ )
     {
     x = list_x + air1 - scroll;
     for ( j=0; j<i; j++ ) x += colwdts[j] + 2*air1;

     for ( j=0; j<N_LINES; j++ )
       {
       alt = i*N_LINES + j;
       if ( alt < n_alts )
         {
         strcpy(tmpstr,altlst[alt]);
         tmpx = x;
         if ( clip_textbutton(&tmpx,list_x+1,list_x+list_dx-1,tmpstr) )
           {
           y = list_y + air3 + j*(alth + air3);
           WPcrtb((wpw_id)iwin_id,tmpx,y,tmpstr,&alt_id[nbuts]);
         ++nbuts;
           }
         }
       }
     }
/*
***Expose.
*/
   WPxpiw(iwinpt);
/*
***The end.
*/
   return(TRUE);
  }

/**********************************************************/
/**********************************************************/

 static void listarea_layout()

/*   Measures lengths of alternative texts and calulates
 *   colwdts[] and colw_tot.
 *
 *   (C)2007-10-31 J.Kjellander
 *
 **********************************************************/

  {
   int i,start,stop,butlen,maxlen,col;
/*
***Longest alternative length, altlen ?
*/
   for ( altlen=1,i=0; i<n_alts; ++i )
     {
     if ( WPstrl(altlst[i]) > altlen ) altlen = WPstrl(altlst[i]);
     }
/*
***How many columns will we have ?
***Not more than MAX_COLS !
*/
   ncols_tot = (int)(ceil((n_alts/(double)N_LINES)));
   if ( ncols_tot > MAX_COLS )
     {
     ncols_tot = MAX_COLS;
     n_alts    = ncols_tot*N_LINES;
     }
/*
***Calculate individual column widths.
*/
   i     = 0;
   col   = 0;
   start = 0;
   if ( n_alts < N_LINES ) stop = n_alts;
   else                    stop = N_LINES;

loop:
   maxlen = 1;
   for ( i=start; i<stop; ++i )
     {
     butlen = WPstrl(altlst[i]);
     if ( butlen > maxlen ) maxlen = butlen;
     }
   colwdts[col++] = maxlen;
   start = stop;
   stop += N_LINES;
   if ( stop > n_alts ) stop = n_alts;
   if ( start < stop ) goto loop;
/*
***Calculate total list width.
*/
   colw_tot = air1;

   for ( i=0; i<ncols_tot; ++i )
     {
     colw_tot += colwdts[i] + 2*air1;
     }
/*
***The end.
*/
   return;
  }

/**********************************************************/
/**********************************************************/

static bool  clip_textbutton(
       int  *pos,
       int   lim1,
       int   lim2,
       char *text)

/*   Clips text buttons to the borders of the list area.
 *
 *   In:  pos  = Proposed position
 *        lim1 = Left border
 *        lim2 = Right border
 *        text = Porposed string
 *
 *   Out: *pos  = Adjusted position (if clipped)
 *        *text = Clipped text
 *
 *   Return: TRUE  = Text is visible and possibly clipped
 *           FALSE = No part of text visible
 *
 *   (C)2007-11-01 J.Kjellander
 *
 **********************************************************/

  {
   int  tl,cl,nc,nclip,clipx;
   char tmpstr[V3STRLEN];
/*
***Inits.
*/
   strcpy(tmpstr,text);

   tl = WPstrl(tmpstr);
   cl = WPstrl("A");
   nc = strlen(tmpstr);
/*
***Is pos left of lim1 ?
*/
   if ( *pos < lim1 )
     {
/*
***Is the entire button left of lim1 ?
*/
     if ( *pos + tl <= lim1 ) return(FALSE);
/*
***No only part of button is left of lim1. Clip needed.
*/
     else
       {
       clipx = lim1 - *pos;
       nclip = floor(((double)clipx/(double)cl) + 0.5) + 1;
       if ( nclip < nc )
         {
         strcpy(text,&tmpstr[nclip]);
        *pos = lim1;
         return(TRUE);
         }
       else return(FALSE);
       }
     }
/*
***pos is not left of lim1. Is entire button right of lim2 ?
*/
   else if ( *pos >= lim2 )
     {
     return(FALSE);
     }
/*
***No, if part of the button is right of lim2, clip.
*/
   else if ( *pos + tl > lim2 )
     {
     clipx = *pos + tl - lim2;
     nclip = floor(((double)clipx/(double)cl) + 0.5) + 1;
     if ( nclip < nc )
       {
       text[nc-nclip] = '\0';
       return(TRUE);
       }
     else return(FALSE);
     }
/*
***No, then the button must be entirely between lim1 and lim2.
*/
   return(TRUE);
  }

/**********************************************************/

