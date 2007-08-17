/*!******************************************************************/
/*  File: isch.h                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.varkon.com                                     */
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

#include <ctype.h>

/*
***F�r att slippa problem i blandade 7-8-bitars ASCII milj�er
***�r det lika bra att till�ta b�de 7- och 8-bitars "������".
***Under SCO/UNIX verkar isialpha() fungera annorlunda �n under
***tex. AIX s� det �r b�st att ha med b�de 7- och 8-bitars 
***svenska tecken explicit.
*/

#define isialpha(c)     (isalpha(c) || ((c) == 229) || ((c) == 228) || ((c) == 246) || ((c) == 197) || ((c) == 196) || ((c) == 214) || ((c) == '}') || ((c) == '{') || ((c) == '|') || ((c) == ']') || ((c) == '[') || ((c) == '\\') )

#define isilower(c)     (islower(c) || ((c) == 229) || ((c) == 228) || ((c) == 246) || ((c) == '}') || ((c) == '{') || ((c) == '|') )

/*
***H�r stog det tidigare _toupper. 1998-11-19
*/
#define toiupper(c)     (toupper(c))

/********************************************************************/
