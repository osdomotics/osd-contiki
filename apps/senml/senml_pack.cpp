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
 * pack (document) without base values
 */

#include <senml_pack.h>
#include <senml_base.h>
#include <senml_helpers.h>
#include <senml_json_parser.h>
#include <senml_cbor_parser.h>
#include <senml_JsonStreamingParser.h>
#include <math.h>
#include <cbor.h>
#include <senml_logging.h>



//todo: inline these:
void SenMLPack::setBaseName(const char* name)
{
    this->_bn = name;
}

const char* SenMLPack::getBaseName()
{
    return this->_bn.c_str();
}

void SenMLPack::setBaseUnit(SenMLUnit unit)
{
    this->_bu = unit;
}


void SenMLPack::setBaseTime(double time)
{
    double prev = this->_bt;
    this->_bt = time;                               //set before asking children -> could be better to do it afterwards?
    SenMLBase *item = this->_start;
    while(item){
        item->adjustToBaseTime(prev, time);
        item = item->getNext();
    }
}


void SenMLPack::setLast(SenMLBase* value)
{
    if(value == this)                           //if we become the last item in the list, then the list is empty.
        this->_end = NULL;
    else
        this->_end = value;
}


bool SenMLPack::add(SenMLBase* item)
{
    if(item->getNext() != NULL){
        log_debug("already in list");
        return false;
    }

    SenMLBase* last = this->_end;
    if(last){
        last->setNext(item);
        item->setPrev(last);
    }
    else{
        this->_start = item;
        item->setPrev(this);
    }
    this->_end = item;
    return true;
}

bool SenMLPack::clear()
{
    SenMLBase *item = this->_start;
    while(item){
        if(item->isPack())                                          //if it's a pack element, it also needs to clear out it's children.
            ((SenMLPack*)item)->clear();
        item->setPrev(NULL);
        SenMLBase *next = item->getNext();
        item->setNext(NULL);
        item = next;
    }
    this->setNext(NULL);
    this->setPrev(NULL);
    this->_end = NULL;
    this->_start = NULL;
    return true;
}

void SenMLPack::fromJson(Stream *source, SenMLStreamMethod format)
{
    JsonStreamingParser parser;
    SenMLJsonListener listener(this);
    
    parser.setListener(&listener);
    char data;
    if(format == SENML_RAW) {
        #ifdef __MBED__
            data = source->getc();
        #else
            data = source->read();
        #endif
    }
    else{
        data = readHexChar(source);
    }
        
    while(data != -1){
        parser.parse(data); 
        if(format == SENML_RAW){
            #ifdef __MBED__
                data = source->getc();
            #else
                data = source->read();
            #endif
        }   
        else
            data = readHexChar(source);
    }
    // when we get here, all the data is stored in the document and callbacks have been called.
}

void SenMLPack::fromJson(const char *source)
{
    JsonStreamingParser parser;
    SenMLJsonListener listener(this);
    
    parser.setListener(&listener);
    for(int i = 0; source[i] != 0; i++){
        parser.parse(source[i]); 
    }
    // when we get here, all the data is stored in the document and callbacks have been called.
}

void SenMLPack::fromCbor(Stream* source, SenMLStreamMethod format)
{
    SenMLCborParser parser(this, format);
    parser.parse(source);
}

void SenMLPack::fromCbor(char* source, int length, SenMLStreamMethod format)
{
    SenMLCborParser parser(this, format);
    parser.parse(source, length);
}


void SenMLPack::toJson(Stream *dest, SenMLStreamMethod format)
{
    StreamContext renderTo;                                              //set up the global record that configures the rendering. This saves us some bytes on the stack and in code by not having to pass along the values as function arguments.
    _streamCtx = &renderTo;
    this->setupStreamCtx(dest, format);
    this->internalToJson();
}

void SenMLPack::toJson(char *dest, int length, SenMLStreamMethod format)
{
    StreamContext renderTo;                                              //set up the global record that configures the rendering. This saves us some bytes on the stack and in code by not having to pass along the values as function arguments.
    _streamCtx = &renderTo;
    this->setupStreamCtx(dest, length, format);
    this->internalToJson();
}

//render the content of the current object to json data (string)
void SenMLPack::internalToJson()
{
    printText("[", 1);
    this->contentToJson();
    printText("]", 1);
}


void SenMLPack::fieldsToJson()
{
    int bnLength = this->_bn.length();
    if(bnLength > 0){
        printText("\"bn\":\"", 6);
        printText(this->_bn.c_str(), bnLength);
        printText("\"", 1);
    }
    if(this->_bu){
        printText(",\"bu\":\"", 7);
        printUnit(this->_bu);
        printText("\"", 1);
    }
    if(!isnan(this->_bt)){
        printText(",\"bt\":", 6);
        printDouble(this->_bt, SENML_MAX_DOUBLE_PRECISION);
    }
}

void SenMLPack::contentToJson()
{
    printText("{", 1);
    this->fieldsToJson();
    SenMLBase *next = this->_start;
    if(next && next->isPack() == false){                        //we can only inline the first record. If the first item is a Pack (child device), then don't inline it.
        printText(",", 1);
        next->fieldsToJson();
        next = next->getNext();
    }
    printText("}", 1);
    while(next){
        printText(",", 1);
        next->contentToJson();
        next = next->getNext();
    }
}



void SenMLPack::setupStreamCtx(char *dest, int length, SenMLStreamMethod format)
{
    _streamCtx->data.blob.data = dest;
    _streamCtx->data.blob.length = length;
    _streamCtx->data.blob.curPos = 0;
    _streamCtx->dataAsBlob = true;
    _streamCtx->format = format;
    _streamCtx->baseValue.baseUint = 0;                                    //by default, there is no base value or sum
    _streamCtx->baseSum.baseUint = 0;
    _streamCtx->baseDataType = CBOR_TYPE_DATA;                             //data never adjusts for basevalue, so this is safe.
}

void SenMLPack::setupStreamCtx(Stream *dest, SenMLStreamMethod format)
{
    _streamCtx->data.stream = dest;
    _streamCtx->format = format;
    _streamCtx->dataAsBlob = false;
    _streamCtx->baseValue.baseUint = 0;                                    //by default, there is no base value or sum
    _streamCtx->baseSum.baseUint = 0;
    _streamCtx->baseDataType = CBOR_TYPE_DATA;                             //data never adjusts for basevalue, so this is safe.
}

int SenMLPack::toCbor(Stream *dest, SenMLStreamMethod format)
{
    StreamContext renderTo;                                              //set up the global record that configures the rendering. This saves us some bytes on the stack and in code by not having to pass along the values as function arguments.
    _streamCtx = &renderTo;
    this->setupStreamCtx(dest, format);
    int res = cbor_serialize_array(this->getArrayLength());
    res += this->contentToCbor();
    return res;
}

int SenMLPack::toCbor(char *dest, int length, SenMLStreamMethod format)
{
    StreamContext renderTo;                                              //set up the global record that configures the rendering. This saves us some bytes on the stack and in code by not having to pass along the values as function arguments.
    _streamCtx = &renderTo;
    this->setupStreamCtx(dest, length, format);
    int res = cbor_serialize_array(this->getArrayLength());
    res += this->contentToCbor();
    return res;
}

int SenMLPack::contentToCbor()
{
    int length = cbor_serialize_map(this->getFieldLength());

    int res = this->fieldsToCbor();
    SenMLBase *next = this->_start;
    if(next && next->isPack() == false){                        //we can only inline the first record. If the first item is a Pack (child device), then don't inline it.
        res += next->fieldsToCbor();
        next = next->getNext();
    }

    while(next){
        if(next->isPack() == false)
            res += next->contentToCbor();
        else
            res += ((SenMLPack*)next)->contentToCbor();
        next = next->getNext();
    }
    return res;
}

int SenMLPack::getArrayLength()
{
    int result = 0;                             //init to 0 cause if there is a record, the first will become part of the first element in the array, if we were to init to 1, we would have 1 record too many.
    SenMLBase *next = this->_start;
    while(next){                                //we can only inline the first record. If the first item is a Pack (child device), then don't inline it.
        result += next->getArrayLength();       //custom record implementations may wrap multiple records.
        next = next->getNext();
    }
    if(result == 0)                             //if there are no items in this pack, then we still render 1 array element, that of the pack itself.
        result = 1;
    return result;
}


int SenMLPack::getFieldLength()
{
    int result = 0;                             
    if(this->_bn.length() > 0 ) result++;
    if(this->_bu) result++;
    if(!isnan(this->_bt)) result++;

    SenMLBase *next = this->_start;
    if(next && next->isPack() == false){        //we can only inline the first record. If the first item is a Pack (child device), then don't inline it.
        result += next->getFieldLength();
        next = next->getNext();
    }
    return result;
}

int SenMLPack::fieldsToCbor()
{
    int res = 0 ;
    if(this->_bn.length() > 0 ){
        res += cbor_serialize_int(SENML_CBOR_BN_LABEL);
        res += cbor_serialize_unicode_string(this->_bn.c_str());
    }
    if(this->_bu){
        res += cbor_serialize_int(SENML_CBOR_BU_LABEL);
        res += cbor_serialize_unicode_string(senml_units_names[this->_bu]);
    }
    if(!isnan(this->_bt)){
        res += cbor_serialize_int(SENML_CBOR_BT_LABEL);
        res += cbor_serialize_double(this->_bt);
    }
    return res;
}






