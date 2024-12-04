//
//
//////////////////////////////////////////////////////////////////////
//
//                 Inclusion de fichiers externes
//
//////////////////////////////////////////////////////////////////////
//
#include "Headers_application.h"
//
//
//////////////////////////////////////////////////////////////////////
//
//                 Déclaration de variables globales
//
//////////////////////////////////////////////////////////////////////
//
//  Variables EEPROM
//
qchar long_char1;
unsigned char buffer_prom[taille_buffer]; //buffer pour enregistrement eeprom
unsigned int tacheprom=0;                 //Tache fonction ecriture EEPROM
unsigned int compteuroctet=0;             //Compteur octets eeprom
unsigned int nboctetprom=0;               //Nombre octet eeprom à enregistrer
unsigned int tpprom=0;                    //Tempo gestion temps enregistrement eeprom
unsigned int adprom=0;                    //Indicateur adresse eeprom
//
//
//////////////////////////////////////////////////////////////////////
//
//                 Ecrit octet sur eeprom
//
//////////////////////////////////////////////////////////////////////
//
void ecrit_octet_eep(unsigned long add,unsigned char valeur) //ecrit octet sur eep
	{
	unsigned char a,b,c;
    if(add<taille_eep)
        {
        val_long_eep=add;
        a=adresse_eep;
        b=(val_charhh_eep&0x03)<<1;
        a|=b;
        b=val_charh_eep;
        c=val_charl_eep;
        start_1();
        write_1(a);
        write_1(b);
        write_1(c);
        write_1(valeur);
        stop_1();
        }
    }
//
//
//////////////////////////////////////////////////////////////////////
//
//                 Ecrit octet sur eeprom
//
//////////////////////////////////////////////////////////////////////
//
void ecrit_secteur_eep(unsigned long add,unsigned char valeur) //ecrit octet sur eep
	{
	unsigned char a,b,c;
    if(add<taille_eep)
        {
	val_long_eep=add;
    a=adresse_eep;
	b=(val_charhh_eep&0x03)<<1;
    a|=b;
    b=val_charh_eep;
	c=val_charl_eep;
    start_1();
    write_1(a);
    write_1(b);
	write_1(c);
	for(a=0;a<255;a++)
		{
		write_1(valeur);
		}
        stop_1();
    }
}
//

//
//////////////////////////////////////////////////////////////////////
//
//             Lit octet sur eeprom
//
//////////////////////////////////////////////////////////////////////
//
unsigned char lit_octet_eep(unsigned long add) //lit octet sur eep
	{
	unsigned char a,b,c;
    if(add<taille_eep)
        {
	val_long_eep=add;
    a=adresse_eep;
	b=(val_charhh_eep&0x03)<<1;
    a|=b;
    b=val_charh_eep;
	c=val_charl_eep;
    start_1();
    write_1(a);
    write_1(b);
	write_1(c);
    restart_1();
	a=a|0x01;
	write_1(a);
	a=read_1();
    nack_1();
    stop_1();
	return(a);
    }
    return(0);
}
//
//
//////////////////////////////////////////////////////////////////////
//
//                 Ecrit données eeprom
//
//////////////////////////////////////////////////////////////////////
//
void gestion_eep (void)
	{
	//unsigned char *add;
	unsigned char val;
	if (tacheprom==0)       //tache = 0
		{
		compteuroctet=1;    //pas d'enregistrement en cours
		nboctetprom=1;      //alors init les compteurs
		goto enregistrefin; //fin de fonction
		}
	if (tacheprom==1)      //tache incrementation compteuroctet
		{
		if(tpprom!=0)         //test si eeprom libre en hard
			{
			goto enregistrefin; //si eeprom en activite saut fin de programme
			}
		val=buffer_prom[(compteuroctet-1)];//*add;
		ecrit_octet_eep((adprom+(compteuroctet-1)),val);
		tpprom=tp_eep;
		tacheprom++;    //passe en tache incrementation compteuroctet
		goto enregistrefin; //fin
		}
	if (tacheprom==2)      //tache incrementation compteuroctet
		{
		if (compteuroctet==nboctetprom)//test si dernier octet
			{
			if(tpprom!=0)         //test si eeprom libre en hard
				{
				goto enregistrefin; //si eeprom en activite saut fin de programme
				}
			tacheprom=0;    //remet la tache
            compteuroctet=1;    //pas d'enregistrement en cours
            nboctetprom=1;      //alors init les compteurs
			goto enregistrefin;//fin
			}
		compteuroctet++;    //pas dernier octet alors incremente nboctetprom
		tacheprom=1;        //remet la tache en effacement ecriture
                goto enregistrefin;//fin
		}
tacheprom=0;
enregistrefin:;          //fin
	}
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//                     Ecrit une variable dans l'eeprom
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
int ecrit_donnee_eep(unsigned long add_eep,unsigned char * ptr_donnee,unsigned short nb_octets_eep) //Ecriture d'une variable en eeprom
    {
    unsigned int i;
    if((tacheprom!=0)||(add_eep>=taille_eep))
        {
        return(-1);
        }
    nboctetprom=nb_octets_eep;
    adprom=add_eep;
    for(i=0;i<nboctetprom;i++)
        {
        buffer_prom[i]=*ptr_donnee;
        ptr_donnee++;
        }
    tacheprom++;
    return(0);
    }
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//                    Lit une variable dans l'eeprom
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
int lit_donnee_eep(unsigned long add_eep,unsigned char * ptr_donnee,unsigned short nb_octets_eep) //Ecriture d'une variable en eeprom
    {
    unsigned int i;
    if((tacheprom!=0)||(add_eep>=taille_eep))
        {
        return(-1);
        }
    for(i=0;i<nb_octets_eep;i++)
        {
        *ptr_donnee=lit_octet_eep(add_eep);
        ptr_donnee++;
        add_eep++;
        }
    return(0);
    }
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//                     Fonction etat eeprom 
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
int etat_eep(void)              //Lecture de l'état de l'eeprom 0= prete -1 = occupée
    {
    if(tacheprom!=0)
        {
        return(-1);
        }
    else
        {
        return(0);
        }
    }
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//                     Timer 1 ms
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
void tp_1ms_eep(void)
    {
    if(tpprom!=0)         //test si eeprom libre en hard
		{
		tpprom--; //si eeprom en activite saut fin de programme
		} 
    }
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//                     Restauration des données
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//
void restaure (void)  //restauration des données eeprom
    {
    while (etat_eep()!=0)
        {
        gestion_eep();
        }
    lit_donnee_eep(smode,(unsigned char *)(&mode_auto), eep_char);
    if((mode_auto<0)||(mode_auto>1))
        {
        mode_auto=dmode_auto;
        }
    }