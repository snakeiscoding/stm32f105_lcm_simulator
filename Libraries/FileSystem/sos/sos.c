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
#include "sos.h"
#include "cmd.h"
#include "../demo/demo.h"


#define F_DIRECTORIES		 1

static char inp[MAX_CMD];

#ifdef Debug_UART

void __printbuf (char *buf, int len)
{
  demo_printbuf(buf,len);
}

void __putchar (unsigned char ch)
{
  demo_putchar(ch);
}

unsigned short __getkey (void)
{
  return demo_getkey();
}

void print_prompt (void)
{
#if F_DIRECTORIES
  char cwd[MAX_CMD];
  f_getcwd(cwd,MAX_CMD);
  __printf1("%s$ ",cwd);
#else
  __printf0("Hakuto$");
#endif
}




void sos_msg (char *msg)
{
  __printf0("\r\n");
  __printf0(msg);
  print_prompt();
}

static unsigned char pos;
void sos_init (void)
{
  pos=0;
  __printf0("\r\n\r\nSystem started...\r\n");

  print_prompt();

}

void sos_run (void)
{
  unsigned char ch;
  unsigned short chr;
#ifdef HISTORY_ENABLE
  char *ins;
#endif

  /*while (1)*/
  {
    chr=__getkey();
    if (chr==0xff00) return;
    ch=(unsigned char)chr;
/*
    __printf("%bd ",ch);
  }
  {
*/
    if (ch==SOS_KEY_CR || ch==SOS_KEY_LF)
    {
      __printf0("\r\n");
      inp[pos]=0;


#ifdef HISTORY_ENABLE
      history_insert(inp);
#endif

      arg_init(inp,MAX_CMD,1);
      cmd_exec(arg_next());

      pos=0;
      print_prompt();
    }
    else
    {
      if (ch==SOS_KEY_BACKSPACE)
      {
        if (pos)
        {
          __putchar(ch);
          __putchar(' ');
          __putchar(ch);
          --pos;
        }
      }
#ifdef HISTORY_ENABLE
      else if (ch==SOS_KEY_CTRL_A || ch==SOS_KEY_CTRL_Z)
      {
        if (ch==SOS_KEY_CTRL_A) ins=history_get_prev();
                       else ins=history_get_next();

        if (ins)
        {
          print_new_cmd(ins,pos);
          pos=strlen(ins);
          strcpy(inp,ins);
        }
      }
#endif
      else if (pos<MAX_CMD)
      {
        if ( (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>='0' && ch<='9')
            || ch=='.' || ch=='/' || ch=='\\' || ch==SOS_KEY_SPACE
            || ch=='~' || ch=='*' || ch=='!' || ch=='?' || ch=='-' || ch==':')
        {
          __putchar(ch);
          inp[pos++]=ch;
        }

      }
    }
  }
}

#endif
