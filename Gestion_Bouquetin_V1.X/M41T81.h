//
////////////////////////////////////////////////////////////////////////////////
//
//           Entête fichier M41T81  
//
////////////////////////////////////////////////////////////////////////////////
//
// Description : Ce fichier contient les prototypes de fonctions pour gérer
//               l'horloge M41T81, ainsi que la définition de la structure
//               pour représenter un horodatage.
//
//
#ifndef M41T81_H    /* Guard against multiple inclusion */
#define M41T81_H

////////////////////////////////////////////////////////////////////////////////
//
//      Définition des ports d'entrées/sorties
//
////////////////////////////////////////////////////////////////////////////////

// Prototype des fonctions pour l'horloge




// Définition de la structure pour l'horodatage
typedef struct {
    unsigned char annee;    // Année
    unsigned char mois;     // Mois (1-12)
    unsigned char jour;     // Jour (1-31)
    unsigned char heure;    // Heure (0-23)
    unsigned char minutes;   // Minutes (0-59)
    unsigned char secondes;  // Secondes (0-59)
} Horodatage;


void horloge_on(void);        // Active l'horloge
void horloge_off(void);       // Désactive l'horloge
void lit_horloge(void);       // Lit les 8 octets de l'horloge
void ecrit_horloge(void);     // Écrit l'heure dans l'horloge
void horloge_sys(void);       // Met à jour l'heure système


////////////////////////////////////////////////////////////////////////////////
//
//          Ram fonctions LCD
//
////////////////////////////////////////////////////////////////////////////////

extern unsigned char horloge[8];  // Buffer pour lecture et stockage de l'horloge
extern struct tm heure_sys;        // Structure pour gérer l'heure système
extern Horodatage horloge_actuelle;


#endif // M41T81_H
