//
///////////////////////////////////////////////////////////////////////////////
//
//             PROGRAMME GESTION COM
//
//         FAIT PAR E.LAUGT LE 18/08/2017
//
//
//////////////////////////////////////////////////////////////////////////////
//
#include "xc.h"
#include "Headers_application.h"
//
////////////////////////////////////////////////////////////////////////
//
//                 DECLARATION DE TYPE DOUBLE CHAR
//
///////////////////////////////////////////////////////////////////////
//
dbchar type2;
#define cpcalcul1 type2.integer    //int stockage 
#define cpcalh1  type2.tchar.hbyt  //octet haut de cpcalcul
#define cpcall1  type2.tchar.lbyt  //octet bas de cpcalcul
//
//
////////////////////////////////////////////////////////////////////////
//
//                 DECLARATION DE TYPE quadruple CHAR
//
///////////////////////////////////////////////////////////////////////
//
qchar long_bits1;
#define cplong1  long_bits1.trlong    //long stockage
#define cplonghhh1  long_bits1.tlong.hhhbyt  //octet haut de cpcalcul
#define cplonghh1  long_bits1.tlong.hhbyt  //octet haut de cpcalcul
#define cplongh1  long_bits1.tlong.hbyt  //octet haut de cpcalcul
#define cplongl1  long_bits1.tlong.lbyt  //octet bas de cpcalcul
//
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE RAM GLOBALE
/////////////////////////////////////////////////////////////////////////////
//
unsigned char buffer1tx[128];   // Buffer emission UART 1 
unsigned char buffer1rx[128];   //Buffer pour réception des donnees UART 1
unsigned short tp_com1=100;     //surveillance uart1
unsigned char recu1=0;         //octet reception UART1
unsigned short index1rx=0;      //index reception UART1
unsigned char commande_com=0;  //commande recue sur l'UART 1 
unsigned short temps_data=1000;    //Tempo dataloggeur en 10ms pour décrémentation dans it timer
unsigned short tp_data=0;       //Tempo data logger
unsigned short index_1=0;       //sauvegarde index pour indication de fin de trame
//
//
/////////////////////////////////////////////////////////////////////////////
//				Reception UART USB/SERIE
/////////////////////////////////////////////////////////////////////////////
//
void __attribute__((__interrupt__,no_auto_psv)) _U1RXInterrupt(void)
{
    recu1=U1RXREG;          //Lit octet recu
    if(index1rx==0)    //test début de trame
        {
        if(recu1!='<')
        {
            index1rx=0;
			goto uart1_suite;  
        }
        buffer1rx[index1rx]=recu1;
        index1rx++;
        tp_com1=100; 
        goto uart1_suite; 
        }
    if(recu1=='>')
        {
        buffer1rx[index1rx]=recu1;
        traitement_trame_carte();        //saut fonction traitement trame usb
        index1rx=0;
        goto uart1_suite; 
        }
    if((index1rx>=127)||(tp_com1==0))     //test dépassement sans fin de trame
        {
        index1rx=0;        //Erreur Raz buffer et index
        goto uart1_suite;
        }
    else
        {
        buffer1rx[(index1rx)]=recu1; // sinon enregistre les caractères recus
        index1rx++; 
        goto uart1_suite; 
        }
    uart1_suite:;           //vide le buffer de reception
    recu1=U1RXREG;          //Lit octet recu
    recu1=U1RXREG;          //Lit octet recu
    recu1=U1RXREG;          //Lit octet recu
    IFS0bits.U1RXIF=0;      //
}
//
/////////////////////////////////////////////////////////////////////////////
//				Emission UART PC USB
/////////////////////////////////////////////////////////////////////////////
//
void __attribute__((__interrupt__,no_auto_psv)) _DMA3Interrupt(void)
{
    IFS2bits.DMA3IF = 0; //Clear the DMA3 Interrupt Flag
    IEC2bits.DMA3IE = 0; // Set the DMA Interrupt Enable bit
    DMA3CONbits.CHEN = 0; // Enable DMA
}
//
//
/////////////////////////////////////////////////////////////////////////////
//				Fonction d'initialisation UARTS
/////////////////////////////////////////////////////////////////////////////
//
void init_com (void)
{
unsigned long a;
tp_com1=100;
a=FCY/16;
U1BRG=(unsigned int) ((a/bds_uart1)-1);
U1MODE=0xA800;
U1STA=0x2500;
IPC2bits.U1RXIP=4;
IFS0bits.U1RXIF=0;
IFS0bits.U1TXIF=0;
IEC0bits.U1RXIE=1;
IPC9bits.DMA3IP=5;
DMA3CONbits.AMODE = 0; // Mode d'adressage 0= avec incrément 1 = sans incrément
//DMA3CONbits.MODE = 1; // Mode de transfert 0 = continu 1= une seule fois
DMA3PAD = (volatile unsigned int)&U1TXREG;  // Adresse du registre du periphérique
DMA3CNT = 4;// Nombre de transfert - 1 (63 = 64 transferts)
DMA3REQ = 12; // Source de déclanchement (voir tableau dans doc)
DMA3STAL = __builtin_dmaoffset(buffer1tx);//Adresse du buffer ram
DMA3STAH = 0x0000;
DMA3CONbits.DIR = 1; //Direction transfert 0= periph vers ram 1= Ram vers périph
DMA3CONbits.SIZE = 1;  //Taille de transfert 0=Word 1=Byte
IFS2bits.DMA3IF = 0; // Clear the DMA Interrupt Flag bit
IEC2bits.DMA3IE = 0; // Clear the DMA Interrupt Enable bit
DMA3CONbits.CHEN = 0; // Enable DMA
}
//
/////////////////////////////////////////////////////////////////////////////
//				Fonction traitement trame carte
/////////////////////////////////////////////////////////////////////////////
//
void traitement_trame_carte(void)
{
    commande_com=buffer1rx[2];          //Récupère l'octet commande dans la trame
    if(commande_com!=0x00)
        {
        gestion_commandes();
        }
}
//
/////////////////////////////////////////////////////////////////////
//
//                      Gestion des commandes
//
/////////////////////////////////////////////////////////////////////
//
void gestion_commandes(void)       //Fonction de gestion des commandes recues
{
    switch(commande_com)
        {
        case 1:                // Commande 1  Réponse pong
            {
//            cpcall1=buffer1rx[3];
//            cpcalh1=buffer1rx[4];
//            temps_data=(cpcalcul1/100);
//            commande_com=0;
            break;
            }
        default:
            {
            commande_com=0;
            break;
            }
        }
}
//
//
/////////////////////////////////////////////////////////////////////
//
//                      Gestion dataloggeur
//
/////////////////////////////////////////////////////////////////////
//
void gestion_com(void)       //Fonction de gestion communication
{
//unsigned char i;
if((tp_data==0)&&(temps_data!=0))               //Attente tempo dataloggeur 
    {
    tp_data=temps_data;
    //
    // Données à envoyer
    //
    IFS0bits.U1TXIF=0;
    DMA3CNT = 25;// Nombre de transfert - 1 (63 = 64 transferts)
    IFS2bits.DMA3IF = 0; // Clear the DMA Interrupt Flag bit
    IEC2bits.DMA3IE = 1; // Set the DMA Interrupt Enable bit
    DMA3CONbits.CHEN = 1; // Enable DMA
    DMA3REQbits.FORCE=1;    
    }
}
//
