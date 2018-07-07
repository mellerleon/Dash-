volatile unsigned char a,b;
volatile unsigned char state;
volatile int time;
volatile unsigned char flag;
volatile int speed = 0;
char MAX_speed_disp[10];
char output_1[10], output_2[10], output_3[10];

void play_note(unsigned short);
void variable_delay_us(int);
void timer1_init(void);