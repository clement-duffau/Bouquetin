//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                Inclusion de fichier
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#ifndef i2c_H    /* Guard against multiple inclusion */
#define i2c_H
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                Définition de constantes
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define horloge_bus_UC 70E6
//
//   Bus I2C  OPTIONS
//
#define I2C1_USE
#define I2C2_USE
//#define I2C3_USE
//#define I2C4_USE
//#define I2C5_USE
//   
//   
//
#ifdef I2C1_USE
//
#define scl_1     LATBbits.LATB12                //Sortie horloge I2C
#define sda_1     PORTBbits.RB13                 //Sortie data I2C
#define sdao_1    LATBbits.LATB13                //Définition de l'entrée switch 1
//
#endif
//
#ifdef I2C2_USE
//
#define scl_2     LATBbits.LATB14               //Sortie horloge I2C
#define sda_2     PORTBbits.RB15                //Sortie data I2C
#define sdao_2    LATBbits.LATB15                //Définition de l'entrée switch 1
//
#endif
#ifdef I2C3_USE
//
#define scl_3     LATFbits.LATF8               //Sortie horloge I2C
#define sda_3     PORTFbits.RF2                 //Sortie data I2C
#define sdao_3    LATFbits.LATF2                //Définition de l'entrée switch 1
//
#endif
#ifdef I2C4_USE
//
#define scl_4     LATGbits.LATG8               //Sortie horloge I2C
#define sda_4     PORTGbits.RG7                 //Sortie data I2C
#define sdao_4    LATGbits.LATG7                //Définition de l'entrée switch 1
//
#endif
#ifdef I2C5_USE
//
#define scl_5     LATFbits.LATF5               //Sortie horloge I2C
#define sda_5     PORTFbits.RF4                 //Sortie data I2C
#define sdao_5    LATFbits.LATF4                //Définition de l'entrée switch 1
//
#endif
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                Prototypes des fonctions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void delais_i2c(unsigned int);
//
#ifdef I2C1_USE
//
int open_i2c_1_master7(unsigned long bdr); //Ouverure I2C 1 en mode master 7 bits d'adresse sans interruptions
void start_1 (void);                   //Fonction start I2C 1
void restart_1 (void);                 //Fonction restart
void ack_1 (void);                     //Fonction acknoledge
void nack_1 (void);                    //Fonction Not acknoledge
void stop_1 (void);                    //Fonction stop
void clock_1 (void);                   //Horloge I2C 1
unsigned char read_1 (void);           //Fonction read
int write_1 (unsigned char); //Fonction write
//
extern unsigned int brg_i2c_1;
//
#endif
#ifdef I2C2_USE
//
int open_i2c_2_master7(unsigned long bdr); //Ouverure I2C 1 en mode master 7 bits d'adresse sans interruptions
void start_2 (void);                   //Fonction start I2C 1
void restart_2 (void);                 //Fonction restart
void ack_2 (void);                     //Fonction acknoledge
void nack_2 (void);                    //Fonction Not acknoledge
void stop_2 (void);                    //Fonction stop
void clock_2 (void);                   //Horloge I2C 1
unsigned char read_2 (void);           //Fonction read
int write_2 (unsigned char); //Fonction write
//
extern unsigned int brg_i2c_2;
//
#endif
#ifdef I2C3_USE
//
int open_i2c_3_master7(unsigned long bdr); //Ouverure I2C 1 en mode master 7 bits d'adresse sans interruptions
void start_3 (void);                   //Fonction start I2C 1
void restart_3 (void);                 //Fonction restart
void ack_3 (void);                     //Fonction acknoledge
void nack_3 (void);                    //Fonction Not acknoledge
void stop_3(void);                    //Fonction stop
void clock_3 (void);                   //Horloge I2C 1
unsigned char read_3 (void);           //Fonction read
int write_3 (unsigned char); //Fonction write
//
extern unsigned int brg_i2c_3;
//
#endif
//
#ifdef I2C4_USE
//
int open_i2c_4_master7(unsigned long bdr); //Ouverure I2C 1 en mode master 7 bits d'adresse sans interruptions
void start_4 (void);                   //Fonction start I2C 1
void restart_4 (void);                 //Fonction restart
void ack_4 (void);                     //Fonction acknoledge
void nack_4 (void);                    //Fonction Not acknoledge
void stop_4(void);                    //Fonction stop
void clock_4 (void);                   //Horloge I2C 1
unsigned char read_4 (void);           //Fonction read
int write_4 (unsigned char); //Fonction write
//
extern unsigned int brg_i2c_4;
//
#endif
//
#ifdef I2C5_USE
//
int open_i2c_5_master7(unsigned long bdr); //Ouverure I2C 1 en mode master 7 bits d'adresse sans interruptions
void start_5 (void);                   //Fonction start I2C 1
void restart_5 (void);                 //Fonction restart
void ack_5 (void);                     //Fonction acknoledge
void nack_5 (void);                    //Fonction Not acknoledge
void stop_5(void);                    //Fonction stop
void clock_5 (void);                   //Horloge I2C 1
unsigned char read_5 (void);           //Fonction read
int write_5 (unsigned char); //Fonction write
//
extern unsigned int brg_i2c_5;
//
#endif
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                Definition ram
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
#endif
//