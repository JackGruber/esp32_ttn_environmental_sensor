#include "lorawan.h"
#include "settings.h"
#include <hal/hal.h>
#include <SPI.h>
#include "io_pins.h"
#include "functions.h"
#include <power.h>
#include <particle.h>
#include <global.h>
#include <bme280.h>

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = PIN_LMIC_NSS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PIN_LMIC_RST,
    .dio = {PIN_LMIC_DIO0, PIN_LMIC_DIO1, PIN_LMIC_DIO2},
};

static uint8_t LORA_DATA[11];

// Schedule TX every this many seconds (might become longer due to duty cycle limitations).
const unsigned TX_INTERVAL = LORA_TX_INTERVAL;

void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

bool GO_DEEP_SLEEP = false;

RTC_DATA_ATTR u4_t RTC_LORAWAN_netid = 0;
RTC_DATA_ATTR devaddr_t RTC_LORAWAN_devaddr = 0;
RTC_DATA_ATTR u1_t RTC_LORAWAN_nwkKey[16];
RTC_DATA_ATTR u1_t RTC_LORAWAN_artKey[16];
RTC_DATA_ATTR u1_t RTC_LORAWAN_dn2Dr;
RTC_DATA_ATTR u1_t RTC_LORAWAN_dnConf;
RTC_DATA_ATTR u4_t RTC_LORAWAN_seqnoDn;
RTC_DATA_ATTR u4_t RTC_LORAWAN_seqnoUp = 0;
RTC_DATA_ATTR s1_t RTC_LORAWAN_adrTxPow;
RTC_DATA_ATTR s1_t RTC_LORAWAN_datarate;
RTC_DATA_ATTR u1_t RTC_LORAWAN_txChnl;
RTC_DATA_ATTR s2_t RTC_LORAWAN_adrAckReq;
RTC_DATA_ATTR u1_t RTC_LORAWAN_rx1DrOffset;
RTC_DATA_ATTR u1_t RTC_LORAWAN_rxDelay;
RTC_DATA_ATTR u4_t RTC_LORAWAN_channelFreq[MAX_CHANNELS];
RTC_DATA_ATTR u2_t RTC_LORAWAN_channelDrMap[MAX_CHANNELS];
RTC_DATA_ATTR u4_t RTC_LORAWAN_channelDlFreq[MAX_CHANNELS];
RTC_DATA_ATTR band_t RTC_LORAWAN_bands[MAX_BANDS];
RTC_DATA_ATTR u2_t RTC_LORAWAN_channelMap;
RTC_DATA_ATTR u2_t RTC_LORAWAN_opmode;

void LoRaWANSetup()
{
    Serial.println(F("LoRaWAN_Setup ..."));

    Serial.print(F("Saved seqnoUp: "));
    Serial.println(RTC_LORAWAN_seqnoUp);

    // LMIC init
    os_init();

    // Let LMIC compensate for +/- 1% clock error
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    if (RTC_LORAWAN_seqnoUp != 0)
    {
        LoraWANLoadLMICFromRTC();
        LMICbandplan_joinAcceptChannelClear();
    }

    // Start job
    LoraWANDo_send(&sendjob);
}

void LoraWANDo_send(osjob_t *j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else if (LMIC.opmode & OP_TXDATA) {
        Serial.println(F("OP_TXDATA, not sending"));
    } else {
        LoraWANGetData();

        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, LORA_DATA, sizeof(LORA_DATA), 0);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent(ev_t ev)
{
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        break;
    case EV_JOINED:
        Serial.println(F("EV_JOINED"));
#ifndef DISABLE_JOIN
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("artKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i)
            {
                Serial.print(artKey[i], HEX);
            }
            Serial.println("");
            Serial.print("nwkKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i)
            {
                Serial.print(nwkKey[i], HEX);
            }
            Serial.println("");
        }
        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
        // size, we don't use it in this example.
        LMIC_setLinkCheckMode(0);
#endif
        break;
    /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        break;
    case EV_TXCOMPLETE:
        Serial.println(F("EV_TXCOMPLETE"));

        if (LMIC.txrxFlags & TXRX_ACK)
        {
            Serial.println(F("Received ack"));
        }

        if (LMIC.dataLen)
        {
            Serial.print(LMIC.dataLen);
            Serial.println(F(" bytes of payload"));
        }

        // Schedule next transmission
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), LoraWANDo_send);
        GO_DEEP_SLEEP = true;

        break;
    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        break;
    /* This event is defined but not used in the code.
        case EV_SCAN_FOUND:
            DisplayPrintln(F("EV_SCAN_FOUND"), LORAWAN_STATE_DISPLAY_LINE);
            break;
        */
    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        break;
    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;
    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

void LoraWANDo(void)
{
    static int loop_count = 0;
    long seconds = millis() / 1000;
    if (GO_DEEP_SLEEP == true && !os_queryTimeCriticalJobs(ms2osticksRound((LORA_TX_INTERVAL * 1000) - 1000)))
    {
        Serial.println(F("Go to DeepSleep ..."));
        Serial.print(F("Runtime was: "));
        Serial.print(seconds);
        Serial.println(F(" seconds"));
        LoraWANSaveLMICToRTC();
        Serial.flush();
        PowerDeepSleepTimer(LORA_TX_INTERVAL - 30 - 8); // 30sec for SDS011, 8 sec for remaining code 
    }
    else
    {
        if(loop_count % 10000 == 0) { Serial.println("Cannot Sleep"); }
        if(loop_count % 50000 == 0) 
        { 
            Serial.print("Runtime: ");
            Serial.print(seconds);
            Serial.println(" seconds");
        }

        os_runloop_once();
    }
    loop_count ++;
}

/*
    Byte 1: VCC
    Byte 2: PM25
    Byte 3: PM25
    Byte 4: PM10
    Byte 5: PM10
    Byte 6: Temperature
    Byte 7: Temperature
    Byte 8: Humidity
    Byte 9: Pressure
    Byte 10: Pressure
    Byte 11: Pressure (first Bit), the remaining not used
*/
void LoraWANGetData()
{
    uint8_t tmp_u8;
    uint16_t tmp_u16;
    uint32_t tmp_u32;
    float tmp_float;

    // VCC
    /**************************************************************************/
    tmp_u8 = (ReadVBat() / 10) - 200;
    LORA_DATA[0] = tmp_u8;

    // PM10
    /**************************************************************************/
    tmp_u16 = (PM25 * 10);
    if (isnan(PM25))
    {
        LORA_DATA[1] = 255;
        LORA_DATA[2] = 255;
    }
    else
    {
        LORA_DATA[1] = tmp_u16 >> 8;
        LORA_DATA[2] = tmp_u16 & 0xFF;
    }

    // PM10
    /**************************************************************************/
    tmp_u16 = (PM10 * 10);
    if (isnan(PM10))
    {
        LORA_DATA[3] = 255;
        LORA_DATA[4] = 255;
    }
    else
    {
        LORA_DATA[3] = tmp_u16 >> 8;
        LORA_DATA[4] = tmp_u16 & 0xFF;
    }

    // Temperature
    /**************************************************************************/
    tmp_u16 = (BME280ReadTemperature() * 10);
    LORA_DATA[5] = tmp_u16 >> 8;
    ;
    LORA_DATA[6] = tmp_u16 & 0xFF;

    // Humidity
    /**************************************************************************/
    tmp_float = BME280ReadHumidity();
    tmp_u8 = tmp_float;
    LORA_DATA[7] = tmp_u8;
    // Bit 8 for decimal 1 = 0.5
    if ((tmp_float - tmp_u8) > 0.251 && (tmp_float - tmp_u8) < 0.751)
    {
        LORA_DATA[7] |= (1 << 7);
    }
    else if ((tmp_float - tmp_u8) > 0.751)
    {
        LORA_DATA[7] = LORA_DATA[7] + 1;
    }

    // Pressure
    /**************************************************************************/
    tmp_float = BME280ReadPressure();
    tmp_u32 = (tmp_float * 100);
    LORA_DATA[8] = (tmp_u32 >> (8 * 0)) & 0xff;
    LORA_DATA[9] = (tmp_u32 >> (8 * 1)) & 0xff;
    LORA_DATA[10] = (tmp_u32 >> (8 * 2)) & 0xff;
}

void LoraWANSaveLMICToRTC()
{
    Serial.println(F("Save LMIC to RTC ..."));
    RTC_LORAWAN_netid = LMIC.netid;
    RTC_LORAWAN_devaddr = LMIC.devaddr;
    memcpy(RTC_LORAWAN_nwkKey, LMIC.nwkKey, 16);
    memcpy(RTC_LORAWAN_artKey, LMIC.artKey, 16);
    RTC_LORAWAN_dn2Dr = LMIC.dn2Dr;
    RTC_LORAWAN_dnConf = LMIC.dnConf;
    RTC_LORAWAN_seqnoDn = LMIC.seqnoDn;
    RTC_LORAWAN_seqnoUp = LMIC.seqnoUp;
    RTC_LORAWAN_adrTxPow = LMIC.adrTxPow;
    RTC_LORAWAN_datarate = LMIC.datarate;
    RTC_LORAWAN_txChnl = LMIC.txChnl;
    RTC_LORAWAN_adrAckReq = LMIC.adrAckReq;
    RTC_LORAWAN_rx1DrOffset = LMIC.rx1DrOffset;
    RTC_LORAWAN_rxDelay = LMIC.rxDelay;
    RTC_LORAWAN_opmode = LMIC.opmode;
    memcpy(RTC_LORAWAN_channelFreq, LMIC.channelFreq, MAX_CHANNELS * sizeof(u4_t));
    memcpy(RTC_LORAWAN_channelDrMap, LMIC.channelDrMap, MAX_CHANNELS * sizeof(u2_t));
    memcpy(RTC_LORAWAN_channelDlFreq, LMIC.channelDlFreq, MAX_CHANNELS * sizeof(u4_t));
    memcpy(RTC_LORAWAN_bands, LMIC.bands, MAX_BANDS * sizeof(band_t));
    RTC_LORAWAN_channelMap = LMIC.channelMap;
}

void LoraWANLoadLMICFromRTC()
{
    Serial.println(F("Load LMIC vars from RTC ..."));
    LMIC_setSession(RTC_LORAWAN_netid, RTC_LORAWAN_devaddr, RTC_LORAWAN_nwkKey, RTC_LORAWAN_artKey);
    LMIC.dn2Dr = RTC_LORAWAN_dn2Dr;
    LMIC.dnConf = RTC_LORAWAN_dnConf;
    LMIC.seqnoDn = RTC_LORAWAN_seqnoDn;
    LMIC_setSeqnoUp(RTC_LORAWAN_seqnoUp);
    LMIC_setDrTxpow(RTC_LORAWAN_datarate, RTC_LORAWAN_adrTxPow);
    LMIC.txChnl = RTC_LORAWAN_txChnl;
    LMIC.adrAckReq = RTC_LORAWAN_adrAckReq;
    LMIC.rx1DrOffset = RTC_LORAWAN_rx1DrOffset;
    LMIC.rxDelay = RTC_LORAWAN_rxDelay;
    LMIC.opmode = RTC_LORAWAN_opmode;
    memcpy(LMIC.channelFreq, RTC_LORAWAN_channelFreq, MAX_CHANNELS * sizeof(u4_t));
    memcpy(LMIC.channelDrMap, RTC_LORAWAN_channelDrMap, MAX_CHANNELS * sizeof(u2_t));
    memcpy(LMIC.channelDlFreq, RTC_LORAWAN_channelDlFreq, MAX_CHANNELS * sizeof(u4_t));
    memcpy(LMIC.bands, RTC_LORAWAN_bands, MAX_BANDS * sizeof(band_t));
    LMIC.channelMap = RTC_LORAWAN_channelMap;
}

void LoraWANPrintVersion(void)
{
    Serial.print(F("LMIC Version: "));
    Serial.print(ARDUINO_LMIC_VERSION_GET_MAJOR (ARDUINO_LMIC_VERSION) );
    Serial.print(F("."));
    Serial.print(ARDUINO_LMIC_VERSION_GET_MINOR (ARDUINO_LMIC_VERSION) );
    Serial.print(F("."));
    Serial.print(ARDUINO_LMIC_VERSION_GET_PATCH (ARDUINO_LMIC_VERSION) );
    Serial.print(F("."));
    Serial.println(ARDUINO_LMIC_VERSION_GET_LOCAL (ARDUINO_LMIC_VERSION) );  
}

// opmode def 
// https://github.com/mcci-catena/arduino-lmic/blob/89c28c5888338f8fc851851bb64968f2a493462f/src/lmic/lmic.h#L233
void LoraWANPrintLMICOpmode(void)
{
    Serial.print(F("LMIC.opmode: "));
    if (LMIC.opmode & OP_NONE) { Serial.print(F("OP_NONE ")); }
    if (LMIC.opmode & OP_SCAN) { Serial.print(F("OP_SCAN ")); }
    if (LMIC.opmode & OP_TRACK) { Serial.print(F("OP_TRACK ")); }
    if (LMIC.opmode & OP_JOINING) { Serial.print(F("OP_JOINING ")); }
    if (LMIC.opmode & OP_TXDATA) { Serial.print(F("OP_TXDATA ")); }
    if (LMIC.opmode & OP_POLL) { Serial.print(F("OP_POLL ")); }
    if (LMIC.opmode & OP_REJOIN) { Serial.print(F("OP_REJOIN ")); }
    if (LMIC.opmode & OP_SHUTDOWN) { Serial.print(F("OP_SHUTDOWN ")); }
    if (LMIC.opmode & OP_TXRXPEND) { Serial.print(F("OP_TXRXPEND ")); }
    if (LMIC.opmode & OP_RNDTX) { Serial.print(F("OP_RNDTX ")); }
    if (LMIC.opmode & OP_PINGINI) { Serial.print(F("OP_PINGINI ")); }
    if (LMIC.opmode & OP_PINGABLE) { Serial.print(F("OP_PINGABLE ")); }
    if (LMIC.opmode & OP_NEXTCHNL) { Serial.print(F("OP_NEXTCHNL ")); }
    if (LMIC.opmode & OP_LINKDEAD) { Serial.print(F("OP_LINKDEAD ")); }
    if (LMIC.opmode & OP_LINKDEAD) { Serial.print(F("OP_LINKDEAD ")); }
    if (LMIC.opmode & OP_TESTMODE) { Serial.print(F("OP_TESTMODE ")); }
    if (LMIC.opmode & OP_UNJOIN) { Serial.print(F("OP_UNJOIN ")); }
    Serial.println("");
}

void LoraWANDebug(void)
{
    LoraWANPrintLMICOpmode();
    Serial.print("LMIC.globalDutyRate = "); Serial.println( LMIC.globalDutyRate );
    Serial.print("LMIC.globalDutyAvail = "); Serial.println( LMIC.globalDutyAvail );
    Serial.print("LMICbandplan_nextTx = "); Serial.println( LMICbandplan_nextTx( os_getTime() ) );
    Serial.print("os_getTime = "); Serial.println(  os_getTime() );
    Serial.print("LMIC.txend = "); Serial.println( LMIC.txend );
    for (u1_t bi = 0; bi<MAX_BANDS; bi++) 
    {
        Serial.print("Band ");
        Serial.print( bi );
        Serial.print( " " );
        Serial.println( LMIC.bands[bi].avail );            
    }
}