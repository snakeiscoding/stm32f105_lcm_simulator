/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOOTLCM_H
#define __BOOTLCM_H

#define	_BootLCMDataSize4

#define   _ON                   1   
#define   _OFF                  0 
#define   _HIGH                 1   
#define   _LOW                  0 




void BootLCM_Initial(void) ;
void BootLCM_WriteCommand( unsigned char cmd ,unsigned int dt ) ;
void BootLCM_WriteData( unsigned char lcd_data,unsigned int dt ) ;

#define   _AutoFillSpace            1



void BootLCMDisplay_show_one_char( unsigned char x , unsigned char y , unsigned char lcd_data) ;
void BootLCMDisplay_show_byte( unsigned char x , unsigned char y , unsigned char lcd_data ) ;
void BootLCMDisplay_show_word( unsigned char x , unsigned char y , unsigned int lcd_data ) ;
void BootLCMDisplay_show_long_bcd( unsigned char x , unsigned char y , unsigned long long_data )  ;
unsigned char BootLCMDisplay_show_word_bcd(unsigned char x ,unsigned char y ,unsigned int hex_data,unsigned char Num ) ;
void BootLCMDisplay_show_string( unsigned char x , unsigned char y , unsigned char *str_ptr ) ;
unsigned char BootLCMDisplay_show_scrollingstring( unsigned char StartX, unsigned char Y , unsigned char BarSize, unsigned char *ptr, unsigned char chCount) ;






#endif /* __BOOTLCM_H */