//
///////////////////////////////////////////////////////////////////////////////
//
//                Entête COM
//
//         FAIT PAR E.LAUGT LE 18/08/2017
//
//
//////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////
//				INCLUSION DE FICHIERS
/////////////////////////////////////////////////////////////////////////////
//
#ifndef COM_H
#define COM_H
//
/////////////////////////////////////////////////////////////////////////////////
//
//      		Définition de constantes
//
/////////////////////////////////////////////////////////////////////////////////
//
#define FCY           		70E6        //valeur de l'horloge après config 70Mhz
//
#define bds_uart1         	115200      //Vitesse de la liaison uart1 liaison wifi PC
#define bds_uart2         	19200      //Vitesse de la liaison uart1 liaison radio PC
//
//
///////////////////////////////////////////////////////////////////////////////
//                DECLARATION DE CONSTANTES
///////////////////////////////////////////////////////////////////////////////

#define BUFFER_UART1_SIZE 512  // Taille du buffer pour UART1
#define BUFFER_UART2_SIZE 128  // Taille du buffer pour UART2
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
//
void init_com (void);               //Fonction d'initialisations UART
void gestion_com(void);             //Fonction de gestion de l'interface COM
void gestion_commandes(void);       //Fonction de gestion des commandes recues
void traitement_trame_wifi(void);  //Fonction de traitement de la trame recue sur UART
void traitement_trame_radio(void);
void gestion_commandes_wifi(void);
void gestion_commandes_radio(void);
void tp_1ms_uart(void);             //
void gestion_enregistrement(void);
//
#define cpcalcul  cpcal.integer
#define cpcalh  cpcal.tchar.hbyt
#define cpcall  cpcal.tchar.lbyt
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE DONNEES EXTERNES
/////////////////////////////////////////////////////////////////////////////
//
extern unsigned short tp_com1;     //surveillance uart1
extern unsigned short index1rx;      //index reception UART1
extern unsigned char buffer1tx[512];   // Buffer emission UART 1 
extern unsigned char buffer1rx[512];   //Buffer pour réception des donnees UART 1



extern unsigned short tp_com2;     // Surveillance UART 2
extern unsigned short index2rx;   // Index réception UART 2
extern unsigned char buffer2tx[128];   // Buffer émission UART 2
extern unsigned char buffer2rx[128];   // Buffer réception UART 2

//
#endif