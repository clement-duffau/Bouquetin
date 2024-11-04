//
//
///////////////////////////////////////////////////////////////////////////////
//
//                PROGRAMME MODELE CHARGEUR SOLAIRE
//                      BASE    PIC24EP256GP202
//                          LE 18/09/2024
//                                       
//////////////////////////////////////////////////////////////////////////////
//
#ifndef CHARGEUR_H
#define CHARGEUR_H
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE TYPE
/////////////////////////////////////////////////////////////////////////////
//
// D�finition de la structure pour l'horodatage
struct Horodatage{
    unsigned char annee;    // Ann�e
    unsigned char mois;     // Mois (1-12)
    unsigned char jour;     // Jour (1-31)
    unsigned char heure;    // Heure (0-23)
    unsigned char minutes;   // Minutes (0-59)
    unsigned char secondes;  // Secondes (0-59)
};
//
/////////////////////////////////////////////////////////////////////////////////
//
//         structure integer de 16 bits affectables
//
/////////////////////////////////////////////////////////////////////////////////
//
//
typedef union {unsigned int mot;
               struct{
                      unsigned  b0:1;    //declaration d'un mot de 16 bits
                      unsigned  b1:1;
                      unsigned  b2:1;
                      unsigned  b3:1;
	                  unsigned  b4:1;
		              unsigned  b5:1;
		              unsigned  b6:1;
	                  unsigned  b7:1;
		              unsigned  b8:1;
		              unsigned  b9:1;
                      unsigned  b10:1;
                      unsigned  b11:1;
		              unsigned  b12:1;
		              unsigned  b13:1;
		              unsigned  b14:1;
	                  unsigned  b15:1;
                    }bita;
              }flag;
//
//
/////////////////////////////////////////////////////////////////////////////////
//
//        structure d'un integer separe en deux char
//
/////////////////////////////////////////////////////////////////////////////////
//
typedef union {unsigned int integer;
               struct{char lbyt:8;    //separation d'un int en deux chars
					  char hbyt:8;
					 }tchar;
			  }dbchar;
//
//
/////////////////////////////////////////////////////////////////////////////////
//
//        Definition d'un long separe en 4 char
//
/////////////////////////////////////////////////////////////////////////////////
//
typedef union {unsigned long trlong;
               struct{unsigned char lbyt:8;    //separation d'un long en 4 chars
					  unsigned char hbyt:8;
					  unsigned char hhbyt:8;
					  unsigned char hhhbyt:8;
					 }tlong;
			  }qchar;
//
//
/////////////////////////////////////////////////////////////////////////////////
//
//      D�finition des ports E/S directes
//

/////////////////////////////////////////////////////////////////////////////////
//
#define det_pres_in      PORTAbits.RA0  //D�tection de pr�sence
#define bp_forcage_in    PORTAbits.RA1  //Bouton for�age
#define bp_secu_in       PORTBbits.RB0  //Bouton s�curit�/blocage
#define rx_ir_in         PORTAbits.RB1  //D�tection IR
#define det_appat_in     PORTAbits.RA4  //D�tection de prise d'app�t
#define img1_in          PORTBbits.RB8  //Entr�e option image 1
#define img2_in          PORTBbits.RB11 //Entr�e option image 2
//
//
#define rst_rad       LATAbits.LATA9      //Reset module radio
#define out_cage      LATBbits.LATB10     //Sortie commande cage
#define out1          LATCbits.LATC9     //Sortie 1
#define out2          LATCbits.LATC7     //Sortie 2
#define lr            LATCbits.LATC4     //Led rouge            
#define led_vext      LATCbits.LATC3     //Led verte ext�rieure
#define al_ir         LATBbits.LATB9    //Alimentation infra rouge
#define al_rad        LATCbits.LATC6    //Alimentation radio
#define lv            LATCbits.LATC5     //Led verte
#define img1_out      LATAbits.LATA10     //out 1 image
#define img2_out      LATCbits.LATC8     //out2 image
//
//
//
/////////////////////////////////////////////////////////////////////////////////
//
//      		D�finition de constantes
//
/////////////////////////////////////////////////////////////////////////////////
//
//#define vpan_max 24500        //Tension maximale du panneau
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
//
void lecture_entrees (void);
void gestoin_actions(void);
void verifier_mode(void);
void lire_plages_horaires(unsigned long adresse_debut);
void ecrire_plages_horaires(unsigned long adresse_debut);
<<<<<<< HEAD
int est_dans_plage (struct Horodatage * ,struct Horodatage *,struct Horodatage *);
=======
int est_dans_plage(Horodatage *actuelle, Horodatage *debut, Horodatage *fin);
>>>>>>> f17b6a2e2777668ac6933232526f304d75d14435
//
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE DONNEES EXTERNES
/////////////////////////////////////////////////////////////////////////////
//
//extern 
//
#endif