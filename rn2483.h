/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef RN2483_H
#define RN2483_H

#include "project.h"
#include <stdio.h>
#include "globals_aq.h"

// constantes
#define RN2483_CONNECT_MODE_OTAA 0
#define RN2483_CONNECT_MODE_ABP  1
/*
#define RN2483_SIZE_DEVEUI  16
#define RN2483_SIZE_APPEUI  16
#define RN2483_SIZE_APPKEY  32
#define RN2483_SIZE_DEVADDR  8
#define RN2483_SIZE_APPSKEY  32
#define RN2483_SIZE_NWKSKEY  32
#define RN2483_SIZE_SF  5
*/
#define RN2483_SIZE_DEVEUI  17
#define RN2483_SIZE_APPEUI  17
#define RN2483_SIZE_APPKEY  33
#define RN2483_SIZE_DEVADDR  9
#define RN2483_SIZE_APPSKEY  33
#define RN2483_SIZE_NWKSKEY  33
#define RN2483_SIZE_SF  6
    
#define RN2483_MAC_TX_MODE_CNF 1
#define RN2483_MAC_TX_MODE_UNCNF 0
#define RN2483_MAC_STATUS_INIT_DONE 1
#define RN2483_MAC_STATUS_CONNECTED 2


#define RN2483_DEFAULT_DEVEUI "F88FDEAD0005F88F\0"
#define RN2483_DEFAULT_APPEUI "DEAD42DEAD42DEAD\0"
#define RN2483_STR(x) RN2483_DEFAULT_APPKEY_STR(x)
#define RN2483_DEFAULT_APPKEY_STR(x) #x
#define RN2483_DEFAULT_APPKEY "0123456789ABCDEF0123456789ABCDEF"
#define RN2483_DEFAULT_SF "sf12\0"

#define FREQ_COUNTER    100000u
#define ATLAS_PH_CONNECTED 0x01
#define ATLAS_EC_CONNECTED 0x02
#define ATLAS_DO_CONNECTED 0x04
#define ATLAS_ORP_CONNECTED 0x08
#define VEML7700_CONNECTED 0x10
#define DS18B20_CONNECTED 0x20
    
typedef struct rn2483_config_struct
{
    char deveui[RN2483_SIZE_DEVEUI];
    char appeui[RN2483_SIZE_APPEUI];
    char appkey[RN2483_SIZE_APPKEY];
    char devaddr[RN2483_SIZE_DEVADDR];
    char appskey[RN2483_SIZE_APPSKEY];
    char nwkskey[RN2483_SIZE_NWKSKEY];
    char sf[RN2483_SIZE_SF];
    char mac_status;
} rn2483_config_t;

// variables globales
extern rn2483_config_t rn2483_config;

// prototypes de fonctions

uint8 rn2483_is_plugged(void); // verifier que le RN2483 est connecte au PSoC
uint8 rn2483_init();
uint8 rn2483_send_data(float temp, float lum, float ph, float cond, float oxy, float red); // envoie les mesures par le LoRaWAN // change to use sensors types (void)

//------------------------------------------- LoRaWAN -------------------------------------------
uint8 rn2483_mac_join(uint8 mode); // connecter le module LoRaWAN RN2483 avec choix du mode (otaa, abp) (join)
uint8 rn2483_mac_tx(uint8 mode, uint8 port, char *payload); // envoyer une chaine de donnee sur un port spécifique (tx)
uint8 rn2483_mac_pause(uint16 timeout); // arreter/mettre en pause la stack LoRaWAN (to change radio configuration) (pause)
uint8 rn2483_mac_resume(void); // reprendre le fonctionnement de la stack LoRaWAN (resume)
uint8 rn2483_mac_save(void); // sauvegarder en EEPROM les paramètres de classe A
uint8 rn2483_mac_set_deveui(char *deveui); // modifier le DevEUI (8 bytes)
uint8 rn2483_mac_get_deveui(void); // modifier le DevEUI (8 bytes)
uint8 rn2483_mac_set_appeui(char *appeui); // modifier l'AppEUI (8 bytes)
uint8 rn2483_mac_get_appeui(void); // recuperer l'AppEUI (8 bytes)
uint8 rn2483_mac_set_appkey(char *appkey); // modifier l'AppKey (16 bytes)
uint8 rn2483_mac_get_appkey(char *appkey); // recuperer l'AppKey (16 bytes)
uint8 rn2483_mac_set_devaddr(char *devaddr); // modifier le DevAddr (4 bytes)
uint8 rn2483_mac_get_devaddr(char *devaddr); // recuperer le DevAddr (4 bytes)
uint8 rn2483_mac_set_appskey(char *appskey); // modifier l'AppSKey (16 bytes)
uint8 rn2483_mac_get_appskey(char *appskey); // recuperer l'AppSKey (16 bytes)
uint8 rn2483_mac_set_nwkskey(char *nwkskey); // modifier le NwkSKey (16 bytes)
uint8 rn2483_mac_get_nwkskey(char *nwkskey); // recuperer le NwkSKey (16 bytes)

//-------------------------------------------- LoRa ---------------------------------------------

uint8 rn2483_radio_tx(char *payload); // envoyer des paquets en LoRa
uint8 rn2483_radio_rx(char *payload); // recevoir des paquets en LoRa
uint8 rn2483_radio_rxstop(void); // arreter la reception continue (rxstop)
uint8 rn2483_radio_set_freq(uint32 freq); // modifier la fréquence de la porteuse
uint8 rn2483_radio_get_freq(uint32 freq); // recuperer la fréquence de la porteuse
uint8 rn2483_radio_set_sf(uint8 sf); // modifier le spreading factor (SF)
uint8 rn2483_radio_get_sf(uint8 sf); // recuperer le spreading factor (SF)
uint8 rn2483_radio_set_mod(uint8 mod); // modifier la modulation (LoRa, FSK)
uint8 rn2483_radio_get_mod(uint8 mod); // recuperer la modulation (LoRa, FSK)
    
#endif

/* [] END OF FILE */
