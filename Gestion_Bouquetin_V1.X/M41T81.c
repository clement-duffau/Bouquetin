#include "Headers_application.h"
#include "M41T81.h"
//
//
//////////////////////////////////////////////////////////////////////
//
//                      Déclaration des variables
//
//////////////////////////////////////////////////////////////////////
//
unsigned char horloge[8];		//buffer pour lecture et stockage horloge
struct tm heure_sys;
Horodatage horloge_actuelle;
//struct horloge_date_heure fonc_date_heure_sys;
//
////
//////////////////////////////////////////////////////////////////////
//
//                      LECTURE HORLOGE
//
//////////////////////////////////////////////////////////////////////
//
//met les 8 octets utiles de l'horloge dans le tableau horloge[0]
//
void lit_horloge(void) {
    unsigned char x = 0;
    start_2();
    
    if (write_2(0xD0) != 0) { // Vérification de l'écriture
        stop_2();
        return; // Sortie si l'écriture échoue
    }

    write_2(1);
    stop_2();

    start_2();
    if (write_2(0xD1) != 0) {
        stop_2();
        return; // Sortie si l'écriture échoue
    }

    for (x = 0; x < 8; x++) {
        horloge[x] = read_2();
        if (x < 7) {
            ack_2();
        } else {
            nack_2();
        }
    }
    stop_2();
    horloge_sys(); // Mettre à jour l'heure actuelle
}
                             //fin retour
//
//////////////////////////////////////////////////////////////////////
//
//               ECRITURE HORLOGE
//
//////////////////////////////////////////////////////////////////////
//
//met les 8 octets horloge[0]
//
void ecrit_horloge (void){   //debut fonction
unsigned char x;
start_2();
write_2(0xD0);
write_2(1);
for (x=0;x<8;x++)           //boucle eciture 8 octets
	{
        if(x==7)
            {
            write_2(0x00);
            }
        else
            {
            write_2(horloge[x]);
            }
    }
stop_2();
}                           //fin retour
//
//
/////////////////////////////////////////////////////////////////////
//
//               MISE ON HORLOGE
//
/////////////////////////////////////////////////////////////////////
//
//demarre l'horloge
//
void horloge_on (void){    //debut fonction
unsigned char a;                 //
start_2();
write_2(0xD0);
write_2(1);
stop_2();
start_2();
write_2(0xD1);
a=read_2();
stop_2();
if ((a & 0x80)!=0)           //si horloge deja on fin
	{
	start_2();
	write_2(0xD0);
    write_2(1);
	write_2(0);
	stop_2();
    start_2();
	write_2(0xD0);
    write_2(0x0C);
	write_2(0);
	stop_2();
	}
else
{
    start_2();
	write_2(0xD0);
    write_2(0x0C);
	write_2(0);
	stop_2();
}
}                      //fin retour
//
//
/////////////////////////////////////////////////////////////////////
//
//               MISE OFF HORLOGE
//
/////////////////////////////////////////////////////////////////////
//
//demarre l'horloge
//
void horloge_off (void){    //debut fonction
unsigned char a;                 //
start_2();
write_2(0xD0);
write_2(1);
stop_2();
start_2();
write_2(0xD1);
a = read_2();
a|=0x80;
stop_2();
start_2();
write_2(0xD0);
write_2(1);
write_2(a);
stop_2();
}                      //fin retour
//
//
/////////////////////////////////////////////////////////////////////
//
//               MISE au format horloge
//
/////////////////////////////////////////////////////////////////////
//
//////struct tm {
//////int tm_sec;/*seconds after the minute ( 0 to 61 )*/
///////*allows for up to two leap seconds*/
//////int tm_min;/*minutes after the hour ( 0 to 59 )*/
//////int tm_hour;/*hours since midnight ( 0 to 23 )*/
//////int tm_mday;/*day of month ( 1 to 31 )*/
//////int tm_mon;/*month ( 0 to 11 where January = 0 )*/
//////int tm_year;/*years since 1900*/
//////int tm_wday;/*day of week ( 0 to 6 where Sunday = 0
//////)*/
//////int tm_yday;/*day of year ( 0 to 365 where January 1
//////= 0 )*/
//////int tm_isdst;/*Daylight Savings Time flag*/
//////}


void horloge_sys(void) {
    horloge_actuelle.secondes = (((horloge[0] & 0x70) >> 4) * 10) + (horloge[0] & 0x0F);
    horloge_actuelle.minutes = (((horloge[1] & 0x70) >> 4) * 10) + (horloge[1] & 0x0F);
    horloge_actuelle.heure = (((horloge[2] & 0x30) >> 4) * 10) + (horloge[2] & 0x0F);
    horloge_actuelle.jour = (((horloge[4] & 0x30) >> 4) * 10) + (horloge[4] & 0x0F);
    horloge_actuelle.mois = (((horloge[5] & 0x10) >> 4) * 10) + (horloge[5] & 0x0F) - 1;
    horloge_actuelle.annee = (((horloge[6] & 0xF0) >> 4) * 10) + (horloge[6] & 0x0F);
}

//////