
// anthrm - http://mylcd.sourceforge.net/
// An LCD framebuffer and text rendering API
// Michael McElligott
// okio@users.sourceforge.net

//  Copyright (c) 2005-2011  Michael McElligott
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU LIBRARY GENERAL PUBLIC LICENSE
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU LIBRARY GENERAL PUBLIC LICENSE for more details.
//
//	You should have received a copy of the GNU Library General Public
//	License along with this library; if not, write to the Free
//	Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include <stdio.h>
#include <ctype.h>
#include "garminhr.h"
#include "console.h"
#include "libantplus.h"

#ifdef __cplusplus
 extern "C" {
#endif

static const unsigned char antkeys[KEY_TOTAL][KEY_SIZE] = {
	{0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45},	// Ant+ sport key
	{0xB9,0xAD,0x32,0x28,0x75,0x7E,0xC7,0x4D},	// Suunto
	{0xA8,0xA4,0x23,0xB9,0xF5,0x5E,0x63,0xC1},	// Garmin
	{00,00,00,00,00,00,00,00}					// Ant+ key
};


static const char astrStatus[5][28] = {
	"STATUS_UNASSIGNED_CHANNEL",
	"STATUS_ASSIGNED_CHANNEL",
	"STATUS_SEARCHING_CHANNEL",
	"STATUS_TRACKING_CHANNEL",
	"UNKNOWN STATUS STATE"
};

/*
void dumpPayload (const unsigned char *payLoad, const size_t dataLength)
{
	dbprintf("payload { ");
	for (int i = 0; i < dataLength; i++)
		dbprintf("0x%.2X ", payLoad[i]);
	dbprintf("}\n");

}*/

int channelEvent (const int chan, const int eventId, const unsigned char *payLoad, const size_t dataLength, void *uPtr)
{
	//dbprintf(" $ chan event: chan:%i, msgId:0x%.2X, payLoad:%p, dataLen:%i, uPtr:%p\n", chan, eventId, payLoad, (int)dataLength, uPtr);
	//dumpPayload(payLoad, dataLength);

	THR *hr = (THR*)uPtr;
	TLIBANTPLUS *ant = hr->ant;
		
	switch (eventId){
	  case EVENT_RX_SEARCH_TIMEOUT:
	  	dbprintf(" $ event RX search timeout\n");
	  	break;
	  	
	  case EVENT_RX_FAIL:
	  	//dbprintf(" $ event RX fail\n");
	  	break;
	  	
	  case EVENT_TX:
	  	dbprintf(" $ event TX\n");
	  	break;
	  	
	  case EVENT_RX_BROADCAST:
	  	//dbprintf(" $ event RX broadcast ");
	  	if (!hr->cstates->chanIdOnce)
      {
	  		hr->cstates->chanIdOnce = 1;
	  		libantplus_RequestMessage(ant, hr->dcfg->channel, MESG_CHANNEL_ID_ID);
	  	}

		  hr->rate->currentSequence = payLoad[STREAM_RXBROADCAST_DEV120_SEQ];
		  if (hr->rate->currentSequence != hr->rate->previousSequence)
      {
	  		memcpy(&hr->rate->bpm[0], &hr->rate->bpm[1], (HRBMP_BUFFERLENGTH*sizeof(unsigned char))-sizeof(unsigned char));
  			hr->rate->currentBmp = payLoad[STREAM_RXBROADCAST_DEV120_HR];
	  		hr->rate->bpm[HRBMP_BUFFERLENGTH-1] = hr->rate->currentBmp;

	  		if (hr->rate->currentBmp < hr->rate->low)
        {
				  hr->rate->low = hr->rate->currentBmp;
        }
			
        if (hr->rate->currentBmp > hr->rate->high)
        {
				  hr->rate->high = hr->rate->currentBmp;
        }

	  		//dbprintf("Seq: %i HR: %i\n", hr->rate->currentSequence, hr->rate->currentBmp);
	  	}
		  hr->rate->previousSequence = hr->rate->currentSequence;
		  break;
	 }	
	return 1;
}

int responseEvent (const int chan, const int msgId, const unsigned char *payLoad, const size_t dataLength, void *uPtr)
{
	//dbprintf(" # response event: msgId:0x%.2X, payLoad:%p, dataLen:%i, uPtr:%p\n", msgId, payLoad, dataLength, uPtr);

	THR *hr = (THR*)uPtr;
	TLIBANTPLUS *ant = hr->ant;

	switch (msgId){
	  case MESG_EVENT_ID:
	  	//dbprintf(" * event\n");
	  	channelEvent(chan, payLoad[STREAM_EVENT_EVENTID], payLoad, dataLength, uPtr);
	  	break;
	  	
	  case MESG_NETWORK_KEY_ID: 
	  	dbprintf(" * network key accepted\n");
	  	// master
	  	//libantplus_AssignChannel(ant, hr->dcfg->channel, PARAMETER_TX_NOT_RX, 0);
	  	// slave
	  	libantplus_AssignChannel(ant, hr->dcfg->channel, hr->dcfg->channelType, hr->dcfg->networkNumber);
	  	break;

	  case MESG_ASSIGN_CHANNEL_ID:
	  	dbprintf(" * channel assign accepted\n");
	  	libantplus_SetChannelPeriod(ant, hr->dcfg->channel, hr->dcfg->channelPeriod);
	  	break; 
	  	
	  case MESG_CHANNEL_MESG_PERIOD_ID:
		  dbprintf(" * channel mesg period accepted\n");
		  libantplus_SetChannelSearchTimeout(ant, hr->dcfg->channel, hr->dcfg->searchTimeout);
		  break;
		
	  case MESG_CHANNEL_SEARCH_TIMEOUT_ID:
	  	dbprintf(" * search timeout period accepted\n");
	  	libantplus_SetChannelRFFreq(ant, hr->dcfg->channel, hr->dcfg->RFFreq);
	  	break;
	  	
	  case MESG_CHANNEL_RADIO_FREQ_ID:
	  	dbprintf(" * radio freq accepted\n");
	  	libantplus_SetSearchWaveform(ant, hr->dcfg->channel, hr->dcfg->searchWaveform);
	  	break;
	  	
	  case MESG_SEARCH_WAVEFORM_ID:
	  	dbprintf(" * search waveform accepted\n");
	  	libantplus_SetChannelId(ant, hr->dcfg->channel, hr->dcfg->deviceNumber, hr->dcfg->deviceType, hr->dcfg->transType);
	  	break; 
	  	
	  case MESG_CHANNEL_ID_ID:
	  	dbprintf(" * set channel id accepted\n");
	  	libantplus_OpenChannel(ant, hr->dcfg->channel);
	  	break; 
	  	
	  case MESG_OPEN_CHANNEL_ID:
	  	dbprintf(" * open channel accepted\n");
	  	hr->cstates->channelStatus = 1;
	  	libantplus_RequestMessage(ant, hr->dcfg->channel, MESG_CHANNEL_STATUS_ID);
	  	libantplus_RequestMessage(ant, hr->dcfg->channel, MESG_CAPABILITIES_ID);
	  	break;
	  	
 	  case MESG_UNASSIGN_CHANNEL_ID:
		  dbprintf(" * channel Unassigned\n");
		  break;

	  case MESG_CLOSE_CHANNEL_ID:
		  dbprintf(" * channel closed\n");
		  hr->cstates->channelStatus = 0;
		  break;
		
 	  case CHANNEL_IN_WRONG_STATE:          
		  dbprintf(" * channel in wrong state\n");
		  break;

 	  case CHANNEL_NOT_OPENED:              
		  dbprintf(" * channel not opened\n");
		  break;
		
 	  case CHANNEL_ID_NOT_SET: //??             
		  dbprintf(" * channel ID not set\n");
		  break;
		
 	  case CLOSE_ALL_CHANNELS: // Start RX Scan mode              
		  dbprintf(" * close all channels\n");
		  break;
		
 	  case TRANSFER_IN_PROGRESS: // TO ack message ID            
		  dbprintf(" * tranfer in progress\n");
		  break;
		
 	  case TRANSFER_SEQUENCE_NUMBER_ERROR:  
		  dbprintf(" * transfer sequence number error\n");
		  break;
		
 	  case TRANSFER_IN_ERROR:               
		  dbprintf(" * transfer in error\n");
		  break;
		
 	  case INVALID_MESSAGE:                 
		  dbprintf(" * invalid message\n");
		  break;
		
 	  case INVALID_NETWORK_NUMBER:          
		  dbprintf(" * invalid network number\n");
		  break;
		
 	  case INVALID_LIST_ID:                 
		  dbprintf(" * invalid list ID\n");
		  break;
		
 	  case INVALID_SCAN_TX_CHANNEL:         
		  dbprintf(" * invalid Scanning transmit channel\n");
		  break;
		
 	  case INVALID_PARAMETER_PROVIDED:      
		  dbprintf(" * invalid parameter provided\n");
   		break;
   		
 	  case EVENT_QUE_OVERFLOW:              
		  dbprintf(" * queue overflow\n");
		  break;
		
	  default:
	  	dbprintf(" #### unhandled response id %i\n", msgId);
	};
	  	
	return 1;
}

int messageEventCb (const int chan, const int msgId, const unsigned char *payLoad, const size_t dataLength, void *uPtr)
{
	//dbprintf(" @ msg event cb: Id:0x%.2X, payLoad:%p, len:%i, ptr:%p\n", msgId, payLoad, (int)dataLength, uPtr);
	//dumpPayload(payLoad, dataLength);

	THR *hr = (THR*)uPtr;
	TLIBANTPLUS *ant = hr->ant;
		
	switch(msgId)
  {
	  case MESG_BROADCAST_DATA_ID:
  	  //dbprintf(" @ broadcast data \n");
	    //dumpPayload(payLoad, dataLength);
	    channelEvent(chan, EVENT_RX_BROADCAST, payLoad, dataLength, uPtr);
  	  break;


	  case MESG_STARTUP_MESG_ID: 
	  	dbprintf(" @ start up mesg reason: 0x%X", payLoad[STREAM_STARTUP_REASON]);
		  libantplus_SetNetworkKey(ant, hr->dcfg->networkNumber, antkeys[hr->dcfg->keyIdx]);
		  break;
		
	  case MESG_RESPONSE_EVENT_ID:
	  	responseEvent(payLoad[STREAM_EVENT_CHANNEL_ID], payLoad[STREAM_EVENT_RESPONSE_ID], payLoad, dataLength, uPtr);
	  	break;
	  	
	  case MESG_CHANNEL_STATUS_ID:
      {
        int ucStatusByte;

	  	  dbprintf(" @ channel status for channel %i is %i", payLoad[STREAM_CHANNEL_ID], payLoad[STREAM_CHANNEL_STATUS]);
         
		    ucStatusByte = payLoad[STREAM_CHANNELSTATUS_STATUS] & STATUS_CHANNEL_STATE_MASK;
		    dbprintf("STATUS: %s\n",astrStatus[ucStatusByte]); 
      }
	  	break;
	  	
	  case MESG_CAPABILITIES_ID:
		  dbprintf(" @ capabilities:\n");
		  dbprintf("   Max ANT Channels: %i\n",payLoad[STREAM_CAP_MAXCHANNELS]);
		  dbprintf("   Max ANT Networks: %i\n",payLoad[STREAM_CAP_MAXNETWORKS]);
		  dbprintf("   Std. option: 0x%X\n",payLoad[STREAM_CAP_STDOPTIONS]);
		  dbprintf("   Advanced: 0x%X\n",payLoad[STREAM_CAP_ADVANCED]);
		  dbprintf("   Advanced2: 0x%X\n",payLoad[STREAM_CAP_ADVANCED2]);
	  	break;
	  
	  
	  case MESG_CHANNEL_ID_ID:
      hr->dev->scidDeviceNumber = payLoad[STREAM_CHANNELID_DEVNO_LO] | (payLoad[STREAM_CHANNELID_DEVNO_HI] << 8);
	    hr->dev->scidDeviceType = payLoad[STREAM_CHANNELID_DEVTYPE];
	    hr->dev->scidTransType = payLoad[STREAM_CHANNELID_TRANTYPE];
	    dbprintf(" @ CHANNEL ID: (%i/%i/%i)", hr->dev->scidDeviceNumber, hr->dev->scidDeviceType, hr->dev->scidTransType);        
           
	    if (hr->dev->scidDeviceType != hr->dcfg->deviceType)
      {
		    dbprintf(" @ CHANNEL ID: this is not the device we're looking for");
		    dbprintf(" @ CHANNEL ID: expecting 0x%X but found 0x%X", hr->dcfg->deviceType, hr->dev->scidDeviceType);
       	libantplus_CloseChannel(ant, hr->dcfg->channel);
       	return 0;
	    }
		  //dumpPayload(payLoad, dataLength);
		  break;
		
	  case MESG_VERSION_ID:
		  dbprintf(" @ version: '%s'", (char*)&payLoad[STREAM_VERSION_STRING]);
		  break;
	};
	
	return 1;
}

