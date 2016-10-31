#define rs (1<<6) //P3.6
#define en (1<<7) //P3.7
#define dados 0xff  //P3.0 ... P3.7

void lcd_comando(int comando);
void lcd_dado(int dado);
void lcd_init(void);
void lcd_goto(int linha, int coluna);
void lcd_clr(void);
void lcd_string(char vector[]);
void lcd_correrString(char vector[]);

void espera(volatile long t);
