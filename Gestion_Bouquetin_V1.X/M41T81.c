#include "Headers_application.h"
#include "M41T81.h"
//
//
//////////////////////////////////////////////////////////////////////
//
//                      D�claration des variables
//
//////////////////////////////////////////////////////////////////////
//
unsigned char horloge[8];		//buffer pour lecture et stockage horloge
struct tm heure_sys;
struct tm horloge_actuelle;
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
    
    if (write_2(0xD0) != 0) { // V�rification de l'�criture
        stop_2();
        return; // Sortie si l'�criture �choue
    }

    write_2(0);
    stop_2();

    start_2();
    if (write_2(0xD1) != 0) {
        stop_2();
        return; // Sortie si l'�criture �choue
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
    horloge_sys(); // Mettre � jour l'heure actuelle
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
write_2(0);
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
write_2(0);
stop_2();
start_2();
write_2(0xD1);
a=read_2();
stop_2();
if ((a & 0x80)!=0)           //si horloge deja on fin
	{
	start_2();
	write_2(0xD0);
    write_2(0);
	write_2(0);
	stop_2();
//    start_2();
//	write_2(0xD0);
//    write_2(0x0C);
//	write_2(0);
//	stop_2();
	}
else
{
//    start_2();
//	write_2(0xD0);
//    write_2(0x0C);
//	write_2(0);
//	stop_2();
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
write_2(0);
stop_2();
start_2();
write_2(0xD1);
a = read_2();
a|=0x80;
stop_2();
start_2();
write_2(0xD0);
write_2(0);
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

    // Convertir les valeurs de l'horloge en format struct tm
    heure_sys.tm_sec = ((horloge[0] & 0x70) >> 4) * 10 + (horloge[0] & 0x0F);
    heure_sys.tm_min = ((horloge[1] & 0x70) >> 4) * 10 + (horloge[1] & 0x0F);
    heure_sys.tm_hour = ((horloge[2] & 0x30) >> 4) * 10 + (horloge[2] & 0x0F);
    heure_sys.tm_mday = ((horloge[4] & 0x30) >> 4) * 10 + (horloge[4] & 0x0F);
    heure_sys.tm_mon = ((horloge[5] & 0x10) >> 4) * 10 + (horloge[5] & 0x0F) - 1; // Mois de 0 � 11
    heure_sys.tm_year = ((horloge[6] & 0xF0) >> 4) * 10 + (horloge[6] & 0x0F) + 100; // Ann�es depuis 1900
    // Initialisation des autres champs de struct tm
    heure_sys.tm_isdst = -1; // -1 pour indiquer que l'information sur l'heure d'�t� n'est pas connue

}

//////