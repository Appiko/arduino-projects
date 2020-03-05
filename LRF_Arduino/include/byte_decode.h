#ifndef BYTE_DECODE_H_
#define BYTE_DECODE_H_
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
void decodeByte(uint8_t bytesToDecode,
        void (*decode_done)(uint8_t * decoded_data,uint16_t len));
#ifdef __cplusplus
}
#endif
#endif //BYTE_DECODE_H_
