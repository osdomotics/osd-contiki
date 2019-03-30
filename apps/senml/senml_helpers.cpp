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
 * felper functions
 */

#include <senml_helpers.h>

#ifdef ESP32
    #include <base64.h>
    #include <arduino.h>                        //needed for sprintf
#elif __MBED__
    #include <base64.h>
    int base64_enc_len(int plainLen) {
	    int n = plainLen;
	    return (n + 2 - ((n + 2) % 3)) / 3 * 4;
    }
#else
    #include <Base64.h>
#endif



//global reference to the stream and stream configuration. This should save us memory
//by not having to pass these values continuously on the stack.
StreamContext* _streamCtx = NULL;

void printDouble(double f, unsigned int digits) 
{
    #ifdef __MBED__
        char s[30];
        sprintf(s, "%f", f);
        printText(s, strlen(s));
    #else
        String temp(f, digits);
        const char* s = temp.c_str();
        int length = temp.length();

        for(int i = length -1; i >0; i--){                              //remove unwanted trailing 0
            if(s[i] != '0'){
                length = i;
                if(s[i] == '.')                                         //if we end on something like x.  then add a last 0, so that it becomes x.0
                    length++;
                break;
            }
        }
        printText(s, length + 1);
    #endif
} 



void printBinaryAsBase64(const unsigned char* data, unsigned int length)
{
    #ifdef ESP32
        String encoded = base64::encode((uint8_t*)data, length);
        printText(encoded.c_str(), encoded.length());
    #else
        int encodedLen = base64_enc_len(length);
        char encoded[encodedLen];
        
        #ifdef __MBED__
            // todo: check result of function
            size_t olen;
            mbedtls_base64_encode((unsigned char*)encoded, encodedLen, &olen, data, length);
        #else
            // note input is consumed in this step: it will be empty afterwards
            base64_encode(encoded, (char*)data, length); 
        #endif
        printText(encoded, encodedLen);
    #endif
}

void printUnit(SenMLUnit unit)
{
    if(unit != SENML_UNIT_NONE)
        printText(senml_units_names[unit], strlen(senml_units_names[unit]));
}

void printText(const char* value, int len)
{
    char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    if(_streamCtx->dataAsBlob){
        if(_streamCtx->format == SENML_RAW){
            for(int i = 0; i < len; i++){
                if(_streamCtx->data.blob.curPos >= _streamCtx->data.blob.length) return;            //if we reached the end of the buffer, stop rendering otherwise we overwrite some other mem which is not good.
                _streamCtx->data.blob.data[_streamCtx->data.blob.curPos++] = value[i];
            }
        }
        else{
            for(int i = 0; i < len; i++){
                if(_streamCtx->data.blob.curPos >= _streamCtx->data.blob.length) return;            //if we reached the end of the buffer, stop rendering otherwise we overwrite some other mem which is not good.
                _streamCtx->data.blob.data[_streamCtx->data.blob.curPos++] = hexTable[value[i] / 16];
                _streamCtx->data.blob.data[_streamCtx->data.blob.curPos++] = hexTable[value[i] % 16];
            }
        }
    }
    else{
        if(_streamCtx->format == SENML_RAW){
            #ifdef __MBED__
                for(int i = 0; i < len; i++)
                    _streamCtx->data.stream->putc(value[i]);
            #else
                _streamCtx->data.stream->write(value, len);
            #endif
        }
        else if (_streamCtx->format == SENML_HEX){
            for(int i = 0; i< len; i++){
                #ifdef __MBED__
                    _streamCtx->data.stream->putc(hexTable[value[i] / 16]);
                    _streamCtx->data.stream->putc(hexTable[value[i] % 16]);
                #else
                    _streamCtx->data.stream->print(hexTable[value[i] / 16]);
                    _streamCtx->data.stream->print(hexTable[value[i] % 16]);
                #endif
            }
        }
    }
}

static bool peeked = false;                     //if we peek the stream in HEX format, we actually need to read 2 bytes, so the peek doesn't work, but we need to read the actual value.
static int peekVal = 0;                        //these values are removed by the compiler if no cbor is used.

int readChar(){
    if(peeked == true){
        peeked = false;
        return peekVal;
    }
    int res;
    if(_streamCtx->dataAsBlob){
		if(_streamCtx->data.blob.curPos < _streamCtx->data.blob.length)         //peekchar has to return -1 if there is no more data, so check for this.
			res = _streamCtx->data.blob.data[_streamCtx->data.blob.curPos++];
		else
			return -1;
        if(_streamCtx->format == SENML_HEX){
            int resB = _streamCtx->data.blob.data[_streamCtx->data.blob.curPos++];
            res = (res > '9')? (res &~ 0x20) - 'A' + 10: (res - '0');
	        resB = (resB > '9')? (resB &~ 0x20) - 'A' + 10: (resB - '0');
            res = (res * 16) + resB;
        }
	} 
	else {                                                                      //data is comming from the stream, this already returns -1 if there is no data.
        if(_streamCtx->format == SENML_RAW){
            #ifdef __MBED__
                res = _streamCtx->data.stream->getc();
            #else
                res = _streamCtx->data.stream->available() ? _streamCtx->data.stream->read() : -1;                          //arduino stream, check if something is available, if not, we can't read anymore.
            #endif
        }
        else{
            int resB;
            #ifdef __MBED__
                res = _streamCtx->data.stream->getc();
                resB = _streamCtx->data.stream->getc();
            #else
              
                if(_streamCtx->data.stream->available())
                    res = _streamCtx->data.stream->read();
                else
                    return -1;
                if(_streamCtx->data.stream->available())
                    resB = _streamCtx->data.stream->read();
                else
                    return -1;
            #endif
            res = (res > '9')? (res &~ 0x20) - 'A' + 10: (res - '0');
            resB = (resB > '9')? (resB &~ 0x20) - 'A' + 10: (resB - '0');
            res = (res * 16) + resB;
        }
	}
    return res;
};

int peekChar(){
    if(peeked == true)                                                              //if already peeked, return the currently buffered value.
        return peekVal;
    peekVal = readChar();
    if(peekVal != -1)                                                               //if no more data, don't try to buffer it, some new data might arrive later on
        peeked = true;
    return peekVal;
};

void flush(){
    peeked = false;
}


