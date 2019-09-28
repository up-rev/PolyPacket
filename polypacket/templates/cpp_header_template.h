/**
  *@file ${proto.cppFileName}.h
  *@brief generated code for ${proto.name} packet service
  *@author make_protocol.py
  *@date ${proto.genTime}
  *@hash ${proto.hash}
  */

#pragma once
/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/

#include <string>
extern "C"
{
  #include "Utilities/PolyPacket/poly_service.h"
}

#define ${proto.prefix.upper()}_SERVICE_HASH 0x${proto.hash}

/*******************************************************************************
  Enums
*******************************************************************************/
% for field in proto.fields:
% if field.isEnum:
/* Enums for ${field.name} field */
typedef enum{
  % for val in field.vals:
  ${proto.prefix.upper()+"_"+field.name.upper() + "_" + val.name.upper()},              /* ${val.desc} */
  % endfor
  ${proto.prefix.upper()+"_"+field.name.upper()}_MAX_LIMIT
} ${proto.prefix}_${field.name.lower()}_e;
%if proto.snippets:
//Switch Snippet
/*
switch(${field.name.lower()})
{
% for val in field.vals:
  case ${proto.prefix.upper()+"_"+field.name.upper() + "_" + val.name.upper()}:    // ${val.desc}
    break;
% endfor
  default:
    break;
}
*/
% endif

% endif
% endfor

/*******************************************************************************
  Bits/Flags
*******************************************************************************/
% for field in proto.fields:
% if field.isMask:
/* Flags for ${field.name} field */
typedef enum{
  % for idx,val in enumerate(field.vals):
  ${proto.prefix.upper()+"_"+field.name.upper() + "_" + val.name.upper()} = ${ field.valsFormat % (1 << idx)},    /* ${val.desc} */
  % endfor
  ${proto.prefix.upper()+"_"+field.name.upper()}_MAX_LIMIT
} ${proto.prefix}_${field.name.lower()}_e;

% endif
% endfor

/*******************************************************************************
  Global Descriptors
*******************************************************************************/
//Declare extern packet descriptors
% for packet in proto.packets:
extern poly_packet_desc_t* ${packet.globalName};
% endfor

% for struct in proto.structs:
extern poly_packet_desc_t* ${struct.globalName};
% endfor


//Declare extern field descriptors
% for field in proto.fields:
extern poly_field_desc_t* ${field.globalName};
% endfor

using namespace std;

class ${proto.camelPrefix()}Packet {
  public:

    ${proto.camelPrefix()}Packet()
    {
      poly_packet_build(&mPacket, NULL, true);
      mPacket.mReusable = true;
    }

    /**
      *@brief initializes a new {proto.prefix}_packet_t
      *@param desc ptr to packet descriptor to model packet from
      */
    ${proto.camelPrefix()}Packet(poly_packet_desc_t* desc)
    {
       poly_packet_build(&mPacket, desc, true);
       mPacket.mReusable = true;
    }


    /**
      *@brief recrusively cleans packet and its contents if it still has ownership
      *@param packet packet to clean
      */
    ~${proto.camelPrefix()}Packet() {clean();}

    /**
      *@brief builds a new ${proto.camelPrefix()}Packet
      *@param desc ptr to packet descriptor to model packet from
      */
    void build(poly_packet_desc_t* desc)
    {
      poly_packet_build(&mPacket, desc, true);
      mPacket.mReusable = true;
    }

    /**
      *@brief frees memory allocated for metapacket
      *@param packet ptr to metaPacket
      *
      */
    void clean()
    {
      //If the packet has been spooled, the spool is responsible for it now
      if(mPacket.mBuilt && (!mPacket.mSpooled))
      {
        poly_packet_clean(&mPacket);
      }

      mPacket.mBuilt = false;
    }

    poly_packet_t* Packet() {return &mPacket;}
    uint16_t Token() {return mPacket.mHeader.mToken;}
    int Interface() {return mPacket.mInterface;}

    /**
      *@brief converts packet to json
      *@param packet ptr to packet to convert
      *@param buf buffer to store string
      *@return length of string
      */
    string toJSON()
    {
      char buf[512];

      poly_packet_print_json(&mPacket, buf, false);

      return string(buf);
    }

    /**
      *@brief parses packet from a buffer of data
      *@param packet ptr to packet to be built
      *@param buf buffer to parse
      *@return status of parse attempt
      */
     ParseStatus_e parse(const uint8_t * buf, int len) { return poly_packet_parse_buffer(&mPacket, buf, len); }

    /**
      *@brief Copies all fields present in both packets from src to dst
      *@param dst ptr to packet to copy to
      *@param src ptr to packet to copy from
      */
    void copyFrom(${proto.camelPrefix()}Packet& src) {poly_packet_copy(&mPacket , &src.mPacket );}

    /**
      *@brief packs packet into a byte array
      *@param packet ptr to packet to be packed
      *@param buf buffer to store data
      *@return length of packed data
      */
    int pack(uint8_t* buf){ return poly_packet_pack(&mPacket, buf);}

    /**
      *@brief packs packet into a COBS encoded byte array
      *@param packet ptr to packet to be packed
      *@param buf buffer to store data
      *@return length of packed data
      */
    int packEncoded(uint8_t* buf){ return poly_packet_pack_encoded(&mPacket, buf);}

    /**
      *@brief gets the descriptor for the packet
      *@param packet ptr to packet to be checked
      */
    const poly_packet_desc_t* desc() {return mPacket.mDesc;}

    /**
      *@brief checks to see if field is present in packet
      *@param packet ptr to packet to be packed
      *@param field ptr to field desc
      *@return true if field is present
      */
    bool hasField(poly_field_desc_t* field) {  return poly_packet_has(&mPacket, field);}

    /*******************************************************************************
    Meta-Packet setters/Getters
    *******************************************************************************/
    % for field in proto.fields:

    %if field.isString:
    /**
      *@brief Sets value of ${field.name} field
      *@param val string to set
      */
    void set${field.camel()}(const string& val) { poly_packet_set_field(&mPacket, ${field.globalName},(void*) val.c_str());}

    /**
      *@brief Gets value of ${field.name} field
      *@return string value
      */
    string get${field.camel()}() const {return (const char*)poly_packet_get_field_ptr(&mPacket, ${field.globalName});}

    % elif field.isArray:
    /**
      *@brief Sets value(s) in ${field.name} field
      *@param val ${field.getParamType()} to copy data from
      */
    void set${field.camel()}(const ${field.getParamType()} val) {poly_packet_set_field(&mPacket, ${field.globalName}, (void*) val);}

    /**
      *@brief Gets value of ${field.name} field
      *@return ${field.getParamType()} of data in field
      */
    int get${field.camel()}(${field.getParamType()} val) const { return poly_packet_get_field(&mPacket, ${field.globalName}, (uint8_t*) val); }

    % else:
    /**
      *@brief Sets value of ${field.name} field
      *@param val ${field.getParamType()} to set field to
      */
    void set${field.camel()}(${field.getParamType()} val) {poly_packet_set_field(&mPacket, ${field.globalName},(void*)  &val);}
    /**
      *@brief Gets value of ${field.name} field
      *@return ${field.getParamType()} data from field
      */
    ${field.getParamType()} get${field.camel()}() const { return * (${field.getParamType()}*)poly_packet_get_field_ptr(&mPacket, ${field.globalName});}

    % endif
    % endfor

    poly_packet_t mPacket;

  private:

#ifdef POLY_PACKET_EASY_DEBUG
    % for field in proto.fields:
    ${field.getParamType()} m${field.camel()};
    % endfor
#endif

};

typedef ${proto.camelPrefix()}Packet ${proto.camelPrefix()}Struct;

/*******************************************************************************
  Service Functions
*******************************************************************************/

class ${proto.cppFileName} {
  public:
    static bool mDescriptorsBuilt;

    /**
      *@brief Constructor for protocol service
      *@param ifaces number of interfaces to use
      */
    ${proto.cppFileName}(int interfaceCount, int spoolSize = 32);

    /**
      *@brief Destructor for protocol service
      */
    ~${proto.cppFileName}();

    /**
      *@brief registers the field/packet descriptors
      *@note needs to only be called once
      */
    static void buildDescriptors();

    /**
      *@brief registers the field/packet descriptors
      *@note needs to only be called once
      */
    static void tearDown();

    /**
      *@brief processes data in buffers
      */
    void process();

    /**
      *@brief 'Feeds' bytes to service at given interface for processing
      *@param iface index of interface to send on
      *@param data data to be processed
      *@param number of bytes
      */
    void feed(int iface, uint8_t* data, int len);

    /**
      *@brief handles json message, and shortcuts the servicing proccess. used for http requests
      *@param req incoming json message string
      *@param resp response data
      *@param number of bytes
      */
    HandlerStatus_e handleJSON(const char* req,int len, char* resp);

    /**
      *@brief 'Feeds' json message to service
      *@param iface index of interface to send on
      *@param msg data to be processed
      *@param number of bytes
      */
    void feedJSON(int iface, const char* msg, int len) {poly_service_feed_json_msg(&mService,iface,msg,len);}

    /**
      *@brief sends packet over given interface
      *@param packet packet to be sent
      *@param iface index of interface to send on
      */
    HandlerStatus_e send( int iface, ${proto.camelPrefix()}Packet& packet);

    /**
      *@brief sends packet to default interface (0)
      *@param packet packet to be sent
      */
    HandlerStatus_e send(${proto.camelPrefix()}Packet& packet);

    /**
      *@brief tells the service time has passed so it can track packets timeouts/retries on the spool
      *@param ms number of milliseconds passed
      *@note this only sets flags/statuses. Nothing is handled until the next call to process the service. So it is fine to call this from a systick handler
      */
      void tick(uint32_t ms) {poly_service_tick(&mService, ms);}

    /**
      *@brief enables/disables the auto acknowledgement function of the service
      *@param enable true enable auto acks, false disables them
      */
    void autoAck(bool enable) {mService.mAutoAck = enable;}

    /**
      *@brief enables/disables the txReady of an interface
      *@param enable true enable auto acks, false disables them
      */
    void enableTx(int iface) {mService.mInterfaces[iface].mTxReady = true;}
    void disableTx(int iface) {mService.mInterfaces[iface].mTxReady = false;}


  protected:

    virtual void txPacket(LcPacket& packet) {}
    virtual void txBytes(uint8_t* data, int len) {}

    /**
      *@brief handles packets and dispatches to handler
      *@param req incoming message
      *@param resp response to message
      *@param number of bytes
      */
    virtual HandlerStatus_e dispatch(${proto.camelPrefix()}Packet& ${proto.prefix}Request, ${proto.camelPrefix()}Packet& ${proto.prefix}Response);

    /*******************************************************************************
      Packet Handlers
    *******************************************************************************/
    % for packet in proto.packets:
    %if packet.hasResponse:
    /*@brief Handler for ${packet.name} packets */
    virtual HandlerStatus_e ${packet.name}Handler(${proto.camelPrefix()}Packet& ${proto.prefix}Request, ${proto.camelPrefix()}Packet& ${proto.prefix}Response );
    %else:
    /*@brief Handler for ${packet.name} packets */
    virtual HandlerStatus_e ${packet.name}Handler(${proto.camelPrefix()}Packet& ${proto.camelPrefix()}Request);
    %endif

    % endfor
    /*@brief Catch-All Handler for unhandled packets */
    virtual HandlerStatus_e defaultHandler(${proto.camelPrefix()}Packet& ${proto.prefix}Request, ${proto.camelPrefix()}Packet& ${proto.prefix}Response );

  private:
    ${proto.camelPrefix()}Packet mRequest;
    ${proto.camelPrefix()}Packet mResponse;
    ${proto.camelPrefix()}Packet mDespool;
    uint8_t mBuffer[512];
    uint8_t mBufferLen;

    MRT_MUTEX_TYPE mMutex;

    poly_service_t mService;

};
