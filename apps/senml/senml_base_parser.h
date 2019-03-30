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
 * base class for all parsers header
 */



#ifndef SENMLBASEPARSER
#define SENMLBASEPARSER

#include <senml_pack.h>
#include <senml_record.h>

/**
 * base class for json and cbor parsers. Provides functionality to connect to 
 * and search in senml packs and recrods. 
*/
class SenMLBaseParser {

  public:
    SenMLBaseParser(SenMLPack* root): root(root), curPack(root) {};


  private:
    

  protected:
    SenMLPack* root;                    //the root document for which we are parsing. Used to search the up all the actuators.
    SenMLRecord* curRec;
    SenMLPack* curPack;                 //used while searching, to store the current pack to use. init to root pack by default. Make it protected, so child parsers can easily access it without too much fuzz
    String curRecName;                  //keeps a ref of the record name, for raising events for unknown records
    String curPackName;                 //name of hte current pack to send the actuator to.

    //looks up the pack object that has to contain the next field. This is in order
    //to support multiple packs (gateway functionality)
    void setCurrentPack(String& name);


    //check if base units match, if not, show error message
    void checkBaseUnit(String& name);

    //look up a record with the spedified name in the current pack 
    void setCurrentRecord(String& name);
};

#endif // SENMLBASEPARSER






