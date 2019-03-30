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
 * support for bool sensor 
 */


#include <senml_bool_record.h>
#include <cbor.h>
#include <senml_helpers.h>

SenMLBoolRecord::SenMLBoolRecord(const char* name): SenMLRecordTemplate(name)
{
}

SenMLBoolRecord::SenMLBoolRecord(const char* name, SenMLUnit unit): SenMLRecordTemplate(name, unit)
{
}

void SenMLBoolRecord::fieldsToJson()
{
    SenMLRecord::fieldsToJson();
    printText(",\"vb\":", 6);
    if(this->get())
        printText("true", 4);
    else
        printText("false", 5);
}

int SenMLBoolRecord::fieldsToCbor()
{
    int res = SenMLRecord::fieldsToCbor();
    res += cbor_serialize_int(SENML_CBOR_VB_LABEL);
    res += cbor_serialize_bool(this->get());
    return res;
}






