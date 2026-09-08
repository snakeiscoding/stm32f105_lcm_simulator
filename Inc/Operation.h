#ifndef _OPERATION_H_
#define _OPERATION_H_
//==============================================================================
//#############################################################################
// Increase
//#############################################################################
#define _NonNeedCycle 0
#define _NeedCycle 1

#define _Operation_Add  0x0000  // +
#define _Operation_Dec  0x8000  // -
unsigned long Operation_AddDec(unsigned long CurrentData, unsigned long MinData,\
                                unsigned long MaxData, unsigned short SizeAdjusted, unsigned char NeedCyle);
void Operation_MoveString(unsigned char *SourceString, unsigned char *TargetString);
void Operation_FIFO_8( unsigned char *ptr, unsigned char NewData , unsigned char Length );
void Operation_FIFO_16( unsigned short *ptr, unsigned short NewData , unsigned char Length ) ;
void Operation_FIFO_32( unsigned long *ptr, unsigned long NewData , unsigned char Length ) ;
unsigned short Operation_Average_16( unsigned short* iSource, unsigned char Length ) ;
unsigned short Operation_Average_32( unsigned long* iSource, unsigned char Length );
void Operation_StringsAddNumber(unsigned char *SourceString, unsigned short Number);
unsigned short Operation_MultiFilter_16(unsigned short* iSource, unsigned char Length, unsigned char DeletedLength_MinVal, unsigned char DeletedLength_MaxVal);
unsigned long Operation_MultiFilter_32(unsigned long* iSource, unsigned char Length, unsigned char DeletedLength_MinVal, unsigned char DeletedLength_MaxVal);
//
void __memset(void *s, char c, long n);
void __memcpy(void *s1, const void *s2, long n) ;
char __memcmp(const void *s1, const void *s2, long n) ;
long __strlen(const char *s) ;
char *__strchr(const char *s, int c) ;
char *__strstr(const char *s1, const char *s2);
char *__strcpy(char *s1, const char *s2) ;
char *__strncpy(char *s1, const char *s2, long n) ;
int __strcmp(const char *s1, const char *s2) ;
unsigned char __CharBCD2DEC(unsigned char s1) ;
//==============================================================================
#endif /* _OPERATION_H_ */