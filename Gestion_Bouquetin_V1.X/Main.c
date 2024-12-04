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
//
/////////////////////////////////////////////////////////////////////////////
//				DECLARATION DE RAM
/////////////////////////////////////////////////////////////////////////////
//
unsigned short entt1, entt2;
unsigned char compteur10ms = 0;    // Compteur 10ms
unsigned char tache = 0;           // Tache programme principal
unsigned short tp_cli = 500;
unsigned char i;
unsigned char var_test=0;

// definition du calendrier

struct tm plages_horaires[5][2]; // 5 plages : [debut][fin]
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
    // Verifier l'etat de l'EEPROM avant de lire
    if (etat_eep() == 0) {
        // Lire les donnees depuis l'EEPROM et les stocker dans la variable globale
        lit_donnee_eep(adresse_debut, (unsigned char *)plages_horaires, sizeof(plages_horaires));
    }
}



void ecrire_plages_horaires(unsigned long adresse_debut) {
    // Verifier l'etat de l'EEPROM avant d'ecrire
    if (etat_eep() == 0) {
        // ecrire les donnees dans l'EEPROM
        ecrit_donnee_eep(adresse_debut, (unsigned char *)plages_horaires, sizeof(plages_horaires));
    }
}


void gestion_actions(void)
{
    // Traitement des actions en fonction des entrees et du mode
    lire_plages_horaires(0xA0);
    if (mode_auto) {
        // Actions automatiques
        if ((dt_pres || dt_appat) && !bp_blocage) {
            // Fermer la cage si detection de présence ou d'appât, et que le bouton de sécuritïé n'est pas activité
            out_cage = 0;
        }
    } 
    else {
        // Actions manuelles
        recevoir_plages_horaires();
        ecrire_plages_horaires(0xA0);
        if (bp_forcage) {
            // Forcage de la fermeture de la cage
            out_cage = 0;
        }
    }
    // Autres actions a traiter...
}


void recevoir_plages_horaires(void)
{
    // Réinitialiser le buffer de réception et l'index
    index1rx = 0;
    tp_com1 = 100; // Réinitialiser le timer de communication
    // Attendre que la trame soit complètement reçue
    while (index1rx == 0) 
    {
        // attendre que l'interruption UART remplisse le buffer
    }
    // Vérifier si une trame valide a été reçue
    if (index1rx >= 11 && buffer1rx[0] == '<' && buffer1rx[index1rx - 1] == '>') // 1 octet de commande + 10 octets pour 5 plages horaires
    {
        // Extraire les plages horaires de la trame reçue
        for (int i = 0; i < 5; i++)
        {
//////////////////////////////////////            plages_horaires[i][0] = buffer1rx[3 + i * 2]; // Premier octet de la plage
//////////////////////////////////////            plages_horaires[i][1] = buffer1rx[4 + i * 2]; // Deuxième octet de la plage
        }
    }
    else
    {
        // Gestion d'erreur 
    }
}


/////////////////////////////////////////////////////////////////////////////
//              Fonction pour verifier les plages horaires
/////////////////////////////////////////////////////////////////////////////
int est_dans_plage(struct tm *actuelle, struct tm *debut, struct tm *fin) {
    // Convertir struct tm en time_t pour la comparaison
    time_t time_actuelle = mktime(actuelle);
    time_t time_debut = mktime(debut);
    time_t time_fin = mktime(fin);
    // Vérifier si l'heure actuelle est dans la plage
    return (time_actuelle >= time_debut && time_actuelle <= time_fin);
}





//int __est_dans_plage(struct tm *actuelle, struct tm *debut, struct tm *fin) {
//    struct tm tm_actuelle = {0};
//    struct tm tm_debut = {0};
//    struct tm tm_fin = {0};
//
//    // Remplir les champs de struct tm pour l'heure actuelle
//    tm_actuelle.tm_year = actuelle->annee + 2000 - 1900; 
//    tm_actuelle.tm_mon = actuelle->mois - 1;             // Mois (0-11)
//    tm_actuelle.tm_mday = actuelle->jour;
//    tm_actuelle.tm_hour = actuelle->heure;
//    tm_actuelle.tm_min = actuelle->minutes;
//    tm_actuelle.tm_sec = actuelle->secondes;
//
//    // Remplir les champs de struct tm pour le début de la plage
//    tm_debut.tm_year = debut->annee + 2000 - 1900;
//    tm_debut.tm_mon = debut->mois - 1;
//    tm_debut.tm_mday = debut->jour;
//    tm_debut.tm_hour = debut->heure;
//    tm_debut.tm_min = debut->minutes;
//    tm_debut.tm_sec = debut->secondes;
//
//    // Remplir les champs de struct tm pour la fin de la plage
//    tm_fin.tm_year = fin->annee + 2000 - 1900;
//    tm_fin.tm_mon = fin->mois - 1;
//    tm_fin.tm_mday = fin->jour;
//    tm_fin.tm_hour = fin->heure;
//    tm_fin.tm_min = fin->minutes;
//    tm_fin.tm_sec = fin->secondes;
//
//    // Convertir struct tm en time_t pour la comparaison
//    time_t time_actuelle = mktime(&tm_actuelle);
//    time_t time_debut = mktime(&tm_debut);
//    time_t time_fin = mktime(&tm_fin);
//
//    // VÃ©rifier si l'heure actuelle est dans la plage
//    return (time_actuelle >= time_debut && time_actuelle <= time_fin);
//}


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
//				TESTS
/////////////////////////////////////////////////////////////////////////////


// Test pour est_dans_plage
void test_est_dans_plage() {
    struct tm actuelle = {0x24, 10, 30, 12, 0, 0};
    struct tm debut = {0x24, 10, 30, 8, 0, 0};
    struct tm fin = {0x24, 10, 30, 16, 0, 0};
 
    int result = est_dans_plage(&actuelle, &debut, &fin);
    if (result) {
        var_test++;
    } else {
        var_test = 0;
    }
}

// Test pour verifier_mode
void test_verifier_mode() {
//////////////////////////    struct tm horloge_actuelle = {2024, 10, 30, 10, 0, 0};  // L?heure actuelle
    plages_horaires[0][0] = (struct tm){2024, 10, 30, 8, 0, 0}; // Dï¿½but plage
    plages_horaires[0][1] = (struct tm){2024, 10, 30, 16, 0, 0}; // Fin plage

    verifier_mode();

    if (mode_auto == 1) {
        var_test++;
    } else {
        var_test = 0;
    }
}


// Test pour gestion_actions
void test_gestion_actions() {
    // Cas 1 : mode automatique avec dï¿½tection de prï¿½sence
    mode_auto = 1;
    dt_pres = 1;
    bp_blocage = 0;
    gestion_actions();
    if (out_cage == 0) {
        var_test++;
    } else {
        var_test = 0;
    }

    // Cas 2 : mode manuel avec forcage
    mode_auto = 0;
    bp_forcage = 1;
    gestion_actions();
    if (out_cage == 0) {
        var_test++;
    } else {
        var_test = 0;
    }
}


/////////////////////////////////////////////////////////////////////////////
//				FONCTION PRINCIPALE
/////////////////////////////////////////////////////////////////////////////
int main(void) {  
//
LATA=0;
LATB=0;               // RAZ des ports E/S
LATC=0;      // RAZ des sorties des ports 
//
//
OSCCON=0x46;          //Proc dure de d blocage d' criture dans les registres.
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
OSCCON=0x46;           //Proc dure de blocage d' criture dans les registres
OSCCON=0x57;		   //
OSCCONbits.IOLOCK=1;   //
//
//
//
////////////////////////////////////////////////////////////////////////////////
//
// Configuration de l'horloge (PLL pour 70 MIPS à partir de 10 Mhz)
//
    PLLFBD = ((FCY/1250000)-2); // M = 70
    CLKDIVbits.PLLPOST = 0; // N2 = 2
    CLKDIVbits.PLLPRE = 0; // N1 = 2
// switch oscillateur FRC vers PLL
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(0x01);
// Attente switch clock OK
    while (OSCCONbits.COSC!= 0b011);
// Attente PLL OK
    while(OSCCONbits.LOCK!= 1) {};
//
ANSELA=0x0000;
ANSELB=0x0000;
//
TRISA=0xF87F;           // initialisation du port A
TRISB=0x09AF;           // initialisation du port B
ODCB=0xF000;
TRISC=0xFC00;           // initialisation du port A

//
LATA=0;
LATB=0;      // RAZ des sorties des ports
LATC=0;      // RAZ des sorties des ports
//
//
//      Configuration du timer principal IT   1ms
//
PR1=8750;       //P riode timer 1
T1CON=0x2010;    //Config timer 1 TCY/8
//
//			 Configuration des p riph riques
//
//  Initialisation du convertisseur analogique/num rique
//
//
//
//  Raz g n rale des flags d'interruption
//
IFS0=0;          //Raz flags interruption
IFS1=0;          //Raz flags interruption
IFS2=0;          //Raz flags interruption
IFS3=0;          //Raz flags interruption
IFS4=0;          //Raz flags interruption
//
//
open_i2c_1_master7(20000);
open_i2c_2_master7(20000);
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
//
init_com ();       //Initialisation UART
//
// It timer  on priorit  1
//
IPC0bits.T1IP=1;	 //Priorit  des interruptions T1 P1
IFS0bits.T1IF = 0; // RAZ flag
IEC0bits.T1IE=1;  // It timer 1 on
//
T1CONbits.TON=1;
//
restaure ();
//
//////////////////////////////////////////////////////////////////////////////////////
//
//                          DEBUT DE BOUCLE PROGRAMME
//
//////////////////////////////////////////////////////////////////////////////////////
//
  
    while (1) {
        
        test_est_dans_plage();
        test_verifier_mode();
        test_gestion_actions();
        gestion_enregistrement();
//        lire_plages_horaires(0xB0);
//        lecture_entrees(); // LECTURE DES ENTREES
//        gestion_com();    //Fonction de gestion communication
//        gestion_eep();    //Fonctions de gestion de l'EEPROM
//        verifier_mode(); // Verifiez si nous sommes en mode automatique
//        gestion_actions(); // Gestion des action en fonction du mode  
        


    }
    return 0;
}