#include <Arduino.h>
#include "main.h"
#include "CBUF.h"
#include "byte_decode.h"
#include "gsm_oper.h"
#include "stdint.h"
#include "ADC_Module.h"
#include <ADC.h>
#include <ADC_util.h>

#define S_to_MS(x) (x * 1000)

#define BAUDRATE_1M (1000000)

#define EMPTY_PKT_FREQ_S (300)
#define EMPTY_PKT_FREQ_MS (S_to_MS(EMPTY_PKT_FREQ_S))

#define CONN_CHK_FREQ_S (50)
#define CONN_CHK_FREQ_MS (S_to_MS(CONN_CHK_FREQ_S))

#define GSM_OBJ_COUNT (10)

gsm_oper g_gsm[GSM_OBJ_COUNT];

#define BATT_ADC_PIN 14

#define SUPPLY_ADC_PIN 22
class pkt_buff_t
{
    public:
        uint8_t len;
        uint8_t data[16];
};

CBUF<uint8_t, 32U, pkt_buff_t> g_cbuff;

void print_data(uint32_t line_no,uint8_t * p_data, uint16_t len)
{
    SerialMon.print("["+String(line_no)+"]:Data : ");
    for (size_t i = 0; i < len; i++)
    {
        SerialMon.print(p_data[i]);
        SerialMon.print(" ");
        /* code */
    }
    SerialMon.println();
    

}

void frame_decode_done (uint8_t * data, uint16_t len);

void frame_decode_done (uint8_t * data, uint16_t len)
{
    pkt_buff_t l_pkt;
    l_pkt.len = len;
    memcpy(l_pkt.data, data, len);
    g_cbuff.Push(l_pkt);
    print_data (__LINE__, data, len);

}


uint8_t batt_volt;
uint8_t supply_volt;

uint32_t empty_pkt_ms = 0;
uint32_t last_empty_pkt = 0;
uint32_t chk_conn_ms = 0;
uint32_t last_conn_chk = 0;


ADC *adc = new ADC();

/**TinyGSM**/
void setup() {
    // put your setup code here, to run once:
    SerialMon.begin(BAUDRATE_1M);
    SerialRF.begin (BAUDRATE_1M);
    delay (2500);
    SerialMon.println ("Hello from Gateway");
    //GSM module has to be initialized only once
    g_gsm[0].gsm_init();
    g_gsm[0].gsm_send_pkt();
    last_empty_pkt = millis();
    last_conn_chk = last_empty_pkt;
    delay (2500);
    // adc->adc0->setAveraging(16); // set number of averages
    // adc->adc0->setResolution(16); // set bits of resolution
    // adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
    // adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed}
}
uint8_t g_1Bbuff;

void loop() {

     //update millis
    // batt_volt =  adc->adc0->analogRead(BATT_ADC_PIN);
    // supply_volt =  adc->adc0->analogRead(SUPPLY_ADC_PIN);
    // delay (100);

    // SerialMon.println ("Batt : "+String(batt_volt)+ " Supply : "+String(supply_volt));
     empty_pkt_ms = millis ();
     chk_conn_ms = empty_pkt_ms;
     if ((chk_conn_ms - last_conn_chk) > CONN_CHK_FREQ_MS)
     {
         /* code */
         g_gsm[0].gsm_varify_connection ();
         last_conn_chk = millis();
     }
     
     if((empty_pkt_ms - last_empty_pkt) > EMPTY_PKT_FREQ_MS)
     {
            g_gsm[0].gsm_send_pkt();
            last_empty_pkt = empty_pkt_ms;
     }
    //update running millis
    // put your main code here, to run repeatedly:
    //check if data is available on RF UART
    if(SerialRF.available())
    {
        SerialMon.println("Data is available");
        //if yes, fetch all the data
        while (SerialRF.available())
        {
            SerialRF.readBytes(&g_1Bbuff, 1);
            SerialMon.print(g_1Bbuff, HEX);
            SerialMon.print(" ");
            decodeByte(g_1Bbuff, frame_decode_done);
        }
        SerialMon.println();
        
        //decode data and push in cbuff
    }

    //check if data is available in cbuff or idle timeout
    int8_t index = 0;
    while(g_cbuff.Len() > 0)
    {
        pkt_buff_t l_pkt; 

        //if data is available : pop and save it to GSM payload
        l_pkt = g_cbuff.Pop();
        bool data_present = true;
        do
        {
            /* code */
            if (g_gsm[index].gsm_is_mem_available(l_pkt.len))
            {
                /* code */
                SerialMon.println("Data append");
                g_gsm[index].gsm_append_payload(l_pkt.len, l_pkt.data);
                data_present = false;
                //append for gateway packet
                g_gsm[index].gsm_update_batt_volt(batt_volt);
                g_gsm[index].gsm_update_supply_volt(supply_volt);
            }
            else
            {
                index++;
            }
        } while (data_present && (index < GSM_OBJ_COUNT)); 
    }
    index = 0;
    while (index < GSM_OBJ_COUNT)
    {
        //send GSM packet
        if(g_gsm[index].gsm_is_payload_present())
        {
            g_gsm[index].gsm_send_pkt();
            last_empty_pkt = millis ();
        }
        index++;
    }
}


