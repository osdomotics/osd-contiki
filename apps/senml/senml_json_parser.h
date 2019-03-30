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
 * parse json header
 */

#ifndef SENMLSenMLJsonListener
#define SENMLSenMLJsonListener

#include "senml_JsonListener.h"
#include <senml_base_parser.h>

/**
 * Internal helper class for parsing json data.
 */
class SenMLJsonListener: public JsonListener, SenMLBaseParser {

  public:
    SenMLJsonListener(SenMLPack* root): JsonListener(), SenMLBaseParser(root) {};


    virtual void key(String key);
    virtual void value(String value);
    //virtual void endObject(){};

    //the following need to be implemented cause they are abstract in the base class.
    //virtual void startDocument(){};
    //virtual void endArray(){};
    //virtual void endDocument(){};
    //virtual void startArray(){};
    //virtual void startObject(){};



  private:
    int expected;               //name of key who's value is expected next
    double baseValue;           //if the current pack defined a base value, add it to all values that we find

    void setValue(const void* value, int length, SenMLDataType dataType);
};

#endif // SENMLSenMLJsonListener






