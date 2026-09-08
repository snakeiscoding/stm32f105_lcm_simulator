#ifndef __LCMDISPLAY_H
#define __LCMDISPLAY_H



#define   _AutoFillSpace            1



void LCMDisplay_show_one_char( unsigned char x , unsigned char y , unsigned char lcd_data) ;
void LCMDisplay_show_byte( unsigned char x , unsigned char y , unsigned char lcd_data ) ;
void LCMDisplay_show_word( unsigned char x , unsigned char y , unsigned int lcd_data ) ;
void LCMDisplay_show_long_bcd( unsigned char x , unsigned char y , unsigned long long_data )  ;
unsigned char LCMDisplay_show_word_bcd(unsigned char x ,unsigned char y ,unsigned int hex_data,unsigned char Num ) ;
void LCMDisplay_show_string( unsigned char x , unsigned char y , unsigned char *str_ptr ) ;
unsigned char LCMDisplay_show_scrollingstring( unsigned char StartX, unsigned char Y , unsigned char BarSize, unsigned char *ptr, unsigned char chCount) ;
void LCMDisplay_CreatCharacter(char Number,const unsigned char *CharPtr) ; //Add 20210420
void LCMDisplay_InitialCreatCharacter(void) ;

#endif /* __LCMDISPLAY_H */
