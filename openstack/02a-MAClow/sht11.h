#ifndef SHT11_H
#define SHT11_H
#include "sht11_port.h"



//accensione e spegnimento

void sht11_turnon(void);
void sht11_turnoff(void);

//funzioni comunicazione con il sensore

void sht11_start_transm(void);
int  sht11_write(unsigned char);
unsigned char sht11_read(int );
void sht11_reset_transm(void);
void sht11_write_reg(void);

// ricezione temperatura

unsigned int sht11_exec_com(unsigned int);
unsigned int sht11_cal_temp(void);
unsigned int sht11_cal_humi(float);
 //checksum CRC8
unsigned char rev8(unsigned char);
unsigned char crc8_check(unsigned char, unsigned char);


//funzioni di supporto

void configureClocks(void);

#endif
