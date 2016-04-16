#include "sht11.h"
#include "msp430f1611.h"
#include "openserial.h"





#ifndef SENS_COM


#define DATA_OUT()      (P1DIR |= SDA)     //direzione dati in uscita        PIN 1.5=1
#define DATA_IN()       (P1DIR &= ~SDA)    //direzioni dati in entrata       PIN 1.5=0
#define DATA_HIGH()     (P1OUT |=SDA)      // output dati in uscita alto     PIN 1.5=1
#define DATA_LOW()      (P1OUT &= ~SDA)    // output dati in uscita  basso   PIN 1.5=1
#define DATA_REC()      (P1IN & SDA)       // ricezioni bit da sensore       AND DATI RICEVUTI DAL SENSORE
#define CLOCK_OUT()     (P1DIR |=SCL)      //direzione clock uscita          PIN 1.6=1
#define CLOCK_HIGH()    (P1OUT |= SCL)     // clock alto                     PIN 1.6
#define CLOCK_LOW()     (P1OUT &= ~SCL )   // clock basso                    PIN 1.6





#endif
#ifndef SHT11_CMD

#define MEAS_TEMP  0x03

#define MEAS_HUMI  0x05



#endif

/*
 *
 *
 *
 *  Dal datasheet del sensore:
 *
 *  tempi per le operazioni di lettura e di scrittura :
 *	TSCKH/L=100ns
 *	TR/F=200ns
 *	TRO=250ns
 *	TFO=200ns
 *	TV=250ns
 *	TSU=150ns
 *	TH=15ns
 *
 *
 *    durata di ogni istruzione 125ns ad una frequenza di elaborazione di 8MHz
 *
 */
typedef union
  {
	unsigned int a;
	float b;
  }last_temp;



  void sht11_turnoff(void)
  {
	  P1SEL &= ~BIT7;
	  P1OUT &=~BIT7;
	  P1DIR &=~ BIT7;


	  __nop();
  }

void sht11_turnon(void)
{

	unsigned int i;


	configureClocks();



    //accensione del sensore
    P1SEL &= ~BIT7;
    P1OUT |= BIT7;
	P1DIR |= BIT7;


	//11ms di attesa prima di entrare in sleep state
	//necessario prima dell'invio di un comando
	for( i=0; i<50; i++)
	__nop();     


}

/*
 * CONTROLLO DATA MSP430
 *
 * start transmission
 *                  _________            ___________
 *                 /         \          /           \
 *   SCK    ______/           \________/             \_________
 *
 *         ___________                         ___________________
 *   DATA             \                       /
                       \_____________________/
*/
void sht11_start_transm(void)
{

	  DATA_OUT();
	  CLOCK_OUT();

	  DATA_HIGH();
	  CLOCK_LOW();
	  __nop();
	  __nop();
	  CLOCK_HIGH();
	  __nop();
	  __nop();
	  __nop();
	  DATA_LOW();
	  __nop();
	  CLOCK_LOW();
	  __nop();
	  __nop();
	   CLOCK_HIGH();
	  __nop();
	  __nop();

	  DATA_HIGH();
	  __nop();
	  __nop();
	  CLOCK_LOW();





}
/*
 * write a correct bit, you have to send a command in this way:
 *                 ________
 *                /        \
 * SCK       ____/          \_________
 *             ________________
 *            /                \
 * DATA   ___/                  \_______
 *
 *                                valid write
 *
 */
int sht11_write(unsigned char com)
{
  unsigned char c = com;
  unsigned char i;
  unsigned char error=0;
  DATA_OUT();
  CLOCK_OUT();

  for(i = 0x80; i >0; i/=2)
  {
    if(c & i)
    {
      DATA_HIGH();
    }
    else
    {
      DATA_LOW();
    }

     CLOCK_HIGH();
     __nop();  __nop();
     __nop();
     CLOCK_LOW();
     __nop();
   }

    DATA_IN();
    __nop();

   CLOCK_HIGH();
   __nop();
   __nop();

   error = DATA_REC();
   CLOCK_LOW();
   __nop();


if(error==0)
{
   DATA_OUT();
   DATA_HIGH();
   __nop();
   //__nop();
}
   return error;


}


/*  read a valid bit:
 *
 *                 ________
 *                /        \
 * SCK       ____/          \_________
 *             _____________
 *            /
 * DATA   ___/              valid read
 *
 *
 *
 */



unsigned char sht11_read(int ack )
{
    unsigned char i,val=0;
    unsigned char error=0;

    DATA_IN();
    __nop();

	for(i=0x80; i>0; i/=2)
	{
	  CLOCK_HIGH();
	  __nop();  __nop();
	  __nop();  __nop();

	 if(DATA_REC())
	{
	  val=(val | i);
	}
	CLOCK_LOW();
	 __nop();
	}

    if(ack==1)
    {
    DATA_OUT();
    DATA_LOW();
    __nop();
	}
    CLOCK_HIGH();
    __nop();  __nop();  __nop();

    CLOCK_LOW();
    __nop();

    DATA_IN();
    __nop();
   return val;

}

void sht11_reset_transm(void)
{

	  int i;
	  DATA_OUT();
	  DATA_HIGH();
	  __nop();
	  CLOCK_OUT();
	  for(i=0;i<9;i++) //9 SCK cycles
	  {
		   CLOCK_HIGH();
		   __nop();
		   __nop();
		   __nop();
		   CLOCK_LOW();
		   __nop();

	  }
	  sht11_start_transm();

}



unsigned int sht11_cal_temp(void)
{


	int error;
	last_temp temp_val;


	const float D1=-39.6;
    const float D2=+0.01;

   //  sht11_reset_transm();
   sht11_start_transm();

    temp_val.a=sht11_exec_com(MEAS_TEMP);
    temp_val.b=(float)temp_val.a;

  	temp_val.b=(temp_val.b*D2)+D1;

    return temp_val.b;


}
unsigned int sht11_cal_humi(float temp)
{


	int error;
	last_temp humi_val;

	float humi;
	unsigned int i,j;


	const float C1=-2.0468;
    const float C2=+0.0367;
    const float C3=-0.0000015955;
    const float T1=0.01;
    const float T2=0.00008;


    sht11_start_transm();


    humi_val.a=sht11_exec_com(MEAS_HUMI);
    humi_val.b=(float)humi_val.a;
    humi=humi_val.b;


  	humi_val.b=C3*(humi_val.b*humi_val.b)+C2*humi_val.b+C1;
  	humi_val.b=(temp-25)*(T1+T2*humi)+humi_val.b;

  	if(humi_val.b>100)humi_val.b=100;
  	if(humi_val.b<0.1)humi_val.b=0.1;

    return humi_val.b;


}

unsigned int sht11_exec_com(unsigned int cmd)
{
	int val;
	    unsigned int i;

	    unsigned char t0,t1,check_sens;
	    unsigned char start=1;

	    unsigned int uni_t=0;
	    unsigned char check;

		val=sht11_write(cmd);


		DATA_IN();
		__nop();
		start=DATA_REC();

		while(start!=0)
		{
		start=DATA_REC();
		}

	    t0=sht11_read(1);
		t1=sht11_read(1);

		check=crc8_check(cmd,0x00);
		check=crc8_check(t0,check);
		check=crc8_check(t1,check);
		check=rev8(check);
		check_sens=sht11_read(0);


	    uni_t |=t0;
	    uni_t =uni_t<<8;
	    uni_t |=t1;

	  if(check_sens==check)
	      return uni_t;

     //openserial_printError(COMPONENT_SHT11,ERR_SHT11,(errorparameter_t)400,(errorparameter_t)400);
     //ritornando 3900 ottengo zero come temperatura, in caso il valore sia errato
	   return 3900;
}





 //controllo del checksum- crc8
 unsigned char crc8_check(unsigned char val, unsigned char ris)
 {
	 unsigned char  add,comp,i,temp;
	    int offset;


	    add=0x80;

	    comp=0x00;
	     offset=0;

	    for(i=0x80;i>0;i=i>>1)
	    {
	     comp=ris & add;
	     temp=val & i;
	     temp=temp<<offset;

	     comp=comp ^ temp;
	     offset=offset +1;
	     ris =ris<<1;

	     if(comp!=0)
	        ris^=0x31;
	    }
       return ris;

 }
// il risultato ottenuto da crc8 deve essere invertito prima di poterlo
 //confrontare con il checksum del sensore
unsigned char rev8(unsigned char inv)
{
	 unsigned char i;
	 unsigned char ris=0;
	 unsigned char j=0x01;

	 for (i=0x80; i>0; i=i>>1)
	  {

	    if((inv & i)!=0)
	        ris=ris | j;

	   j=j<<1;
	  }

	   return ris;
}


void configureClocks(void)
{


	 WDTCTL = WDTPW + WDTHOLD;

   //DCOCTL  |= DCO0 | DCO1 | DCO2;                 // MCLK at 8MHz
    //BCSCTL1 |= RSEL0 | RSEL1 | RSEL2;              // MCLK at 8MHz

 }







