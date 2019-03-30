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
 * support for binary sensors
 */

#include <senml_binary_record.h>
#include <senml_helpers.h>
#include <cbor.h>

SenMLBinaryRecord::SenMLBinaryRecord(const char* name): SenMLRecord(name)
{
}

SenMLBinaryRecord::SenMLBinaryRecord(const char* name, SenMLUnit unit): SenMLRecord(name, unit)
{
}

bool SenMLBinaryRecord::set(unsigned char* value, unsigned int length, double time) 
{
    this->_value = (unsigned char*)malloc(length);
    memcpy(this->_value, value, length);
    this->_length = length;
    return this->setTime(time);
}


void SenMLBinaryRecord::fieldsToJson()
{
    SenMLRecord::fieldsToJson();
    printText(",\"vd\":\"", 7);
    printBinaryAsBase64(this->_value, this->_length);
    printText("\"", 1);
}

int SenMLBinaryRecord::fieldsToCbor()
{
    int res = SenMLRecord::fieldsToCbor();
    res += cbor_serialize_int(SENML_CBOR_VD_LABEL);
    res += cbor_serialize_byte_string((const char*)this->_value, this->_length);
    return res;
}






