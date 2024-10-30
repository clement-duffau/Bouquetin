//
//
///////////////////////////////////////////////////////////////////////////////
//
//                PROGRAMME MODELE CHARGEUR SOLAIRE
//                      BASE    PIC24EP256GP202
//                          LE 19/09/2024
//                                       
//////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////
//				INCLUSION DE FICHIERS
/////////////////////////////////////////////////////////////////////////////
//
#include <xc.h>
#include "Bits_config.h"
#include "Headers_application.h"
//
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE RAM
/////////////////////////////////////////////////////////////////////////////
//
unsigned short entt1,entt2;
unsigned char compteur10ms=0;		//Compteur 10ms
unsigned char tache=0;              //tache programme principal
//
unsigned short tp_cli=500;
//
///////////////////////////////////////////////////////////////////////
//
//                  Definition des entrees
//
///////////////////////////////////////////////////////////////////////
//
flag val_entrees;               
#define entrees      val_entrees.mot   			// D�finition du mot entrees
#define bp_blocage   val_entrees.bita.b0        //Bouton blocage porte ouverte
#define ctx_ir       val_entrees.bita.b1        //Entr�e 1 image
#define dt_pres      val_entrees.bita.b2        //D�tection de pr�sence animal
#define bp_forcage   val_entrees.bita.b3        //Bouton de for�age fermeture
#define dt_appat     val_entrees.bita.b6        //Bouton de for�age fermeture
#define in1_img      val_entrees.bita.b8        //Entr�e 1 image
#define in2_img      val_entrees.bita.b11        //Entr�e 2 image

//
//
///////////////////////////////////////////////////////////////////////
//
//                  Definition des fronts montants des entrees
//
///////////////////////////////////////////////////////////////////////
//
flag val_fronts_entrees;               
#define fronts_entrees      val_fronts_entrees.mot   			// D�finition du mot entrees
#define fbp_blocage   val_fronts_entrees.bita.b0        //Bouton blocage porte ouverte
#define fctx_ir       val_fronts_entrees.bita.b1        //Entr�e 1 image
#define fdt_pres      val_fronts_entrees.bita.b2        //D�tection de pr�sence animal
#define fbp_forcage   val_fronts_entrees.bita.b3        //Bouton de for�age fermeture
#define fdt_appat     val_fronts_entrees.bita.b6        //Bouton de for�age fermeture
#define fin1_img      val_fronts_entrees.bita.b8        //Entr�e 1 image
#define fin2_img      val_fronts_entrees.bita.b11        //Entr�e 2 image
//
//
///////////////////////////////////////////////////////////////////////
//
//                  Definition flag application
//
///////////////////////////////////////////////////////////////////////
//
flag flag_appli_bits;               
#define fl_appli      flag_appli_bits.mot   			// D�finition du mot entrees
#define regul         flag_appli_bits.bita.b0           //D�finition du bouton poussoir
#define fl_cli        flag_appli_bits.bita.b1           //Flag clignotant 0,5/0,5s
//
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE RAM GLOBALE
/////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////
//				ZONE ECRITURE DES INTERRUPTIONS
/////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////
//				         INTERRUPTION TIMER  1ms
/////////////////////////////////////////////////////////////////////////////
//
void __attribute__((interrupt,no_auto_psv))_T1Interrupt(void)
{
IFS0bits.T1IF=0;                      // raz flag
//
//  Filtres analogiques
//
//
tp_1ms_eep();
//
//
if(tp_com1!=0)
    {
    tp_com1--;
    }
//
if(tp_cli!=0)
    {
    tp_cli--;
    }
else
    {
    tp_cli=500;
    fl_cli=~fl_cli;
    }
//
}//Fin IT timer
//
//
/////////////////////////////////////////////////////////////////////////////
//              Fonction traitement des entr�es/sorties
/////////////////////////////////////////////////////////////////////////////
//
void lecture_entrees (void)      //LECTURE DES ENTREES + DETECTION FRONTS
{
entt1=((PORTA&0x0013)<<2);
entt1|=(PORTB&0x0903);
if(entt1==entt2)
    {
    fronts_entrees=~entrees&entt1;      //detection des fronts
    entrees=entt1;
    }
else
    {
    entt2=entt1;
    }
}
//
//
/////////////////////////////////////////////////////////////////////////////
//				ZONE PROGRAMME PRINCIPAL
/////////////////////////////////////////////////////////////////////////////
//
int main(void) {
//
LATB=0;               // RAZ des ports E/S
PORTB=0;
LATA=0;
PORTA=0;    
//
//
OSCCON=0x46;          //Proc�dure de d�blocage d'�criture dans les registres.
OSCCON=0x57;		  //
OSCCONbits.IOLOCK=0;  //
//
// affectation des broches affectables
//
RPINR18bits.U1RXR=37;    //uart1 RX
RPOR1bits.RP36R=1;       //UART1 TX
RPINR19bits.U2RXR=39;    //uart2 RX
RPOR2bits.RP38R=2;       //UART2 TX

RPOR0bits.RP20R = 16;    //PWM1 16
//
OSCCON=0x46;           //Proc�dure de blocage d'�criture dans les registres
OSCCON=0x57;		   //
OSCCONbits.IOLOCK=1;   //
//
//
// Configuration PLL pour 40MIPS � partir de 10Mhz
//
PLLFBD = 54; // M = 33
CLKDIVbits.PLLPOST = 0; // N2 = 2
CLKDIVbits.PLLPRE = 0; // N1 = 2
//
// switch oscillateur FRC vers PLL
//
__builtin_write_OSCCONH(0x03);
__builtin_write_OSCCONL(0x01);
//
// Attente switch clock OK
//
while (OSCCONbits.COSC!= 0b011);
//
// Attente PLL OK
//
while(OSCCONbits.LOCK!= 1) {};
//
//
TRISB=0x09AF;           // initialisation du port B
TRISA=0xF87F;           // initialisation du port A
//
LATA=0;
PORTA=0;     //RAZ des entr�es des ports
LATB=0;      // RAZ des sorties des ports
PORTB=0;

//
//      Configuration du timer principal IT � 1ms
//
PR1=8750;       //P�riode timer 1
T1CON=0xA010;    //Config timer 1 TCY/8
//
//			 Configuration des p�riph�riques
//
//  Initialisation du convertisseur analogique/num�rique
//
ANSELA=0x0000;
ANSELB=0x0000;
//
//
//  Raz g�n�rale des flags d'interruption
//
IFS0=0;          //Raz flags interruption
IFS1=0;          //Raz flags interruption
IFS2=0;          //Raz flags interruption
IFS3=0;          //Raz flags interruption
IFS4=0;          //Raz flags interruption
//
//
//           Initialisation de variables
//
//
//   Configuration et autorisation des int�rruptions 
//
//
OC1CON1=0;
OC1CON2=0;
OC1CON1bits.OCTSEL = 0x07;
OC1CON1bits.OCM = 6;
OC1CON2bits.SYNCSEL = 0x1F;
OC1RS=3500;    //frequence 20kHz (1/3500)*70e6
OC1R=0;     //rapport cyclique en %   ((0/3500)*100)
//
AD1CON1bits.ADON=1;
//
init_com ();       //Initialisation UART
//
restaure ();
//
// It timer  on priorit� 1
//
IPC0bits.T1IP=1;	 //Priorit� des interruptions T1 P1
//
IEC0bits.T1IE=1;  // It timer 1 on
//
//
//////////////////////////////////////////////////////////////////////////////////////
//
//                          DEBUT DE BOUCLE PROGRAMME
//
//////////////////////////////////////////////////////////////////////////////////////
//
while(1)
{
//
    lecture_entrees();    //Fonction de lecture des entr�es
    gestion_com();    //Fonction de gestion communication
    gestion_eep();    //Fonctions de gestion de l'EEPROM
//
}//      FIN DE BOUCLE
//
//     FIN FONCTION PRINCIPALE
}///////// NE PAS ENLEVER///////////////////////
//
