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
 * support for float sensors
 */


#include <senml_float_record.h>
#include <senml_helpers.h>
#include <cbor.h>
#include <senml_double_pack.h> 
#include <senml_int_pack.h> 

SenMLFloatRecord::SenMLFloatRecord(const char* name): SenMLRecordTemplate(name)
{
}

SenMLFloatRecord::SenMLFloatRecord(const char* name, SenMLUnit unit): SenMLRecordTemplate(name, unit)
{
}

float SenMLFloatRecord::getAdjustedValue()
{
    float adjustedValue = this->get();
    if(_streamCtx->baseDataType == CBOR_TYPE_DOUBLE){
        if(this->asSum())
            adjustedValue -= ((SenMLDoublePack*)this->getRoot())->getBaseSum();
        else
            adjustedValue -= ((SenMLDoublePack*)this->getRoot())->getBaseValue();
    }
    else if( _streamCtx->baseDataType == CBOR_TYPE_INT){
        if(this->asSum())
            adjustedValue -= ((SenMLIntPack*)this->getRoot())->getBaseSum();
        else
            adjustedValue -= ((SenMLIntPack*)this->getRoot())->getBaseValue();
    }
    return adjustedValue;
}

void SenMLFloatRecord::fieldsToJson()
{
    SenMLRecord::fieldsToJson();
    if(this->asSum())
        printText(",\"s\":", 5);
    else
        printText(",\"v\":", 5);
    printDouble(this->getAdjustedValue(), SENML_MAX_DOUBLE_PRECISION);
}

int SenMLFloatRecord::fieldsToCbor()
{
    int res = SenMLRecord::fieldsToCbor();
    if(this->asSum())
        res += cbor_serialize_int(SENML_CBOR_S_LABEL);
    else
        res += cbor_serialize_int(SENML_CBOR_V_LABEL);
    res += cbor_serialize_double(this->getAdjustedValue());
    return res;
}






