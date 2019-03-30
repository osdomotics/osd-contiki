
/*  _  __  ____    _   _ 
 * | |/ / |  _ \  | \ | |
 * | ' /  | |_) | |  \| |
 * | . \  |  __/  | |\  |
 * |_|\_\ |_|     |_| \_|
 * 
 * (c) 2018 KPN
 * License: MIT License.
 * Author: Jan Bogaerts
 * 
 * support for binary actuators
 */

#include <senml_binary_actuator.h>
#include <senml_logging.h>

#ifdef ESP32
    extern "C" {
    #include "libb64/cdecode.h"
    }
    int base64_dec_len(char * input, int inputLen) {
        int i = 0;
        int numEq = 0;
        for(i = inputLen - 1; input[i] == '='; i--) {
            numEq++;
        }

        return ((6 * inputLen) / 8) - numEq;
    }
#elif __MBED__
    #include <base64.h>
    int base64_dec_len(char * input, int inputLen) {
        int i = 0;
        int numEq = 0;
        for(i = inputLen - 1; input[i] == '='; i--) {
            numEq++;
        }

        return ((6 * inputLen) / 8) - numEq;
    }
#else
    #include <Base64.h>
#endif

void SenMLBinaryActuator::actuate(const void* value, int dataLength, SenMLDataType dataType)
{
    if(dataType == SENML_TYPE_DATA){
        int decodedLen = base64_dec_len((char*)value, dataLength);
        char decoded[decodedLen];
        #ifdef ESP32
            base64_decode_chars((const char*)value, dataLength, decoded);
        #elif __MBED__
            // todo: check result of function
            size_t olen;
            mbedtls_base64_decode((unsigned char*)decoded, decodedLen, &olen, (const unsigned char*)value, dataLength);
        #else
            base64_decode(decoded, (char*)value, dataLength); 
        #endif 

        this->set((unsigned char*)decoded, decodedLen);
        if(this->callback)
            this->callback((unsigned char*)decoded, decodedLen);
    }
    else if(dataType == CBOR_TYPE_DATA){
        this->set((unsigned char*)value, dataLength);
        if(this->callback)
            this->callback((unsigned char*)value, dataLength);
    }
    else
        log_debug("invalid type");
}







