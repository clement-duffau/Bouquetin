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


unsigned short entt1, entt2;
unsigned char compteur10ms = 0;    // Compteur 10ms
unsigned char tache = 0;           // Tache programme principal
unsigned short tp_cli = 500;
unsigned char i;
unsigned char OUT_CAGE;

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




void lire_plages_horaires(unsigned long adresse_debut) {
    // Vérifier l'état de l'EEPROM avant de lire
    if (etat_eep() == 0) {
        // Lire les données depuis l'EEPROM et les stocker dans la variable globale
        lit_donnee_eep(adresse_debut, (unsigned char *)plages_horaires, sizeof(plages_horaires));
    }
}



void ecrire_plages_horaires(unsigned long adresse_debut) {
    // Vérifier l'état de l'EEPROM avant d'écrire
    if (etat_eep() == 0) {
        // Écrire les données dans l'EEPROM
        ecrit_donnee_eep(adresse_debut, (unsigned char *)plages_horaires, sizeof(plages_horaires));
    }
}


void gestion_actions(void)
{
    // Traitement des actions en fonction des entrees et du mode
    if (mode_auto) {
        // Actions automatiques
        if ((dt_pres || dt_appat) && !bp_blocage) {
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
        lire_plages_horaires(0xB0);
        lecture_entrees(); // LECTURE DES ENTREES
        gestion_com();    //Fonction de gestion communication
        gestion_eep();    //Fonctions de gestion de l'EEPROM
        verifier_mode(); // Verifiez si nous sommes en mode automatique
        gestion_actions(); // Gestion des action en fonction du mode  
        


    }
    return 0;
}