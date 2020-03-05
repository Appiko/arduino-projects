#ifndef GSM_OPER_H_
#define GSM_OPER_H_
#include "stdint.h"

#define GATEWAY_ID_LEN (3)
#define GATEWAY_INFO_LEN (2) 
#define GSM_PAYLOAD_HEADER_SIZE (2)

class gsm_oper
{
    private : 
        uint8_t proj_id;
        uint16_t dev_id;
        uint8_t payload[(GSM_PACKET_MAX_LEN
         - GSM_PAYLOAD_HEADER_SIZE 
         - GATEWAY_ID_LEN - GATEWAY_INFO_LEN)];
        uint8_t pkt[GSM_PACKET_MAX_LEN];
        uint8_t * current_loc;
        bool gprs_available = true;

        uint8_t batt_volt;
        uint8_t supply_volt;

        typedef enum
        {
            PKT_NODE = 1,
            PKT_GATEWAY = 2
        }packet_types_t;

        void gsm_sleep ()
        {
            //GSM Sleep

        }
        void gsm_wakeup ()
        {
            //GSM wakeup
        }
        bool gsm_restart ();
    public : 

        gsm_oper ();

        bool gsm_init ();

        void gsm_append_payload (uint8_t len, uint8_t * p_payload);

        uint8_t gsm_available_mem ();

        bool gsm_is_mem_available (uint8_t len = 9);

        bool gsm_is_payload_present ();
        
        void gsm_send_pkt ();

        void gsm_update_batt_volt (uint8_t volt);

        void gsm_update_supply_volt (uint8_t volt);

        bool gsm_varify_connection ();

};
#endif //GSM_OPER_H_
