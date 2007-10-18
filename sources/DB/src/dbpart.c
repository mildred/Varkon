/**********************************************************************
*
*    dbpart.c
*    ========
*
*
*    This file includes the following public functions:
*
*    DBinsert_part();           Inserts/opens a part entity
*    DBclose_part();            Closes a part
*    DBread_part();             Reads a part entity
*    DBread_part_parameters();  Reads parameter data
*    DBupdate_part_parameter(); Updates a parameter value
*    DBread_part_attributes();  Reads attribute data
*    DBdelete_part();           Deletes a part entity
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

/*!******************************************************/

        DBstatus DBinsert_part(
        GMPRT    *prtpek,
        GMPDAT   *prtdat,
        V2PATLOG typarr[],
        char     valarr[],
        DBId     *idpek,
        V2NAPA   *pnp,
        DBptr    *lapek)

/*      Huvudrutin f�r lagring av part. En tom ID-tabell
 *      med r�tt storlek skapas f�rst. D�refter lagras
 *      part-data posten med sina data. Adresser till
 *      ID-tabell och part-data lagras i part-posten som
 *      f�rst d�refter lagras sj�lv. N�r parten
 *      lagrats uppdateras de globala variabler som
 *      h�ller reda p� aktuell part och ID-tabell. Dessa
 *      variabler beh�ller sitt v�rde tills n�sta anrop
 *      till DBinsert_part() eller DBclose_part().
 *
 *      In: prtpek => Pointer to part.
 *          prtdat => Pointer to part data.
 *          typarr => Pointer to type array or NULL.
 *          valarr => Pointer to value array or NULL.
 *          idpek  => Pointer to part ID.
 *          pnp    => Pointer to graphical attributes or NULL.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => The DB pointer.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanf�r virtuellt omr�de.
 *          -2  => IDTAB full.
 *          -3  => Data f�r inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *          -5  => Partens ID-tabell f�r inte plats
 *          -6  => ID-tabellens storlek < 0.
 *
 *      (C)microform ab 23/12/84 J. Kjellander
 *
 *      14/10/85   Headerdata, J. Kjellander
 *      1/4/86     Part-data, J. Kjellander
 *      21/10/86   valsiz > PAGSIZ, J. Kjellander
 *      20/3/92    Attribut-data, J. Kjellander
 *      1999-02-08 pnp+prtdat=NULL, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status,valsiz;
    DBptr   la_idt,la_typ,la_val,la_pdt,la_att;
    GMPDAT *datpek,pd;

/*
***F�r att kunna anropa med prtdat = NULL kollar vi h�r
***och skapar en egen om det beh�vs.
*/
   if ( prtdat == NULL )
     {
     pd.mtyp_pd = 2;
     pd.matt_pd = 3;
     pd.csp_pd = DBNULL;
     pd.npar_pd = 0;
     datpek = &pd;
     }
   else datpek = prtdat;
/*
***Allokera plats f�r ID-tabellen. Om parten inte beh�ver
   n�gon ID-tabell s�tts dess ID-tabell-pekare = 0. Detta
   betyder dock ingenting. Det viktiga �r att ID-tabellens
   storlek och d�rmed actidm �r riktiga s� att inpost()
   kan generera fel om man f�rs�ker lagara en storhet i en
   part som saknar ID-tabell.
*/
    if ( prtpek->its_pt > 0 ) la_idt=allidt(prtpek->its_pt);
    else if ( prtpek->its_pt == 0 ) la_idt = 0;
    else return(-6);
    if ( la_idt < 0 ) return(-5);
/*
***Lagra part-data. F�rst typ-array:en.
*/
    if ( datpek->npar_pd > 0 )
      {
      status = wrdat1((char *)typarr,&la_typ,datpek->npar_pd*sizeof(V2PATLOG));
      if ( status < 0 ) return(status);
/*
***Ber�kna valarr:s storlek. Denna = offset till sista parametern
***plus sista parameterns storlek.
*/
      valsiz = (short)typarr[datpek->npar_pd-1].log_offs;

      switch ( typarr[datpek->npar_pd-1].log_id )
        {
        case C_INT_VA: valsiz += v2intsz; break;
        case C_FLO_VA: valsiz += v2flosz; break;
        case C_STR_VA:
        valsiz += strlen(&valarr[typarr[datpek->npar_pd-1].log_offs])+1;
        break;
        case C_VEC_VA: valsiz += v2vecsz; break;
        case C_REF_VA: valsiz += v2refsz; break;
        case C_FIL_VA: valsiz += v2filesz; break;
        case C_ADR_VA: valsiz += v2addrsz; break;
        }
/*
***Lagra val-array:en.
*/
      if ( valsiz <= PAGSIZ )
        {
        status = wrdat1((char *)valarr,&la_val,valsiz);
        if ( status < 0 ) return(status);
        }
      else
        {
        status = wrdat2((char *)valarr,&la_val,valsiz);
        if ( status < 0 ) return(status);
        }
/*
***Fyll i part-data posten. Om inga parametrar, DBNULL och 0.
*/
      datpek->typp_pd = la_typ;
      datpek->valp_pd = la_val;
      datpek->vsiz_pd = valsiz;
      }
    else
      {
      datpek->typp_pd = DBNULL;
      datpek->valp_pd = DBNULL,
      datpek->vsiz_pd = 0;
      }
/*
***Om aktiv part �r huvud-parten skall �ven attribut lagras.
*/
    if ( actprt == huvprt  &&  pnp != NULL )
      {
      status = wrdat1((char *)pnp,&la_att,sizeof(V2NAPA));
        if ( status < 0 ) return(status);
      datpek->attp_pd = la_att;
      datpek->asiz_pd = sizeof(V2NAPA);
      }
    else
      {
      datpek->attp_pd = DBNULL;
      datpek->asiz_pd = 0;
      }
/*
***Lagra sj�lva part-data posten.
*/
    status = wrdat1((char *)datpek,&la_pdt,sizeof(GMPDAT));
    if ( status < 0 ) return(status);
/*
***Typ-specifika data.
*/ 
    prtpek->hed_pt.type = PRTTYP;
    prtpek->hed_pt.vers = GMPOSTV0;
    prtpek->itp_pt = la_idt;
    prtpek->dts_pt = sizeof(GMPDAT);
    prtpek->dtp_pt = la_pdt;
/*
***Lagra part-posten.
*/
    status = inpost((GMUNON *)prtpek,idpek,lapek,sizeof(GMPRT));
    if ( status < 0 ) return(status);
/*
***Uppdatera actidt,actidm och actprt.
*/
    actidt = la_idt;
    actidm = prtpek->its_pt-1;
    actprt = *lapek;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBclose_part()

/*      St�nger den aktiva parten. Anropas av interpre-
 *      tatorn n�r instruktionen ENDMODULE; exekveras.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0  => Ok.
 *         -1  => Aktiv part = huvudmodulen.
 *
 *      (C)microform ab 24/12/84 J. Kjellander
 *
 ******************************************************!*/

  {
    GMPRT  part;

    if ( actprt == huvprt ) return(-1);

    rddat1((char *) &part, actprt, sizeof(GMPRT));
    actprt = part.hed_pt.p_ptr;    /* Ny aktiv part */

    rddat1((char *) &part, actprt, sizeof(GMPRT));
    actidt = part.itp_pt;          /* Ny ID-tabell */

    if (actprt == huvprt) actidm=huvidm;
    else actidm = part.its_pt-1;        /* Ny id-max */

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_part(
        GMPRT   *prtpek,
        DBptr    la)

/*      Huvudrutin f�r l�sning av part.
 *
 *      In: prtpek => Pekare till en part-structure.
 *          la     => Part-postens adress i GM.
 *
 *      Ut: *prtpek => Part-post.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 24/12/84 J. Kjellander
 *
 ******************************************************!*/

  {
    rddat1( (char *)prtpek, la, sizeof(GMPRT));
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_part_parameters(
        GMPDAT   *pdtpek,
        V2PATLOG typarr[],
        char     valarr[],
        DBptr    la,
        short    size)

/*      Rutin f�r l�sning av part-data.
 *
 *      In: pdtpek => Pekare till en part-data structure.
 *          typarr => Pekare till array f�r typinfo.
 *          valarr => Pekare till array f�r parameterv�rden.
 *          la     => Part-data postens adress i GM.
 *          size   => Part-data postens storlek.
 *
 *      Ut: *prdtpek => Part-data post.
 *          *typarr  => Typinfo.
 *          *valarr  => Parameterv�rden.
 *
 *      FV:       0 => Ok.
 *           GM0074 => Resultatfilen har rev. < 2
 *
 *      (C)microform ab 26/2/86 J.Kjellander
 *
 *      4/4/86   Test av rev., J.Kjellander
 *      21/10/86 vsiz > PAGSIZ, J.Kjellander
 *      1999-01-26 gmsdat_org, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Kolla att resultatfilen inte skapats av ett V3
***med l�gre revision �n 2.
*/
    if ( gmsdat_org.vernr == 1 && gmsdat_org.revnr < 2 )
      return(erpush("DB0074",""));
/*
***L�s sj�lva part-data posten.
*/
    rddat1( (char *)pdtpek, la, size);
/*
***Ev. l�sning av typdata.
*/
    if ( typarr != NULL && pdtpek->typp_pd != DBNULL )
       rddat1( (char *)typarr, pdtpek->typp_pd,
                pdtpek->npar_pd * sizeof(V2PATLOG) );
/*
***Ev. l�sning av parameterv�rden.
*/
    if ( valarr != NULL && pdtpek->valp_pd != DBNULL )
      {
      if ( pdtpek->vsiz_pd <= PAGSIZ )
        {
        rddat1( (char *)valarr, pdtpek->valp_pd, pdtpek->vsiz_pd );
        }
      else
        {
        rddat2( (char *)valarr, pdtpek->valp_pd, pdtpek->vsiz_pd );
        }
      }
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_part_parameter(
        GMPRT   *prtpek,
        short    pnum,
        V2LITVA *newval)

/*      Uppdaterar (byter ut) ett parameter-v�rde.
 *
 *      In: prtpek => Pekare till en part-structure.
 *          pnum   => Parameterns ordningsnummer, 1:a = 1.
 *          newval => Pekare till nytt v�rde.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 2/4/94 J. Kjellander
 *
 *       10/4/95  Bug newsiz, J. Kjellander
 *
 ******************************************************!*/

  {
   short    status;
   DBint    oldlen,newlen,diff,oldsiz,newsiz=0;
   char    *valarr,*valadr;
   DBptr    la_val;
   DBint   *intpek;
   DBfloat *fltpek;
   GMPDAT   pdata;
   V2PATLOG typarr[V2MPMX];

/*
***L�s sj�lva part-data posten.
*/
   rddat1((char *)&pdata,prtpek->dtp_pt,prtpek->dts_pt);
/*
***Hur stor plats tar valarr ? Allokera minne f�r valarr +
***lite till s� att en ev. str�ng-parameter kan f�rl�ngas.
*/
   oldsiz = pdata.vsiz_pd;
   if ( (valarr=v3mall(oldsiz+V3STRLEN,"DBupdate_part_parameter")) == NULL ) return(-2);
/*
***L�s typarr och valarr.
*/
   rddat1((char *)typarr,pdata.typp_pd,pdata.npar_pd*sizeof(V2PATLOG));

   if ( oldsiz <= PAGSIZ ) rddat1(valarr,pdata.valp_pd,oldsiz);
   else                    rddat2(valarr,pdata.valp_pd,oldsiz);
/*
***Parameterns C-adress (i valarr).
*/
   valadr = valarr + typarr[pnum-1].log_offs;
   intpek = (DBint *)valadr;
   fltpek = (DBfloat *)valadr;
/*
***Skriv �ver det gamla v�rdet med det nya.
*/
   switch ( typarr[pnum-1].log_id )
     {
     case C_INT_VA:
    *intpek = newval->lit.int_va;
     newsiz = oldsiz;
     break;

     case C_FLO_VA:
    *fltpek = newval->lit.float_va;
     newsiz = oldsiz;
     break;

     case C_VEC_VA:
    *fltpek = newval->lit.vec_va.x_val; ++fltpek;
    *fltpek = newval->lit.vec_va.y_val; ++fltpek;
    *fltpek = newval->lit.vec_va.z_val;
     newsiz = oldsiz;
     break;

     case C_REF_VA:
     V3MOME(&newval->lit.ref_va[0],valadr,v2refsz);
     newsiz = oldsiz;
     break;
/*
***En str�ng kan vara l�ngre eller kortare. Detta spelar normalt
***ingen roll eftersom str�ngar lagras i valarr med det offset
***de har p� RTS och upptar den plats i valarr som de deklarerats
***till i MBS, dvs. st�rsta t�nkbara plats oavsett aktuellt v�rde.
***Det finns dock ett undantag och det g�ller om sista parametern
***�r en str�ng. DBinsert_part() r�knar d� bara med den aktuella l�ngden.
***�ndrar vi l�ngden p� p� en str�ng-parameter som �r sist m�ste
***vi justera valarr:s storlek ocks�.
*/
     case C_STR_VA:
     if ( pnum == pdata.npar_pd )
       {
       oldlen = strlen(valadr);
       newlen = strlen(newval->lit.str_va);
       diff = newlen - oldlen;
       newsiz = oldsiz + diff;
       }
     else newsiz = oldsiz;

     strcpy(valadr,newval->lit.str_va);
     break;
     }
/*
***Sudda gamla valarr i GM och lagra ny (p� ny plats).
*/
   if ( oldsiz <= PAGSIZ ) rldat1(pdata.valp_pd,oldsiz);
   else                    rldat2(pdata.valp_pd,oldsiz);

   if ( newsiz <= PAGSIZ ) status = wrdat1(valarr,&la_val,newsiz);
   else                    status = wrdat2(valarr,&la_val,newsiz);
/*
***Uppdatera pdata-postens pekare till valarr och storlek.
*/
    pdata.valp_pd = la_val;
    pdata.vsiz_pd = newsiz;
    updata((char *)&pdata,prtpek->dtp_pt,sizeof(GMPDAT));
/*
***L�mns tillbaks allokerat minne.
*/
    v3free(valarr,"DBupdate_part_parameter");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_part_attributes(
        char  *attp,
        DBint  attsiz,
        DBptr  la)

/*      L�ser attribut ur GM.
 *
 *      In: attp   => Pekare till resultat.
 *          attsiz => Antal bytes att l�sa.
 *          la     => Attributens adress i GM.
 *
 *      Ut: *attp   => Attribut-data.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 20/3/92 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***N�r nya attribut l�ggs till i V2NAPA �kar
***storleken p� densamma. L�ser man d� attributen
***f�r en gammal part ur GM f�r man inte med alla.
***Om nya attribut alltid l�ggs till p� slutet �r
***detta inget problem.
*/
    return(rddat1(attp,la,attsiz));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_part(DBptr la)

/*      Stryker en part-post.
 *
 *      In: la => Partens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 1/2/86 J. Kjellander
 *
 *      1/4/86   Part-data, J. Kjellander
 *      21/10/86 vsiz > PAGSIZ, J. Kjellander
 *      28/11/92 idtab, J. Kjellander
 *      19/5/94  Attribut och 1.10, J. Kjellander
 *      1997-12-04 idtab igen, J.Kjellander
 *
 ******************************************************!*/

  {
    DBpagnum pagnum;
    DBptr    idla,size;
    DBint    offset,left;
    GMPRT    part;
    GMPDAT   data;

/*
***L�s part-post och data-post.
*/
    DBread_part(&part,la);
    DBread_part_parameters(&data,NULL,NULL,part.dtp_pt,part.dts_pt);
/*
***Stryk typarr om den finns.
*/
    if ( data.typp_pd != DBNULL )
      rldat1(data.typp_pd,data.npar_pd*sizeof(V2PATLOG));
/*
***Stryk valarr om den finns.
*/
    if ( data.valp_pd != DBNULL )
      {
      if ( data.vsiz_pd <= PAGSIZ ) rldat1(data.valp_pd,(DBint)data.vsiz_pd);
      else                          rldat2(data.valp_pd,(DBint)data.vsiz_pd);
      }
/*
***Stryk ev. attribut. I 1.10 fanns inga attribut. Samma
***villkor som i "�ndra part".
*/
    if ( part.dts_pt == sizeof(GMPDAT)  &&  data.attp_pd != DBNULL )
      rldat1(data.attp_pd,(DBint)data.asiz_pd);
/*
***Stryk data-posten.
*/
    rldat1(part.dtp_pt,(DBint)part.dts_pt);
/*
***Stryk idtab. Ny metod 1997-12-01, J.Kjellander
*/
    if ( part.its_pt > 0 )
      {
      idla = part.itp_pt;
      pagnum = idla/PAGSIZ;
      offset = idla - PAGSIZ*pagnum;
      left = PAGSIZ - offset;
      size = part.its_pt*sizeof(DBptr);
/*
***Antingen ligger hela indextabellen p� en och samma sida....
*/
      if ( size <= left ) rldat1(idla,(DBint)size);
/*
***eller ocks� p� flera sammanh�ngande sidor.
*/
      else
        {
        rldat1(idla,(DBint)left);                  /* Stryk 1:a sidan */
        size -= left;
        idla += left;
        while ( size >= PAGSIZ )                 /* Stryk alla utom sista */
          {
          rldat1(idla,PAGSIZ);
          size -= PAGSIZ;
          idla += PAGSIZ;
          }
        if ( size > 0 ) rldat1(idla,(DBint)size);  /* Sista sidan */
        }
      }
/*
***Stryk sj�lva part-posten.
*/
    rldat1(la,sizeof(GMPRT));

    return(0);
  }

/********************************************************/
