/**********************************************************************
*
*    db3.c
*    =====
*
*    This file includes the following internal routines:
*
*    alldat();   Allocates space for max 1 page of data
*    allidt();   Allocates space for ID-table
*    clrpag();   Clears one page
*    wrdat1();   Allocates and writes max 1 page of data
*    wrdat2();   Allocates and writes multiple pages of data
*    rddat1();   Reads max 1 page of data
*    rddat2();   Reads multiple pages of data
*    updata();   Rewrites max 1 page of data
*    rldat1();   Releases max 1 page of data
*    rldat2();   Releases multiple pages of data
*   *gmgadr();   DB pointer to C-adress
*    gtlain();   DB pointer and type for instance
*    gmdlid();   Deletes an ID
*    gmagpp();   Adds a GMGRP pointer
*
*    This file is part of the VARKON Database Library.
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*
***Det �r fel av allidt att s�tta hela sidan til NOTUSED n�r
***bara en del av sidan anv�nds! rldat() kan inte sl�ppa.
*/

#define DATSIZ PAGSIZ-sizeof(DBptr)    /* M�ngd data/sida */

/*!******************************************************/

        DBptr alldat(DBint size)

/*      Allokerar plats f�r data. Om tillr�ckligt med plats
 *      finns p� samma sida som sist allokeras plats d�r,
 *      annars allokeras en ny sida fr�n slutet av LOGTAB.
 *      Om en ny sida allokeras skrivs den full med EMPTY.
 *
 *      In: size  => Antal char som skall allokeras.
 *                   Tex. sizeof(GMPOI) f�r en punkt.
 *
 *      Ut: Inget.
 *
 *      FV:  >=0  => Logisk adress till b�rjan av datablocket.
 *          -1    => Slut p� virtuellt minne.
 *          -2    => Size > PAGSIZ.
 *
 *      (C)microform ab 6/11/84 J. Kjellander
 *
 *      11/10/86 IBMATX, J. Kjellander
 *      22/5/89  gmgrow(), J. Kjellander
 *
 ******************************************************!*/     

  {
    DBptr    la;
    DBstatus status;

    if ( size > PAGSIZ ) return(-2);
/*
***Prova att fylla p� i samma sida som sist.
*/
    if ( (PAGSIZ-datofs) >= size )
      {
      la = dpgnum*PAGSIZ + datofs;
      datofs += size;
      return(la);
      }
/*
***Data fick inte plats p� denna sida. Leta upp en ny sida.
*/
    else
      {
      dpgnum = metdat.logmax - 1;
      while ( logtab[dpgnum].all && dpgnum>IDTSIZ-1 ) --dpgnum;
/*
***Om ingen ledig sida utanf�r IDTAB fins att tillg� m�ste
***GM:s virtuella storlek �kas.
*/
      if ( dpgnum == IDTSIZ-1 )
        {
        if ( (status=gmgrow()) < 0 ) return(status);
        else dpgnum = metdat.logmax - 1;
        }
/*
***Sidan dpgnum �r ledig, allokera och EMPTY-st�ll.
*/
      logtab[dpgnum].all = TRUE;
      clrpag(dpgnum);
/*
***Uppdatera datofs och �terv�nd.
*/
      datofs = size;
      return(dpgnum*PAGSIZ);
      }
  }

/********************************************************/
/*!******************************************************/

        DBptr allidt(DBseqnum size)

/*      Allokerar plats f�r en ID-tabell. Om tabellen f�r
 *      plats p� sidan dpgnum allokeras plats d�r annars
 *      allokeras erfoderligt antal sammanh�ngande nya
 *      sidor. Alla entry:s i tabellen initieras under
 *      alla omst�ndigheter till NOTUSD.
 *
 *      In: size  => Antal entry i tabellen.
 *
 *      Ut: Inget.
 *
 *      FV:  > 0  => Logisk adress till b�rjan av tabellen.
 *          -1    => Slut p� virtuellt minne.
 *
 *      (C)microform ab 24/12/84 J. Kjellander
 *
 *      22/10/85   reflg, J. Kjellander
 *      7/11/85    NOTUSD, J. Kjellander
 *      11/10/86   IBMATX, J. Kjellander
 *      16/5/87    Bugfix, J. Kjellander
 *      23/9/87    Fix av bugfix, J. Kjellander
 *      1997-12-01 gmgrow(), J.Kjellander
 *      1997-12-04 Sista sidan, J.Kjellander
 *      2004-02-28 ipgnum->IDTSIZ, J.Kjellander
 *
 ******************************************************!*/

  {
    DBseqnum j,nidppg,nleft;
    DBpagnum pagnum,pagant,antal,i,lstpag;
    DBptr    la,numchr,pagadr,gmbadr;
    char    *gm_pek,*no_pek;
    char     nousb1,nousb2,nousb3,nousb4;

    static DBptr noused = NOTUSD;

/*
***Lite initiering.
*/
    no_pek = (char *)&noused;
    nousb1 = *no_pek; ++no_pek;
    nousb2 = *no_pek; ++no_pek;
    nousb3 = *no_pek; ++no_pek;
    nousb4 = *no_pek;
/*
***Kolla om tabellen f�r plats p� sidan dpgnum.
*/
    numchr = size*sizeof(DBptr);

    if ( PAGSIZ-datofs >= numchr )
      {
      la = dpgnum*PAGSIZ + datofs;
      if ( (pagadr=logtab[dpgnum].ptr) < 0 ) pagadr=pfault(dpgnum);

      fystab[pagadr/PAGSIZ].wrflg = TRUE;
      fystab[pagadr/PAGSIZ].reflg = TRUE;

      gm_pek = &gmbuf[pagadr+datofs];

      for (j=0; j<size; ++j)
        {
        *gm_pek = nousb1; ++gm_pek;
        *gm_pek = nousb2; ++gm_pek;
        *gm_pek = nousb3; ++gm_pek;
        *gm_pek = nousb4; ++gm_pek;
        }
      datofs += (DBshort)numchr;                /* Uppdatera datofs */
      return(la);
      }
/*
***Vi f�r inte plats p� dpgnum.
***Ber�kna antal sidor som beh�ver allokeras.
*/
    else
      {
      pagant = numchr/PAGSIZ;
      if ( numchr > pagant*PAGSIZ) ++pagant;
/*
***Om mer �n en sida m�ste allokeras m�ste dom ligga
***direkt efter varandra. Leta f�rst upp en ledig sida.
***Don't touch the first IDTSIZ pages reserved for the
***ID-table of the top level module. 040228JK
*/
      i = metdat.logmax - 1;

loop: while ( logtab[i].all  &&  i>IDTSIZ+pagant ) --i;

      if ( i == IDTSIZ+pagant )
        {
        if ( gmgrow() < 0 ) return(-1);
        else
          {
          i = metdat.logmax - 1;
          goto loop;
          }
        }
/*
***Sidan i �r ledig, kolla att erfoderligt antal
***ytterligare sidor direkt f�re ocks� �r lediga.
*/
     antal = 0;
     while ( logtab[i].all == FALSE  && antal < pagant )
       {
       ++antal;
       --i;
       }

     if ( antal < pagant ) goto loop;
     else ++i;
/*
***pagant sidor fr.o.m. sidan i �r lediga, allokera och g�r NOTUSD.
***Vi anv�nder inte n�dv�ndigtvis hela sista sidan.
***Ej utnyttjat utrymme m�ste nollst�llas s� att 
***sidan kan sl�ppas av rldat1().
*/
     lstpag = i+pagant-1;
     nidppg = PAGSIZ/sizeof(DBptr);

     for ( pagnum=i; pagnum<i+pagant; ++pagnum)
       {
       logtab[pagnum].all = TRUE;
       if ( (gmbadr=logtab[pagnum].ptr) < 0 ) gmbadr=pfault(pagnum);

       fystab[gmbadr/PAGSIZ].wrflg = TRUE;
       fystab[gmbadr/PAGSIZ].reflg = TRUE;

       gm_pek = &gmbuf[gmbadr];
/*
***Sista sidan.
*/
       if ( pagnum == lstpag )
         {
         nleft = size - (pagant-1)*nidppg;
         for ( j=0; j < nleft; ++j )                     /* Ej anv�nt ID */
           {
          *gm_pek = nousb1; ++gm_pek;
          *gm_pek = nousb2; ++gm_pek;
          *gm_pek = nousb3; ++gm_pek;
          *gm_pek = nousb4; ++gm_pek;
           }
         for ( j=nleft*sizeof(DBptr); j < PAGSIZ; ++j )  /* Ej anv�nt minne */
           {
          *gm_pek = EMPTY; ++gm_pek;
           }
         }
/*
***�vriga sidor.
*/
       else
         {
         for ( j=0; j < nidppg; ++j )
           {
          *gm_pek = nousb1; ++gm_pek;
          *gm_pek = nousb2; ++gm_pek;
          *gm_pek = nousb3; ++gm_pek;
          *gm_pek = nousb4; ++gm_pek;
           }
         }
       }
     return(i*PAGSIZ);
     }
  }

/********************************************************/
/*!******************************************************/

        DBstatus clrpag(DBpagnum pagnum)

/*      EMPTY-st�ller sidan pagnum.
 *
 *      In: pagnum => Sidnummer.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 7/1/85 J. Kjellander
 *
 ******************************************************!*/
  {
    DBptr  pagadr;
    DBint  i;
/*
***H�mta sidadress.
*/
      if ( (pagadr=logtab[pagnum].ptr) < 0 ) pagadr=pfault(pagnum);
/*
***S�tt sidans skriv- och referensflagga.
*/
      fystab[pagadr/PAGSIZ].wrflg = TRUE;
      fystab[pagadr/PAGSIZ].reflg = TRUE;
/*
***Skriv.
*/
      for ( i=0 ; i < PAGSIZ ; ++i )
         gmbuf[pagadr++] = EMPTY;           /* Empty-st�ll */

      return(0);

  }
/********************************************************/
/*!******************************************************/

        DBstatus wrdat1(char *chrpek, DBptr *lapek, DBint size)

/*      Allokerar ny plats f�r data och lagrar.
 *      Data m�ste s�kert f� plats p� en sida.
 *
 *      In: chrpek => Char-pekare till data.
 *          lapek  => Pekare till DBptr-variabel.
 *          size   => Antal char som skall lagras.
 *
 *      Ut: *la    => GM-adress d�r data lagrats.
 *
 *      FV:  0  => Ok.
 *          -3  => Data f�r inte plats.
 *
 *      (C)microform ab 15/4/85 J. Kjellander
 *
 ******************************************************!*/

  {

    if ( size == 0 ) return(0);
/*
***Allokera plats f�r data.
*/
    if ( (*lapek=alldat(size) ) < 0) return(-3);
/*
***Skriv in data p� allokerad plats.
*/
    updata(chrpek,*lapek,size);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus wrdat2(char *chrpek, DBptr *lapek, DBint size)

/*      Allokerar ny plats f�r data och lagrar. Oavsett
 *      hur mycket data som skall lagras (size) allokeras
 *      minst en hel sida. Om fler sidor beh�vs l�nkas
 *      de ihop. En gm-pekare lagras p� f�rsta sidan 
 *      och d�refter data. Sista sidan har pekaren = DBNULL.
 *
 *      In: chrpek => Char-pekare till data.
 *          lapek  => Pekare till DBptr-variabel.
 *          size   => Antal char som skall lagras.
 *
 *      Ut: *la    => GM-adress d�r data lagrats.
 *
 *      FV:  0  => Ok.
 *          -3  => Data f�r inte plats.
 *
 *      (C)microform ab 10/7/85 J. Kjellander
 *
 *      11/10/86   IBMATX, J. Kjellander
 *      2/6/93     alldat(), J. Kjellander
 *      1997-05-28 Bug dpgnum, J.Kjellander
 *
 ******************************************************!*/

  {
    DBpagnum oldpgn;
    DBshort  oldato;
    DBptr    lstadr,nxtadr;
    char    *datpek;
    DBint    left;

    if ( size == 0 ) return(0);
/*
***Eftersom h�r kommer att allokeras bara hela sidor sparar vi
***dpgnum och datofs s� att vi kan �terst�lla dom efter oss.
***Annars kommer dpgnum och datofs att peka p� sista allokerade sidan
***n�r vi �r klara och n�sta anrop till alldat allokera en helt ny sida.
***Is�fall utnyttjas inte det utrymme som finns p� dpgnum. Lite on�digt !
***Om datofs=0 f�r man dock passa sig f�r d� allokeras just denna sida !
***Se l�ngre ner...
*/
     oldpgn = dpgnum;
     oldato = datofs;
/*
***Allokera plats f�r f�rsta sidan.
*/
    if ( (lstadr=alldat(PAGSIZ)) < 0 ) return(-3);
/*
***Initiera lokala variabler.
*/
    left = size;
    datpek = chrpek;
    *lapek = lstadr;
/*
***Resten av sidorna.
*/
    while ( left > DATSIZ )
      {
      if ( (nxtadr=alldat(PAGSIZ)) < 0 ) return(-3);
      updata((char *)&nxtadr,lstadr,sizeof(DBptr)); /* L�nk */
      updata(datpek,lstadr+sizeof(DBptr),DATSIZ);   /* Data */

      lstadr = nxtadr;
      left -= DATSIZ;
      datpek += DATSIZ;
      }
/*
***Sista sidan. Eftersom den inte s�kert fylls helt med data
***m�ste den f�rst nollst�llas s� att den kan frig�ras
***av rldat2() vid ett senare tillf�lle. Detta g�rs av alldat().
*/
    nxtadr = DBNULL;
    updata((char *)&nxtadr,lstadr,sizeof(DBptr));   /* L�nk */
    updata(datpek,lstadr+sizeof(DBptr),left);       /* Data */
/*
***�terst�ll aktiv data-sida och offset s� att detta utrymme
***kan allokeras av n�sta anrop till alldat. Om oldato == 0
***f�r man dock passa sig f�r d� har �ven oldpgn allokerats.
***Bug 1997-05-28 JK.
*/
    if ( oldato > 0 )
      {
      dpgnum = oldpgn;
      datofs = oldato;
      }
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus rddat1(char *chrpek, DBptr la, DBint size)

/*      L�ser data ur GM. M�ngden data som skall
 *      l�sas f�r inte �verstiga 1 sida.
 *
 *      In: chrpek   => Char-pekare dit data �nskas.
 *          la       => GM-adress d�r data finns.
 *          size     => Antal char som skall l�sas.
 *
 *      Ut: *chrpek  => H�mtar data ur GM.
 *
 *      FV:  0 => Ok.
 *          -1 => Mer data �n en sida har beg�rts.
 *
 *      (C)microform ab 22/12/84 J. Kjellander
 *
 *      11/10/86  IBMATX, J. Kjellander
 *
 ******************************************************!*/

  {
    DBint    offset;
    DBpagnum pagnum;
    DBptr    pagadr;

/*
***Kontrollera indata.
*/
    if ( size > PAGSIZ ) return(-1);
/*
***Ber�kna sidnummer, adress och offset.
*/
    pagnum = la/PAGSIZ;           /* Ber�kna sidnummer */
    offset = la - PAGSIZ*pagnum; /* Offset inom sidan */
    if ( (pagadr=logtab[pagnum].ptr) < 0 ) pagadr=pfault(pagnum);

    fystab[pagadr/PAGSIZ].reflg = TRUE;
/*
***L�s data.
*/    
    V3MOME(gmbuf+pagadr+offset, chrpek, size);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus rddat2(char *chrpek, DBptr la, DBint size)

/*      L�ser data ur GM. Data f�ruts�tts ha skrivits
 *      med wrdat2, dvs. data kan sp�nna �ver flera
 *      sidor.
 *
 *      In: chrpek   => Char-pekare dit data �nskas.
 *          la       => GM-adress till f�rsta sidan.
 *          size     => Antal char som skall l�sas.
 *
 *      Ut: *chrpek => H�mtar data ur GM.
 *
 *      FV:  0 => Ok.
 *
 *      (C)microform ab 10/7/85 J. Kjellander
 *
 *      2/2/86  size == 0, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  lstadr,nxtadr;
    char  *datpek;
    DBint  left;

    if ( size == 0 ) return(0);
/*
***Initiera lokala variabler.
*/
    lstadr = la;
    rddat1((char *)&nxtadr,lstadr,sizeof(DBptr));  /* 1:a sidans l�nk */
    datpek = chrpek;
    left = size;
/*
***L�s-loop.
*/
    while ( nxtadr != DBNULL )
      {
      rddat1(datpek,lstadr+sizeof(DBptr),DATSIZ);   /* Data */
      lstadr = nxtadr;
      rddat1((char *)&nxtadr,lstadr,sizeof(DBptr)); /* N�sta l�nk */
      left -= DATSIZ;
      datpek += DATSIZ;
      }
/*
***Sista sidan.
*/    
    rddat1(datpek,lstadr+sizeof(DBptr),left);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus updata(char *chrpek, DBptr la, DBint size)

/*      Skriver �ver existerande data. Max en sida.
 *
 *      In: chrpek => Char-pekare till data.
 *          la     => GM-adress.
 *          size   => Antal char som skall lagras.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 27/4/85 J. Kjellander
 *
 *      11/10/86  IBMATX, J. Kjellander
 *
 ******************************************************!*/

  {
    DBint    offset;
    DBptr    pagadr;
    DBpagnum pagnum;

    if ( size == 0 ) return(0);
/*
***Skriv.
*/
    pagnum = la/PAGSIZ;           /* Ber�kna sidnummer */
    offset = la - PAGSIZ*pagnum; /* Offset inom sidan */
    if ( (pagadr=logtab[pagnum].ptr) < 0 ) pagadr=pfault(pagnum);

    fystab[pagadr/PAGSIZ].wrflg = TRUE;
    fystab[pagadr/PAGSIZ].reflg = TRUE;

    V3MOME( chrpek, gmbuf+pagadr+offset, size);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus rldat1(DBptr la, DBint size)

/*      EMPTY-st�ller size stycken char fr�n la. Kollar
 *      sen om hela sidan �r tom och deallokerar den om
 *      s� �r fallet. Om sidan=dpgnum och sidan blev helt
 *      tom nollst�lls datofs s� att ny allokering sker
 *      fr�n b�rjan av sidan. Sidan deallokeras is�fall 
 *      inte.
 *
 *      In: la   => LA.
 *          size => Antal char.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *          -1  => Otill�ten LA.
 *          -2  => Size g�r �ver page-gr�ns.
 *
 *      (C)microform ab 16/12/84 J. Kjellander
 *
 *      11/10/86 IBMATX, J. Kjellander
 *      29/11/92 Sl�pp fystab, J. Kjellander
 * 
 *
 ******************************************************!*/
  {
    DBptr    pagadr,gmbadr;
    DBpagnum pagnum,ftabnr;
    DBint    offset,i;
    FTBSTR  *ftabpk;

/*
***Indatakontroll.
*/
    if ( size == 0 ) return(0);
/*
***Ber�kna sidnummer samt offset.
*/
    pagnum = la/PAGSIZ;
    if ( pagnum > metdat.logmax-1 ) return(-1);
    offset = la - PAGSIZ*pagnum;
    if ( PAGSIZ-offset < size ) return(-2);
/*
***Skriv in EMPTY.
*/
    if ( (pagadr=logtab[pagnum].ptr) < 0 ) pagadr=pfault(pagnum);

    ftabnr = pagadr/PAGSIZ;
    ftabpk = &fystab[ftabnr];
    ftabpk->wrflg = TRUE;
    ftabpk->reflg = TRUE;

    gmbadr = pagadr + offset;        /* Adress i gmbuf */
    for ( i=0 ; i<size; ++i )
        gmbuf[gmbadr+i] = EMPTY;
/*
***Kolla om hela sidan EMPTY.
*/
    i=0;
    while ( gmbuf[pagadr+i]==EMPTY && i<PAGSIZ) ++i;
/*
***Om det �r sidan dpgnum nollst�ller vi datofs.
*/
    if ( i == PAGSIZ )
      {
      if ( pagnum == dpgnum ) datofs=0;
/*
***Om det inte �r sidan dpgnum s�tter vi logtab.all = FALSE.
***Om motsvarande sida i fystab inte �nnu har n�gon PF-sida
***allokerad sl�pper vi den helt och situationen �r densamma
***som efter gmclr(). 
***Om den har PF-sida allokerad ger vi tillbaks den till PF
***och sl�pper den s� att den kan allokeras p� nytt direkt.
*/
      else
        {
        logtab[pagnum].all = FALSE;

        if ( ftabpk->pfpadr == PGFPNA ) logtab[pagnum].ptr =  PGFPNA;
        else                            logtab[pagnum].ptr = -ftabpk->pfpadr;

        ftabpk->pfpadr = PGFPNA;
        ftabpk->pagnum = GMBPNA;
        ftabpk->wrflg  = FALSE;
        ftabpk->reflg  = FALSE;
        if ( ftabnr < fystal ) fystal = ftabnr;
        }
      }
    return(0);
    }

/********************************************************/
/*!******************************************************/

        DBstatus rldat2(DBptr la, DBint size)

/*      EMPTY-st�ller ett datablock som skrivits med
 *      hj�lp av wrdat2. Sidorna frig�rs genom upp-
 *      repade anrop till rldat1().
 *
 *      In: la   => LA.
 *          size => Antal char.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 10/7/85 J. Kjellander
 *
 *      28/11/92 Bug, J. Kjellander
 *
 ******************************************************!*/
  {
    DBptr  lstadr,nxtadr;
    DBint  left;

    if ( size == 0 ) return(0);
/*
***Initiera lokala variabler.
*/
    lstadr = la;
    rddat1((char *)&nxtadr,lstadr,sizeof(DBptr));  /* 1:a sidans l�nk */
    left = size;
/*
***Loop. Bug, "if" bytt mot "while" 28/11/92 JK.
*/
    while ( nxtadr != DBNULL )
      {
      rldat1(lstadr,PAGSIZ);                        /* Nollst�ll */
      lstadr = nxtadr;
      rddat1((char *)&nxtadr,lstadr,sizeof(DBptr)); /* N�sta l�nk */
      left -= DATSIZ;
      }
/*
***Sista sidan inkl pekare.
*/    
    rldat1(lstadr,left+sizeof(DBptr));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        char *gmgadr(DBptr la)

/*      Ber�knar den mot la svarande C-adressen och
 *      ser till att sidan finns i GM:s buffer.
 *
 *      In: la       => GM-adress d�r max 1 sida data finns.
 *
 *      Ut: Inget.
 *
 *      FV: C-pekare. 
 *
 *      (C)microform ab 17/3/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint    offset;
    DBpagnum pagnum;
    DBptr    pagadr;

/*
***Ber�kna sidnummer, adress och offset.
*/
    pagnum = la/PAGSIZ;           /* Ber�kna sidnummer */
    offset = la - PAGSIZ*pagnum;  /* Offset inom sidan */
    if ( (pagadr=logtab[pagnum].ptr) < 0 ) pagadr=pfault(pagnum);

    fystab[pagadr/PAGSIZ].reflg = TRUE;
/*
***Returnera adressen.
*/    
    return((char *)(gmbuf+pagadr+offset));
  }

/********************************************************/
/*!******************************************************/

        DBptr gtlain(DBptr la, DBordnum ordnr, DBetype *ptyp)

/*      Returnerar la f�r en viss instans.
 *
 *
 *      In: la     => LA f�r sista instansen.
 *          ordnr  => Instansens ordningsnummer
 *          ptyp   => Pekare till DBetype-variabel
 *
 *      Ut: *typ   => Instansens typ
 *
 *      FV:     >0 => Instansens LA.
 *              -1 => Instans med angivet ordningsnummer
 *                    finns ej.
 *
 *      (C)microform ab 21/10/85 J. Kjellander
 *
 *      10/11/85 Uppsnabbning, J. Kjellander
 *
 ******************************************************!*/

  {
     DBptr la_tmp;
     GMRECH hed;

/*
***Initiering.
*/
     la_tmp = la;
/*
***Kolla om n�gon instans med angivet ordningsnummer finns.
*/
     do
       {
       DBread_header(&hed,la_tmp);
       if ( hed.ordnr == ordnr )
         {
         *ptyp = hed.type;
         return(la_tmp);
         }
       else if ( hed.ordnr > ordnr)
         {
         la_tmp = hed.n_ptr;
         }
       else
         {
         return(-1);
         }
       }
     while ( la_tmp != DBNULL );
/*
***Ingen instans med angivet ordningsnr. har hittats.
*/
     return(-1);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmdlid(DBseqnum sekvnr)

/*      Stryker ett entry i idtab.
 *
 *      In: sekvnr => Sekvensnummer som skall strykas.
 *
 *      Ut: Inget.
 *
 *      FV:   0 => Ok.
 *           -1 => Sekvensnumret ligger utanf�r idtab.
 *
 *      (C)microform ab 5/5/85 J. Kjellander
 *
 ******************************************************!*/

  {
    if ( sekvnr > actidm ) return(-1);

    wrid( sekvnr,(DBptr)ERASED);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmagpp(DBptr la, DBptr la_grp)

/*      L�gger till en ny grupp-pekare til storheten
 *      vid la.
 *
 *      In: la     => Storhetens GM-adress.
 *          la_grp => Gruppens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *          -1  => Alla 3 grupp-pekare upptagna.
 *
 *      (C)microform ab 5/8/85 J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH  hed;

/*
***L�s storhetens record-header.
*/
    DBread_header( &hed, la);
/*
***Leta upp ledig grupp-pekare.
*/
         if ( hed.g_ptr[0] == DBNULL ) hed.g_ptr[0] = la_grp;
    else if ( hed.g_ptr[1] == DBNULL ) hed.g_ptr[1] = la_grp;
    else if ( hed.g_ptr[2] == DBNULL ) hed.g_ptr[2] = la_grp;
    else return(-1);
/*
***Lagra record-header igen.
*/
    DBupdate_header( &hed, la);

    return(0);

  }

/********************************************************/
