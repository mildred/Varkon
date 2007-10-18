/*!*****************************************************
*
*    exodbc.c
*    ========
*
*    EXodco();      Interface routine for ODBC_Connect
*    EXoded();      Interface routine for ODBC_Execdirect
*    EXodfe();      Interface routine for ODBC_Fetch
*    EXodgs();      Interface routine for ODBC_Getstring
*    EXoddc();      Interface routine for ODBC_Disconnect
*    EXoder();      Interface routine for ODBC_Error
*
*    This file is part of the VARKON Execute  Library.
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"

#ifdef V3_ODBC
#ifdef WIN32
#include <windows.h>
#endif
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#endif
/*
*******************************************************!*/

#ifdef V3_ODBC
static bool  connected = FALSE;  /* TRUE om vi �r connected */
static bool  statement = FALSE;  /* TRUE om statement exekverats */
static UWORD column;             /* Kolumn att h�mta data ur */
static char  errtxt[V3STRLEN+1] = "No datasource connected !";
                                 /* ODBC-felmeddelande */
static HENV  env_handle;         /* Gemensam variabel */
static HDBC  connect_handle;     /* Gemensam variabel */
static HSTMT stmt_handle;        /* Gemensam variabel */
#endif

extern short erpush();

/*!******************************************************/

        short  EXodco(
        char  *source,
        char  *user,
        char  *passw,
        DBint *pstat)
 
/*      Interface-rutin f�r ODBC_CONNECT.
 *
 *      In: source = Datak�lla
 *          user   = Username
 *          passw  = Password
 *
 *      Ut: *pstat = Status fr�n ODBC.
 *
 *      Felkoder: EX3153 = Not implemented in UNIX-VARKON
 *                EX3172 = Already connected to %s
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef V3_ODBC
   RETCODE return_code;
   UCHAR   state[SQL_MAX_MESSAGE_LENGTH];
   UCHAR   message[SQL_MAX_MESSAGE_LENGTH];
   SDWORD  native_error;
   SWORD   meslen;

/*
***Kolla att vi inte redan �r connectade.
*/
   if ( connected ) return(erpush("EX3172",source));
/*
***Allokera ett environment- och ett connection-handle.
*/
   message[0] = '\0';

   return_code = SQLAllocEnv(&env_handle);
   if ( return_code == SQL_ERROR )
     {
     SQLError(SQL_NULL_HENV,SQL_NULL_HDBC,SQL_NULL_HSTMT,
              state,&native_error,message,
              SQL_MAX_MESSAGE_LENGTH-1,&meslen);
     goto error;
     }

   return_code = SQLAllocConnect(env_handle,&connect_handle);
   if ( return_code == SQL_ERROR )
     {
     SQLError(env_handle,SQL_NULL_HDBC,SQL_NULL_HSTMT,
              state,&native_error,message,
              SQL_MAX_MESSAGE_LENGTH-1,&meslen);
     SQLFreeEnv(env_handle);
     goto error;
     }
/*
***G�r connect till databasen.
*/
   return_code = SQLConnect(connect_handle,
                            source,(SWORD)strlen(source),
                            user,(SWORD)strlen(user),
                            passw,(SWORD)strlen(passw));

   if ( return_code == SQL_ERROR ) 
     {
     SQLError(env_handle,connect_handle,SQL_NULL_HSTMT,
              state,&native_error,message,
              SQL_MAX_MESSAGE_LENGTH-1,&meslen);
     SQLFreeConnect(connect_handle);
     SQLFreeEnv(env_handle);
     goto error;
     }
   else
     {
    *errtxt = '\0';
     connected = TRUE;
    *pstat = 0;
     return(0);
     }
/*
***Felhantering.
*/
error:
   if ( strlen(message) > V3STRLEN ) message[V3STRLEN] = '\0';
   strcpy(errtxt,message);
  *pstat = -1;
   return(0);
#else
   return(erpush("EX3153",""));
#endif
  }

/********************************************************/
/*!******************************************************/

        short  EXoded(
        char  *sqlstr,
        DBint *pstat)
 
/*      Interface-rutin f�r ODBC_EXECDIRECT.
 *
 *      In: sqlstr = SQL-str�ng
 *
 *      Ut: *pstat = Status
 *
 *      Felkoder: EX3153 = Not implemented in UNIX-VARKON
 *                EX3162 = %s, No ODBC source connected
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef V3_ODBC
   RETCODE return_code;
   UCHAR   state[SQL_MAX_MESSAGE_LENGTH];
   UCHAR   message[SQL_MAX_MESSAGE_LENGTH];
   SDWORD  native_error;
   SWORD   meslen;
/*
***Kolla att vi �r connectade.
*/
   if ( !connected ) return(erpush("EX3162","ODBC_EXECDIRECT"));
/*
***Frig�r ev. aktivt statement.
*/
   if ( statement )
     {
     SQLFreeStmt(stmt_handle,SQL_DROP);
     statement = FALSE;
     }
/*
***Allokera nytt statement-handle.
*/
   return_code = SQLAllocStmt(connect_handle,&stmt_handle);
   if ( return_code == SQL_ERROR )
     {
     SQLError(env_handle,connect_handle,SQL_NULL_HSTMT,
              state,&native_error,message,
              SQL_MAX_MESSAGE_LENGTH-1,&meslen);
     goto error;
     }
/*
***Exekvera SQL-satsen.
*/
   return_code = SQLExecDirect(stmt_handle,
                               sqlstr,(SDWORD)strlen(sqlstr));

   if ( return_code == SQL_ERROR )
     {
     SQLError(env_handle,connect_handle,stmt_handle,
              state,&native_error,message,
              SQL_MAX_MESSAGE_LENGTH-1,&meslen);
     SQLFreeStmt(stmt_handle,SQL_DROP);
     goto error;
     }
   else
     {
     statement = TRUE;
    *errtxt = '\0';
    *pstat = 0;
     return(0);
     }
/*
***Felhantering.
*/
error:
   if ( strlen(message) > V3STRLEN ) message[V3STRLEN] = '\0';
   strcpy(errtxt,message);
  *pstat = -1;
   return(0);
#else
   return(erpush("EX3153",""));
#endif
  }

/********************************************************/
/*!******************************************************/

        short EXodfe(DBint *pstat)
 
/*      Interface-rutin f�r ODBC_FETCH.
 *
 *      In: Inget.
 *
 *      Ut: *pstat = Status
 *
 *      Felkoder: EX3153 = Not implemented in UNIX-VARKON
 *                EX3162 = %s, No ODBC source connected
 *
 *      (C)microform ab 1998-10-13 J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef V3_ODBC
   RETCODE return_code;
   UCHAR   state[SQL_MAX_MESSAGE_LENGTH];
   UCHAR   message[SQL_MAX_MESSAGE_LENGTH];
   SDWORD  native_error;
   SWORD   meslen;

/*
***Kolla att vi �r connectade.
*/
   if ( !connected ) return(erpush("EX3162","ODBC_FETCH"));
/*
***Kolla att SQL-sats exekverats. Om det inte �r en s�kning
***finns visserligen inget resultat att h�mta men den
***felsituationen f�r ODBC ta hand om.
*/
   if ( !statement ) return(erpush("EX3182","ODBC_FETCH"));
/*
***SQLFetch() positionerar cursorn p� n�sta rad
***i resultatet om det finns n�gon.
*/
   return_code = SQLFetch(stmt_handle);
/*
***Fel med felmeddelande fr�n ODBC.
*/
   if ( return_code == SQL_ERROR )
     {
     SQLError(env_handle,connect_handle,stmt_handle,
              state,&native_error,message,
              SQL_MAX_MESSAGE_LENGTH-1,&meslen);
     goto error;
     }
/*
***Det fanns inget resultat.
*/
   else if ( return_code == SQL_NO_DATA_FOUND )
     {
     strcpy(message,"ODBC_FETCH: No data found !");
     goto error;
     }
/*
***Det gick bra.
*/
   else if ( return_code == SQL_SUCCESS  ||
             return_code == SQL_SUCCESS_WITH_INFO )
     {
     column = 1;
    *errtxt = '\0';
    *pstat = 0;
     return(0);
     }
/*
***�vriga fel.
*/
   else
     {
     switch ( return_code )
       {
       case SQL_STILL_EXECUTING:
       strcpy(message,"ODBC_FETCH: STILL EXECUTING");
       break;

       case SQL_INVALID_HANDLE:
       strcpy(message,"ODBC_FETCH: INVALID HANDLE");
       break;

       default:
       sprintf(message,"ODBC_FETCH: Unknown error %d",return_code);
       break;
       }
     goto error;
     }
/*
***Felhantering.
*/
error:
   if ( strlen(message) > V3STRLEN ) message[V3STRLEN] = '\0';
   strcpy(errtxt,message);
  *pstat = -1;
   return(0);
#else
   return(erpush("EX3153",""));
#endif
  }

/********************************************************/
/*!******************************************************/

        short  EXodgs(
        char  *bufptr,
        DBint *pstat)
 
/*      Interface-rutin f�r ODBC_GETSTRING.
 *
 *      In: Inget.
 *
 *      Ut: *bufptr = H�mtat v�rde
 *          *pstat  = Status
 *
 *      Felkoder: EX3153 = Not implemented in UNIX-VARKON
 *                EX3162 = %s, No ODBC source connected
 *                EX3182 = %s, Inget statement aktivt
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef V3_ODBC
   RETCODE return_code;
   UCHAR   state[SQL_MAX_MESSAGE_LENGTH];
   UCHAR   message[SQL_MAX_MESSAGE_LENGTH];
   SDWORD  native_error;
   SWORD   meslen;
   SDWORD  nbytes;
/*
***Kolla att vi �r connectade.
*/
   if ( !connected ) return(erpush("EX3162","ODBC_GETSTRING"));
/*
***Kolla att SQL-sats exekverats. Om det inte �r en s�kning
***finns visserligen inget resultat att h�mta men den
***felsituationen f�r ODBC ta hand om.
*/
   if ( !statement ) return(erpush("EX3182","ODBC_GETSTRING"));
/*
***SQLGetData() kopierar v�rde fr�n aktuell fetch-
***rad till lokal C-variabel. Ett anrop f�r
***varje kolumn. B�rja fr�n v�nster och g�
***�t h�ger. EXodfe() s�tter column = 1 och
***h�r r�knar vi upp den efter varje lyckad l�sning.
*/
   return_code = SQLGetData(stmt_handle,
                             column,
                             SQL_C_CHAR,
                             bufptr,
                             (SDWORD)V3STRLEN,
                             &nbytes);
/*
***Fel med felmeddelande fr�n ODBC.
*/
   if ( return_code == SQL_ERROR )
     {
     SQLError(env_handle,connect_handle,stmt_handle,
              state,&native_error,message,
              SQL_MAX_MESSAGE_LENGTH-1,&meslen);
     goto error;
     }
/*
***Det fanns inget resultat.
*/
   else if ( return_code == SQL_NO_DATA_FOUND )
     {
     strcpy(message,"ODBC_GETSTRING: No data found !");
     goto error;
     }
/*
***Jo det gjorde det.
*/
   else if ( return_code == SQL_SUCCESS  ||
             return_code == SQL_SUCCESS_WITH_INFO )
     {
    ++column;
    *errtxt = '\0';
    *pstat = 0;
     return(0);
     }
/*
***�vriga fel.
*/
   else
     {
     switch ( return_code )
       {
       case SQL_STILL_EXECUTING:
       strcpy(message,"ODBC_GETSTRING: STILL EXECUTING");
       break;

       case SQL_INVALID_HANDLE:
       strcpy(message,"ODBC_GETSTRING: INVALID HANDLE");
       break;

       default:
       sprintf(message,"ODBC_GETSTRING: Unknown error %d",return_code);
       break;
       }
     goto error;
     }
/*
***Felhantering.
*/
error:
   if ( strlen(message) > V3STRLEN ) message[V3STRLEN] = '\0';
   strcpy(errtxt,message);
  *pstat = -1;
   return(0);
#else
   return(erpush("EX3153",""));
#endif
  }

/********************************************************/
/*!******************************************************/

        short EXoddc(DBint *pstat)
 
/*      Interface-rutin f�r ODBC_DISCONNECT.
 *
 *      In: Inget.
 *
 *      Ut: *pstat = Status
 *
 *      Felkoder: EX3153 = Not implemented in UNIX-VARKON
 *                EX3162 = %s, No ODBC source connected
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef V3_ODBC
/*
***Kolla att vi �r connectade.
*/
   if ( !connected ) return(erpush("EX3162","ODBC_DISCONNECT"));
/*
***Frig�r ev. vilande statement.
*/
   if ( statement ) SQLFreeStmt(stmt_handle,SQL_DROP);
   statement = FALSE;
/*
***Koppla ner.
*/
   SQLDisconnect(connect_handle);
   connected = FALSE;
/*
***Frig�r resten av resurserna.
*/
   SQLFreeConnect(connect_handle);
   SQLFreeEnv(env_handle);

   strcpy(errtxt,"No datasource connected !");

  *pstat = 0;

   return(0);
#else
   return(erpush("EX3153",""));
#endif
  }

/********************************************************/
/*!******************************************************/

        short EXoder(char *buf)
 
/*      Interface-rutin f�r ODBC_ERROR.
 *
 *      In: Inget.
 *
 *      Ut: *buf = Senaste felmeddelande
 *
 *      Felkoder: EX3153 = Not implemented in UNIX-VARKON
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef V3_ODBC
   strcpy(buf,errtxt);
   return(0);
#else
   return(erpush("EX3153",""));
#endif
  }

/********************************************************/
