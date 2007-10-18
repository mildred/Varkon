/*!******************************************************
*
*    db1.c
*    ====
*
*    This file includes the following internal functions:
*
*    DBreset();   Resets DB
*    gmsvpf();    Writes pagefile to disc
*    gmldpf();    Loads pagefile from disc
*    gmclr();     Clears DB
*    gmgrow();    Increases DB virtual size
*    gmcrpf();    Creates new pagefile
*    gmclpf();    Closes pagefile
*    gmidst();    DBId to string conversion
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
*********************************************************/

#include <string.h>
#include "../include/DB.h"
#include "../include/DBintern.h"

char *gmbuf = NULL;

/*   gmbuf �r en character-array som utg�r gm:s fysiska
     minne. Sidor i gm skyfflas mellan gmbuf och page-
     filen vid pagefault. fystal h�ller reda p� antalet
     allokerade sidor. Minne f�r gmbuf allokeras dynamiskt
     vid uppstart. */


FILE *gmpfpk = NULL;

/*   gmpfpk �r filpekaren till page-filen. gmpfpk initieras
     till NULL vilket betyder att filen ej �r �ppen. Detta
     testas vid skrivning och l�sning. gmpfpk s�tts ocks�
     till NULL av gmclpf() */

DBint pfsiz;

/*   pfsiz �r storleken p� pagefilen. N�r en ny sida
     allokeras placeras den sist i pagefilen. 1:a
     biten av filen reserveras f�r pfsiz sj�lv var-
     f�r pfsiz initieras till sizeof(pfsiz) alt. ftell(). */

LTBSTR *logtab = NULL;

/*   logtab �r en map-tabell mellan logiskt sidnummer och
     fysiskt. Varje logisk sida motsvaras av ett element i
     logtab. logtab[i].ptr �r en pekare till en mot-
     svarande fysisk sida. Om pekaren > 0 finns sidan p�
     motsvarande adress i gmbuf. Om pekaren < 0 finns sidan i
     page-filen. Observera att pekarna i logtab inte �r
     vanliga C-pekare utan heltal som pekar p� enskilda
     element i gmbuf. Om pekaren = NOPGFA �r sidan inte
     allokerad, dvs finns ej i gmbuf och ej heller i page-
     filen. Om logtab[i].all �r TRUE �r sidan upptagen av
     data eller IDTAB annars �r den ledig. Fr.o.m. V1.8E
     allokeras minne f�r logtab dynamiskt. logtab = NULL
     inneb�r att inget minne �nnu har allokerats.  */


FTBSTR  *fystab = NULL;
DBpagnum fystsz,fystal;

/*   fystab �r en array av structures. Varje structure
     inneh�ller information om motsvarande sida i gmbuf,
     dvs. det fysiska minnet. Mot varje sida i det fysiska
     minnet (gmbuf) svarar ett element i FYSTAB. pagnum
     �r sidnumret f�r den sida som ligger p� motsvarande
     plats i gmbuf. pfpadr �r adressen till den sida i
     page-filen d�r sidan h�r hemma. wrflg �r TRUE om
     skrivning skett p� sidan, annars FALSE. reflg �r
     TRUE om sidan refererats, annars FALSE. fystsz h�ller
     reda p� antal element i fystab och d�rmed storleken
     p� gmbuf. fystal h�ller reda p� hur m�nga som �r
     allokerade. */


DBpagnum dpgnum;
DBshort  datofs;

/*   dpgnum �r sidnumret f�r den sida d�r data sist lagrats.
     datofs �r offset inom dpgnum. datofs �r initierad till
     PAGSIZ s� att f�rsta anropet till alldat() s�kert skall
     medf�ra att en ny sida allokeras. dpgnum �r initierad
     till slutet av LOGTAB mest f�r ordnings skull. */


DBpagnum ipgnum;

/*   ipgnum �r sidnumret f�r huvidm, dvs huvudmodulens
     id-tabell:s storlek i sidor. ipgnum initieras till
      -1 f�r att sidor s�kert skall allokeras f�r 1:a id
     som lagras. */


DBseqnum huvidm;
DBptr    huvprt;

/*   huvidm �r huvudmodulens st�rsta id. huvidm initieras
     till -1 s� att f�rsta storhet som lagras s�kert upp-
     daterar huvidm,ipgnum och annat p� r�tt s�tt. huvprt
     �r en pekare till huvudmodulens part-post. */


DBptr    actidt;
DBseqnum actidm;
DBptr    actprt;

/*    actidt �r en gm-pekare till b�rjan av den aktiva ID-
      tabellen. actidt initieras till 0 d�r ID-tabellen
      f�r huvud-modulen alltid skall finnas. N�r modul B
      anropas av modul A, via part-instruktionen, skapas
      en ny part-post i GM och minne f�r modul B:s ID-
      tabell allokeras. D�refter s�tts actidt att peka p�
      den nya ID-tabellen. N�r modul B �r f�rdigk�rd och
      exekveringen �terv�nder till modul A st�ngs parten B
      av interpretatorn genom ett anrop till DBclose_part(). D�rvid
      �terst�lls actidt att peka p� ID-tabellen f�r modul A
      igen. actidm �r huvidm f�r den aktiva modulen. 
      actprt �r en gm-pekare till den aktiva partens part-
      post. */


DBptr    templa = DBNULL;
DBseqnum tempsn = 0;

/*    templa �r la till senaste tempor�ra storhet. templa =
      DBNULL inneb�r att ingen s�dan finns. tempsn �r storhetens
      serienummer. Variablerna nollst�lls av gmclr() och
      utnyttjas av gmmtm(), gmumtm() och gmrltm(). */


DBint    nkeys = 0;
KEYDATA *keytab = NULL;

/*    keytab is a pointer to the current table of data
      saved by key. nkeys is the number of entries in keytab
      including not used entries. */


V3MDAT gmsdat_org;

V3MDAT gmsdat_db = { 0,              /* Not used by DB */
                     DB_LIBVERSION,  /* Version */
                     DB_LIBREVISION, /* Revision */
                     DB_LIBLEVEL,    /* Level */
                     0,0,0,0,0,      /* Creation date */
                     0,0,0,0,0,      /* Last update */
                     "?",            /* OS or Hostname */
                     " ",            /* Not used by DB */
                     0,              /* Not used by DB */
                     0,              /* Not used by DB */
                     "?",            /* OS Release */
                     "?",            /* OS Version */
                     0 };            /* Not used by DB */

/*   gmsdat_db is the system data for the current DB library.
     DBinit() sets Creation date and OS info. DBexit() stets
     the update date and writes gmsdat_db to the DB file.
     DBLoad() checks that the file loaded is compatible with
     the current library version. */

V3MSIZ gmssiz;

/*    gmssiz �r en kopia av aktuell systemstorlek.
      gmssiz.gm anv�nds f�r att s�tta GM:s fysiska
      storlek i antal sidor. */

GMMDAT metdat;

/*    metdat �r en structure med data v�sentliga f�r GM:s
      minneshantering. metdat lagras precis som gmsdat i page-
      filen. */

/*!******************************************************/

        DBstatus DBreset()

/*      Clears the DB with gmclr() and creates the top
 *      level part entity named "Root_part".
 *
 *      (C)microform ab 29/7/85 J. Kjellander
 *
 *      1998-12-16 Tagit bot jobnam, J.Kjellander
 *      1999-01-24 Init ptr. to keytable.
 *      2004-07-18 Mesh, J.Kjellander, �rebro uniersity
 *
 ******************************************************!*/

  {
    DBstatus status;
    GMPRT    part;

/*
***Init the DB with default size of logtab.
*/
    if ( (status=gmclr(LTSDEF)) < 0 ) return(status);
/*
***Create the root part. From. 1.16E a complete part is created
***with all members set. Earlier versions only had some members
**set.
*/
    part.hed_pt.type     = PRTTYP;
    part.hed_pt.p_ptr    = DBNULL;     /* The root part has no parent */
    part.hed_pt.n_ptr    = DBNULL;     /* From 1.16E */
    part.hed_pt.seknr    = 0;          /* From 1.16E */
    part.hed_pt.ordnr    = 0;          /* From 1.16E */
    part.hed_pt.vers     = GMPOSTV0;   /* From 1.16E */
    part.hed_pt.g_ptr[0] = DBNULL;     /* Used for keytab from 1.16E */
    part.hed_pt.g_ptr[1] = DBNULL;     /* Currently not used */
    part.hed_pt.g_ptr[2] = DBNULL;     /* Currently not used */
    part.hed_pt.blank    = 0;          /* From 1.16E */
    part.hed_pt.hit      = 0;          /* From 1.16E */
    part.hed_pt.pen      = 0;          /* From 1.16E */
    part.hed_pt.level    = 0;          /* From 1.16E */

    strcpy(part.name_pt,"Root_part");  /* Name */
    part.dummy_pt  = 0;                /* From 1.16E */
    part.dts_pt    = 0;                /* From 1.16E */
    part.dtp_pt    = DBNULL;           /* From 1.16E */
    part.itp_pt    = 0;                /* Pointer to root ID table */
    part.its_pt    = 0;                /* Current size of root ID table */

    wrdat1( (char *)&part,&huvprt,sizeof(GMPRT)); /* Store in DB */
    actprt = huvprt;                              /* Update pointer */
/*
***This DB will have the same version as the current system.
*/
    V3MOME(&gmsdat_db,&gmsdat_org,sizeof(V3MDAT));
/*
***Clear the patchdata cache of DBread_one_patch() so that any
***old patchdata is not used again.
*/
    DBread_one_patch(NULL,NULL,0,0);
/*
***Clear the mesh data cache of DBread_mesh() so that any
***old data is not used again.
*/
    DBread_mesh(NULL,DBNULL,0);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmsvpf()

/*      Skriver ut GM till disc. F�rst skrivs alla
 *      modifierade sidor i prim�rminne ut till page-
 *      filen. D�rmed �r alla GM:s sidor placerade
 *      i pagefilen. F�r att kunna l�sa in den igen
 *      skrivs ocks� LOGTAB mm. ut sist i page-filen.
 *      F�r att kunna hitta LOGTAB skrivs pfsiz in f�rst
 *      i pagefilen.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          GM0043 => Fel vid skrivning till disk.
 *          GM0113 => Skrivning till ej �ppnad page-fil.
 *
 *      (C)microform ab 28/12/84 J. Kjellander
 *
 *      9/10/86  IBMATX, J. Kjellander
 *      2/11/88  Felhantering, J. Kjellander
 *      17/11/92 VARREC, J. Kjellander
 *      1999-02-09 gmsdat_db, J.Kjellander
 *
 ******************************************************!*/

  {
    char   pagbuf[PAGSIZ];
    char  *bptr;
    DBint  i,size;

/*
***Dont try to save a DB that is not initiated.
*/
    if ( gmpfpk == NULL ) return(erpush("DB0113",""));
/*
***Write the keytable cache to pf.
*/
    gmsvkt();
/*
***If an unlinked entity exits remove it now so that
***it is not written to disc.
*/
    if ( templa != DBNULL ) gmrltm();
/*
***Write all modifed pages in the buffer to the pagefile.
*/
    for ( i=0; i < fystsz; ++i )
      {
      if ( fystab[i].wrflg == TRUE )
/*
***This page is modified. Write it to the pf.
*/
        {
        if ( fystab[i].pfpadr == PGFPNA )
          {
          fystab[i].pfpadr = pfsiz;
          if ( fseek( gmpfpk, (long)fystab[i].pfpadr, 0) != 0 ) goto error;
          if ( fwrite((char *)&gmbuf[PAGSIZ*i],PAGSIZ,1,gmpfpk) == 0 )
            goto error;
          pfsiz += PAGSIZ;
          }
        else
          {
          if ( fseek( gmpfpk, (long)fystab[i].pfpadr, 0) != 0 ) goto error;
          if ( fwrite((char *)&gmbuf[PAGSIZ*i],PAGSIZ,1,gmpfpk) == 0 )
            goto error;
          }
        fystab[i].wrflg = FALSE;
        }
/*
***This page is not modified. Give it back to the pf without
***writing.
*/
      if( fystab[i].pagnum != GMBPNA )
         logtab[ fystab[i].pagnum ].ptr = -fystab[i].pfpadr;
      fystab[i].pagnum = GMBPNA;
      }
/*
***Now write all metadata to the PF as well. Note that gmsdat_org
***is now replaced with gmsdat_db.
*/
    bptr = pagbuf;
    V3MOME(&gmsdat_db,bptr,sizeof(V3MDAT));  bptr += sizeof(V3MDAT);
    V3MOME(&metdat,bptr,sizeof(GMMDAT));     bptr += sizeof(GMMDAT);
    V3MOME(&dpgnum,bptr,sizeof(dpgnum));     bptr += sizeof(dpgnum);
    V3MOME(&datofs,bptr,sizeof(datofs));     bptr += sizeof(datofs);
    V3MOME(&ipgnum,bptr,sizeof(ipgnum));     bptr += sizeof(ipgnum);
    V3MOME(&huvidm,bptr,sizeof(huvidm));     bptr += sizeof(huvidm);
    V3MOME(&huvprt,bptr,sizeof(DBptr));      bptr += sizeof(DBptr);

    size = (DBint)(bptr - pagbuf);           /* Size of metadata */
    if ( fseek( gmpfpk, (long)pfsiz, 0) != 0 ) goto error;
    if ( fwrite(pagbuf,(size_t)size,1,gmpfpk) == 0 ) goto error;
/*
***Last of all, write logtab.
*/
    if ( fwrite((char *)logtab,sizeof(LTBSTR),
                (size_t)metdat.logmax,gmpfpk) == 0 ) goto error;
/*
***First 4 bytes hold pf size.
*/
    if ( fseek( gmpfpk, (long)0, 0) != 0 ) goto error;
    if ( fwrite((char *)&pfsiz,sizeof(pfsiz),1,gmpfpk) == 0 ) goto error;
/*
***End.
*/
    return(0);
/*
***Error exit.
*/
error:
    return(erpush("DB0043",""));
    }
     
/********************************************************/
/*!******************************************************/

        DBstatus gmldpf()

/*      Loads a pagefile from disc. 
 *
 *      FV:      0 => Ok.
 *          GM0033 => Invalid data or disc read error.
 *
 *      (C)microform ab 28/12/84 J. Kjellander
 *
 *      17/2/86    gmsdat, J. Kjellander
 *      5/4/86     Ny felhantering, J. Kjellander
 *      9/10/86    IBMATX, J. Kjellander
 *      17/11/92   VARREC, J. Kjellander
 *      1999-01-24 gmsdat_org, J.Kjellander
 *      1999-02-09 errorchecking, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***First, read pfsiz.
*/
   if ( fseek( gmpfpk,(long)0, 0) != 0 ) goto read_error;
   if ( fread((char *)&pfsiz,sizeof(pfsiz),1,gmpfpk) == 0 ) goto read_error;
   if ( pfsiz < 0 ) goto data_error;
/*
***Now read various metadata.
*/
   if ( fseek(gmpfpk, (long)pfsiz, 0) != 0 ) goto read_error;

   if ( fread((char *)&gmsdat_org,sizeof(V3MDAT),1,gmpfpk) == 0 )
     goto read_error;
   if ( fread((char *)&metdat,sizeof(GMMDAT),1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&dpgnum,sizeof(dpgnum),1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&datofs,sizeof(datofs), 1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&ipgnum,sizeof(ipgnum),1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&huvidm,sizeof(huvidm),1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&huvprt,sizeof(DBptr), 1,gmpfpk) == 0 )
      goto read_error;
/*
***Check for fatal errors. This is not a waterproof test
***of the DB file validity but will discover obvoius errors.
*/
   if ( dpgnum <   0 ) goto data_error;
   if ( datofs <   0 ) goto data_error;
   if ( ipgnum <  -1 ) goto data_error;
   if ( huvidm <  -1 ) goto data_error;
   if ( huvprt <=  0 ) goto data_error;
/*
***Read logtab.
*/
   if ( fread((char *)logtab,sizeof(LTBSTR),
                           (size_t)metdat.logmax,gmpfpk) == 0 ) goto read_error;
/*
***Init act-variables.
*/
   actidm = huvidm;
   actprt = huvprt;
/*
***Load and init keytab.
*/
  if ( keytab != NULL )
    {
    v3free(keytab,"gmldpf");
    keytab = NULL;
    nkeys  = 0;
    }
   gmldkt();
/*
***End.
*/
   return(0);
/*
***Read error.
*/
read_error:
   gmclpf();
   return(erpush("DB0033",""));
/*
***Corrupt data.
*/
data_error:
   gmclpf();
   return(erpush("DB0223",""));
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmldkt()

/*      Loads a keytable if one exists. Called
 *      by gmldpf() after everything else is
 *      loaded and initialized.
 *
 *      (C)microform ab 1999-01-24 J. Kjellander
 *
 ******************************************************!*/

  {
   DBptr keytab_ptr;
   DBint nbytes;
   GMPRT part;

/*
***Keytables were introduced in Varkon 1.16E by using
***the first 2 group pointers of the Root part.
***Group pointers from older files are not valid.
*/
   if ( gmsdat_org.vernr == 1  &&  (gmsdat_org.revnr < 16  ||
        (gmsdat_org.revnr == 16 && gmsdat_org.level < 'E')) ) return(0);
/*
***Files 1.16E or later may have a keytable stored. Read
***the Root part and check the first group pointer.
*/
   rddat1((char *)&part,huvprt,sizeof(GMPRT));
   keytab_ptr = part.hed_pt.g_ptr[0];
/*
***If the file includes a keytable, load it.
*/
   if ( keytab_ptr != DBNULL )
     {
     nkeys = (DBint)part.hed_pt.g_ptr[1];
     nbytes = nkeys*sizeof(KEYDATA);
     keytab = (KEYDATA *)v3mall(nbytes,"gmldkt");
     if ( keytab == NULL ) return(erpush("DB0193",""));
     if ( nbytes <= PAGSIZ ) rddat1((char *)keytab,keytab_ptr,nbytes);
     else                    rddat2((char *)keytab,keytab_ptr,nbytes);
     }
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmsvkt()

/*      Writes the keytable to pf. Called
 *      by gmsvpf().
 *
 *      (C)microform ab 1999-01-24 J. Kjellander
 *
 ******************************************************!*/

  {
   DBptr keytab_ptr;
   DBint keytab_size,nbytes;
   GMPRT part;

/*
***Get the Root part.
*/
   rddat1((char *)&part,huvprt,sizeof(GMPRT));
/*
***If an old table exists in pf, delete it. Only files 1.16E
***or later can have keytables.
*/
   if ( gmsdat_org.vernr > 1  ||
        (gmsdat_org.vernr == 1 && (gmsdat_org.revnr > 16 ||
         (gmsdat_org.revnr == 16 && (gmsdat_org.level >= 'E')))) )
     {
     keytab_ptr = part.hed_pt.g_ptr[0];
     if ( keytab_ptr != DBNULL )
       {
       keytab_size = part.hed_pt.g_ptr[1];
       nbytes = keytab_size*sizeof(KEYDATA);
       if ( nbytes <= PAGSIZ ) rldat1(keytab_ptr,nbytes);
       else                    rldat2(keytab_ptr,nbytes);
       }
     }
/*
***Older versions have incomplete Root parts. Since this pf
***will be saved with curent version we fix that here so that
***next time it is loaded it's a valid 1.16E or later.
*/
   else
     {
     part.hed_pt.type     = PRTTYP;
     part.hed_pt.p_ptr    = DBNULL;     /* The root part has no parent */
     part.hed_pt.n_ptr    = DBNULL;     /* From 1.16E */
     part.hed_pt.seknr    = 0;          /* From 1.16E */
     part.hed_pt.ordnr    = 0;          /* From 1.16E */
     part.hed_pt.vers     = GMPOSTV0;   /* From 1.16E */
     part.hed_pt.g_ptr[0] = DBNULL;     /* Used for keytab from 1.16E */
     part.hed_pt.g_ptr[1] = DBNULL;     /* Currently not used */
     part.hed_pt.g_ptr[2] = DBNULL;     /* Currently not used */
     part.hed_pt.blank    = 0;          /* From 1.16E */
     part.hed_pt.hit      = 0;          /* From 1.16E */
     part.hed_pt.pen      = 0;          /* From 1.16E */
     part.hed_pt.level    = 0;          /* From 1.16E */

     strcpy(part.name_pt,"Root_part");  /* Name */
     part.dummy_pt  = 0;                /* From 1.16E */
     part.dts_pt    = 0;                /* From 1.16E */
     part.dtp_pt    = DBNULL;           /* From 1.16E */
     part.itp_pt    = 0;                /* Pointer to root ID table */
     part.its_pt    = 0;                /* Current size of root ID table */
     }
/*
***Does a keytable cache exist ? If so, save it in pf.
*/
   if ( keytab != NULL )
     {
     keytab_size = nkeys;
     nbytes = keytab_size*sizeof(KEYDATA);
     if ( nbytes <= PAGSIZ ) wrdat1((char *)keytab,&keytab_ptr,nbytes);
     else                    wrdat2((char *)keytab,&keytab_ptr,nbytes);
     }
/*
***If not, set the pointer and size = 0.
*/
   else
     {
     keytab_ptr = DBNULL;
     keytab_size = 0;
     }
/*
***Save the Root part with the new keytab pointer and size.
*/
   part.hed_pt.g_ptr[0] = keytab_ptr;
   part.hed_pt.g_ptr[1] = keytab_size;

   updata((char *)&part,huvprt,sizeof(GMPRT));
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmclr(DBint logmax)

/*      Clears (deletes) everyting in the DB. This routine
 *      marks all pages unused, clears keytab, inits act-
 *      variables, removes temporary entites, allocates
 *      not allocated buffers etc. etc.
 *
 *      gmclr() is used by DBreset() to reset the DB to
 *      initial status.
 *      
 *      In: logmax = �nskat antal logiska sidor, dvs. GM:s
 *                   maximala storlek.
 *
 *      Ut: Inget.
 *
 *      Felkoder: GM0123 = Kan ej allokera minne f�r logtab
 *
 *      (C)microform ab 29/7/85 J. Kjellander
 *
 *      21/10/85 reflg, J. Kjellander
 *      17/2/86  gmsdat, J. Kjellander
 *      22/5/89  logmax, J. Kjellander
 *      30/3/92  templa, J. Kjellander
 *      18/11/92 fystsz, J. Kjellander
 *      14/3/95  v3mall()/v3free(), J. Kjellander
 *      1999-01-21 keytab, J.Kjellander
 *      2004-07-18 Mesh, J.Kjellander, �rebro university
 *
 ******************************************************!*/

  {
    DBint  i;
    char   buf[40];

/*
***Allokera minne f�r gmbuf och fystab. Om sysize.gm
***har ett v�rde har detta angetts p� kommandoraden n�r
***systemet startades. Om inte tar vi default fr�n
***DBintern.h.
*/
    if ( gmssiz.gm > 0 ) fystsz = (DBpagnum)gmssiz.gm;
    else fystsz = FYSTSZ;

    if ( fystab == NULL )
      {
      if ( (gmbuf=(char *)v3mall(fystsz*PAGSIZ,"gmbuf")) == NULL )
        {
        sprintf(buf,"%d",(int)fystsz);
        return(erpush("DB0133",buf));
        }
      if ( (fystab=(FTBSTR *)v3mall(fystsz*sizeof(FTBSTR),"fystab")) == NULL )
        {
        sprintf(buf,"%d",(int)fystsz);
        return(erpush("DB0143",buf));
        }
      }
/*
***Init FYSTAB.
*/
    for ( i=0; i<fystsz; ++i) 
      {
      fystab[i].pagnum = GMBPNA;     /* Sidan �r ej allokerad */
      fystab[i].pfpadr = PGFPNA;     /* PF-sida �r ej allokerad */
      fystab[i].wrflg = FALSE;       /* Ej heller modifierad */
      fystab[i].reflg = FALSE;       /* eller ens refererad */
      }
    cirpek = cirdef = fystsz - 10;
    fystal = 0;
/*
***Init LOGTAB.
*/
    if ( logtab != NULL ) v3free((char *)logtab,"gmclr");
    if ( (logtab=(LTBSTR *)v3mall(logmax*sizeof(LTBSTR),"logtab")) == NULL )
      {
      sprintf(buf,"%d",(int)logmax);
      return(erpush("DB0123",buf));
      }

    for ( i=0; i<logmax; ++i)
      {
      logtab[i].ptr = PGFPNA;        /* Pagefil-sida ej allokerad */
      logtab[i].all = FALSE;         /* Sidan �r ledig */
      }

    metdat.logmax = logmax;
/*
***Init IDTAB.
*/
    ipgnum = -1;
    huvidm = -1;
/*
***Init data-area.
*/
    dpgnum = logmax - 1;
    datofs = PAGSIZ;
    pfsiz  = sizeof(pfsiz);
/*
***Init act-variables.
*/
    actidm = huvidm;
    actidt = 0;
/*
***Init last-variables.
*/
    lstprt = 0;
    lstid  = 0;
    nxtins = DBNULL;
    lstidm = 0,
    lstidt = 0;
/*
***Init temp-variables.
*/
    templa = DBNULL;
    tempsn = 0;
/*
***Init keytab.
*/
    if ( keytab != NULL ) v3free((char *)keytab,"gmclr");
    keytab = NULL;
    nkeys = 0;
/*
***Clear the patchdata cache of DBread_one_patch() so that any
***old patchdata is not used again.
*/
    DBread_one_patch(NULL,NULL,0,0);
/*
***Clear the mesh data cache of DBread_mesh() so that any
***old data is not used again.
*/
    DBread_mesh(NULL,DBNULL,0);

/*
***End.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmgrow()

/*      �kar storleken p� logtab med LTSDEF.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: GM0123 = Kan ej allokera minne f�r logtab
 *
 *      (C)microform ab 22/5/89 J. Kjellander
 *
 *      28/6/89 Bug act-variabler, J. Kjellander
 *      31/10/92 realloc(), J. Kjellander
 *      14/3/95  v3rall(), J. Kjellander
 *
 ******************************************************!*/

  {
   DBint newmax,i;
   char  buf[80];

/*
***Ber�kna ny storlek p� logtab.
*/
   newmax = metdat.logmax + LTSDEF;
/*
***Ut�ka logtabs storlek med realloc(). Den gamla algoritmen
***bestod i att g�ra gmsvpf() + gmclr() + gmldpf(). Detta tog
***f�r mycket tid, s�rskilt p� VAX. 921031 JK.
*/
   if ( (logtab=(LTBSTR *)v3rall((char *)logtab,
      (unsigned)(newmax*sizeof(LTBSTR)),"gmgrow")) == NULL )
     {
     sprintf(buf,"%d",(int)newmax);
     return(erpush("DB0123",buf));
     }
/*
***Initiera de nya sidorna.
*/
    for ( i=metdat.logmax; i<newmax; ++i )
      {
      logtab[i].ptr = PGFPNA;        /* Pagefil-sida ej allokerad */
      logtab[i].all = FALSE;         /* Sidan �r ledig */
      }
/*
***St�ll om GM:s storlek i meta-data strukturen.
*/
   metdat.logmax += LTSDEF;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmcrpf(char *pagfil)

/*      Skapar en ny pagefil.
 *
 *      In: pagfil = Pagefilens namn.
 *
 *      Ut: Inget.
 *
 *      FV:  0       => Ok.
 *           GM0013  => Kan ej skapa page-fil.
 *
 *      (C)microform ab 29/7/85 J. Kjellander
 *
 *      17/11/92 VARREC, J. Kjellander
 *
 ******************************************************!*/

  {

/*
***�ppna en ny fil f�r l�sning och skrivning.
*/
#ifdef WIN32
    if ( (gmpfpk=fopen(pagfil,"w+b")) == NULL ) goto error;
#else
    if ( (gmpfpk=fopen(pagfil,"w+")) == NULL ) goto error;
#endif
/*
***Slut.
*/
    return(0);
/*
***Fel.
*/
error:
       return(erpush("DB0013",pagfil));
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmclpf()

/*      St�nger pagefilen.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:  0       => Ok.
 *           GM0023  => Kan ej st�nga pagefilen.
 *
 *      (C)microform ab 29/7/85 J. Kjellander
 *
 *      23/5/89  free(), J. Kjellander
 *      14/3/95  v3free(), J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Prova att st�nga.
*/
    if ( fclose(gmpfpk) == EOF )
       return(erpush("DB0023",""));
/*
***Om det gick bra, s�tt gmpfpk = NULL vilket betyder att 
***page-filen �r st�ngd, deallokera minne f�r logtab och
***returnera 0.
*/
    else
      {
      gmpfpk = NULL;
      if ( logtab != NULL ) v3free(logtab,"gmclpf");
      logtab = NULL;
      return(0);
      }
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmidst(DBId *idvek, char *idstr)

/*      Konverterar en identitet till str�ng av ASCII-
 *      tecken.
 *
 *      In: idvek => Pekare till lista av id-structures.
 *          idstr => Pekare till char-array.
 *
 *      Ut: *idstr => Str�ng, null-terminated.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1998-12-15 J. Kjellander
 *
 ******************************************************!*/

  {
    char  tmp[V3STRLEN];
    DBId *idptr;

    idstr[0] = '\0';
    idptr = idvek;

    if ( idptr->seq_val < 0 ) strcat(idstr,"#");

    do
      {
      sprintf(tmp,"#%d.%d",(int)abs(idptr->seq_val),(int)idptr->ord_val);
      strcat(idstr,tmp);
      }
    while ( (idptr=idptr->p_nextre) != NULL );

    return(0);
  }

/********************************************************/
