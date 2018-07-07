volatile char received_string[5];
volatile unsigned char rx_start;
volatile unsigned char received;
int num;

#define FOSC 16000000
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD - 1)


void serial_txchar(char);
void serial_stringout(int);
void serial_init(unsigned short);