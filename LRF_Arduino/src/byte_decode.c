#include "byte_decode.h"

#define START_FLAG 0x12
#define END_FLAG 0x13
#define ESCAPE_FLAG 0x7D

#define MAX_FRAME_SIZE 32
#define MAX_ENCODED_FRAME_SIZE (MAX_FRAME_SIZE*2+2)

typedef enum {WAIT_HEADER, IN_MSG, AFTER_ESCAPE} DecodeState;

typedef struct{
    DecodeState state;
    uint8_t decodedOutput[MAX_FRAME_SIZE];
    uint8_t *currentOutput;
}DecodeData;

DecodeData data = 
{
    .state = WAIT_HEADER,

};

void decodeByte(uint8_t bytesToDecode,
        void (*decode_done)(uint8_t * decoded_data,uint16_t len))
{
    /* Decode data start values */
    
    const uint8_t * currentInput = &bytesToDecode;


        if (data.state==WAIT_HEADER)
        {
            if (*currentInput==START_FLAG)
            {
                data.currentOutput = data.decodedOutput;
                data.state=IN_MSG;
            }
            else
            {
                data.state=WAIT_HEADER;
            }
        }
        else if (data.state==IN_MSG)
        {
            if (*currentInput==ESCAPE_FLAG)
            {
                data.state=AFTER_ESCAPE;
            }
            else if (*currentInput==END_FLAG)
            {
                (*decode_done) (data.decodedOutput, 
                    (data.currentOutput - data.decodedOutput));
                data.state=WAIT_HEADER;
                data.currentOutput = data.decodedOutput;
            }
            else if (*currentInput==START_FLAG)
            {
                /* Something wrong happened!, restarting.. */
                data.state=WAIT_HEADER;
                data.currentOutput = data.decodedOutput;
                /* Skip increment currentInput. Maybe currentInput is START_FLAG from next packet */
            }
            else
            {
                if (data.currentOutput-data.decodedOutput<MAX_FRAME_SIZE)
                {
                    data.state=IN_MSG;
                    *data.currentOutput = *currentInput;
                    data.currentOutput++;
                }
                else
                {
                    /* Something wrong happened!, restarting.. */
                    data.state=WAIT_HEADER;
                    data.currentOutput = data.decodedOutput;
                    /* Skip increment currentInput. Maybe currentInput is START_FLAG from next packet */
                }
            }
        }
        else if (data.state==AFTER_ESCAPE)
        {
            if (*currentInput==START_FLAG || *currentInput==END_FLAG || *currentInput==ESCAPE_FLAG)
            {
                if ((data.currentOutput-data.decodedOutput)<MAX_FRAME_SIZE)
                {
//                    printf("State:AFTER_ESCAPE, appending byte 0x%X to decoded frame\n",*currentInput);
                    data.state=IN_MSG;
                    *data.currentOutput = *currentInput;
                    data.currentOutput++;
                }
                else
                {
                    /* Something wrong happened!, restarting.. */
                    data.state=WAIT_HEADER;
                    data.currentOutput = data.decodedOutput;
                    /* Skip increment currentInput. Maybe currentInput is START_FLAG from next packet */
                }
            }
            else
            {
                /* Something wrong happened!, restarting.. */
                data.state=WAIT_HEADER;
                data.currentOutput = data.decodedOutput;
                /* Skip increment currentInput. Maybe currentInput is START_FLAG from next packet */
            }
        }
        else
        {
            data.state=WAIT_HEADER;
            data.currentOutput = data.decodedOutput;
        }

        /* Next input uint8_t */
    
}
