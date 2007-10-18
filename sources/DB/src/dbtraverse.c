/**********************************************************************
*
*    dbtraverse.c
*    ============
*
*
*    This file includes the following public functions:
*
*    DBset_root_id();    Set root in DB hiearchy
*    DBget_next_ptr();    Search/traverse for next entity
*
*
*    This file is part of the VARKON Database Library.
*    URL:  http://varkon.sourceforge.net
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

#include "../include/DB.h"
#include "../include/DBintern.h"

static DBetype  last_type =  0;    /* Sista storhets typ  */
static DBptr    last_part =  0;    /* LA f�r sista part */
static DBptr    last_la   =  0;    /* LA f�r sista storhet */
static DBptr    next_inst =  0;    /* LA f�r n�sta instans */
static DBptr    last_idt  =  0;    /* LA f�r sista ID-tab */
static DBseqnum last_id   =  0;    /* Sista ID */
static DBseqnum last_idm  =  0;    /* Sista ID-tab:s  storlek */
static DBptr    root      = -1;    /* Root-part, -1 = Hela GM */

/* Ovanst�ende 7 static-variabler h�ller reda p� var vi
   �r i GM med GET_NEXT_GM(). Genom att initiera dom till
   0 �r GM:s root satt till toppen redan fr�n b�rjan. */

static bool gmcmpw();  /* Compares strings with wildcards */

/*!******************************************************/

        DBstatus DBset_root_id(DBId *id)

/*      Servar MBS-rutinen SET_ROOT_GM().
 *
 *      In: id = Ny root-id eller #0 f�r hela GM. 
 *
 *      FV:  0  => Ok.
 *
 *      Felkoder: GM0152 = id finns ej.
 *                GM0162 = id=%s ej en part.
 *
 *      (C)microform ab 1996-02-14 J. Kjellander
 *
 ******************************************************!*/

  {
   DBptr   la;
   DBetype typ;
   char    errbuf[V3STRLEN];
   GMPRT   part;

/*
***Om det g�ller �terst�llning till hela GM �r det enkelt.
*/
   if ( id->seq_val == 0 )
     {
     last_type =  0;
     last_part =  0;
     last_la   =  0;
     next_inst =  0;
     last_idt  =  0;
     last_id   =  0;
     last_idm  =  0;
     root      = -1;
     }
/*
***Om inte b�rjar vi med att kontrollera att den angivna
***identiteten verkligen �r ID f�r en existerande part.
*/
   else
     {
     if ( DBget_pointer('I',id,&la,&typ) < 0 ) return(erpush("DB0152",""));
     if ( typ != PRTTYP )
       {
       gmidst(id,errbuf);
       return(erpush("DB0162",errbuf));
       }
/*
***En part �r det, d� g�r vi denna till root.
*/
     rddat1((char *)&part,la,sizeof(GMPRT));
     last_type = 0;
     last_part = la;
     last_la   = la;
     next_inst = 0;
     last_idt  = part.itp_pt;
     last_id   = 0;
     last_idm  = part.its_pt - (DBseqnum)1;
     root      = la;
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBget_next_ptr(
        DBint    trmode,
        DBetype  typmsk,
        char    *namn,
        char    *grupp,
        DBptr   *la)

/*      Servar MBS-rutinen GET_NEXT_GM().
 *
 *      In: trmode = Traverseringsmode, 0 = Fullst�ndig
 *                                      1 = Bara denna niv�
 *          typmsk = Typ(er) av storhet.
 *          namn   = Storhetens namn eller NULL.
 *          grupp  = Gruppnamn eller NULL.
 *          la_pek = Pekare till utdata.
 *
 *      Ut: *la_pek = LA till s�kt objekt eller DBNULL.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 1996-02-14 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr    la_tmp,adress;
    GMPRT    part,prt;
    GMCSY    csy;
    GMGRP    grp;
    GMRECH   header;

/*
***H�r b�rjar huvudloopen f�r n�sta storhet.
*/
nextid:
/*
***�r vi i b�rjan eller slutet av en ID-tabell.
*/
    if ( last_id == last_idm && next_inst == DBNULL  &&  last_type != PRTTYP )
      {
/*
***Slutet av huvudpartens eller den aktiva root-partens
***ID-tabell.
*/
      if ( last_part == huvprt  ||  last_part == root )
        {
       *la = DBNULL;
        return(0);
        }
/*
***B�rjan av huvudpartens ID-tabell.
*/
      else if ( last_part == 0 )
        {
        last_part = huvprt;
        last_type = 0;
        last_id   = -1;
        last_idm  = huvidm;
        last_idt  = 0;
        next_inst = DBNULL;
        goto nextid;
        }
/*
***Slutet av en anropad parts ID-tabell.
***Om trmode = 1 �r det d�rmed slut p� storheter.
*/
      else if ( trmode == 1 )
        {
       *la = DBNULL;
        return(0);
        }
/*
***Om trmode = 0 �terv�nder vi till parten p� n�rmast h�gre niv�.
*/
      else
        {
        rddat1((char *)&part,last_part,sizeof(GMPRT)); /* Nuv. part-post */
        last_part = part.hed_pt.p_ptr;  /* �terv�nd till f�reg. part */
        last_id = part.hed_pt.seknr;   /* Sista id */
        next_inst = part.hed_pt.n_ptr;  /* N�sta instans */
        last_type = 0;

        if ( last_part == huvprt) 
          {
          last_idm = huvidm;           /* F�reg. part == huvudpart */
          last_idt = 0;
          }
        else 
          {
          rddat1((char *)&part,last_part,sizeof(GMPRT)); /* F�reg. ej huvprt */
          last_idm = part.its_pt - (DBseqnum)1;
          last_idt = part.itp_pt;
          }
        goto nextid;
        }
      }
/*
***N�sta instans eller ID i tabellen.
***Om f�reg�ende storhet var en part skall vi
***normalt g� ned i den. Dock ej om trmode = 1.
*/
    else
      {
      if ( last_type == PRTTYP )
        {
        last_type = 0;
        if ( trmode == 0 )
          {
          last_part = last_la;
          last_id   = -1;
          rddat1((char *)&part,last_la,sizeof(GMPRT));
          last_idm  = part.its_pt - (DBseqnum)1;
          last_idt  = part.itp_pt;
          next_inst = DBNULL;
          }
        goto nextid;
        }
/*
***N�sta storhet �r en instans av f�reg�ende.
*/
      if ( next_inst != 0 )
        {
        DBread_header(&header,next_inst);
       *la = last_la = next_inst;
        last_type = header.type;
        next_inst = header.n_ptr;
        }
/*
***N�sta storhet �r en ny storhet.
*/
      else
        {
        ++last_id;
        adress = sizeof(DBptr)*last_id + last_idt;
        rddat1((char *)&la_tmp,adress,sizeof(DBptr));
        if ( (*la=la_tmp) < 0 )
          {
          last_type = 0;
          goto nextid;
          }
        DBread_header( &header, *la );
        last_la = *la;
        last_type = header.type;
        next_inst = header.n_ptr;
        }
/*
***Har storheten r�tt typ ? Om inte provar vi med n�sta.
***Fler instanser av denna beh�ver vi is�fall inte bry oss om.
*/
      if ( !(typmsk&header.type) )
        {
        next_inst = 0;
        goto nextid;
        }
/*
***Skall storheten ha ett visst namn ? Is�fall m�ste den
***vara en part, ett koordinatsystem eller en grupp.
*/
      if ( namn )
        {
        switch ( header.type )
          {
          case PRTTYP:
          rddat1((char *)&prt,*la,sizeof(GMPRT));
          if ( !gmcmpw(namn,prt.name_pt) ) goto nextid;
          break;

          case CSYTYP:
          rddat1((char *)&csy,*la,sizeof(GMCSY));
          if ( !gmcmpw(namn,csy.name_pl) ) goto nextid;
          break;

          case GRPTYP:
          rddat1((char *)&grp,*la,sizeof(GMGRP));
          if ( !gmcmpw(namn,grp.name_gp) ) goto nextid;
          break;

          default:
          goto nextid;
          }
        }
/*
***Skall storheten vara medlem i en viss grupp ?
*/
      if ( grupp )
        {
        if ( header.g_ptr[0] != DBNULL )
          {
          rddat1((char *)&grp,header.g_ptr[0],sizeof(GMGRP));
          if ( gmcmpw(grupp,grp.name_gp) ) return(0);
          }
        if ( header.g_ptr[1] != DBNULL )
          {
          rddat1((char *)&grp,header.g_ptr[1],sizeof(GMGRP));
          if ( gmcmpw(grupp,grp.name_gp) ) return(0);
          }
        if ( header.g_ptr[2] != DBNULL )
          {
          rddat1((char *)&grp,header.g_ptr[2],sizeof(GMGRP));
          if ( gmcmpw(grupp,grp.name_gp) ) return(0);
          }
        goto nextid;
        }
/*
***Slut.
*/
      return(0);
      }
  }

/********************************************************/
/*!******************************************************/

 static bool  gmcmpw(
        char *wc_str,
        char *tststr)

/*      J�mf�r 2 str�ngar varav en med wildcard "*".
 *      Godtyckligt antal stj�rnor f�r f�rekomma i 
 *      s�kstr�ngen (wc_str) varsomhelst men inte intill
 *      varandra. Om s� returneras FALSE.
 *
 *      Om n�gon av str�ngarna = "" returneras FALSE.
 *
 *      In: wc_str => Str�ng med 0, 1 eller flera stj�rnor.
 *          tststr => Str�ng utan stj�rnor.
 *
 *      Ut:  TRUE  = Str�ngarna matchar.
 *           FALSE = Matchar ej.
 *
 *      (C)microform ab 1996-02-12 J. Kjellander
 *
 *      1998-04-02 Bug, J.Kjellander
 *
 ******************************************************!*/

  {
   register char *p1,*p2;

/*
***Initiering.
*/
   p1 = wc_str;
   p2 = tststr;
/*
***Innan vi b�rjar j�mf�ra m�ste vi kolla om n�gon eller
***b�da = "" eftersom resten av rutinen f�ruts�tter att
***s� inte �r fallet.
*/
   if ( *p1 == '\0'  ||  *p2 == '\0' ) return(FALSE);
/*
***Ingen av str�ngarna �r "". S�tt ig�ng och j�mf�r!
*/
loop1:
/*
***�r det en stj�rna.
*/
   if ( *p1 == '*' )
     {
   ++p1;
/*
***Ja, s�kstr�ngen slutar med en stj�rna.
*/
     if ( *p1 == '\0' ) return(TRUE);
/*
***2 stj�rnor efter varandra �r inte till�tet.
*/
     else if ( *p1 == '*' ) return(FALSE);
/*
***Stj�rnan ej i slutet. Leta upp n�sta tecken i p2 som matchar
***tecknet efter stj�rnan.
*/
     else
       {
loop2:
/*
***Ett tecken i p2 matchar tecknet efter stj�rnan i p1.
***Strippa eventuella ytterligare lika tecken i p2 som inte
***matchar n�sta tecken i p1. Tex. p1=*poi och p2=me_ppoi
***1998-04-02 JK.
*/
       if ( *p1 == *p2 )
         {
         while ( *p1 == *(p2+1)  &&  *(p2+1) != *(p1+1) ) ++p2;
         goto loop1;
         }
       else
         {
       ++p2;
         if ( *p2 == '\0' ) return(FALSE);
         goto loop2;
         }
       }
     }
/*
***Ingen stj�rna, j�mf�r 2 riktiga tecken.
*/
   else
     {
/*
***Lika.
*/
     if ( *p1 == *p2 )
       {
     ++p1;
     ++p2;
       if ( *p1 == '\0' )
         {
/*
***p1 och p2 b�da slut.
*/
         if ( *p2 == '\0' ) return(TRUE);
/*
***p1 slut men inte p2.
*/
         else return(FALSE);
         }
       else
         {
/*
***p1 inte slut men p2.
*/
         if ( *p2 == '\0' )
           {
           if ( *p1 == '*'  &&  *(p1+1) == '\0' ) return(TRUE);
           else                                   return(FALSE);
           }
/*
***Varken p1 eller p2 slut.
*/
         else goto loop1;
         }
       }
/*
***Ej lika.
*/
     else return(FALSE);
     }
  }

/********************************************************/
