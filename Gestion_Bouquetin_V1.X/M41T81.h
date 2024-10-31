//
////////////////////////////////////////////////////////////////////////////////
//
//           Ent�te fichier M41T81  
//
////////////////////////////////////////////////////////////////////////////////
//
// Description : Ce fichier contient les prototypes de fonctions pour g�rer
//               l'horloge M41T81, ainsi que la d�finition de la structure
//               pour repr�senter un horodatage.
//
//
#ifndef M41T81_H    /* Guard against multiple inclusion */
#define M41T81_H

////////////////////////////////////////////////////////////////////////////////
//
//      D�finition des ports d'entr�es/sorties
//
////////////////////////////////////////////////////////////////////////////////

// Prototype des fonctions pour l'horloge




// D�finition de la structure pour l'horodatage
typedef struct {
    unsigned char annee;    // Ann�e
    unsigned char mois;     // Mois (1-12)
    unsigned char jour;     // Jour (1-31)
    unsigned char heure;    // Heure (0-23)
    unsigned char minutes;   // Minutes (0-59)
    unsigned char secondes;  // Secondes (0-59)
} Horodatage;


void horloge_on(void);        // Active l'horloge
void horloge_off(void);       // D�sactive l'horloge
void lit_horloge(void);       // Lit les 8 octets de l'horloge
void ecrit_horloge(void);     // �crit l'heure dans l'horloge
void horloge_sys(void);       // Met � jour l'heure syst�me


////////////////////////////////////////////////////////////////////////////////
//
//          Ram fonctions LCD
//
////////////////////////////////////////////////////////////////////////////////

extern unsigned char horloge[8];  // Buffer pour lecture et stockage de l'horloge
extern struct tm heure_sys;        // Structure pour g�rer l'heure syst�me
extern Horodatage horloge_actuelle;


#endif // M41T81_H
