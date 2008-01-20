/**********************************************************************
*
*    WPprint_GL.c
*    ============
*
*    This file is part of the VARKON WindowPac library.
*    URL: http://www.varkon.com
*
*    WP??
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
#include "../../WP/include/WP.h"


/********************************************************/

         short WPprint_GL(WPRWIN *rwinpt)

/*      A first test of Armins TIFF creator. Called
 *      from the button handler of WPRWIN as f150.
 *      WPGWIN currently also uses f150 but calls
 *      WPprint_dialog(). This will be fixed later.
 *
 *      In: rwinpt = C ptr to the WPRWIN to be printed
 *
 *      (C)2008-01-16 J.Kjellander
 *
 ********************************************************/

  {
   int      status;
   double   dpi;
   GLdouble mdwMatrix[16];

/*
***A noise so that we know we are here !
*/
   WPbell();
/*
***Activate the OpenGL RC of this window.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Set up the input to Armin Faltl's TIFF creator.
***1. The projMatrix[] is to be removed according to Armin ?
***2. Get the mdvwMatrix[] from GL. This works as long as
***   we want to print what we see.
*/
   glGetDoublev(GL_MODELVIEW_MATRIX,mdwMatrix);
/*
***3. Camera settings.
*/

/*
***4. dpi is the resolution. 300.0 is a good start according
***   to Armin.
*/
   dpi = 300.0;
/*
***5. width
***6. height
***7. nerarVal
***8. farVal
***9. Filename
*/


/*
***Call the TIFF creator.
*/
/*   status = gl_plot(mdvwMatrix); */
/*
***The end.
*/
   return(0);
  }

/********************************************************/
