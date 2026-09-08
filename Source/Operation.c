#include "Operation.h"

/*------------------------------------------------------------------------------
//-- ¥[´î¹Bºâ
------------------------------------------------------------------------------*/
unsigned long Operation_AddDec(unsigned long CurrentData, unsigned long MinData, 
                                unsigned long MaxData, unsigned short SizeAdjusted, unsigned char NeedCyle)
{
    unsigned short by_Adjust = (SizeAdjusted&0x7FFF);
    
    if(by_Adjust == 0)
        return CurrentData;
    
    if((SizeAdjusted & _Operation_Dec) != 0)
    {// - 
        if(CurrentData < (MinData + by_Adjust))
            CurrentData = CurrentData == MinData ? (NeedCyle == _NonNeedCycle ? MinData : MaxData) : MinData;
        else
            CurrentData -= by_Adjust;
    }
    else
    {// +
        if((CurrentData + by_Adjust) > MaxData)
            CurrentData = CurrentData == MaxData ? (NeedCyle == _NonNeedCycle ? MaxData : MinData) : MaxData;
        else
            CurrentData += by_Adjust;
    }
    return CurrentData;
}
/* -----------------------------------------------------------------------------
 * Move the string
 * ---------------------------------------------------------------------------*/
void Operation_MoveString(unsigned char *SourceString, unsigned char *TargetString)
{
    unsigned char i = 0;
    
    while(*(SourceString+i) != 0)
    {
        *(TargetString + i) = *(SourceString + i);
        i++;
    }
    *(TargetString + i) = 0;
}
/* -----------------------------------------------------------------------------
 * Fisrt Input First Output
 * ---------------------------------------------------------------------------*/
void Operation_FIFO_8( unsigned char *ptr, unsigned char NewData , unsigned char Length ) 
{
    unsigned char i ;
    //----------------------------------------------------------------------------
    for( i = (Length-1)  ; i > 0 ; i-- )
        ptr[i] = ptr[i-1] ;
    ptr[0] = NewData ;	
    //----------------------------------------------------------------------------
    return  ;
}
void Operation_FIFO_16( unsigned short *ptr, unsigned short NewData , unsigned char Length ) 
{
    unsigned char i ;
    //----------------------------------------------------------------------------
    for( i = (Length-1)  ; i > 0 ; i-- )
        ptr[i] = ptr[i-1] ;
    ptr[0] = NewData ;	
    //----------------------------------------------------------------------------
    return  ;
}
void Operation_FIFO_32( unsigned long *ptr, unsigned long NewData , unsigned char Length ) 
{
    unsigned char i ;
    //----------------------------------------------------------------------------
    for( i = (Length-1)  ; i > 0 ; i-- )
        ptr[i] = ptr[i-1] ;
    ptr[0] = NewData ;	
    //----------------------------------------------------------------------------
    return  ;
}
//#############################################################################
//--- Average filter
//#############################################################################
unsigned short Operation_Average_16( unsigned short* iSource, unsigned char Length ) 
{
    unsigned long iAverage = 0;
    unsigned char i = 0;
    //----------------------------------------------------------------------------
    for( i = 0 ; i < Length ; i++ )
    {
        iAverage = iAverage + (unsigned long)iSource[i];
    }
    //----------------------------------------------------------------------------
    return (Length == 0 ? 0 : ((unsigned short)(iAverage/Length)));
}
unsigned short Operation_Average_32( unsigned long* iSource, unsigned char Length ) 
{
    unsigned long long iAverage = 0;
    unsigned char i = 0;
    //----------------------------------------------------------------------------
    for( i = 0 ; i < Length ; i++ )
    {
        iAverage += (unsigned long long)iSource[i];
    }
    //----------------------------------------------------------------------------
    return (Length == 0 ? 0 : ((unsigned long)(iAverage/Length)));
}

//#############################################################################
//--- Add the number in back of strings
//#############################################################################
void Operation_StringsAddNumber(unsigned char *SourceString, unsigned short Number)
{
    unsigned char i = 0, Digits = 1;
    unsigned short by_Dat = 0;
    
    by_Dat = Number;
    while((by_Dat/10) != 0)
    {
        by_Dat /= 10;
        Digits++;
    }
    
    while(*(SourceString+i) != 0)
        i++;
    *(SourceString + i) = ' ';
    
    for(int j=Digits; j>0; j--)
    {
        *(SourceString + i + j) = (Number%10)+'0';
        Number /= 10;
    }
    *(SourceString + i + Digits + 1) = 0;
}

/* -----------------------------------------------------------------------------
 * Band pass filter
 * ---------------------------------------------------------------------------*/
#define _Max_DeletedNum  10
unsigned short Operation_MultiFilter_16(unsigned short* iSource, unsigned char Length, unsigned char DeletedLength_MinVal, unsigned char DeletedLength_MaxVal)
{
    unsigned long long by_Sum = 0;
    unsigned short by_Check = 0, by_Buff;
    unsigned char DeleteLength, by_Check2;
    struct
    {
        unsigned short MaxVal[_Max_DeletedNum];
        unsigned short MinVal[_Max_DeletedNum];
    }Delete = {0};
    
    if((DeletedLength_MinVal + DeletedLength_MaxVal) >= Length)
        return 0;
    if(DeletedLength_MinVal > _Max_DeletedNum)
        DeletedLength_MinVal = _Max_DeletedNum;
    if(DeletedLength_MaxVal > _Max_DeletedNum)
        DeletedLength_MaxVal = _Max_DeletedNum;
    
    for(int i=0; i<Length; i++)
    {//
        by_Check = iSource[i];
        by_Sum += by_Check;
        
        if(DeletedLength_MinVal == 0 && DeletedLength_MaxVal == 0)
        {// Average filter
            
        }
        else if(DeletedLength_MinVal != 0 && DeletedLength_MaxVal != 0)
        {// Band-pass filter
             if(i < DeletedLength_MinVal)
                Delete.MinVal[i] = by_Check;
            else if(i < (DeletedLength_MinVal + DeletedLength_MaxVal))
            {
                for(int j=0; j< DeletedLength_MinVal; j++)
                {
                    if(by_Check < Delete.MinVal[j])
                    {
                        by_Buff = Delete.MinVal[j];
                        Delete.MinVal[j] = by_Check;
                        by_Check = by_Buff;
                    }
                }
                Delete.MaxVal[i - DeletedLength_MinVal] = by_Check;
            }
            else
            {
                for(int j=0; j<DeletedLength_MinVal; j++)
                {
                    if(by_Check < Delete.MinVal[j])
                    {// search min value
                        by_Buff = Delete.MinVal[j];
                        Delete.MinVal[j] = by_Check;
                        by_Check = by_Buff;
                    }
                }
                
                for(int j=0; j<DeletedLength_MaxVal; j++)
                {
                    if(by_Check > Delete.MaxVal[j])
                    {// search max value
                        by_Buff = Delete.MaxVal[j];
                        Delete.MaxVal[j] = by_Check;
                        by_Check = by_Buff;
                    }
                }
            }
        }
        else
        {// Low-pass filter, High-pass filter
            DeleteLength = DeletedLength_MaxVal != 0 ? DeletedLength_MaxVal : DeletedLength_MinVal;
            
            if(i < DeleteLength)
                Delete.MaxVal[i] = by_Check;
            else
            {
                for(int j=0; j<DeleteLength; j++)
                {
                    by_Check2 = 0;
                    if(DeletedLength_MaxVal == 0)
                    {// High-pass filter
                         if(by_Check < Delete.MaxVal[j])
                            by_Check2 = 1;
                    }
                    else
                    {// Low-pass filter
                        if(by_Check > Delete.MaxVal[j])
                            by_Check2 = 1;
                    }
                    
                    if(by_Check2 == 1)
                    {
                        by_Buff = Delete.MaxVal[j];
                        Delete.MaxVal[j] = by_Check;
                        by_Check = by_Buff;
                    }
                }
            }
        }
    }
    
    for(int i=0; i<DeletedLength_MinVal; i++)
        by_Sum -= Delete.MinVal[i];
    for(int i=0; i<DeletedLength_MaxVal; i++)
        by_Sum -= Delete.MaxVal[i];
    
    return (unsigned short)(by_Sum/(Length - DeletedLength_MinVal - DeletedLength_MaxVal));
}
unsigned long Operation_MultiFilter_32(unsigned long* iSource, unsigned char Length, unsigned char DeletedLength_MinVal, unsigned char DeletedLength_MaxVal)
{
    unsigned long long by_Sum = 0;
    unsigned long by_Check = 0, by_Buff;
    unsigned char DeleteLength, by_Check2;
    struct
    {
        unsigned long MaxVal[_Max_DeletedNum];
        unsigned long MinVal[_Max_DeletedNum];
    }Delete = {0};
    
    if((DeletedLength_MinVal + DeletedLength_MaxVal) >= Length)
        return 0;
    if(DeletedLength_MinVal > _Max_DeletedNum)
        DeletedLength_MinVal = _Max_DeletedNum;
    if(DeletedLength_MaxVal > _Max_DeletedNum)
        DeletedLength_MaxVal = _Max_DeletedNum;
    
    for(int i=0; i<Length; i++)
    {//
        by_Check = iSource[i];
        by_Sum += by_Check;
        
        if(DeletedLength_MinVal == 0 && DeletedLength_MaxVal == 0)
        {// Average filter
            
        }
        else if(DeletedLength_MinVal != 0 && DeletedLength_MaxVal != 0)
        {// Band-pass filter
             if(i < DeletedLength_MinVal)
                Delete.MinVal[i] = by_Check;
            else if(i < (DeletedLength_MinVal + DeletedLength_MaxVal))
            {
                for(int j=0; j< DeletedLength_MinVal; j++)
                {
                    if(by_Check < Delete.MinVal[j])
                    {
                        by_Buff = Delete.MinVal[j];
                        Delete.MinVal[j] = by_Check;
                        by_Check = by_Buff;
                    }
                }
                Delete.MaxVal[i - DeletedLength_MinVal] = by_Check;
            }
            else
            {
                for(int j=0; j<DeletedLength_MinVal; j++)
                {
                    if(by_Check < Delete.MinVal[j])
                    {// search min value
                        by_Buff = Delete.MinVal[j];
                        Delete.MinVal[j] = by_Check;
                        by_Check = by_Buff;
                    }
                }
                
                for(int j=0; j<DeletedLength_MaxVal; j++)
                {
                    if(by_Check > Delete.MaxVal[j])
                    {// search max value
                        by_Buff = Delete.MaxVal[j];
                        Delete.MaxVal[j] = by_Check;
                        by_Check = by_Buff;
                    }
                }
            }
        }
        else
        {// Low-pass filter, High-pass filter
            DeleteLength = DeletedLength_MaxVal != 0 ? DeletedLength_MaxVal : DeletedLength_MinVal;
            
            if(i < DeleteLength)
                Delete.MaxVal[i] = by_Check;
            else
            {
                for(int j=0; j<DeleteLength; j++)
                {
                    by_Check2 = 0;
                    if(DeletedLength_MaxVal == 0)
                    {// High-pass filter
                         if(by_Check < Delete.MaxVal[j])
                            by_Check2 = 1;
                    }
                    else
                    {// Low-pass filter
                        if(by_Check > Delete.MaxVal[j])
                            by_Check2 = 1;
                    }
                    
                    if(by_Check2 == 1)
                    {
                        by_Buff = Delete.MaxVal[j];
                        Delete.MaxVal[j] = by_Check;
                        by_Check = by_Buff;
                    }
                }
            }
        }
    }
    
    for(int i=0; i<DeletedLength_MinVal; i++)
        by_Sum -= Delete.MinVal[i];
    for(int i=0; i<DeletedLength_MaxVal; i++)
        by_Sum -= Delete.MaxVal[i];
    
    return (unsigned long)(by_Sum/(Length - DeletedLength_MinVal - DeletedLength_MaxVal));
}
/*unsigned short Operation_BandPassFilter_16(unsigned short* iSource, unsigned char Length, unsigned char DeletedLength)
{
    unsigned long long by_Sum = 0;
    unsigned short by_Check = 0, by_Buff;
    struct
    {
        unsigned short MaxVal[_Max_DeletedNum];
        unsigned short MinVal[_Max_DeletedNum];
    }Delete = {0};
    
    if(((unsigned short)DeletedLength*2) >= (unsigned short)Length)
        DeletedLength = (Length<<1) - 1;
    if(DeletedLength > _Max_DeletedNum)
        DeletedLength = _Max_DeletedNum;
    
    for(int i=0; i<Length; i++)
    {//
        by_Check = iSource[i];
        by_Sum += by_Check;
        
        if(i < DeletedLength)
            Delete.MinVal[i] = by_Check;
        else if(i < (DeletedLength + DeletedLength))
        {
            for(int j=0; j< DeletedLength; j++)
            {
                if(by_Check < Delete.MinVal[j])
                {
                    by_Buff = Delete.MinVal[j];
                    Delete.MinVal[j] = by_Check;
                    by_Check = by_Buff;
                }
            }
            Delete.MaxVal[i - DeletedLength] = by_Check;
        }
        else
        {
            for(int j=0; j<DeletedLength; j++)
            {
                if(by_Check < Delete.MinVal[j])
                {// search min value
                    by_Buff = Delete.MinVal[j];
                    Delete.MinVal[j] = by_Check;
                    by_Check = by_Buff;
                }
                
                if(by_Check > Delete.MaxVal[j])
                {// search max value
                    by_Buff = Delete.MaxVal[j];
                    Delete.MaxVal[j] = by_Check;
                    by_Check = by_Buff;
                }
            }
        } 
    }
    
    for(int i=0; i<DeletedLength; i++)
    {
        by_Sum -= Delete.MinVal[i];
        by_Sum -= Delete.MaxVal[i];
    }
    
    return (unsigned short)(by_Sum/(Length - DeletedLength - DeletedLength));
}
*/
/* -----------------------------------------------------------------------------
 * Memory process function
 * ---------------------------------------------------------------------------*/
void __memset(void *s, char c, long n)
{       /* store c throughout unsigned char s[n] */
  const unsigned char uc = c;
  unsigned char *su = (unsigned char *)s;
  
  for (; 0 < n; ++su, --n)
  *su = uc;
  return ;
}



void __memcpy(void *s1, const void *s2, long n)
{       /* copy char s2[n] to s1[n] in any order */
  char *su1 = (char *)s1;
  const char *su2 = (const char *)s2;
  
  for(; 0 < n; ++su1, ++su2, --n)
      *su1 = *su2;
  return ;
}


char __memcmp(const void *s1, const void *s2, long n)
{       /* compare unsigned char s1[n], s2[n] */
  const unsigned char *su1 = (const unsigned char *)s1;
  const unsigned char *su2 = (const unsigned char *)s2;
  
  for (; 0 < n; ++su1, ++su2, --n)
    if (*su1 != *su2)
      return (*su1 < *su2 ? -1 : +1);
  return (0);
}



long __strlen(const char *s)
{       /* find length of s[] */
  const char *sc;
  
  for (sc = s; *sc != '\0'; ++sc)
    ;
  return (sc - s);
}

char *__strchr(const char *s, int c)
{       /* find first occurrence of c in char s[] */
  const char ch = c;

  for (; *s != ch; ++s)
    if (*s == '\0')
      return (0);
  return ((char *)s);
}

char *__strstr(const char *s1, const char *s2)
{       /* find first occurrence of s2[] in s1[] */
  if (*s2 == '\0')
    return ((char *)s1);
  for (; (s1 = __strchr(s1, *s2)) != 0; ++s1)
  {     /* match rest of prefix */
    const char *sc1, *sc2;

    for (sc1 = s1, sc2 = s2; ; )
      if (*++sc2 == '\0')
        return ((char *)s1);
      else if (*++sc1 != *sc2)
        break;
  }
  return (0);
}



char *__strcpy(char *s1, const char *s2)
{       /* copy char s2[] to s1[] */
  char *s = s1;
  
  for (s = s1; (*s++ = *s2++) != '\0'; )
    ;
  return (s1);
}

char *__strncpy(char *s1, const char *s2, long n)
{       /* copy char s2[max n] to s1[n] */
  char *s;
  
  for (s = s1; 0 < n && *s2 != '\0'; --n)
    *s++ = *s2++;       /* copy at most n chars from s2[] */
  for (; 0 < n; --n)
    *s++ = '\0';
  return (s1);
}

int __strcmp(const char *s1, const char *s2)
{       /* compare unsigned char s1[], s2[] */
  for (; *s1 == *s2; ++s1, ++s2)
    if (*s1 == '\0')
      return (0);
  return (*(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : +1);
}


unsigned char __CharBCD2DEC(unsigned char s1)
{
    return(((s1 / 16) * 10)+(s1 % 16)) ;
}
