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
 * pack (document) with int base values
 */


#include <senml_int_pack.h> 
#include <senml_helpers.h>
#include <cbor.h>
 

void SenMLIntPack::setupStreamCtx(Stream *dest, SenMLStreamMethod format)
{
    SenMLPack::setupStreamCtx(dest, format);
    _streamCtx->baseValue.baseInt = this->getBaseValue();
    _streamCtx->baseSum.baseInt = this->getBaseValue();
    _streamCtx->baseDataType = CBOR_TYPE_INT;
}

void SenMLIntPack::setupStreamCtx(char *dest, int length, SenMLStreamMethod format)
{
    SenMLPack::setupStreamCtx(dest, length, format);
    _streamCtx->baseValue.baseInt = this->getBaseValue();
    _streamCtx->baseSum.baseInt = this->getBaseValue();
    _streamCtx->baseDataType = CBOR_TYPE_INT;
}

void SenMLIntPack::fieldsToJson() 
{
    int val;
    String strVal;
    SenMLPack::fieldsToJson();
    val = this->getBaseValue();
    if(val != 0){
        printText(",\"bv\":", 6);
        strVal = val;
        printText(strVal.c_str(), strVal.length());
    }

    val = this->getBaseSum();
    if(val != 0){
        printText(",\"bs\":", 6);
        strVal = val;
        printText(strVal.c_str(), strVal.length());
    }

}

int SenMLIntPack::fieldsToCbor() 
{
    int val;
    int res = SenMLPack::fieldsToCbor();
    
    val = this->getBaseValue();
    if(val){
        res += cbor_serialize_int(SENML_CBOR_VB_LABEL);
        res += cbor_serialize_int(val);
    }

    val = this->getBaseSum();
    if(val){
        res += cbor_serialize_int(SENML_CBOR_BS_LABEL);
        res += cbor_serialize_int(val);
    }
    return res;
}






