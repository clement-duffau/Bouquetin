//
//
///////////////////////////////////////////////////////////////////////////////
//
//                PROGRAMME MODELE GESTION BOUQUETIN
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


/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE RAM
/////////////////////////////////////////////////////////////////////////////
//

// Definition des broches en fonction du schema
#define BP_FOR        PORTAbits.RA0      // Bouton Forcage (broche 19)
#define DET_PRES      PORTAbits.RA1      // Detection Presence (broche 20)
#define BP_SEC        PORTAbits.RA2      // Bouton Securite (broche 22)



#define TX_IMG        PORTBbits.RB4      // Transmission Image (broche 33)
#define RX_IMG        PORTBbits.RB5      // Reception Image (broche 41)
#define TX_RAD        PORTBbits.RB6      // Transmission Radio (broche 42)
#define RX_RAD        PORTBbits.RB7      // Reception Radio (broche 43)
#define AL_TX_RX_IR   PORTBbits.RB8      // Alarme Transmission/Reception IR (broche 25)
#define IN1_IMG       PORTBbits.RB9      // Entree Image 1 (broche 8)
#define IN2_IMG       PORTBbits.RB10     // Entree Image 2 (broche 11)

#define OUT1_IMG      LATBbits.LATB11    // Sortie Image 1 (broche 12)
#define OUT2_IMG      LATBbits.LATB12    // Sortie Image 2 (broche 35)
#define OUT_CAGE      LATBbits.LATB13    // Sortie Cage (broche 9)
#define RST_RAD       LATBbits.LATB14    // Reinitialisation Radio (broche 14)
#define DT_AP         LATAbits.LATA4     // Detection Appat (broche 34)
#define OUT1          LATBbits.LATB2     // Sortie 1 (broche 5)
#define OUT2          LATBbits.LATB3     // Sortie 2 (broche 38)
#define AL_RAD        LATCbits.LATC0     // Alarme Radio (broche 6)

unsigned short entt1, entt2;
unsigned char compteur10ms = 0;    // Compteur 10ms
unsigned char tache = 0;           // Tache programme principal
unsigned short tp_cli = 500;
unsigned char i;

// definition du calendrier

Horodatage plages_horaires[5][2]; // 5 plages : [debut][fin]
unsigned char mode_auto = 0; // 0 pour manuel, 1 pour automatique

// Declaration de la structure flag pour les entrees
flag val_entrees;               
#define entrees      val_entrees.mot    // Definition du mot entrees
#define bp_blocage   val_entrees.bita.b0 // Bouton blocage porte ouverte
#define ctx_ir       val_entrees.bita.b1 // Entree 1 image
#define dt_pres      val_entrees.bita.b2 // Detection de presence animal
#define bp_forcage   val_entrees.bita.b3 // Bouton de forcage fermeture
#define dt_appat     val_entrees.bita.b6 // Detection appat
#define in1_img      val_entrees.bita.b8 // Entree 1 image
#define in2_img      val_entrees.bita.b11 // Entree 2 image

// Declaration de la structure flag pour les fronts montants des entrees
flag val_fronts_entrees;               
#define fronts_entrees      val_fronts_entrees.mot    // Definition du mot fronts_entrees
#define fbp_blocage   val_fronts_entrees.bita.b0 // Bouton blocage porte ouverte
#define fctx_ir       val_fronts_entrees.bita.b1 // Entree 1 image
#define fdt_pres      val_fronts_entrees.bita.b2 // Detection de presence animal
#define fbp_forcage   val_fronts_entrees.bita.b3 // Bouton de forcage fermeture
#define fdt_appat     val_fronts_entrees.bita.b6 // Detection appat
#define fin1_img      val_fronts_entrees.bita.b8 // Entree 1 image
#define fin2_img      val_fronts_entrees.bita.b11 // Entree 2 image

// Declaration de la structure flag pour les bits d'application
flag flag_appli_bits;               
#define fl_appli      flag_appli_bits.mot    // Definition du mot fl_appli
#define regul         flag_appli_bits.bita.b0 // Bouton poussoir regulation
#define fl_cli        flag_appli_bits.bita.b1 // Flag clignotant 0,5/0,5s

#define TEMPS_CLI 500
#define TIMER_PERIOD 8750

/////////////////////////////////////////////////////////////////////////////
//				ZONE ECRITURE DES INTERRUPTIONS
/////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////
//				         INTERRUPTION TIMER  1ms
/////////////////////////////////////////////////////////////////////////////
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0; // RAZ flag

    tp_1ms_eep(); // Filtres analogiques

    if (tp_com1 != 0) {
        tp_com1--;
    }

    if (tp_cli != 0) {
        tp_cli--;
    } else {
        tp_cli = 500;
        fl_cli = ~fl_cli;
    }
}

/////////////////////////////////////////////////////////////////////////////
//              Fonction traitement des entrees/sorties
/////////////////////////////////////////////////////////////////////////////
void lecture_entrees(void) { // LECTURE DES ENTREES + DETECTION FRONTS
    entt1 = ((PORTA & 0x0013) << 2);
    entt1 |= (PORTB & 0x0903);
    if (entt1 == entt2) {
        fronts_entrees = ~entrees & entt1; // Detection des fronts
        entrees = entt1;
    } else {
        entt2 = entt1;
    }
}

void gestion_actions(void)
{
    // Traitement des actions en fonction des entrees et du mode
    if (mode_auto) {
        // Actions automatiques
        if ((DET_PRES || DT_AP) && !BP_SEC) {
            // Fermer la cage si détection de présence ou d'appât, et que le bouton de sécurité n'est pas activé
            OUT_CAGE = 0;
        }
    } 
    else {
        // Actions manuelles
        if (bp_forcage) {
            // Forcage de la fermeture de la cage
            OUT_CAGE = 0;
        }
    }

    // Autres actions à traiter...
}


/////////////////////////////////////////////////////////////////////////////
//              Fonction pour verifier les plages horaires
/////////////////////////////////////////////////////////////////////////////
int est_dans_plage(Horodatage *actuelle, Horodatage *debut, Horodatage *fin) {
    // Comparer l'heure actuelle avec les plages debut et fin

    // if (actuelle.annee<debut.annee ) -> non
    // if acutelle.mois < debut.mois -> non
    // if actuelle.jour < debut.jour -> non

    if ((actuelle->annee > debut->annee || 
        (actuelle->annee == debut->annee && actuelle->mois > debut->mois) ||
        (actuelle->annee == debut->annee && actuelle->mois == debut->mois && actuelle->jour > debut->jour) ||
        (actuelle->annee == debut->annee && actuelle->mois == debut->mois && actuelle->jour == debut->jour && actuelle->heure > debut->heure) ||
        (actuelle->annee == debut->annee && actuelle->mois == debut->mois && actuelle->jour == debut->jour && actuelle->heure == debut->heure && actuelle->minutes > debut->minutes) ||
        (actuelle->annee == debut->annee && actuelle->mois == debut->mois && actuelle->jour == debut->jour && actuelle->heure == debut->heure && actuelle->minutes == debut->minutes && actuelle->secondes >= debut->secondes)) &&
        (actuelle->annee < fin->annee || 
        (actuelle->annee == fin->annee && actuelle->mois < fin->mois) ||
        (actuelle->annee == fin->annee && actuelle->mois == fin->mois && actuelle->jour < fin->jour) ||
        (actuelle->annee == fin->annee && actuelle->mois == fin->mois && actuelle->jour == fin->jour && actuelle->heure < fin->heure) ||
        (actuelle->annee == fin->annee && actuelle->mois == fin->mois && actuelle->jour == fin->jour && actuelle->heure == fin->heure && actuelle->minutes < fin->minutes) ||
        (actuelle->annee == fin->annee && actuelle->mois == fin->mois && actuelle->jour == fin->jour && actuelle->heure == fin->heure && actuelle->minutes == fin->minutes && actuelle->secondes <= fin->secondes))) {
        return 1; // Dans la plage
    }
    return 0; // Pas dans la plage
}

/////////////////////////////////////////////////////////////////////////////
//              Verifier le mode automatique
/////////////////////////////////////////////////////////////////////////////
void verifier_mode(void) {
    mode_auto = 0; // Par défaut, mode manuel
    for (int i = 0; i < 5; i++) {
        if (est_dans_plage(&horloge_actuelle, &plages_horaires[i][0], &plages_horaires[i][1])) {
            mode_auto = 1;
            return;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//				FONCTION PRINCIPALE
/////////////////////////////////////////////////////////////////////////////
int main(void) {

    while (1) {
        lecture_entrees(); // LECTURE DES ENTREES
        gestion_com();    //Fonction de gestion communication
        gestion_eep();    //Fonctions de gestion de l'EEPROM
        verifier_mode(); // Verifiez si nous sommes en mode automatique
        gestion_actions(); // Gestion des action en fonction du mode  
        


    }
    return 0;
}