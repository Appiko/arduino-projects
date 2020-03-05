
#include <Arduino.h>

#include "main.h"

#include "gsm_oper.h"
#include "sim800_config.h"

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

#define PING_ATTEMPTS 3

TinyGsm modem(SerialAT);

TinyGsmClient client(modem);
HttpClient http(client, server, port);

gsm_oper::gsm_oper ()
{
    proj_id = GATEWAY_PROJ_ID;
    dev_id = GATEWAY_DEV_ID;
    current_loc = payload;
}

void gsm_oper::gsm_append_payload (uint8_t len, uint8_t * p_payload)
{
    *current_loc = PKT_NODE;
    current_loc++;
    *current_loc = len;
    current_loc++;
    memcpy(current_loc, p_payload, len);
    
    current_loc = current_loc+len;
}

uint8_t gsm_oper::gsm_available_mem ()
{
    return ((payload + sizeof(payload) - current_loc));
}


bool gsm_oper::gsm_is_mem_available (uint8_t len)
{
    return ((sizeof(payload) > (current_loc + (len + GSM_PAYLOAD_HEADER_SIZE)- payload)) ? true : false);
}

bool gsm_oper::gsm_is_payload_present ()
{
    return (payload !=current_loc)? true : false;
}

bool gsm_oper::gsm_init ()
{
    bool retval = 0;
    //Enable GSM Module
    SerialAT.begin(115200);
    delay(5000);
    modem.restart();
    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem: ");
    SerialMon.println(modemInfo);
    SerialMon.println("Waiting for network...");
    uint8_t cnt = 0;
    while ((cnt < 5) &&(!modem.waitForNetwork(3000))) {
        cnt++;
        SerialMon.println(" fail");
        if(cnt == 5)
        {
          return retval;
        }
    }
    SerialMon.println(" OK");
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      SerialMon.println(" fail");
      delay(1000);
      return retval;
    }
    else
    {
        SerialMon.println("OK");
    }
}

bool gsm_oper::gsm_restart ()
{
    modem.restart();
    modem.waitForNetwork(15000);
    modem.gprsConnect(apn, gprsUser, gprsPass);        
}

bool gsm_oper::gsm_varify_connection ()
{
    //fucntion to check connection and reconnect if disconnected
    uint32_t l_attempts = 0;
    bool conn_status = true;
    while ((http.connect(server, port) == false) && l_attempts < PING_ATTEMPTS)
    {
        /* code */
        SerialMon.println ("Trying to reconnect..!");
        modem.waitForNetwork(12000);
        modem.gprsConnect(apn, gprsUser, gprsPass);
        conn_status = http.connect(server, port);
        l_attempts++;        
    }
    return conn_status;
    
}

void gsm_oper::gsm_send_pkt ()
{
    SerialMon.println("SGP");
    if(gprs_available)
    {
        gprs_available = false;
        gsm_wakeup ();
        uint8_t * l_pkt = pkt;
        uint32_t l_total_len = 0;
        l_total_len = current_loc - payload;
        *l_pkt = proj_id;
        l_pkt++;
        *((uint16_t * )l_pkt) = dev_id;
        l_pkt += 2;
        memcpy (l_pkt, payload, l_total_len);
        l_pkt += l_total_len;
        *l_pkt = PKT_GATEWAY;
        l_pkt++;
        *l_pkt = 2;
        l_pkt++;
        *l_pkt = batt_volt;
        l_pkt++;
        *l_pkt = supply_volt;
        l_pkt++;
        l_total_len = l_pkt - pkt;
        if(!modem.isGprsConnected());
        {
        }
        
        if(http.connect(server, port))
        {
            SerialMon.println ("HTTP Len : "+String(l_total_len));

            http.print(String("POST ") + resource + " HTTP/1.1\r\n" +
        "Host: " + server + "\r\n" +
        "Connection: close\r\n"+   
        "Content-Length: " + String(l_total_len) + "\r\n" +
        "Content-Type: application/octet-stream\r\n\r\n");
            http.write(pkt, l_total_len);
            http.print("\r\n");
            int status = http.responseStatusCode();
            SerialMon.println ("Status code " +String(status));
            if (status == 200)
            {
                current_loc = payload;
            }
        }
        else
        {
            /* code */
        
            SerialMon.println ("GPRS Dissconnected");
            this->gsm_restart();
        }
        if((current_loc - payload) > 1020)
        {
            current_loc = payload;
        }

        gprs_available = true;
    }

}


void gsm_oper::gsm_update_batt_volt (uint8_t volt)
{
    batt_volt = volt;
}

void gsm_oper::gsm_update_supply_volt (uint8_t volt)
{
    supply_volt = volt;
}

