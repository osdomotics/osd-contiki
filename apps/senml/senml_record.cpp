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
 * record base class 
 */

#include <senml_record.h>
#include <senml_helpers.h>
#include <senml_pack.h>
#include <cbor.h>
#include <senml_logging.h>

SenMLRecord::SenMLRecord(const char* name): _name(name), _unit(SENML_UNIT_NONE), _time(NAN), _updateTime(0)
{
}

SenMLRecord::SenMLRecord(const char* name, SenMLUnit unit): _name(name), _unit(unit), _time(NAN), _updateTime(0)
{
}

bool SenMLRecord::setTime(double value, bool absolute)
{
    SenMLBase* root = this->getRoot();
    if(absolute){
        if(root){
            if(root->isPack()){
                double baseTime = ((SenMLPack*)root)->getBaseTime();
                if(!isnan(baseTime))
                    value -= baseTime;
            }
            else{
                return false;
            }
        }
    }
    else if(root == NULL){
        return false;
    }
    this->_time = value;
    return true;
}

bool SenMLRecord::setUpdateTime(double value, bool absolute)
{
    SenMLBase* root = this->getRoot();
    if(absolute){
        if(root){
            if(root->isPack()){
                double baseTime = ((SenMLPack*)root)->getBaseTime();
                if(!isnan(baseTime))
                    value -= baseTime;
            }
            else{
                return false;
            }
        }
    }
    else if(root == NULL){
        return false;
    }
    this->_updateTime = value;
    return true;
}

void SenMLRecord::contentToJson()
{
    printText("{", 1);
    this->fieldsToJson();
    printText("}", 1);
}

void SenMLRecord::adjustToBaseTime(double prev, double time)
{
    if(!isnan(this->_time)){
        if(!isnan(prev))
        this->_time += prev;
        if(!isnan(time))
            this->_time -= time;
    }
    if(!isnan(this->_updateTime)){
        if(!isnan(prev))
        this->_updateTime += prev;
        if(!isnan(time))
            this->_updateTime -= time;
    }
}

void SenMLRecord::fieldsToJson()
{
    int bnLength = this->_name.length();
    if(bnLength){
        printText("\"n\":\"", 5);
        printText(this->_name.c_str(), bnLength);
        printText("\"", 1);
    }
    if(!isnan(this->_time)){
        printText(",\"t\":", 5);
        printDouble(this->_time, SENML_MAX_DOUBLE_PRECISION);
    }
    if(this->_unit != SENML_UNIT_NONE){
        printText(",\"u\":\"", 6);
        printUnit(this->_unit);
        printText("\"", 1);
    }
    if(this->_updateTime != 0){
        printText(",\"ut\":", 5);
        #ifdef __MBED__
            char buf[10];
            sprintf(buf, "%d", this->_updateTime);
            String val = buf;
        #else
            String val(this->_updateTime);
        #endif
        printText(val.c_str(), val.length());
    }
}

void SenMLRecord::actuate(const void* value, int dataLength, SenMLDataType dataType)
{
    log_debug(this->getName());
    log_debug("no actuator");
}

int SenMLRecord::contentToCbor()
{
    int res = cbor_serialize_map(this->getFieldLength());
    res += this->fieldsToCbor();
    return res;
}

int SenMLRecord::getFieldLength()
{
    int result = 1;                                 //always have 1 item for the value
    if(this->_name.length() > 0) result++;
    if(!isnan(this->_time)) result++;
    if(this->_unit != SENML_UNIT_NONE) result++;
    if(this->_updateTime != 0) result ++;
    return result;
}

int SenMLRecord::fieldsToCbor()
{
    int res = 0;
    if(this->_name.length() > 0){
        res += cbor_serialize_int(SENML_CBOR_N_LABEL);
        res += cbor_serialize_unicode_string(this->_name.c_str());
    }
    if(!isnan(this->_time)){
        res += cbor_serialize_int(SENML_CBOR_T_LABEL);
        res += cbor_serialize_double(this->_time);
    }
    if(this->_unit != SENML_UNIT_NONE){
        res += cbor_serialize_int(SENML_CBOR_U_LABEL);
        res += cbor_serialize_unicode_string(senml_units_names[this->_unit]);
    }
    if(this->_updateTime != 0){
        res += cbor_serialize_int(SENML_CBOR_UT_LABEL);
        res += cbor_serialize_int(this->_updateTime);
    }
    return res;
}






