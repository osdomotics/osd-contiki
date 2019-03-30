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
 * pack (document) without base values headers
 */

#ifndef SENMLPACK
#define SENMLPACK

#ifdef __MBED__
    #include "mbed.h"
    #include <string> 
    using namespace std;
    #define String string
#endif

#include <senml_base.h>


#define PACK_ACTUATOR_SIGNATURE void (*callback)(const char*, const char*, const void*, int, SenMLDataType)

/** 
 * SenMLPack represents a single senml document that can be sent or received. 
 * 
 * A senmlPack object has SenMLRecords and/or other SenMLPack objects as children. These
 * represent the data that the object contains.
 * A SenMLRecord represents a single value, while child SenMLPacks represent data of other 
 * devices. When a SenMLPack contains other SenMLPack objects, the root object represents a
 * gateway.
 * 
 * A SenMLPack object is able to render and parse the data to/from a json string or to/from binary 
 * CBOR data. Both rendering and parsing can be done either directly from a stream (like an UART 
 * connected to a modem), or from a memory buffer. 
 * Rendering to and parsing from a stream is useful for devices that have extreme low memory available. 
 * Almost no buffers are used in this case. Error handling is limited in this case though.
 * The parser and generator are able to render in the native format (strings for json, binary data for
 * cbor) and as a hex string. This is especially useful when directly working on a stream: some modems 
 * (ex lora) accept instructions with data in HEX format.
 * 
 * example:
 * 
 * @code
 * #include <kpn_senml.h>
 * 
 * SenMLPack doc("device_name");
 * 
 * void setup(){
 *     Serial.begin(57600);
 *     senMLSetLogger(&Serial);
 *     delay(1000);
 *     Serial.println("start");
 * }

 * void loop(){
 *     int val = 10;                                   //just give it some value
 *     SenMLFloatRecord rec("temp", SENML_UNIT_DEGREES_CELSIUS, val);
 *     doc.add(&rec);                      
 *     doc.toJson(&Serial);                            //as text
 *     Serial.println();
 *     doc.toJson(&Serial, SENML_HEX);                 //in hex format (often used in communication with lora modems)
 *     Serial.println();
 *     delay(1000);
 * }

 * @endcode
 * 
 */ 
class SenMLPack: public SenMLBase
{
    friend class SenMLJsonListener; 
    friend class SenMLCborParser;
    friend class SenMLBase;
    public:

        /**
         * create a SenMLPack object.
         */
        SenMLPack():  _bu(SENML_UNIT_NONE), 
                      _bt(NAN),
                      _end(NULL) {};

        /**
         * create a SenMLPack object.
         * @param baseName the string that will be prepended to all records in this pack. 
         *                 Is used to represent the name of the device.
         */
        SenMLPack(const char* baseName):  _bn(baseName),                                                    //mbed compiler doesnt support delegating constructors
                                          _bu(SENML_UNIT_NONE), 
                                          _bt(NAN),
                                          _end(NULL) {};

        /**
         * create a SenMLPack object.
         * @param baseName the string that will be prepended to all records in this pack. 
         *                 Is used to represent the name of the device.
         * @param baseUnit the unit name that will be used by default if the record doesnt
         *                 not define one.
         */                                          
        SenMLPack(const char* baseName, SenMLUnit baseUnit):  _bn(baseName),                               
                                                              _bu(baseUnit), 
                                                              _bt(NAN),
                                                              _end(NULL) {};

        /**
         * create a SenMLPack object.
         * @param baseName the string that will be prepended to all records in this pack. 
         *                 Is used to represent the name of the device.
         * @param baseUnit the unit name that will be used by default if the record doesnt
         *                 not define one.
         * @param baseTime the time that will be added to each record. When specified, each
         *                 record that does not specify a time, will receive this time. When
         *                 the record does have a time, the baseTime of the pack is added to it,
         *                 so the time of the record becomes relative to that of the pack.
         */                                                              
        SenMLPack(const char* baseName, SenMLUnit baseUnit, double baseTime): _bn(baseName),                
                                                                              _bu(baseUnit), 
                                                                              _bt(baseTime),
                                                                              _end(NULL) {};

        /**
         * create a SenMLPack object.
         * @param callback a function that will be called while parsing incomming data, when no
         *                 record can be found that matches any of the defined ones in the object.
         *                 The parameters of the callback must be: 
         *                    const char* packName the name of the pack that the record belongs to.
         *                                         The data is for a child SenMLPack when this
         *                                         field is different then the name of the root pack.
         *                    const char* recordName the name of the record 
         *                    const void* value a pointer to the memory blob that contains the actual value.
         *                    int size the size of the memory blobl
         *                    SenMLDataType dataType: defines how to interprete the memory blob (ex: pointer to integer,..)                
         */
        SenMLPack(PACK_ACTUATOR_SIGNATURE): _bn(""),                                  
                                            _bu(SENML_UNIT_NONE), 
                                            _bt(NAN),
                                            _end(NULL),
                                            callback(callback) {};

        /**
         * create a SenMLPack object.
         * @param baseName the string that will be prepended to all records in this pack. 
         *                 Is used to represent the name of the device.
         * @param callback a function that will be called while parsing incomming data, when no
         *                 record can be found that matches any of the defined ones in the object.
         *                 The parameters of the callback must be: 
         *                    const char* packName the name of the pack that the record belongs to.
         *                                         The data is for a child SenMLPack when this
         *                                         field is different then the name of the root pack.
         *                    const char* recordName the name of the record 
         *                    const void* value a pointer to the memory blob that contains the actual value.
         *                    int size the size of the memory blobl
         *                    SenMLDataType dataType: defines how to interprete the memory blob (ex: pointer to integer,..)                
         */                                            
        SenMLPack(const char* baseName, PACK_ACTUATOR_SIGNATURE): _bn(baseName), 
                                                                  _bu(SENML_UNIT_NONE), 
                                                                  _bt(NAN),
                                                                  _end(NULL),
                                                                  callback(callback) {};

        /**
         * create a SenMLPack object.
         * @param baseName the string that will be prepended to all records in this pack. 
         *                 Is used to represent the name of the device.
         * @param baseUnit the unit name that will be used by default if the record doesnt
         *                 not define one.
         * @param callback a function that will be called while parsing incomming data, when no
         *                 record can be found that matches any of the defined ones in the object.
         *                 The parameters of the callback must be: 
         *                    const char* packName the name of the pack that the record belongs to.
         *                                         The data is for a child SenMLPack when this
         *                                         field is different then the name of the root pack.
         *                    const char* recordName the name of the record 
         *                    const void* value a pointer to the memory blob that contains the actual value.
         *                    int size the size of the memory blobl
         *                    SenMLDataType dataType: defines how to interprete the memory blob (ex: pointer to integer,..)                
         */                                                                     
        SenMLPack(const char* baseName, SenMLUnit baseUnit, PACK_ACTUATOR_SIGNATURE): _bn(baseName), 
                                                                                      _bu(baseUnit), 
                                                                                      _bt(NAN),
                                                                                      _end(NULL),
                                                                                      callback(callback) {};

        /**
         * create a SenMLPack object.
         * @param baseName the string that will be prepended to all records in this pack. 
         *                 Is used to represent the name of the device.
         * @param baseUnit the unit name that will be used by default if the record doesnt
         *                 not define one.
         * @param baseTime the time that will be added to each record. When specified, each
         *                 record that does not specify a time, will receive this time. When
         *                 the record does have a time, the baseTime of the pack is added to it,
         *                 so the time of the record becomes relative to that of the pack.
         * @param callback a function that will be called while parsing incomming data, when no
         *                 record can be found that matches any of the defined ones in the object.
         *                 The parameters of the callback must be: 
         *                    const char* packName the name of the pack that the record belongs to.
         *                                         The data is for a child SenMLPack when this
         *                                         field is different then the name of the root pack.
         *                    const char* recordName the name of the record 
         *                    const void* value a pointer to the memory blob that contains the actual value.
         *                    int size the size of the memory blobl
         *                    SenMLDataType dataType: defines how to interprete the memory blob (ex: pointer to integer,..)   
         */                                                                                       
        SenMLPack(const char* baseName, SenMLUnit baseUnit, double baseTime, PACK_ACTUATOR_SIGNATURE): _bn(baseName), 
                                                                                                       _bu(baseUnit), 
                                                                                                       _bt(baseTime),
                                                                                                       _end(NULL),
                                                                                                       callback(callback) {};

        /**
         * destroys the SenMLPack object.
         */
        ~SenMLPack(){};

        /**
         * render the content of the current object to json data (string).
         * This function is ideal for devices with low memory usage but offers less control over the rendering process.
         * @param dest the destination stream to where the data will be rendered without buffering it in memory
         * @param format determins how the data will be rendered. See SenMLStreamMethod for possible methods.
         * @returns none
         */
        void toJson(Stream* dest, SenMLStreamMethod format=SENML_RAW);

        /**
         * render the content of the current object to json data (string).
         * This function renders the data to a memory buffer. If the buffer is full before the entire object is 
         * rendered, an error will be written to the debug stream.
         * @param dest a memory buffer to which the data will be rendred.
         * @param length the length of the memory buffer.
         * @param format determins how the data will be rendered. See SenMLStreamMethod for possible methods.
         * @returns none
         */
        void toJson(char *dest, int length, SenMLStreamMethod format=SENML_RAW);

        /**
         * render the content of the current object to cbor data (binary).
         * This function is ideal for devices with low memory usage but offers less control over the rendering process.
         * @param dest the destination stream to where the data will be rendered without buffering it in memory
         * @param format determins how the data will be rendered. See SenMLStreamMethod for possible methods.
         * @returns nr of bytes that were rendered
         */
        int toCbor(Stream* dest, SenMLStreamMethod format=SENML_RAW);

        /**
         * render the content of the current object to cbor data (binary).
         * This function renders the data to a memory buffer. If the buffer is full before the entire object is 
         * rendered, an error will be written to the debug stream.
         * @param dest a memory buffer to which the data will be rendred.
         * @param length the length of the memory buffer.
         * @param format determins how the data will be rendered. See SenMLStreamMethod for possible methods.
         * @returns nr of bytes that were rendered
         */
        int toCbor(char *dest, int length, SenMLStreamMethod format=SENML_RAW);

        /**
         * read and parse a senml json string from the specified source and, for each registered actuator, call the
         * appropriate event on the actuator itself, for others, the callback function PACK_ACTUATOR_SIGNATURE 
         * will be called, if present.
         * This method is ideal for devices with very littel ram memory. It will block on most devices if there is 
         * no more input to be read from the stream and the end of the json structure is not yet reached.
         * Note: on mbed systems, the blocking nature is not garanteed. Instead, if no more data is available before
         * the end is reached, parsing will fail.
         * @param source the source stream to read the data from.
         * @param format determins how the data will be read (ex: as normal text or in HEX format). 
         *               See SenMLStreamMethod for possible methods.
         * @returns none
         */
        void fromJson(Stream* source, SenMLStreamMethod format=SENML_RAW);

        /**
         * parse a senml json string from the specified source and, for each registered actuator, call the
         * appropriate event on the actuator itself, for others, the callback function PACK_ACTUATOR_SIGNATURE 
         * will be called, if present.
         * This method takes a string stored in memory as input. The json must be fully defined. It is up to the
         * caller to transform it to a regular text string, if needed (ex: lora devices might send it in hex format).
         * @param source the source string to use as input. This must be null terminated.
         * @returns none
         */
        void fromJson(const char* source);

        /**
         * read and parse senml cbor from the specified source and, for each registered actuator, call the
         * appropriate event on the actuator itself, for others, the callback function PACK_ACTUATOR_SIGNATURE 
         * will be called, if present.
         * This method is ideal for devices with very littel ram memory. It will block on most decices if there is no 
         * more input to be read from the stream and the end of the cbor structure is not yet reached.
         * Note: on mbed systems, the blocking nature is not garanteed. Instead, if no more data is available before
         * the end is reached, parsing will fail.
         * @param source the source stream to read the data from.
         * @param format determins how the data will be read (ex: as normal binary or in HEX format). 
         *               See SenMLStreamMethod for possible methods.
         * @returns none
         */
        void fromCbor(Stream* source, SenMLStreamMethod format=SENML_RAW);
        
        /**
         * parse senml cbor from the specified memory and, for each registered actuator, call the
         * appropriate event on the actuator itself, for others, the callback function PACK_ACTUATOR_SIGNATURE 
         * will be called, if present.
         * This method takes a memory blob as input. The data must be fully defined. 
         * @param source the source data to use as input.
         * @param length the length of the source data.
         * @param format determins how the data will be read (ex: as normal binary or in HEX format). 
         *               See SenMLStreamMethod for possible methods.
         * @returns none
         */
        void fromCbor(char* source, int length, SenMLStreamMethod format);

        /**
         * assign a basename to the SenMLPack object. This represents the name of the device.
         * see the spec on [base fields](https://tools.ietf.org/html/draft-ietf-core-senml-13#section-4.1) for more info.
         * Every SenMLPack object must have a basename. This field will always be rendered in the output, even if the
         * string is empty.
         * @param name an immutable string that will be used to represent the name of the device. An internal
         * copy of the value will be made.
         * @returns none
         */ 
        void setBaseName(const char* name);

        /**
         * Get the base name. see the spec on [base fields](https://tools.ietf.org/html/draft-ietf-core-senml-13#section-4.1) for more info.
         * @returns the name of the device as an immutable string.
         */
        const char* getBaseName();

        /**
         * Set the base unit that will be used as the default unit for all records that don't define their own unit. 
         * see the spec on [base fields](https://tools.ietf.org/html/draft-ietf-core-senml-13#section-4.1) for more info.
         * Set to SENML_UNIT_NONE for ommiting the base unit from the output (default).
         * @param unit the unit to use as default. See SenMLUnit for all supported unit names.
         * @returns none
         */
        void setBaseUnit(SenMLUnit unit);

        /**
         * Get the base unit. see the spec on [base fields](https://tools.ietf.org/html/draft-ietf-core-senml-13#section-4.1) for more info.
         * @returns a SenMLUnit enum value that is used as the default unit for records that don't define a unit of their own.
         */
        inline SenMLUnit getBaseUnit() { return this->_bu; };


        /**
         * Set the base time. see the spec on [base fields](https://tools.ietf.org/html/draft-ietf-core-senml-13#section-4.1) for more info.
         * @param time the value to use as base time. set bt to NaN if the field should not be included in the output.
         * @returns none
         */ 
        void setBaseTime(double time);

        /**
         * Get the base time. see the spec on [base fields](https://tools.ietf.org/html/draft-ietf-core-senml-13#section-4.1) for more info.
         * @returns a double value that is used as the default unit for records that don't define a unit of their own. 
         *          if no base time is set, NaN will be returned.
         */
        inline double getBaseTime() { return this->_bt; };

        /**
         * Adds the specified SenML object to the document. The item will be appended to the end of the linked list. 
         * The item being added, can be a regular SenMLRecord or another SenMLPack object if you want to send data 
         * for multiple devices in 1 SenML message.
         * Check the result of the function to see if the operation was successful or not. Possible reasons for failure:
         * - if the item being added is already part of a document.
         * @param item a pointer to a SenMlRecord or SenMLPack that needs to be added to the document.
         * @returns true upon success, otherwise false.
         */        
        bool add(SenMLBase* item);

        /**
         * Clear out the document and remove all the children. Children aren't destroyed, this is up to the developer.
         * @returns true (at the moment, the function does not yet return false as it doesn't detect any errors)
         */
        bool clear();
        
        /**
         * get the first recrod of in this pack element. 
         * @returns null when this object is empty (has no children), otherwise, the first item (SenMLRecord or SenMLPack)
         * of the list.
         */
        inline SenMLBase* getFirst() { return this->_start; };


        /**
         * renders all the fields to json, without the starting and ending brackets.
         * Inheriters can extend this function if they want to add extra fields to the json output
         * note: this is public so that custom implementations for the record object can use other objects 
         * internally and render to json using this function (ex: coordinatesRecord using 3 floatRecrods for lat, lon & alt.
         * @returns: None
        */
        virtual void fieldsToJson();

        /**
         * renders all the fields to cbor format. renders all the fields of the object without the length info 
         * at the beginning
         * note: this is public so that custom implementations for the record object can use other objects 
         * internally and render to json using this function (ex: coordinatesRecord using 3 floatRecrods for 
         * lat, lon & alt.
         * @returns: The number of bytes that were written.
        */
        virtual int fieldsToCbor();

    protected:


        //derived classes can use this function to see if the root object (getRoot) is a SenMLPack
        //class or not.
        virtual bool isPack() { return true; }

        //when the user did not register a specific record actuator, this will be called.
        inline void actuate(const char* pack, const char* record, const void* value, int valueLength, SenMLDataType dataType)
        {
            if(this->callback)
                this->callback(pack, record, value, valueLength, dataType);
        };

        //store a ref to the last item in the list for quick link operations
        void setLast(SenMLBase* value);

        //renders the content of the pack object without []
        virtual int contentToCbor();    


        //calculates the nr of items that there will be in the json array in senml representation
        //this is used for rendering cbor which needs to declare the nr of elements in an array.
        virtual int getArrayLength();

        virtual void setupStreamCtx(char *dest, int length, SenMLStreamMethod format);

        virtual void setupStreamCtx(Stream *dest, SenMLStreamMethod format);

    private:
        String _bn;
        SenMLUnit _bu;
        double _bt;
        SenMLBase *_end;                                //keeps track of the end of the list
        SenMLBase *_start;                              //keeps track of the start of the list
        PACK_ACTUATOR_SIGNATURE;                        //for registering actuator callbacks.
        
        
        //renders the content of the pack object without []
        virtual void contentToJson();              

        //calculates the nr of json fields that this object uses in a senml structure
        virtual int getFieldLength();


        void internalToJson();

        inline char readHexChar(Stream *source){
            #ifdef __MBED__
                unsigned char first = source->getc();
                unsigned char second = source->getc();
            #else
                unsigned char first = source->read();
                unsigned char second = source->read();
            #endif
            first = (first < '9') ? first - '0' : first - '7';
            second = (second < '9') ? second - '0' : second - '7';
            return (16 * first) + second;
        };
};


#endif // SENMLPACK







