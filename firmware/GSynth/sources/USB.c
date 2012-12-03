#include "USB.h"

extern void USBUserTask(uint8_t data);
extern void USBReceiveTask();
extern void USBDeviceTask();
extern uint8_t USBDeviceReady();
extern void USBDeviceSend(uint8_t* data);

void USBParseSendTask();

uint8_t	usb_rx_buffer[USB_RX_BUFFER_SIZE],
	usb_rx_start = 0,
	usb_rx_end = 0,
	usb_rx_length = 0;

uint8_t	usb_tx_buffer[USB_TX_BUFFER_SIZE],
	usb_tx_start = 0,
	usb_tx_end = 0,
	usb_tx_length = 0,
	usb_tx_pgm_length = 0;
const uint8_t* usb_tx_pgm = 0;

uint8_t TXMIDIEvent[EVENT_LENGTH] = {0,0,0,0};
uint8_t RTMIDIEvent[EVENT_LENGTH] = {0,0,0,0};
uint8_t packet_ready = 0;
uint8_t	data_count = 0;				

void USBTask()
{
	//Receive data
	USBReceiveTask();
	
	//Process the received data
	if (usb_rx_length)
	{
		USBUserTask(usb_rx_buffer[usb_rx_start++]);
		usb_rx_start %= USB_RX_BUFFER_SIZE;
		usb_rx_length--;
	}
	
	//Send data
	USBParseSendTask();

	//Device specific routines
	USBDeviceTask();	
}

//*** RX Section ***//
//push data onto RX fifo queue 
void USBRXPush(uint8_t data)
{
	if (usb_rx_length < USB_RX_BUFFER_SIZE)
	{
		usb_rx_buffer[usb_rx_end++] = data;
		usb_rx_end %= USB_RX_BUFFER_SIZE;
		usb_rx_length++;	
	}	
}

//return the remaining free rx space
uint8_t USBRXSpace() { return (USB_RX_BUFFER_SIZE - usb_rx_length); }

//*** TX Section ***//
void USBParseSendTask()
{
	if (!packet_ready)
	{
		uint8_t data;
		
		if (usb_tx_pgm_length && !packet_ready)
		{
			data = pgm_read_byte(usb_tx_pgm++);
			usb_tx_pgm_length--;
		}
		else if (usb_tx_length && !packet_ready)
		{
			//pop the data from the tx fifo queue
			data = usb_tx_buffer[usb_tx_start++];
			usb_tx_start %= USB_TX_BUFFER_SIZE;
			usb_tx_length--;
		}
		else
			return;
		
		if (data > 0xF7) //real-time messages
		{
			RTMIDIEvent[0] = 0x0F;
			RTMIDIEvent[1] = data;
			packet_ready = 1;
		}
		else
		{
			if (data > 0x7F) //status byte
			{
				if (data == 0xF7) // handle end of sysex message
				{
					if (TXMIDIEvent[0] == 0x4)
					{
						if (data_count == 0)
							TXMIDIEvent[0] = 0x5;
						else if (data_count == 1)
							TXMIDIEvent[0] = 0x6;
						else if (data_count == 2)
							TXMIDIEvent[0] = 0x7;					
						packet_ready = 1;
					}
				}
				else
				{
					data_count = 0;
			
					//assign the right usb command
					if (data == 0xF0)
						TXMIDIEvent[0] = 0x4;
					else if ((data == 0xF1) || (data == 0xF3))
						TXMIDIEvent[0] = 0x2;
					else if (data == 0xF2)
						TXMIDIEvent[0] = 0x3;
					else 			
						TXMIDIEvent[0] = data >> 4;
				}
			}
			else //data byte
			{
				if ((data_count == 1) && ((TXMIDIEvent[0] == 0x2) || (TXMIDIEvent[0] == 0xC) || (TXMIDIEvent[0] == 0xD)))
					packet_ready = 1;
				else if ((data_count == 2) && ((TXMIDIEvent[0] == 0x3) || (TXMIDIEvent[0] == 0x4)
					|| (TXMIDIEvent[0] == 0x8) || (TXMIDIEvent[0] == 0x9) || (TXMIDIEvent[0] == 0xA)
					|| (TXMIDIEvent[0] == 0xB) || (TXMIDIEvent[0] == 0xE)))
					packet_ready = 1;
			}

			data_count++;
			TXMIDIEvent[data_count] = data;					
		}
	}	

	//try to send a packet
	if (packet_ready && USBDeviceReady())
	{
		if (RTMIDIEvent[0])
		{
			USBDeviceSend(RTMIDIEvent);
			RTMIDIEvent[0] = 0;			
		}
		else
		{
			USBDeviceSend(TXMIDIEvent);
			if (TXMIDIEvent[0] == 0x4) //parse long sysex messages 
				data_count = 0;
			else if ((TXMIDIEvent[0] > 0x7) && (TXMIDIEvent[0] < 0xF)) // this implements the running status
				data_count = 1;
		}
		packet_ready = 0;
	}
}

//return the remaining free space
static inline uint8_t USBTXSpace() { return (USB_TX_BUFFER_SIZE - usb_tx_length); }

void USBSendByte(uint8_t data)
{
	if (usb_tx_length < USB_TX_BUFFER_SIZE)
	{
		usb_tx_buffer[usb_tx_end++] = data;
		usb_tx_end %= USB_TX_BUFFER_SIZE;
		usb_tx_length++;
	}			
}

void USBSend(uint8_t d1, uint8_t d2, uint8_t d3)
{
	switch (d1 >> 4)
	{
	case 0x8: case 0x9: case 0xA: case 0xB: case 0xE:
		if (USBTXSpace() >= 3)
		{
			USBSendByte(d1); 
			USBSendByte(d2); 
			USBSendByte(d3);
		}			
		break;
	case 0xC: case 0xD:
		if (USBTXSpace() >= 2)
		{
			USBSendByte(d1); 
			USBSendByte(d2);
		}			
		break;
	case 0xF:
		USBSendByte(d1);
		break;
	default: 
		break;
	}
}

void USBSendSysex(unsigned char *data, unsigned char length)
{
	if (USBTXSpace() >= (length + 3))
	{
		USBSendByte(0xF0);
		USBSendByte(VENDOR_ID);
		for (uint8_t i = 0; i < length; i++)
			USBSendByte(*data++);
		USBSendByte(0xF7);
	}
}

void USBSendP(const uint8_t *data, unsigned char length)
{
	if (!usb_tx_pgm_length && !usb_tx_length)
	{
		usb_tx_pgm = data;
		usb_tx_pgm_length = length;
	}
}
