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
#include "cmd.h"
//Added By Jason
#include "stm32f10x_lib.h"

extern char *FileName;
extern char *WriteFileName;


unsigned long EraseCounter = 0x00, Address = 0x00;

unsigned long NbrOfPage = 0x00;

u16 ProgramData=0;
u16 Temp=0;

u8 *BufferPtr;
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
u32 JumpAddress;
//......................

#define DUMP_ENABLE		0

#define F_TESTSUITE		0

#define F_FINDING		1
#define F_DIRECTORIES	1
#define F_MKDIR			1
#define F_RMDIR			1
#define F_WRITING		1
#define F_WRITE			1
#define F_RENAME		1
#define F_DELETE		1
#define F_FORMATTING	0
#define F_GETFREESPACE	0
#define F_FILELENGTH	1



#if F_FINDING
static void _dir (void);
#endif
#if F_DIRECTORIES


 void _chdir_b (void);

#if F_WRITING
#if F_MKDIR
static void _mkdir (void);
#endif
#if F_RMDIR
static void _rmdir (void);
#endif
#endif
#endif
#if F_WRITING
#if F_RENAME
static void _rename (void);
#endif
#if F_DELETE
static void _delete (void);
#endif
#if F_FORMATTING
static void _format (void);
#endif
#endif

#if F_GETFREESPACE
static void _stat (void);
#endif




#if DUMP_ENABLE
static void _dump (void);
static void _mr (void);
static void _mw (void);
static void _reg (void);
#endif


#if (F_WRITING && F_WRITE && F_DELETE)
static void _test (void);
#endif
#if (F_WRITING && F_TESTSUITE)
static void _test2 (void);
#endif

static void _help (void);
static void _prog (void);


//Added By Jason

static void _runap (void);

typedef void (*GFN) (void);
typedef struct {
  char *cmd;
  char *help;
  GFN fn;
} t_SOS_ins;

static t_SOS_ins SOS_ins[] = {
#if F_FINDING
  { "dir",	"Lists the contents of actual directory", _dir },
#endif
#if F_DIRECTORIES

#if F_WRITING
#if F_MKDIR
  { "mkdir",	"Creates a directory", _mkdir },
#endif
#if F_RMDIR
  { "rmdir",	"Deletes a directory", _rmdir },
#endif
#endif
#endif
#if F_WRITING
#if F_RENAME
  { "ren",	"Renames a file", _rename },
#endif
#if F_DELETE
  { "del",	"Deletes a file ([all])", _delete },
#endif
#if F_FORMATTING
  { "format",	"Format a volume. Par: 1-FAT12/2-FAT16/3-FAT32 102-soft/201-hard", _format },
#endif
#endif
#if F_GETFREESPACE
  { "stat", "Space statistics of thre volume", _stat },
#endif


#if DUMP_ENABLE
  { "d",	"Dump memory (addr [len])", _dump},
  { "mr",	"Read memory content (addr [size(1|2|4)])", _mr},
  { "mw",	"Read memory content (addr size(1|2|4) val)", _mw},
  { "reg",	"Register read/write (addr [write value])", _reg},
#endif
//  { "dummy",	"???", _dummy},
#if (F_WRITING && F_WRITE && F_DELETE)
  { "test",	"Do r/w/del test with 32k data", _test},
#endif

  { "prog",	"Prog a file", _prog},
  { "run",        "Run Ap Code\r\n", _runap},
   { "help",	"Help list", _help },
  { NULL, NULL, NULL }
};

 unsigned char buf[1025];

unsigned char buf2[128];



int _atoi (char *s)
{
  int r=0;
  int sign;
  if (*s=='-') { sign=-1; ++s; }
          else sign=1;
  do {
    r*=10;
    if (*s>='0' && *s<='9') r+=(*s-'0');
    ++s;
  } while (*s);
  return r*sign;
}

#if DUMP_ENABLE
static unsigned long ahtoi (char *b)
{
  unsigned long rc=0;
  if (*(b+1)=='x' || *(b+1)=='X') b+=2;
  while (*b)
  {
    rc<<=4;
    if (*b>='0' && *b<='9') rc+=(*b-'0');
    else if (*b>='a' && *b<='f') rc+=(*b-'a'+10);
    else if (*b>='A' && *b<='F') rc+=(*b-'A'+10);
	++b;
  }
  return(rc);
}
#endif


#if F_FINDING
static F_FIND f;
int _dircnt=0;
static void _dir (void)
{
  int rc;
  char all[]={"*.*"};
  char *arg;

  arg=arg_next();
  if (arg==NULL) arg=all;
  rc=f_findfirst(arg,&f);
  if (rc==0)
  {
    do {
	  if (f.attr&F_ATTR_DIR)
        { __printf1("\t%s\t<DIR>\r\n",f.filename); }
	  else
        { 

		__printf2("\t%s\t%lu",f.filename,f.filesize);
		
		//Added By Jason For Display Time and date .
		
		__printf5("  %u/%02u/%02u %02u:%02u\r\n ",
			(f.cdate >> 9) + 1980, (f.cdate >> 5) & 15, f.cdate & 31,
			(f.ctime >> 11),(f.ctime >> 5) & 63);
	  	}	

		
    	} while (f_findnext(&f)==0);
  }
  else __printf1("Error code:%d\r\n",rc);
  ++_dircnt;
}
#endif


#if F_DIRECTORIES

 void _chdir_b (void)
{
  unsigned char rc;

  rc=f_chdrive(1);
  if (rc) __printf1("Error:%d\r\n",rc);
}


#if F_WRITING
#if F_MKDIR
static void _mkdir (void)
{
  unsigned char rc;

  rc=f_mkdir(arg_next());
  if (rc) __printf1("Error:%d\r\n",rc);
}
#endif

#if F_RMDIR
static void _rmdir (void)
{
  unsigned char rc;

  rc=f_rmdir(arg_next());
  if (rc) __printf1("Error:%d\r\n",rc);
}
#endif
#endif
#endif


#if F_WRITING
#if F_RENAME
static void _rename (void)
{
  char *p1,*p2;
  p1=arg_next();
  p2=arg_next();
  if (f_rename(p1,p2)) __printf0("File not found.\r\n");
}
#endif


#if F_DELETE && F_WRITING
static void _delete (void)
{
    char *arg;

    arg=arg_next();
#if (F_FINDING && F_DIRECTORIES  && F_RMDIR)
    if (strcmp(arg,"all")==0)
    {
      F_FIND f2;
      unsigned char sd=0,rc,fl=0;

      do {
        rc=f_findfirst("*.*",&f);
        while (rc==0 && f.filename[0]=='.') rc=f_findnext(&f);

        if (rc==0)
        {
          if (f.attr&F_ATTR_DIR)
          {
            ++sd;
            fl=1;
            f2=f;
            f_chdir(f.filename);
            continue;
          }
          else
          {
            f_delete(f.filename);
            rc=f_findnext(&f);
          }
        }

        if (rc && sd && fl)
        {
          f_chdir("..");
          --sd;
          fl=0;
          f=f2;
          f_rmdir(f.filename);
          rc=f_findnext(&f);
        }

        if (rc && sd && !fl)
        {
          f_chdir("/");
          sd=0;
          rc=0;
        }
      } while (rc==0);
    }
    else
#endif
    {
      if (f_delete(arg)) __printf0("File not found.\r\n");
    }
}
#endif


#if F_FORMATTING
static void _format (void)
{
  unsigned char type,m,rc;

  type=_atoi(arg_next());
  m=_atoi(arg_next());
  if (type<1 || type>3)
  {
    __printf0("Not allowed.\r\n");
  }
  else
  {
    rc=1;
    if (m==102) rc=f_format(0,type);

    if (rc) { __printf1("Failed (%d).\r\n",rc); }
       else { __printf0("Succeded.\r\n"); }
  }
}
#endif
#endif


#if F_GETFREESPACE
void _stat (void)
{
  F_SPACE sp;
  (void)f_getfreespace(0,&sp);
  __printf2("Total: %lu\r\nFree : %lu\r\n",sp.total,sp.free);
  __printf2("Used : %lu\r\nBad  : %lu\r\n",sp.used,sp.bad);
}
#endif




#if DUMP_ENABLE
static unsigned char  *_dump_ptr=(unsigned char *)0;
void _dump (void)
{
  unsigned int len;
  unsigned char i,j,k;
  unsigned char ch[17];
  char *p_src, *p_len;

  p_src=arg_next();
  p_len=arg_next();
  if (p_src) _dump_ptr=(unsigned char *)ahtoi(p_src);
  if (p_len) len=_atoi(p_len);
         else len=256;

  while (len)
  {
    __printf1("%p:\t",_dump_ptr);
    j=(len>16?16:(unsigned char)len);
    len-=j;
    for (i=0;i<j;i++)
	{
	  k=*_dump_ptr++;
	  __printf1("%02bx ",k);
	  if (k>=0x20 && k<=0x80) ch[i]=k;
	                     else ch[i]='.';
	}
	ch[i]=0;
    if (i<16)
	{
	  for (j=0;j<16-i;j++) __printf0("   ");
	}
    __printf1("  %s\r\n",ch);
  }
}


static void _mr (void)
{
  unsigned char  *src;
  unsigned char l=0;
  char *p_src,*p_len;

  p_src=arg_next();
  p_len=arg_next();
  if (p_src) src=(unsigned char *)ahtoi(p_src);
        else return;
  if (p_len) l=_atoi(p_len);
  __printf1("%p: ",src);
  if (l==1)
  {
    __printf1("%02x\r\n",*(unsigned char *)src);
  }
  else if (l==2)
  {
    __printf1("%04x\r\n",*(unsigned int *)src);
  }
  else if (l==4)
  {
    __printf2("%04x%04x\r\n",*(unsigned int *)src,*(unsigned int *)(src+2));
  }
}

static void _mw (void)
{
  unsigned char  *src;
  unsigned char l=0;
  unsigned long v=0;
  char *p_src,*p_len,*p_val;

  p_src=arg_next();
  p_len=arg_next();
  p_val=arg_next();
  if (p_src) src=(unsigned char *)ahtoi(p_src);
        else return;
  if (p_len) l=_atoi(p_len);
  if (p_val) v=ahtoi(p_val);

  if (l==1)
  {
    *(unsigned char *)src=v;
  }
  else if (l==2)
  {
    *(unsigned short *)src=v;
  }
  else if (l==4)
  {
    *(unsigned long *)src=v;
  }
}


static void _reg (void)
{
  unsigned char  *_reg_ptr;
  unsigned char *ptr,val;

  ptr=arg_next();
  if (ptr)
  {
    _reg_ptr=(unsigned char *)ahtoi(ptr);
    ptr=arg_next();
    if (ptr)
    {
      val=ahtoi(ptr);
      *_reg_ptr=val;
    }
    val=*_reg_ptr;
    __printf2("Reg %x=%x\r\n",(unsigned char)_reg_ptr,val);
  }
}
#endif




#if (F_WRITING && F_TESTSUITE)
#include "../fat/src/mst/mst.h"
#include "../fat/src/test/test_f.h"
int _f_poweron(void)
{
	int rc;

	//rc=f_enterFS();
	//if (rc) return rc;

    rc=f_initvolume(0,mst_initfunc,0);
	if (rc) return rc;

	return f_chdrive(0);
}

int _f_poweroff(void)
{
	return f_delvolume(0);
}


void _f_dump(char *s)
{
	__printf1("%s\r\n",s);
}

int _f_result(int line,long result)
{
	if (line!=-1)
	{
		__printf2("ERROR: test.c at line %d result %d\r\n",line,result);
	}
	else
	{
		__printf1("ERROR: found %d error(s)\r\n",result);
	}
	return 1;
}

static void _test2 (void)
{
  _f_poweroff();
  f_dotest();
}
#endif

#if (F_WRITING && F_WRITE && F_DELETE)
static void _test (void)
{
  unsigned long i;
  unsigned char ch;
  F_FILE *f;
  #define BUF_N 2048

  ch='a';
  memset(buf,ch,2048);
  f_delete("test.bin");
  #ifdef Debug_UART
  __printf0("Write\r\r\n");
  #endif
  f=f_open("test.bin","w");
  if (f)
  {
    for (i=0;i<BUF_N;i++)
	{
	  if (f_write(buf,1,2048,f)!=2048) break;
	}
	f_close(f);
	if (i==BUF_N)
	{
	  #ifdef Debug_UART
	  __printf0("Read\r\r\n");
	  #endif
	  f=f_open("test.bin","r");
	  if (f)
	  {
	    for (i=0;i<BUF_N;i++)
		{
		  if (f_read(buf,1,2048,f)!=2048) break;
		}
		f_close(f);
		if (i<BUF_N) { __printf0("Read error\r\r\n"); }
                else { __printf0("Done.\r\r\n"); }
	  }
	}
	else __printf0("Write error\r\r\n");
  }
  else __printf0("Wopen error\r\r\n");
}

#endif











static void _help (void)
{
  t_SOS_ins *act_ins;

  __printf0("\r\nHelp:\r\n");
  act_ins=SOS_ins;
  while (act_ins->cmd)
  {
    __printf2("\t%s\t%s\r\n",act_ins->cmd,act_ins->help);
	++act_ins;
  }
  __printf0("\r\n");
}

//..........................................

//Prog Command

//......................................
static void _prog (void) 
{
	F_FILE *f;
	F_FILE *f2;
  	long ret;

 
 	f=f_open(FileName,"r");
	f2=f_open(WriteFileName,"w");

	if ((f)&&(f2))
  	{
  		/* Unlock the Flash Program Erase controller */
  		FLASH_Unlock();

  		/* Define the number of page to be erased */
  		NbrOfPage=(EndAddr - StartAddr) / FLASH_PAGE_SIZE;

  		/* Clear All pending flags */
  		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  		/* Erase the FLASH pages */
  		for(EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter++)
  		{
			FLASH_ErasePage(StartAddr + (FLASH_PAGE_SIZE * EraseCounter));
  		}
  
   		/*  FLASH  StartAddr */
   		Address = StartAddr;

      		while (ret=f_read(buf,1,1024,f))
     		 {
			BufferPtr=buf;

			f_write(buf,1,ret,f2);

		  	while (ret--)
  			{
			Temp=((*BufferPtr++));
                	ret--;
			ProgramData=(Temp |( (*BufferPtr++)<<8));
    	 		FLASH_ProgramHalfWord(Address, ProgramData);
    			Address = Address + 2;
  			}	
			
        	}     

	/* Lock the Flash Program Erase controller */
  	FLASH_Lock();
	#ifdef Debug_UART
	__printf0("Program OK\r\n"); 
	#endif
    	f_close(f);
	f_close(f2);
	}
 
  else
  {
  #ifdef Debug_UART
    __printf0("Open failed.\r\n");
  #else
	__asm("nop");

  #endif
  }

}









static void _runap (void)
{
 	JumpAddress = *(vu32*) (0x08018000 + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __MSR_MSP(*(vu32*) 0x08018000);
      Jump_To_Application();


}



void cmd_exec (char *ins)
{
  t_SOS_ins *tmp_ins;

  if (*ins)
  {
    tmp_ins=SOS_ins;
    while (tmp_ins->cmd && strcmp(ins,tmp_ins->cmd)) ++tmp_ins;
    if (tmp_ins->cmd)
    {
      (tmp_ins->fn)();
    }
    else __printf0("Command not found.\r\n");
  }
}


