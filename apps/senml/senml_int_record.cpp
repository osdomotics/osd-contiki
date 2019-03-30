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
 * support for int sensors
 */

#include <senml_int_record.h>
#include <cbor.h>
#include <senml_helpers.h>
#include <senml_int_pack.h> 
#include <senml_double_pack.h> 

SenMLIntRecord::SenMLIntRecord(const char* name): SenMLRecordTemplate(name)
{
}

SenMLIntRecord::SenMLIntRecord(const char* name, SenMLUnit unit): SenMLRecordTemplate(name, unit)
{
}

int SenMLIntRecord::getAdjustedValue()
{
    int adjustedValue = this->get();
    if(_streamCtx->baseDataType == CBOR_TYPE_INT){
        if(this->asSum())
            adjustedValue -= ((SenMLIntPack*)this->getRoot())->getBaseSum();
        else
            adjustedValue -= ((SenMLIntPack*)this->getRoot())->getBaseValue();
    }
    return adjustedValue;
}

double SenMLIntRecord::getAdjustedValueD()
{
    double adjustedValue = this->get();
    if(_streamCtx->baseDataType == CBOR_TYPE_DOUBLE){
        if(this->asSum())
            adjustedValue -= ((SenMLDoublePack*)this->getRoot())->getBaseSum();
        else
            adjustedValue -= ((SenMLDoublePack*)this->getRoot())->getBaseValue();
    }
    return adjustedValue;
}

void SenMLIntRecord::fieldsToJson()
{
    SenMLRecord::fieldsToJson();
    if(this->asSum())
        printText(",\"s\":", 5);
    else
        printText(",\"v\":", 5);

    //if the parent pack has a base value or base sum of type double, then we need to render a double value.
    if(_streamCtx->baseDataType == CBOR_TYPE_INT || _streamCtx->baseDataType == CBOR_TYPE_DATA){
        #ifdef __MBED__
            char buf[10];
            sprintf(buf, "%d", this->getAdjustedValue());
            String val = buf;
        #else
            String val(this->getAdjustedValue());
        #endif
        printText(val.c_str(), val.length());
    }
    else{
        printDouble(this->getAdjustedValueD(), SENML_MAX_DOUBLE_PRECISION);
    }
}


int SenMLIntRecord::fieldsToCbor()
{
    int res = SenMLRecord::fieldsToCbor();
    if(this->asSum())
        res += cbor_serialize_int(SENML_CBOR_S_LABEL);
    else
        res += cbor_serialize_int(SENML_CBOR_V_LABEL);

    //if the parent pack has a base value or base sum of type double, then we need to render a double value.
    if(_streamCtx->baseDataType == CBOR_TYPE_INT || _streamCtx->baseDataType == CBOR_TYPE_DATA)        
        res += cbor_serialize_int(this->getAdjustedValue());
    else
        res += cbor_serialize_double(this->getAdjustedValueD());
    return res;
}






