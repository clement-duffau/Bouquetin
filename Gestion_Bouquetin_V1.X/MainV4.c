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

// Définition des broches en fonction du schéma
#define BP_FOR        PORTAbits.RA0      // Bouton Forçage (broche 19)
#define DET_PRES      PORTAbits.RA1      // Détection Présence (broche 20)
#define BP_SEC        PORTAbits.RA2      // Bouton Sécurité (broche 22)

#define TX_IMG        PORTBbits.RB4      // Transmission Image (broche 33)
#define RX_IMG        PORTBbits.RB5      // Réception Image (broche 41)
#define TX_RAD        PORTBbits.RB6      // Transmission Radio (broche 42)
#define RX_RAD        PORTBbits.RB7      // Réception Radio (broche 43)
#define AL_TX_RX_IR   PORTBbits.RB8      // Alarme Transmission/Réception IR (broche 25)
#define IN1_IMG       PORTBbits.RB9      // Entrée Image 1 (broche 8)
#define IN2_IMG       PORTBbits.RB10     // Entrée Image 2 (broche 11)

#define OUT1_IMG      LATBbits.LATB11    // Sortie Image 1 (broche 12)
#define OUT2_IMG      LATBbits.LATB12    // Sortie Image 2 (broche 35)
#define OUT_CAGE      LATBbits.LATB13    // Sortie Cage (broche 9)
#define RST_RAD       LATBbits.LATB14    // Réinitialisation Radio (broche 14)
#define DT_AP         LATAbits.RA4       // Détection Appât (broche 34)
#define OUT1          LATBbits.LATB2     // Sortie 1 (broche 5)
#define OUT2          LATBbits.LATB3     // Sortie 2 (broche 38)
#define AL_RAD        LATCbits.LATC0     // Alarme Radio (broche 6)

unsigned short entt1, entt2;
unsigned char compteur10ms = 0;    // Compteur 10ms
unsigned char tache = 0;           // Tâche programme principal
unsigned short tp_cli = 500;

// définition du calendrier
typedef struct {
    unsigned char annee;   // 1 octet
    unsigned char mois;    // 1 octet
    unsigned char jour;    // 1 octet
    unsigned char heure;   // 1 octet
    unsigned char minutes; // 1 octet
    unsigned char secondes; // 1 octet
} Horodatage;

Horodatage plages_horaires[5][2]; // 5 plages : [début][fin]
unsigned char mode_auto = 0; // 0 pour manuel, 1 pour automatique

// Déclaration de la structure flag pour les entrées
flag val_entrees;               
#define entrees      val_entrees.mot    // Définition du mot entrees
#define bp_blocage   val_entrees.bita.b0 // Bouton blocage porte ouverte
#define ctx_ir       val_entrees.bita.b1 // Entrée 1 image
#define dt_pres      val_entrees.bita.b2 // Détection de présence animal
#define bp_forcage   val_entrees.bita.b3 // Bouton de forçage fermeture
#define dt_appat     val_entrees.bita.b6 // Détection appât
#define in1_img      val_entrees.bita.b8 // Entrée 1 image
#define in2_img      val_entrees.bita.b11 // Entrée 2 image

// Déclaration de la structure flag pour les fronts montants des entrées
flag val_fronts_entrees;               
#define fronts_entrees      val_fronts_entrees.mot    // Définition du mot fronts_entrees
#define fbp_blocage   val_fronts_entrees.bita.b0 // Bouton blocage porte ouverte
#define fctx_ir       val_fronts_entrees.bita.b1 // Entrée 1 image
#define fdt_pres      val_fronts_entrees.bita.b2 // Détection de présence animal
#define fbp_forcage   val_fronts_entrees.bita.b3 // Bouton de forçage fermeture
#define fdt_appat     val_fronts_entrees.bita.b6 // Détection appât
#define fin1_img      val_fronts_entrees.bita.b8 // Entrée 1 image
#define fin2_img      val_fronts_entrees.bita.b11 // Entrée 2 image

// Déclaration de la structure flag pour les bits d'application
flag flag_appli_bits;               
#define fl_appli      flag_appli_bits.mot    // Définition du mot fl_appli
#define regul         flag_appli_bits.bita.b0 // Bouton poussoir régulation
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
//              Fonction traitement des entrées/sorties
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

/////////////////////////////////////////////////////////////////////////////
//              Fonction pour vérifier les plages horaires
/////////////////////////////////////////////////////////////////////////////
int est_dans_plage(Horodatage *actuelle, Horodatage *debut, Horodatage *fin) {
    // Comparer l'heure actuelle avec les plages début et fin

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
//              Vérifier le mode automatique
/////////////////////////////////////////////////////////////////////////////
void verifier_mode(void) {
    Horodatage heure_actuelle;// TODO a deplacer
    recuperer_heure(&heure_actuelle); // Fonction pour récupérer l'heure

    // Vérifiez si l'heure actuelle est dans l'une des plages horaires
    for (int i = 0; i < 5; i++) {
        if (est_dans_plage(&heure_actuelle, &plages_horaires[i][0], &plages_horaires[i][1])) {
            mode_auto = 1;
            return;
        }
    }
    mode_auto = 0;
}

/////////////////////////////////////////////////////////////////////////////
//				FONCTION PRINCIPALE
/////////////////////////////////////////////////////////////////////////////
int main(void) {
    config_timer();
    config_ports();

    // Initialisation des horaires
    initialiser_plages_horaires();

    while (1) {
        lecture_entrees(); // LECTURE DES ENTREES
        verifier_mode(); // Vérifiez si nous sommes en mode automatique

        // Traitement des actions en fonction des entrées et du mode
        if (mode_auto) {
            // Actions automatiques
            if (dt_pres) {
                // Ouvrir la cage si présence
                OUT_CAGE = 1;
            }
        } else {
            // Actions manuelles
            if (bp_forcage) {
                // Forçage de la fermeture de la cage
                OUT_CAGE = 0;
            }
        }

        // Autres actions à traiter...
    }

    return 0;
}
