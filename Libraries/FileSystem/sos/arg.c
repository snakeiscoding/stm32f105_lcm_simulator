/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded 
 *
 * This software is copyrighted by and is the sole property of 
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,  
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#include "arg.h"

typedef struct {
  unsigned char maxlen;

  char *ptr;
  char *e_ptr;
  
  char *p_ptr;
  char *a_ptr;
} s_Arg;
static s_Arg Arg;

void arg_init (char *str, unsigned char maxlen, unsigned char _new)
{
  unsigned char i,len;

  len=strlen(str);
  if (_new)
  {
    Arg.maxlen=maxlen;
    Arg.ptr=str;
    Arg.a_ptr=Arg.ptr;
    Arg.e_ptr=str+len;
  }

  for (i=0;i<len;i++)
  {
    if (*(str+i)==SOS_KEY_SPACE) *(str+i)=0; 
  }
}

char *arg_next (void)
{
  Arg.p_ptr=NULL;
  if (Arg.a_ptr<Arg.e_ptr)
  {
    while (*(Arg.a_ptr)==0 && Arg.a_ptr<Arg.e_ptr) ++(Arg.a_ptr);
    if (Arg.a_ptr<Arg.e_ptr)
    {
      Arg.p_ptr=Arg.a_ptr;
      while (*(Arg.a_ptr) && Arg.a_ptr<Arg.e_ptr) ++(Arg.a_ptr);
    }
  }
  return(Arg.p_ptr);
}

void arg_destroy (void)
{
  for (Arg.a_ptr=Arg.ptr;Arg.a_ptr<Arg.e_ptr;Arg.a_ptr++)
  {
    if (*(Arg.a_ptr)==0) *(Arg.a_ptr)=SOS_KEY_SPACE;
  }
  Arg.a_ptr=Arg.ptr;
}







