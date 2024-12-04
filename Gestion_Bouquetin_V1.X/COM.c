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

// uart 1 wifi 
unsigned char buffer1tx[BUFFER_UART1_SIZE];   // Buffer emission UART 1 
unsigned char buffer1rx[BUFFER_UART1_SIZE];   //Buffer pour réception des donnees UART 1
unsigned short tp_com1=100;     //surveillance uart1
unsigned char recu1=0;         //octet reception UART1
unsigned short index1rx=0;      //index reception UART1
unsigned char commande_com1=0;  //commande recue sur l'UART 1 
unsigned short temps_data=1000;    //Tempo dataloggeur en 10ms pour décrémentation dans it timer
unsigned short tp_data=0;       //Tempo data logger
unsigned short index_1=0;       //sauvegarde index pour indication de fin de trame
//
unsigned char fl_fin_trame=0;

//uart 2 radio
unsigned char buffer2tx[BUFFER_UART2_SIZE];   // Buffer emission UART 1 
unsigned char buffer2rx[BUFFER_UART2_SIZE];   //Buffer pour réception des donnees UART 1
unsigned short tp_com2=100;     //surveillance uart1
unsigned char recu2=0;         //octet reception UART1
unsigned short index2rx=0;      //index reception UART1
unsigned char commande_com2=0;  //commande recue sur l'UART 1 

unsigned char fl_fin_trame2=0;
//
/////////////////////////////////////////////////////////////////////////////
//				Timer uar-t
/////////////////////////////////////////////////////////////////////////////
//
void tp_1ms_uart(void)
{
    if(tp_com1!=0)
        {
        tp_com1--;
        }
    else
        {
        tp_com1=100; 
        index1rx=0;        //Erreur Raz buffer et index
        }
    if(tp_com2!=0)
        {
        tp_com2--;
        }
    else
        {
        tp_com2=100; 
        index2rx=0;        //Erreur Raz buffer et index
        }
}
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                U1RX      wifi
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    recu1 = U1RXREG;  // Lit octet reçu

    if (index1rx == 0)
    {
        if (recu1 != 0xAA)  // Début de trame STX
        {
            index1rx = 0;
            goto uart1_suite;   
        }
        tp_com1 = 100;
        buffer1rx[index1rx] = recu1; 
        index1rx++;
        fl_fin_trame = 0;
        goto uart1_suite;
    }

    if (index1rx >= BUFFER_UART1_SIZE || tp_com1 == 0)
    {
        index1rx = 0;  // Erreur RAZ buffer et index
        goto uart1_suite;
    }

    if (recu1 == 0x55 && fl_fin_trame == 0)  // Fin de trame
    {
        tp_com1 = 100;
        buffer1rx[index1rx] = recu1; 
        index1rx++;
        fl_fin_trame = 0xFF;
        goto uart1_suite;
    }

    if (recu1 == 0x55 && fl_fin_trame != 0)  // Fin de trame STX
    {
        tp_com1 = 100;
        buffer1rx[index1rx] = recu1; 
        index1rx++;
        fl_fin_trame = 0;
        traitement_trame_wifi();
        index1rx = 0;	
        goto uart1_suite;
    }  
    else
    {
        tp_com1 = 100;
        buffer1rx[index1rx] = recu1; 
        index1rx++;
        fl_fin_trame = 0;
        goto uart1_suite;
    }

    uart1_suite:;
    recu1=U1RXREG;  
    recu1=U1RXREG;
    recu1=U1RXREG;
    recu1=U1RXREG;
    IFS0bits.U1RXIF = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                U2RX      radio
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void)
{
    recu2 = U2RXREG;  // Lit octet reçu

    if (index2rx == 0)
    {
        if (recu2 != 0xAA)  // Début de trame STX
        {
            index2rx = 0;
            goto uart2_suite;   
        }
        tp_com2 = 100;
        buffer2rx[index2rx] = recu2; 
        index2rx++;
        fl_fin_trame2 = 0;
        goto uart2_suite;
    }

    if (index2rx >= BUFFER_UART2_SIZE || tp_com2 == 0)
    {
        index2rx = 0;  // Erreur RAZ buffer et index
        goto uart2_suite;
    }

    if (recu2 == 0x55 && fl_fin_trame2 == 0)  // Fin de trame
    {
        tp_com2 = 100;
        buffer2rx[index2rx] = recu2; 
        index2rx++;
        fl_fin_trame2 = 0xFF;
        goto uart2_suite;
    }

    if (recu2 == 0x55 && fl_fin_trame2 != 0)  // Fin de trame STX
    {
        tp_com2 = 100;
        buffer2rx[index2rx] = recu2; 
        index2rx++;
        fl_fin_trame2 = 0;
        //    Test CRC  ****************************
        traitement_trame_radio();
        index2rx = 0;	
        goto uart2_suite;
    }  
    else
    {
        tp_com2 = 100;
        buffer2rx[index2rx] = recu2; 
        index2rx++;
        fl_fin_trame2 = 0;
        goto uart2_suite;
    }

    uart2_suite:;
    recu2=U2RXREG;
    recu2=U2RXREG;
    recu2=U2RXREG;
    recu2=U2RXREG;
    IFS1bits.U2RXIF = 0;
}
//
//
/////////////////////////////////////////////////////////////////////////////
//				Emission UART PC USB
/////////////////////////////////////////////////////////////////////////////
//
void __attribute__((__interrupt__,no_auto_psv)) _DMA2Interrupt(void)
{
    IFS1bits.DMA2IF = 0; //Clear the DMA3 Interrupt Flag
    IEC1bits.DMA2IE = 0; // Set the DMA Interrupt Enable bit
    DMA2CONbits.CHEN = 0; // Enable DMA
}
//
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
//
IPC2bits.U1RXIP=4;
IFS0bits.U1RXIF=0;
IFS0bits.U1TXIF=0;
IEC0bits.U1RXIE=1;
IPC9bits.DMA3IP=5;
//
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
//
//
tp_com1=100;
a=FCY/16;
U2BRG=(unsigned int) ((a/bds_uart2)-1);
U2MODE=0xA800;
U2STA=0x2500;
//
IPC7bits.U2RXIP=4;
IFS1bits.U2RXIF=0;
IFS1bits.U2TXIF=0;
IEC1bits.U2RXIE=1;
IPC6bits.DMA2IP=5;
//
DMA2CONbits.AMODE = 0; // Mode d'adressage 0= avec incrément 1 = sans incrément
//DMA3CONbits.MODE = 1; // Mode de transfert 0 = continu 1= une seule fois
DMA2PAD = (volatile unsigned int)&U2TXREG;  // Adresse du registre du periphérique
DMA2CNT = 4;// Nombre de transfert - 1 (63 = 64 transferts)
DMA2REQ = 12; // Source de déclanchement (voir tableau dans doc)
DMA2STAL = __builtin_dmaoffset(buffer2tx);//Adresse du buffer ram
DMA2STAH = 0x0000;
DMA2CONbits.DIR = 1; //Direction transfert 0= periph vers ram 1= Ram vers périph
DMA2CONbits.SIZE = 1;  //Taille de transfert 0=Word 1=Byte
IFS1bits.DMA2IF = 0; // Clear the DMA Interrupt Flag bit
IEC1bits.DMA2IE = 0; // Clear the DMA Interrupt Enable bit
DMA2CONbits.CHEN = 0; // Enable DMA
}
//
/////////////////////////////////////////////////////////////////////////////
//				Fonction traitement trame carte
/////////////////////////////////////////////////////////////////////////////
//
void traitement_trame_wifi(void)
{
//gestion_commandes_wifi();
}
//
/////////////////////////////////////////////////////////////////////////////
//				Fonction traitement trame carte
/////////////////////////////////////////////////////////////////////////////
//
unsigned char buffer_temp_prom[taille_buffer];
unsigned short nb_temp_prom=0;
unsigned char flag_enreg=0;
unsigned short crc_com=0;
dbchar cpcal;

void traitement_trame_radio(void)
{
    unsigned short i;
    commande_com2=buffer2rx[2];          //Récupère l'octet commande dans la trame
    switch(commande_com2)
        {
        case 1:                // Commande 1  Réponse pong     45
            {
//                Mettre au format Horloge MT41T00 les infos soit Horloge[0]= (SEC/10)<<4       ==>0x4x
                //                                                vartemp =(SEC/10)             ==>0x04
                //                                                Horloge[0]|= SEC - (vartemp*10) ==> 45 - (4 *10) ==> 5
                // ......
                
                
                // buffer2rx[3] à buffer2rx[8] doit contenir les valeurs suivantes :
                // buffer2rx[3] : secondes (0-59)
                // buffer2rx[4] : minutes (0-59)
                // buffer2rx[5] : heures (0-23)
                // buffer2rx[6] : jour (1-31)
                // buffer2rx[7] : mois (1-12)
                // buffer2rx[8] : année (ex: 2023)
                
                
                horloge[0] = (buffer2rx[3] / 10) << 4 | (buffer2rx[3] % 10);
                horloge[1] = (buffer2rx[4] / 10) << 4 | (buffer2rx[4] % 10);
                horloge[2] = (buffer2rx[5] / 10) << 4 | (buffer2rx[5] % 10);
                horloge[3] = buffer2rx[6];
                horloge[4] = (buffer2rx[7] / 10) << 4 | (buffer2rx[7] % 10);
                horloge[5] = ((buffer2rx[8] / 10) << 4) | (buffer2rx[8] % 10);
                horloge[6] = ((buffer2rx[9] - 2000) / 10) << 4 | ((buffer2rx[9] - 2000) % 10); 

                // Écrire l'heure dans l'horloge
                ecrit_horloge();
            break;
            }
        case 2:                // Commande 1  Réponse pong     45
            {
            if(flag_enreg==0)
                {
                for(i=0;i<63;i++)
                {
                buffer_temp_prom[i]=buffer2rx[(i+3)];
                // stocker aussi en RAM   ****************************
                }
            nb_temp_prom=63;
            flag_enreg=1;     
                }
            break;
            }
         case 3:                // Commande 1  Réponse pong     45
            {
            buffer2tx[0]=0xAA; 
            buffer2tx[0]=0xAA; 
            buffer2tx[0]=0x03; 
            buffer2tx[0]=0xAA; //   placer les plages stockées en RAM 
           //............
            crc_com=0;
            for(i=0;i<66;i++)
                {
                crc_com+=buffer2tx[i];  
                }
            cpcalcul=crc_com;
            buffer2tx[65]=cpcalh;
            buffer2tx[66]=cpcall;
            buffer2tx[67]=0x55; 
            buffer2tx[68]=0x55;
            IFS0bits.U1TXIF=0;
            DMA3CNT = 68;// Nombre de transfert - 1 (63 = 64 transferts)
            IFS2bits.DMA3IF = 0; // Clear the DMA Interrupt Flag bit
            IEC2bits.DMA3IE = 1; // Set the DMA Interrupt Enable bit
            DMA3CONbits.CHEN = 1; // Enable DMA
            DMA3REQbits.FORCE=1;    
            }
        default:
            {
            commande_com2=0;
            break;
            }
        }
}
//
/////////////////////////////////////////////////////////////////////
//
//                      Gestion des enregistrements
//
/////////////////////////////////////////////////////////////////////
//
void gestion_enregistrement(void)
    {
    if(flag_enreg!=0)
        {
        if(etat_eep()==0)
            {
            ecrit_donnee_eep(splage, (unsigned char *) ( &buffer_temp_prom[0]), 63);
            flag_enreg=0;
            }
        }
    }
//
/////////////////////////////////////////////////////////////////////
//
//                      Gestion des commandes
//
/////////////////////////////////////////////////////////////////////
//
void gestion_commandes_wifi(void)       //Fonction de gestion des commandes recues
{
    switch(commande_com1)
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
            commande_com1=0;
            break;
            }
        }
}
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
