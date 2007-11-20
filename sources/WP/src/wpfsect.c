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

/*
***For some reason popen and pclose dont seem to be defined
***in stdio.h as they should according to the C documentation.
***To avoid compilation warnings we define them here.
*/
extern FILE *popen(const char *command, const char *type);
extern int   pclose(FILE *stream);

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
static void path_up(char *oldpath,char *newpath);
static void make_filelist(char *inpath, char *pattern, int maxfiles,
                          int maxsize, char *fnptrs[], char *fnbuf, DBint *nf);

/********************************************************/

     short WPfile_selector(
     char *title,
     char *outpath,
     bool  path_edit,
     char *def_file,
     char *def_filter,
     bool  filter_edit,
     char *outfile)

/*   A file selector under development.
 *
 *   In:   title       = Window title/prompt
 *         outpath     = Initial path
 *         path_edit   = True if path may be edited
 *         def_file    = Default filename or ""
 *         def_filter  = Initial filter
 *         filter_edit = True if filter may be edited
 *
 *   Out: *outfile = The file selected
 *        *outpath = The path selected
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
            help[81],helptt[81],uptt[81];
   char    *fnptrs[MAX_FILES],fnbuf[FNBUF_SIZE],
            act_path[V3PTHLEN+1],new_path[V3PTHLEN+1];
   int      iwin_x,iwin_y,iwin_dx,iwin_dy,i,scr_width,
            scr_height,pmtlen,butlen,buth,edtlen,edth,
            alt_x,alt_y,x1,y1,x2,y2;
   short    status;
   DBint    but_id,okey_id,help_id,reject_id,path_id,file_id,
            filter_id,pmt_id,sb_id,up_id;
   unsigned int dum1,dum2;
   char    *type[20],iconam[V3PTHLEN+1];
   XrmValue value;
   WPIWIN  *iwinpt;
   WPICON  *icoptr;
   WPBUTT  *butptr;
   WPEDIT  *edtptr;

/*
***Init static variables.
*/
   altlst = fnptrs;
/*
***Texts from the ini-file.
*/
   if ( !WPgrst("varkon.input.up.tooltip",uptt) )         strcpy(uptt,"");
   if ( !WPgrst("varkon.input.file",file) )               strcpy(file,"File:");
   if ( !WPgrst("varkon.input.file.tooltip",filett) )     strcpy(filett,"file");
   if ( !WPgrst("varkon.input.filter",filter) )           strcpy(filter,"Filter:");
   if ( !WPgrst("varkon.input.filter.tooltip",filtertt) ) strcpy(filtertt,"filter");
   if ( !WPgrst("varkon.input.okey",okey) )               strcpy(okey,"Okey");
   if ( !WPgrst("varkon.input.okey.tooltip",okeytt) )     strcpy(okeytt,"");
   if ( !WPgrst("varkon.input.reject",reject) )           strcpy(reject,"Reject");
   if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) ) strcpy(rejecttt,"");
   if ( !WPgrst("varkon.input.help",help) )               strcpy(help,"Help");
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
***Get file list.
*/
   strcpy(act_path,outpath);
start:
   make_filelist(act_path,def_filter,MAX_FILES,FNBUF_SIZE,fnptrs,fnbuf,&n_alts);
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
             32   +         /* Up icon */
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
***The up icon.
*/
   alt_x  = air1;
   alt_y  = air1;

   strcpy(iconam,IGgenv(VARKON_ICO));
   strcat(iconam,"/Varkon_uparrow.xpm");
   WPmcic(iwin_id,alt_x,alt_y,1,iconam,WP_BGND1,WP_BGND1,&up_id);
   icoptr = (WPICON *)iwinpt->wintab[up_id].ptr;
   strcpy(icoptr->tt_str,uptt);
/*
***The path label.
*/
   alt_x  = air1 + 32 + air1 + air1;
   alt_y  = air1 + 16;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(act_path),WPstrh(),act_path,&path_id);
/*
***The list area outline.
*/
   list_x = alt_x = air1;
   list_y = alt_y += 16 + air2;

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
***List area background = white.
*/
   x1 = list_x + 1;
   y1 = list_y + 1;
   x2 = list_dx - 1;
   y2 = list_dy - 1;
   if ( sbar ) y2 -= WPstrh();
   WPcreate_fillrect(iwin_id,x1,y1,x2,y2,WPgcol(0));
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

   if ( filter_edit )
     {
     WPmced((wpw_id)iwin_id,alt_x,alt_y,edtlen,edth,(short)1,
                     def_filter,JNLGTH,&filter_id);
     edtptr = (WPEDIT *)iwinpt->wintab[filter_id].ptr;
     strcpy(edtptr->tt_str,filtertt);
     }
   else
     {
     WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(def_filter),edth,def_filter,&pmt_id);
     }
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
***WPwshw() gives focus to the first edit.
***Make sure that focus is on the file edit.
*/
   edtptr = (WPEDIT *)iwinpt->wintab[file_id].ptr;
   WPfoed(edtptr,TRUE);
/*
***Wait for action.
*/
   status = 0;
loop:
   WPwwtw(iwin_id,SLEVEL_NONE,&but_id);
/*
***Using the up button is only allowed if
***path_edit = TRUE.
*/
   if ( but_id == up_id )
     {
     WPwdel(iwin_id);
     path_up(act_path,new_path);
     strcpy(act_path,new_path);
     goto start;
     }
/*
***Action in the file edit. We don't like an empty edit.
*
   else if ( but_id == file_id )
     {
     WPgted(iwin_id,file_id,outfile);
     if ( outfile[0] == '\0' )
       {
       goto loop;
       }
     else goto exit;
     }
*
***Action in the filter edit is only allowed if
***filter_edit = TRUE.
*
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
*
***Okey.
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
         if ( outfile[strlen(outfile)-1] == '/' )
           {
           strcat(act_path,outfile);
           goto start;
           }
         strcpy(outpath,act_path);
         status = 0;
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
           butptr = (WPBUTT *)iwinpt->wintab[alt_id[nbuts]].ptr;
           butptr->color.bckgnd = WPgcol(0);
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
/**********************************************************/

static void  path_up(
       char *oldpath,
       char *newpath)

/*   Removes the last part of a path string so that
 *   the path to the parent directory is left.
 *
 *   In:  oldpath = Original path
 *
 *   Out: *newpath = Path to parent directory
 *
 *   (C)2007-11-19 J.Kjellander
 *
 **********************************************************/

  {
   int i;

/*
***Copy the original string.
*/
   strcpy(newpath,oldpath);
/*
***Calculate the index to the last character.
***If the string is empty something is probably wrong.
*/
   i = strlen(newpath);

   if      ( i == 0 )                        return;
   else if ( i == 1  &&  newpath[0] == '/' ) return;
   else                                    --i;
/*
***If the string ends with a slash, remove it.
*/
   if ( newpath[i] == '/' ) newpath[i] = '\0';
/*
***Look up last /.
*/
   while ( i > 0  &&  newpath[i] != '/' ) i--;
/*
***Strip everything after the /.
*/
   newpath[i+1] = '\0';
/*
***The end.
*/
   return;
  }

/**********************************************************/
/*!******************************************************/

 static void   make_filelist(
        char  *inpath,
        char  *pattern,
        int    maxfiles,
        int    maxsize,
        char  *fnptrs[],
        char  *fnbuf,
        DBint *nf)

/*      Create the current filelist.
 *
 *      In:
 *          inpath   = Directory path optionally with trailing /.
 *          pattern  = Search pattern for regular files.
 *          maxfiles = Max number of file names.
 *          maxsize  = Max number of chars.
 *          fnbuf    = Place to store file names.
 *
 *      Out:
 *          fnptrs = Array with nf ptrs to file names.
 *          nf     = Number of files.
 *
 *      (C)2007-11-19 J.Kjellander
 *
 ******************************************************!*/

  {
   char  lscmd[V3PTHLEN+1],path[V3PTHLEN+1],buf[V3PTHLEN+1];
   char *actptr;
   int   n,actsize,i;
   FILE *fp;

/*
***Init.
*/
   actptr  = fnbuf;
   actsize = 0;
  *nf      = 0;
   strcpy(path,inpath);
/*
***Strip trailing '/' or '.' but leave a single / for the root.
*/
   i = strlen(path);

   if ( i > 1 )
     {
     i = strlen(path) - 1;
     if ( path[i] == '/' ) path[i] = '\0';

     i = strlen(path) - 1;
     if ( path[i] == '.' ) path[i] = '\0';
     }
/*
***Create pipe command. -F adds slashes to directories.
*/
   if ( strlen(path) > 0 )
     {
     strcpy(lscmd,"cd ");
     strcat(lscmd,path);
     strcat(lscmd,";ls -F");
     }
   else
     {
     strcpy(lscmd,"ls -F");
     }
/*
***Execute the pipe and extract directories.
*/
   if ( (fp=popen(lscmd,"r")) == NULL ) return;
/*
***Read filenames.
*/
   while ( fgets(buf,V3PTHLEN,fp) != NULL  &&
                     *nf < maxfiles  &&  actsize+JNLGTH+5 < maxsize )
     {
     if ( (n=strlen(buf)) > 0 ) buf[n-1] = '\0';
/*
***Is the file a directory ?
*/
     if ( buf[strlen(buf)-1] == '/' )
       {
       strcpy(actptr,buf);
       fnptrs[*nf] = actptr;
       actptr  += strlen(buf)+1;
       actsize += strlen(buf)+1;
      *nf += 1;
       }
     }
   pclose(fp);
/*
***Execute the pipe again and extract ordinary files that match the filter pattern.
*/
   if ( (fp=popen(lscmd,"r")) == NULL ) return;
/*
***Read filenames.
*/
   while ( fgets(buf,V3PTHLEN,fp) != NULL  &&
                     *nf < maxfiles  &&  actsize+JNLGTH+5 < maxsize )
     {
     if ( (n=strlen(buf)) > 0 ) buf[n-1] = '\0';
/*
***Is the file a regular file ?
*/
     if ( buf[strlen(buf)-1] != '/' )
       {
       if ( IGcmpw(pattern,buf) )
         {
         strcpy(actptr,buf);
         fnptrs[*nf] = actptr;
         actptr  += strlen(buf)+1;
         actsize += strlen(buf)+1;
        *nf += 1;
         }
       }
     }
   pclose(fp);
/*
***The end.
*/
   return;
  }

/********************************************************/
