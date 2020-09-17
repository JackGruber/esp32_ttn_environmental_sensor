#pragma once
#ifndef _LORAWAN_H
#define _LORAWAN_H

#include <Arduino.h>
#include <lmic.h>
#include "lora_credentials.h"

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = TTN_APPEUI;

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = TTN_DEVEUI;

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = TTN_APPKEY;

static osjob_t sendjob;

// LMIC State save for reboot
extern RTC_DATA_ATTR u4_t RTC_LORAWAN_netid;
extern RTC_DATA_ATTR devaddr_t RTC_LORAWAN_devaddr;
extern RTC_DATA_ATTR u1_t RTC_LORAWAN_nwkKey[16];
extern RTC_DATA_ATTR u1_t RTC_LORAWAN_artKey[16];
extern RTC_DATA_ATTR u1_t RTC_LORAWAN_dn2Dr;
extern RTC_DATA_ATTR u1_t RTC_LORAWAN_dnConf;
extern RTC_DATA_ATTR u4_t RTC_LORAWAN_seqnoDn;
extern RTC_DATA_ATTR u4_t RTC_LORAWAN_seqnoUp;
extern RTC_DATA_ATTR s1_t RTC_LORAWAN_adrTxPow;
extern RTC_DATA_ATTR s1_t RTC_LORAWAN_datarate;
extern RTC_DATA_ATTR u1_t RTC_LORAWAN_txChnl;
extern RTC_DATA_ATTR s2_t RTC_LORAWAN_adrAckReq;
extern RTC_DATA_ATTR u1_t RTC_LORAWAN_rx1DrOffset;
extern RTC_DATA_ATTR u1_t RTC_LORAWAN_rxDelay;
extern RTC_DATA_ATTR u2_t RTC_LORAWAN_opmode;
extern RTC_DATA_ATTR u4_t RTC_LORAWAN_channelFreq[MAX_CHANNELS];
extern RTC_DATA_ATTR u2_t RTC_LORAWAN_channelDrMap[MAX_CHANNELS];
extern RTC_DATA_ATTR u4_t RTC_LORAWAN_channelDlFreq[MAX_CHANNELS];
extern RTC_DATA_ATTR band_t RTC_LORAWAN_bands[MAX_BANDS];
extern RTC_DATA_ATTR u2_t RTC_LORAWAN_channelMap;

void os_getArtEui(u1_t *buf);
void os_getDevEui(u1_t *buf);
void os_getDevKey(u1_t *buf);

void LoRaWANSetup(void);
void LoraWANDo_send(osjob_t *j);
void LoraWANDo(void);
void LoraWANGetData(void);
void LoraWANSaveLMICToRTC(void);
void LoraWANLoadLMICFromRTC(void);
void LoraWANPrintVersion(void);
void LoraWANPrintLMICOpmode(void);
void LoraWANDebug(void);

#endif
