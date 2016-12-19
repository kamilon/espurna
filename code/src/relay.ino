/*

ESPurna
RELAY MODULE

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <EEPROM.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <vector>

std::vector<unsigned char> _relays;
bool recursive = false;
#ifdef SONOFF_DUAL
    unsigned char dualRelayStatus = 0;
#endif
Ticker inching;

// -----------------------------------------------------------------------------
// RELAY
// -----------------------------------------------------------------------------

void relayMQTT(unsigned char id) {
    char buffer[10];
    sprintf(buffer, MQTT_RELAY_TOPIC, id);
    mqttSend(buffer, (char *) (relayStatus(id) ? "1" : "0"));
}

void relayMQTT() {
    for (unsigned int i=0; i < _relays.size(); i++) {
        relayMQTT(i);
    }
}

String relayString() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& relay = root.createNestedArray("relayStatus");
    for (unsigned char i=0; i<relayCount(); i++) {
        relay.add(relayStatus(i));
    }
    String output;
    root.printTo(output);
    return output;
}

void relayWS() {
    String output = relayString();
    wsSend((char *) output.c_str());
}

bool relayStatus(unsigned char id) {
    #ifdef SONOFF_DUAL
        return ((dualRelayStatus & (1 << id)) > 0);
    #else
        return (digitalRead(_relays[id]) == HIGH);
    #endif
}

void relayInchingBack(unsigned char id) {
    relayToggle(id);
    inching.detach();
}

void relayInching(unsigned char id) {

    byte relayInch = getSetting("relayInch", String(RELAY_INCHING)).toInt();
    if (relayInch == RELAY_INCHING_NONE) return;

    bool status = relayStatus(id);
    if ((relayInch == RELAY_INCHING_ON) & (status)) return;
    if ((relayInch == RELAY_INCHING_OFF) & (!status)) return;

    inching.attach(
        getSetting("relayInchTime", String(RELAY_INCHING_TIME)).toInt(),
        relayInchingBack,
        id
    );

}

bool relayStatus(unsigned char id, bool status, bool report) {

    bool changed = false;

    if (relayStatus(id) != status) {

        DEBUG_MSG("[RELAY] %d => %s\n", id, status ? "ON" : "OFF");
        changed = true;

        #ifdef SONOFF_DUAL

            dualRelayStatus ^= (1 << id);
            Serial.flush();
            Serial.write(0xA0);
            Serial.write(0x04);
            Serial.write(dualRelayStatus);
            Serial.write(0xA1);
            Serial.flush();

        #else
            digitalWrite(_relays[id], status);
        #endif

        if (!recursive) {
            relayInching(id);
            relaySync(id);
            relaySave();
        }

    }

    if (report) relayMQTT(id);
    if (!recursive) relayWS();
    return changed;
    
}

void relaySync(unsigned char id) {

    if (_relays.size() > 1) {

        recursive = true;

        byte relaySync = getSetting("relaySync", String(RELAY_SYNC)).toInt();
        bool status = relayStatus(id);

        // If RELAY_SYNC_SAME all relays should have the same state
        if (relaySync == RELAY_SYNC_SAME) {
            for (unsigned short i=0; i<_relays.size(); i++) {
                if (i != id) relayStatus(i, status);
            }

        // If NONE_OR_ONE or ONE and setting ON we should set OFF all the others
        } else if (status) {
            if (relaySync != RELAY_SYNC_ANY) {
                for (unsigned short i=0; i<_relays.size(); i++) {
                    if (i != id) relayStatus(i, false);
                }
            }

        // If ONLY_ONE and setting OFF we should set ON the other one
        } else {
            if (relaySync == RELAY_SYNC_ONE) {
                unsigned char i = (id + 1) % _relays.size();
                relayStatus(i, true);
            }
        }

        recursive = false;

    }

}

void relaySave() {
    unsigned char bit = 1;
    unsigned char mask = 0;
    for (unsigned int i=0; i < _relays.size(); i++) {
        if (relayStatus(i)) mask += bit;
        bit += bit;
    }
    EEPROM.write(0, mask);
    EEPROM.commit();
}

void relayRetrieve() {
    recursive = true;
    unsigned char bit = 1;
    unsigned char mask = EEPROM.read(0);
    for (unsigned int i=0; i < _relays.size(); i++) {
        relayStatus(i, ((mask & bit) == bit));
        bit += bit;
    }
    recursive = false;
}

void relayToggle(unsigned char id) {
    relayStatus(id, !relayStatus(id));
}

unsigned char relayCount() {
    return _relays.size();
}

void relayMQTTCallback(unsigned int type, const char * topic, const char * payload) {

    static bool isFirstMessage = true;

    if (type == MQTT_CONNECT_EVENT) {
        relayMQTT();
        mqttSubscribe("/relay/#");
    }

    if (type == MQTT_MESSAGE_EVENT) {

        // Match topic
        if (memcmp("/relay/", topic, 7) != 0) return;

        // If relayMode is not SAME avoid responding to a retained message
        if (isFirstMessage) {
            isFirstMessage = false;
            byte relayMode = getSetting("relayMode", String(RELAY_MODE)).toInt();
            if (relayMode != RELAY_MODE_SAME) return;
        }

        // Get relay ID
        unsigned int relayID = topic[strlen(topic)-1] - '0';
        if (relayID >= relayCount()) relayID = 0;

        // Action to perform
        if ((char)payload[0] == '0') {
            relayStatus(relayID, false, false);
        }
        if ((char)payload[0] == '1') {
            relayStatus(relayID, true, false);
        }
        if ((char)payload[0] == '2') {
            relayToggle(relayID);
        }

    }

}

void relaySetup() {

    #ifdef SONOFF_DUAL

        // Two dummy relays for the dual
        _relays.push_back(0);
        _relays.push_back(0);

    #else

        #ifdef RELAY1_PIN
            _relays.push_back(RELAY1_PIN);
        #endif
        #ifdef RELAY2_PIN
            _relays.push_back(RELAY2_PIN);
        #endif
        #ifdef RELAY3_PIN
            _relays.push_back(RELAY3_PIN);
        #endif
        #ifdef RELAY4_PIN
            _relays.push_back(RELAY4_PIN);
        #endif

    #endif

    EEPROM.begin(4096);
    byte relayMode = getSetting("relayMode", String(RELAY_MODE)).toInt();

    for (unsigned int i=0; i < _relays.size(); i++) {
        pinMode(_relays[i], OUTPUT);
        if (relayMode == RELAY_MODE_OFF) relayStatus(i, false);
        if (relayMode == RELAY_MODE_ON) relayStatus(i, true);
    }

    if (relayMode == RELAY_MODE_SAME) relayRetrieve();

    mqttRegister(relayMQTTCallback);

    DEBUG_MSG("[RELAY] Number of relays: %d\n", _relays.size());

}
