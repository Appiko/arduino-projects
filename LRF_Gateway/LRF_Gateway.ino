#include <Arduino.h>
#include <SPI.h>
#include "rf_comm.h"
#include "CBUF.h"
#include "ms_timer.h"
#include "nrf_gpiote.h"
// #include <stdint.h>


#define WAKEUP_FREQ_MS 1250
#define LRF_PKT_CHK_FREQ 8000

#define GATEWAY_ALIVE_FREQ (10 * 60 * 1000)

/**TinyGSM**/
#define TINY_GSM_MODEM_SIM800

#define TINY_GSM_RX_BUFFER 650

#define SerialAT Serial

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

#define GSM_PIN ""
#define CC1125_GPIO0   5
#define CC1125_GPIO1   
#define CC1125_GPIO2   6
#define CC1125_GPIO3   12
#define CC1125_RESET   11

#define CC1190_HGM     4
#define CC1190_PA_EN   2  
#define CC1190_LNA_EN  3

const char apn[]  = "bsnlnet";
const char gprsUser[] = "";
const char gprsPass[] = "";
const char wifiSSID[]  = "YourSSID";
const char wifiPass[] = "SSIDpw";

// Server details
const char server[] = "139.59.67.180";
const char resource[] = "/";
const int  port = 3335;


#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// #include <AppikoMsTimer.h>
TinyGsm modem(SerialAT);

TinyGsmClient client(modem);
HttpClient http(client, server, port);


/**End**/
// #ifdef SOFTSERIAL_DEBUG
#include <SoftwareSerial.h>
<<<<<<< HEAD
SoftwareSerial SerialMon (8, 6);
=======
SoftwareSerial SerialMon (22, 20);
>>>>>>> 55ba45aa37d02593b50884052ae70d44f74b8b4f
// #define SerialMon.begin (x)   SerialMon.begin((x))
// #define SerialMon.print (...) SerialMon.print (__VA_ARGS__)
// #define SerialMon.println (...) SerialMon.println (__VA_ARGS__)
// #elif
// #define SerialMon.begin (x)   
// #define SerialMon.print (...) 
// #define SerialMon.println (...) 
// #endif

class pkt_buff_t
{
  public:
    uint8_t len;
    uint8_t data[16];
    int8_t rssi;
};

typedef enum
{
    PKT_NODE = 1,
    PKT_GATEWAY = 2
}packet_types_t;

CBUF<uint8_t, 32U, pkt_buff_t> g_cbuff;

class gateway_pkt
{
    private : 
        uint8_t proj_id;
        uint16_t dev_id;
        uint8_t payload[512];
        uint8_t pkt[515];
        uint8_t * current_loc;
        bool gprs_available = true;

        void gsm_sleep ()
        {
            //GSM Sleep

        }
        void gsm_wakeup ()
        {
            //GSM wakeup
        }
    public : 
        gateway_pkt ()
        {
            proj_id = *((uint8_t * )(0x10001090));
            dev_id = *((uint16_t * )(0x10001091));
            current_loc = payload;
        }

        void append_payload (packet_types_t pkt_type,
         uint8_t len, uint8_t * p_payload)
        {
            *current_loc = pkt_type;
            current_loc++;
            *current_loc = len;
            current_loc++;
            memcpy(current_loc, p_payload, len);
            
            current_loc = current_loc+len;
        }

        void send_gsm_pkt ()
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
                    modem.waitForNetwork(12000);
                    modem.gprsConnect(apn, gprsUser, gprsPass);
                }
                if((current_loc - payload) > 1020)
                {
                    current_loc = payload;
                }

                gprs_available = true;
            }

        }

};
gateway_pkt g_gateway_pkt;



SPISettings spiSet(2000000, MSBFIRST, SPI_MODE0);
const uint8_t ss_pin = SS;
rf_comm_radio_t l_radio_params;
rf_comm_hw_t l_radio_hw;
uint32_t g_irq_evt;

typedef enum
{
    LRF_STATE_NONE    = 0,
    LRF_STATE_IDLE    = 1, 
    LRF_STATE_PKT_CHK = 2, 
    LRF_STATE_COMM    = 3, 
}lrf_states_t;

void lrf_state_none ();
void lrf_state_idle ();
void lrf_state_pkt_chk ();
void lrf_state_comm ();

void (* arr_lrf_state[]) () = 
{
    lrf_state_none,
    lrf_state_idle,
    lrf_state_pkt_chk,
    lrf_state_comm
};

volatile lrf_states_t g_current_state = LRF_STATE_IDLE;

lrf_states_t *gp_current_state = &g_current_state;


void state_assign (lrf_states_t state, uint32_t line_no)
{
    SerialMon.print(line_no);
    SerialMon.print (", ");
    SerialMon.println(state);
    memcpy(gp_current_state, &state, sizeof(lrf_states_t));

}

uint8_t g_gsm_pkt[512];
uint32_t g_current_gsm_pkt_loc;

void radio_receive (uint32_t line_no)
{
    // SerialMon.println(line_no);
    rf_comm_wake ();

    // rf_comm_idle ();
    rf_comm_rx_enable();
    // SerialMon.println();
}

void radio_packet_received (uint32_t status)
{
    SerialMon.println("PR "+String(status));
    //get received packet in local memory
    pkt_buff_t pkt;
    rf_comm_pkt_receive (pkt.data, &pkt.len);
    pkt.rssi = rf_comm_get_rssi();
    SerialMon.print ("Data : ");
    for(uint32_t i = 0; i < pkt.len; i++)
    {
        SerialMon.print(pkt.data[i]);
        SerialMon.print(" ");
    }
    SerialMon.println();

    //push radio packet in circular buffer
    g_cbuff.Push (pkt);

    //restart radio reception
    radio_receive (__LINE__);
}

void radio_packet_drop (uint32_t status)
{
    SerialMon.println("Dropped!");
    //restart radio reception
    radio_receive (__LINE__);
}

void generate_gateway_pkt ()
{
    uint8_t l_gsm_payload[17];
    //type = gateway_pkt
    l_gsm_payload[0] = 0;
    l_gsm_payload[1] = 0;
    g_gateway_pkt.append_payload(PKT_GATEWAY, 2, l_gsm_payload);
    //for no of pkt in circular buffer
    static pkt_buff_t l_pkt_buff;
    while (g_cbuff.IsEmpty() == false)
    {
        /* code */
        //pop from circular buffer
        l_pkt_buff = g_cbuff.Pop();
        //payload =  RSSI + poped pkt
        l_gsm_payload[0] = l_pkt_buff.rssi;
        memcpy(&l_gsm_payload[1], l_pkt_buff.data, l_pkt_buff.len);
        //type = node_pkt
        //len = pkt.len
        g_gateway_pkt.append_payload(PKT_NODE, (l_pkt_buff.len+1),
            l_gsm_payload);
    }
}

bool gsm_init ()
{
    bool retval = 0;
    //Enable GSM Module
    SerialAT.begin(115200);
    delay(1000);
    modem.restart();
    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem: ");
    SerialMon.println(modemInfo);
    SerialMon.print("Waiting for network...");
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


void lrf_state_none ()
{
    return;
}

void lrf_state_idle ()
{
    //Do nothing
    SerialMon.println("IDLE");
    //  radio_receive();
    return;
}

void lrf_state_pkt_chk ()
{
    //check if any RF pkts are present
    //pkt present 
        //Reset GSM pkt done flag
        //generate GSM pkt
        //switch to comm state (GSM)
    generate_gateway_pkt ();
    state_assign (LRF_STATE_COMM, __LINE__);
    SerialMon.println("PKT");
    
}
void lrf_state_comm ()
{
    //switch on GSM
    //send GSM pkt
    //check status
    //if success switch to LRF_IDLE
    //else go back to packet check

    g_gateway_pkt.send_gsm_pkt();
    // radio_receive();
    state_assign (LRF_STATE_IDLE, __LINE__);
    SerialMon.println("COMM");

}

void led_bootup ()
{
    pinMode(17, OUTPUT);
    digitalWrite(17, LOW);
    delay (250);
    digitalWrite(17, HIGH);
    delay (250);
    digitalWrite(17, HIGH);
    delay (250);
    digitalWrite(17, LOW);
    delay (250);
    digitalWrite(17, HIGH);
}
uint32_t g_last_check;

void ms_timer_handler ()
{
    uint32_t l_state = rf_com_get_state();
    SerialMon.println ("S "+String (l_state));
    g_last_check += WAKEUP_FREQ_MS;
    if(g_last_check > LRF_PKT_CHK_FREQ)
    {
        state_assign (LRF_STATE_PKT_CHK, __LINE__);
        g_last_check = 0;
        if(l_state == 0)
        {
            radio_receive(__LINE__);
        }

    }
    SerialMon.println(g_current_state);
    arr_lrf_state[g_current_state]();

}

void setup()
{
    SerialMon.begin(115200);
    delay(10);
    ms_timer_init (6);
    SerialMon.println("Hello world from Gateway");
    led_bootup ();
    //Init radio
    l_radio_params.bitrate = 300;
    l_radio_params.center_freq = 866000;
    l_radio_params.freq_dev = 2;
    l_radio_params.rx_bandwidth = 8;
    l_radio_params.rf_rx_done_handler = radio_packet_received;
    l_radio_params.rf_rx_failed_handler = radio_packet_drop;

    l_radio_hw.rf_gpio0_pin = CC1125_GPIO0;
    l_radio_hw.rf_gpio2_pin = CC1125_GPIO2;
    l_radio_hw.rf_gpio3_pin = CC1125_GPIO3;
    l_radio_hw.rf_reset_pin = CC1125_RESET;
    pinMode(ss_pin,OUTPUT);

    pinMode(CC1190_PA_EN, OUTPUT);
    pinMode(CC1190_LNA_EN, OUTPUT);
    pinMode(CC1190_HGM, OUTPUT);

    digitalWrite(CC1190_PA_EN, 0);
    digitalWrite(CC1190_LNA_EN, 1);
    digitalWrite(CC1190_HGM, 1);
    
    SPI.begin();
    // SPI.beginTransaction (spiSet);

    NRF_SPIM_Type * l_spi = SPI.get_spi_ptr ();
    uint32_t s_addr = (uint32_t)l_spi;
    uint32_t e_addr = (uint32_t)&l_spi->EVENTS_END;
    uint32_t e_val = (uint32_t) l_spi->EVENTS_END;
    uint32_t t_addr = (uint32_t)&l_spi->TASKS_START;
    uint32_t t_val = (uint32_t) l_spi->TASKS_START;
    uint32_t con = (uint32_t)l_spi->CONFIG;
    uint32_t freq = (uint32_t)l_spi->FREQUENCY;
    uint32_t miso = (uint32_t) l_spi->PSEL.MISO;
    uint32_t mosi = (uint32_t) l_spi->PSEL.MOSI;
    uint32_t sck  = (uint32_t) l_spi->PSEL.SCK;
    SerialMon.println("S "+String(s_addr));
    SerialMon.println("E "+String(e_addr)+ "   "+String (e_val));
    SerialMon.println("T "+String(t_addr)+ "   "+String(t_val));
    SerialMon.println("C "+String(con));
    SerialMon.println("F "+String(freq));
    SerialMon.println("MISO "+String(miso));
    SerialMon.println("MOSI "+String(mosi));
    SerialMon.println("SCK "+String(sck));

    SerialMon.println ("Radio ID : "+ String(rf_comm_get_radio_id()));
    g_irq_evt = rf_comm_radio_init (&l_radio_params, &l_radio_hw);
    gsm_init();
    state_assign (LRF_STATE_IDLE, __LINE__);
    //check gsm : send activated signal
    ms_timer_start (MS_TIMER0, MS_REPEATED_CALL, WAKEUP_FREQ_MS, ms_timer_handler);
    radio_receive (__LINE__);

    //Start Radio 
    
}


void loop()
{
    // SerialMon.println ("No Pkt "+String(g_cbuff.Len()));
    {
        // __WFI ();
    }

}
