/**
  *@file ${proto.fileName}.c
  *@brief generated code for ${proto.name} packet service
  *@author make_protocol.py
  *@date ${proto.genTime}
  */

/***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************/

#include "${proto.fileName}.h"
#include <assert.h>

//Define Standard packet IDs
% for packet in proto.packets:
%if packet.standard:
#define ${packet.globalName}_ID ${packet.packetId}
%endif
% endfor

//Define Struct IDs
% for packet in proto.structs:
#define ${packet.globalName}_ID ${packet.packetId}
% endfor

//Define packet IDs
% for packet in proto.packets:
%if not packet.standard:
#define ${packet.globalName}_ID ${packet.packetId}
%endif
% endfor


//Global descriptors
% for packet in proto.packets:
poly_packet_desc_t* ${packet.globalName};
% endfor

% for packet in proto.structs:
poly_packet_desc_t* ${packet.globalName};
% endfor

% for field in proto.fields:
poly_field_desc_t* ${field.globalName};
% endfor

//Global descriptors
% for packet in proto.packets:
poly_packet_desc_t _${packet.globalName};
% endfor

% for packet in proto.structs:
poly_packet_desc_t _${packet.globalName};
% endfor

% for field in proto.fields:
poly_field_desc_t _${field.globalName};
% endfor

static poly_service_t ${proto.service()};

/*******************************************************************************
  Service Functions
*******************************************************************************/

/**
  *@brief initializes ${proto.prefix}_protocol
  *@param interfaceCount number of interfaces to create
  */
void ${proto.prefix}_service_init(int interfaceCount, int depth)
{
  //initialize core service
  poly_service_init(&${proto.service()},${len(proto.packets) + len(proto.structs)}, interfaceCount);

//Build Standard Packet Descriptors
% for packet in proto.packets:
  %if packet.standard:
  ${packet.globalName} = poly_packet_desc_init(&_${packet.globalName} ,${packet.globalName}_ID,"${packet.name}", ${len(packet.fields)});
  %endif
% endfor

//Build Struct Descriptors
% for packet in proto.structs:
  ${packet.globalName} = poly_packet_desc_init(&_${packet.globalName} ,${packet.globalName}_ID,"${packet.name}", ${len(packet.fields)});
% endfor

//Build Packet Descriptors
% for packet in proto.packets:
%if not packet.standard:
  ${packet.globalName} = poly_packet_desc_init(&_${packet.globalName} ,${packet.globalName}_ID,"${packet.name}", ${len(packet.fields)});
%endif
% endfor

  //Build Field Descriptors
% for field in proto.fields:
  ${field.globalName} = poly_field_desc_init( &_${field.globalName} ,"${field.name}", TYPE_${field.type.upper()}, ${field.arrayLen}, ${field.format.upper()});
% endfor

% for packet in proto.packets:
% if len(packet.fields) > 0:
  //Setting Field Descriptors for packet: ${packet.name}
  % for field in packet.fields:
  poly_packet_desc_add_field(${packet.globalName} , ${field.globalName} , ${str(field.isRequired).lower()} );
  % endfor
% endif

% endfor

% for packet in proto.structs:
% if len(packet.fields) > 0:
  //Setting Field Descriptors for struct: ${packet.name}
  % for field in packet.fields:
  poly_packet_desc_add_field(${packet.globalName} , ${field.globalName} , ${str(field.isRequired).lower()} );
  % endfor
% endif

% endfor

  //Register standard packet descriptors with the service
% for packet in proto.packets:
%if packet.standard:
  poly_service_register_desc(&${proto.service()}, ${packet.globalName});
%endif
% endfor

  //Register Struct descriptors with the service
% for packet in proto.structs:
  poly_service_register_desc(&${proto.service()}, ${packet.globalName});
% endfor

  //Register packet descriptors with the service
% for packet in proto.packets:
%if not packet.standard:
  poly_service_register_desc(&${proto.service()}, ${packet.globalName});
%endif
% endfor

  poly_service_start(&${proto.service()}, depth);

}


void ${proto.prefix}_service_teardown()
{
  //deinit Packet Descriptors
% for packet in proto.packets:
  ${packet.globalName} = poly_packet_desc_deinit(&_${packet.globalName});
% endfor

  //deinitialize core service
  poly_service_deinit(&${proto.service()});

}

HandlerStatus_e ${proto.prefix}_service_dispatch(${proto.prefix}_packet_t* packet, ${proto.prefix}_packet_t* response)
{

  HandlerStatus_e ${proto.prefix}_status;

  //Dispatch packet
  switch(packet->mDesc->mTypeId)
  {
    case ${proto.prefix.upper()}_PACKET_PING_ID:
      ${proto.prefix}_packet_build(response, ${proto.prefix.upper()}_PACKET_ACK);
      ${proto.prefix}_status = ${proto.prefix}_Ping_handler(packet, response);
      break;
    case ${proto.prefix.upper()}_PACKET_ACK_ID:
      ${proto.prefix}_status = ${proto.prefix}_Ack_handler(packet);
      break;
% for packet in proto.packets:
% if not packet.standard:
    case ${packet.globalName}_ID:
    %if packet.hasResponse:
     ${proto.prefix}_packet_build(response, ${packet.response.globalName});
     ${proto.prefix}_status = ${proto.prefix}_${packet.camel()}_handler(packet , response );
    %else:
      ${proto.prefix}_status = ${proto.prefix}_${packet.camel()}_handler(packet);
    %endif
      break;
% endif
% endfor
    default:
      //we should never get here
      assert(false);
      break;
  }

  //If this packet doe not have an explicit response and AutoAck is enabled, create an ack packet
  if(( ${proto.prefix.upper()}_SERVICE.mAutoAck ) & (!response->mBuilt) && (!(packet->mHeader.mToken & POLY_ACK_FLAG)))
  {
    ${proto.prefix}_packet_build(response, ${proto.prefix.upper()}_PACKET_ACK);
  }


  //If the packet was not handled, throw it to the default handler
  if(${proto.prefix}_status == PACKET_NOT_HANDLED)
  {
    ${proto.prefix}_status = ${proto.prefix}_default_handler(packet, response);
  }


  return ${proto.prefix}_status;
}

/**
  *@brief attempts to process data in buffers and parse out packets
  */
void ${proto.prefix}_service_process()
{
  static ${proto.prefix}_packet_t packet;
  static ${proto.prefix}_packet_t response;

  HandlerStatus_e ${proto.prefix}_status = PACKET_NOT_HANDLED;

  //reset states of static packets
  packet.mBuilt = false;
  packet.mSpooled = false;
  response.mSpooled = false;
  response.mBuilt = false;

  if(poly_service_try_parse(&${proto.service()}, &packet) == PACKET_VALID)
  {
    //if we get here, then the inner packet was built by the parser
    packet.mBuilt = true;

    ${proto.prefix}_status = ${proto.prefix}_service_dispatch(&packet,&response);

    //If a response has been build and the ${proto.prefix}_status was not set to ignore, we send a response on the intrface it came from
    if(( ${proto.prefix}_status == PACKET_HANDLED) && (response.mBuilt) && ((packet.mHeader.mToken & POLY_ACK_FLAG)==0))
    {
      //set response token with ack flag
			response.mHeader.mToken = packet.mHeader.mToken | POLY_ACK_FLAG;

      ${proto.prefix}_send(packet.mInterface , &response);
    }

    //Clean the packets
    ${proto.prefix}_clean(&packet);
    ${proto.prefix}_clean(&response);
  }

  //despool any packets ready to go out
  poly_service_despool(&${proto.service()});

}


void ${proto.prefix}_service_register_bytes_tx( int iface, poly_tx_bytes_callback txBytesCallBack)
{
  poly_service_register_bytes_tx_callback(&${proto.service()}, iface,txBytesCallBack);
}

void ${proto.prefix}_service_register_packet_tx( int iface, poly_tx_packet_callback txPacketCallBack)
{
  poly_service_register_packet_tx_callback(&${proto.service()}, iface,txPacketCallBack);
}

void ${proto.prefix}_service_feed(int iface, uint8_t* data, int len)
{
  poly_service_feed(&${proto.service()},iface,data,len);
}

void ${proto.prefix}_service_set_retry(int iface, uint16_t retries, uint32_t timeoutMs)
{
  poly_service_set_retry(&${proto.service()}, iface,  retries,  timeoutMs);
}

HandlerStatus_e ${proto.prefix}_handle_json(const char* req, int len, char* resp)
{
  ${proto.prefix}_packet_t packet;
  ${proto.prefix}_packet_t response;

  //reset states of static packets
  HandlerStatus_e ${proto.prefix}_status = PACKET_NOT_HANDLED;
  packet.mBuilt = false;
  packet.mSpooled = false;
  response.mSpooled = false;
  response.mBuilt = false;


  if(poly_service_parse_json(&${proto.service()}, &packet, req, len) == PACKET_VALID)
  {
    //if we get here, then the inner packet was built by the parser
    packet.mBuilt = true;

    ${proto.prefix}_status = ${proto.prefix}_service_dispatch(&packet,&response);


    //If a response has been build and the ${proto.prefix}_status was not set to ignore, we send a response on the intrface it came from
    if(( ${proto.prefix}_status == PACKET_HANDLED) && (response.mBuilt) )
    {
      //set response token with ack flag
			response.mHeader.mToken = packet.mHeader.mToken | POLY_ACK_FLAG;
      poly_packet_print_json(&response, resp, false);
    }

    //Clean the packets
    ${proto.prefix}_clean(&packet);
    ${proto.prefix}_clean(&response);

  }
  else
  {
    MRT_SPRINTF(resp,"{\"Error\":\"Parsing Error\"}");
  }

  return ${proto.prefix}_status;
}


void ${proto.prefix}_service_feed_json(int iface, const char* msg, int len)
{
  poly_service_feed_json_msg(&${proto.service()},iface,msg,len);
}


void ${proto.prefix}_tick(uint32_t ms)
{
  poly_service_tick(&${proto.service()}, ms);
}

void ${proto.prefix}_auto_ack(bool enable)
{
  ${proto.service()}.mAutoAck = enable;
}

void ${proto.prefix}_enable_tx(int iface)
{
  ${proto.service()}.mInterfaces[iface].mTxReady = true;
}

void ${proto.prefix}_disable_tx(int iface)
{
  ${proto.service()}.mInterfaces[iface].mTxReady = false;
}


/*******************************************************************************
  Meta packet
*******************************************************************************/

/**
  *@brief initializes a new {proto.prefix}_packet_t
  *@param desc ptr to packet descriptor to model packet from
  */
void ${proto.prefix}_packet_build(${proto.prefix}_packet_t* packet, poly_packet_desc_t* desc)
{
  //create new allocated packet
  poly_packet_build(packet, desc, true);
  packet->mAckType = ACK_TYPE_NONE;
  packet->mBuilt = true;
  packet->mSpooled = false;
}


/**
  *@brief frees memory allocated for metapacket
  *@param packet ptr to metaPacket
  *
  */
void ${proto.prefix}_clean(${proto.prefix}_packet_t* packet)
{
  //If the packet has been spooled, the spool is responsible for it now
  if(packet->mBuilt && (!packet->mSpooled))
  {
    poly_packet_clean(packet);
  }

}

HandlerStatus_e ${proto.prefix}_send(int iface, ${proto.prefix}_packet_t* packet)
{
  HandlerStatus_e ${proto.prefix}_status;

  ${proto.prefix}_status = poly_service_spool(&${proto.service()}, iface, packet);

  if(${proto.prefix}_status == PACKET_SPOOLED)
  {
    packet->mSpooled = true;
  }

  return ${proto.prefix}_status;
}


/*******************************************************************************

  Meta-Packet setters

*******************************************************************************/

% for field in proto.fields:
%if field.isArray:
  %if field.isString:
/**
  *@brief Sets value(s) in ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@param val ${field.getParamType()} to copy data from
  */
void ${proto.prefix}_set${field.camel()}(${proto.prefix}_packet_t* packet, const ${field.getParamType()} val)
  %else:
/**
  *@brief Sets value(s) in ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@param val ${field.getParamType()} to copy data from
  */
void ${proto.prefix}_set${field.camel()}(${proto.prefix}_packet_t* packet, const ${field.getParamType()} val, uint32_t len)
  % endif
% else:
/**
  *@brief Sets value of ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@param val ${field.getParamType()} to set field to
  */
void ${proto.prefix}_set${field.camel()}(${proto.prefix}_packet_t* packet, ${field.getParamType()} val)
%endif
{
%if field.isArray:
  %if field.isString:
  poly_packet_set_field(packet, ${field.globalName}, val, strlen(val));
  %else:
  poly_packet_set_field(packet, ${field.globalName}, val, len);
  %endif
% else:
  poly_packet_set_field(packet, ${field.globalName}, &val, 1);
% endif
}

% endfor

/*******************************************************************************
  Meta-Packet getters
*******************************************************************************/

% for field in proto.fields:
%if field.isArray:
/**
  *@brief Gets value of ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@return ${field.getParamType()} of data in field
  */
%else:
/**
  *@brief Gets value of ${field.name} field
  *@param packet ptr to ${proto.prefix}_packet
  *@return ${field.getParamType()} data from field
  */
%endif
%if field.isArray:
uint32_t ${proto.prefix}_get${field.camel()}(${proto.prefix}_packet_t* packet, ${field.getParamType()} val)
{
  return poly_packet_get_field(packet, ${field.globalName}, val);
}
% else:
${field.getParamType()} ${proto.prefix}_get${field.camel()}(${proto.prefix}_packet_t* packet)
{
  ${field.getParamType()} val;
  poly_packet_get_field(packet, ${field.globalName}, &val );
  return val;
}
% endif

% endfor

/*******************************************************************************
  Quick send functions
*******************************************************************************/

HandlerStatus_e ${proto.prefix}_sendPing(int iface)
{
  HandlerStatus_e ${proto.prefix}_status;
  //create packet
  ${proto.prefix}_packet_t packet;
  ${proto.prefix}_packet_build(&packet, ${proto.prefix.upper()}_PACKET_PING);
  ${proto.prefix}_setIcd(&packet,  ${proto.prefix.upper()}_SERVICE_HASH );

  ${proto.prefix}_status = ${proto.prefix}_send(iface,&packet); //send packet
  ${proto.prefix}_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return ${proto.prefix}_status;
}

% for packet in proto.packets:
%if not packet.standard:
HandlerStatus_e ${proto.prefix}_send${packet.camel()}(int iface\
  %for idx,field in enumerate(packet.fields):
  %if field.isRequired:
,\
  %if field.isArray:
    %if field.isString:
 const ${field.getParamType()} ${field.name}\
    %else:
 const ${field.getParamType()} ${field.name} ,int ${field.name}_len \
    %endif
  %else:
 ${field.getParamType()} ${field.name}\
  %endif
  %endif
  %endfor
)
{
  HandlerStatus_e ${proto.prefix}_status;
  //create packet
  ${proto.prefix}_packet_t packet;
  ${proto.prefix}_packet_build(&packet,${packet.globalName});

  //set fields
  %for field in packet.fields:
  %if field.isRequired:
  %if field.isArray and not field.isString:
  ${proto.prefix}_set${field.camel()}(&packet, ${field.name}, ${field.name}_len);
  %else:
  ${proto.prefix}_set${field.camel()}(&packet, ${field.name});
  %endif
  %endif
  %endfor

  ${proto.prefix}_status = ${proto.prefix}_send(iface,&packet); //send packet
  ${proto.prefix}_clean(&packet); //This will only free the underlying packet if the spooling was unsuccessful
  return ${proto.prefix}_status;
}
%endif
% endfor


/*******************************************************************************
  Weak packet handlers

  Do not modify these, just create your own without the '__weak' attribute
*******************************************************************************/
/**
  *@brief Handler for receiving ping packets
  *@param ${proto.prefix}_ping ptr to incoming ping packet
  *@param ${proto.prefix}_ack ptr to repsonding ack
  *@return PACKET_HANDLED
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_Ping_handler(${proto.prefix}_packet_t* ${proto.prefix}_ping, ${proto.prefix}_packet_t* ${proto.prefix}_ack)
{
  /* Ack token has already been set as ping token with POLY_ACK_FLAG*/
  uint32_t icd_hash = ${proto.prefix}_getIcd(${proto.prefix}_ping);
  ${proto.prefix}_setIcd(${proto.prefix}_ack, ${proto.prefix.upper()}_SERVICE_HASH);
  
  //assert(icd_hash == ${proto.prefix.upper()}_SERVICE_HASH );

  return PACKET_HANDLED;
}

/**
  *@brief Handler for receiving ack packets
  *@param ${proto.prefix}_ack ptr to ack
  *@return PACKET_HANDLED
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_Ack_handler(${proto.prefix}_packet_t* ${proto.prefix}_ack)
{
  return PACKET_HANDLED;
}

% for packet in proto.packets:
%if not packet.standard:
%if not packet.hasResponse:
/**
  *@brief Handler for receiving ${packet.name} packets
  *@param ${packet.name} incoming ${packet.name} packet
  *@return handling ${proto.prefix}_status
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_${packet.camel()}_handler(${proto.prefix}_packet_t* ${proto.prefix}_${packet.name})
%else:
/**
  *@brief Handler for receiving ${packet.name} packets
  *@param ${packet.name} incoming ${packet.name} packet
  *@param ${packet.response.name} ${packet.response.name} packet to respond with
  *@return handling ${proto.prefix}_status
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_${packet.camel()}_handler(${proto.prefix}_packet_t* ${proto.prefix}_${packet.name}, ${proto.prefix}_packet_t* ${proto.prefix}_${packet.response.name})
%endif
{
  /*  Get Required Fields in packet */
% for field in packet.fields:
  //${field.getDeclaration()};  //${field.desc}
%endfor

% for field in packet.fields:

  %if field.isArray:
    %if field.isString:
  //${proto.prefix}_get${field.camel()}(${proto.prefix}_${packet.name}, ${field.name});
    %else :
  //${proto.prefix}_get${field.camel()}(${proto.prefix}_${packet.name}, ${field.name},  );
    %endif
  %else:
  //${field.name} = ${proto.prefix}_get${field.camel()}(${proto.prefix}_${packet.name});
  %endif
% endfor
%if packet.hasResponse:
  /*    Set required Fields in response  */
% for field in packet.response.fields:
  //${proto.prefix}_set${field.camel()}(${proto.prefix}_${packet.response.name}, value );  //${field.desc}
%endfor
%endif


  /* NOTE : This function should not be modified! If needed,  It should be overridden in the application code */

  return PACKET_NOT_HANDLED;
}

%endif
% endfor


/**
  *@brief catch-all handler for any packet not yet handled
  *@param ${proto.prefix}_packet ptr to incoming message
  *@param ${proto.prefix}_response ptr to response
  *@return handling ${proto.prefix}_status
  */
__attribute__((weak)) HandlerStatus_e ${proto.prefix}_default_handler( ${proto.prefix}_packet_t * ${proto.prefix}_packet, ${proto.prefix}_packet_t* ${proto.prefix}_response)
{

  /* NOTE : This function should not be modified, when the callback is needed,
          ${proto.prefix}_default_handler  should be implemented in the user file
  */

  return PACKET_NOT_HANDLED;
}
