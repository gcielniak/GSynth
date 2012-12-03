#include "USB.h"
#include "LUFADescriptors.h"

extern void USBUserTask(uint8_t data);
extern uint8_t RXQueueSpace();
extern void RXQueuePush(uint8_t data);
extern uint8_t USBRXSpace();
extern void USBRXPush(uint8_t data);

void USBReceiveTask();
void USBSendTask();

/** LUFA MIDI Class driver interface configuration and state information. This structure is
 *  passed to all MIDI Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_MIDI_Device_t MIDI_Device =
	{
		.Config =
			{
				.StreamingInterfaceNumber = 1,
				.DataINEndpoint           =
					{
						.Address          = MIDI_STREAM_IN_EPADDR,
						.Size             = MIDI_STREAM_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint          =
					{
						.Address          = MIDI_STREAM_OUT_EPADDR,
						.Size             = MIDI_STREAM_EPSIZE,
						.Banks            = 1,
					},
			},
	};

void USBInit() { USB_Init(); }

uint8_t USBDeviceReady() { return 1; }

void USBDeviceSend(uint8_t* data) 
{
	MIDI_Device_SendEventPacket(&MIDI_Device, (MIDI_EventPacket_t*)data);
	MIDI_Device_Flush(&MIDI_Device);
}

void USBDeviceTask()
{
	MIDI_Device_USBTask(&MIDI_Device);
	USB_USBTask();
}

//*** RX Section ***//
void USBReceiveTask()
{	
	MIDI_EventPacket_t RXMIDIEvent;

	if (MIDI_Device_ReceiveEventPacket(&MIDI_Device, &RXMIDIEvent))
	{
		switch (RXMIDIEvent.Event)
		{
		case 0x3: case 0x4: case 0x7: case 0x8:
		case 0x9: case 0xA: case 0xB: case 0xE:
			if (USBRXSpace() >= 3)
			{
				USBRXPush(RXMIDIEvent.Data1);
				USBRXPush(RXMIDIEvent.Data2);
				USBRXPush(RXMIDIEvent.Data3);
			}
			break;
		case 0x2: case 0x6: case 0xC: case 0xD:
			if (USBRXSpace() >= 2)
			{
				USBRXPush(RXMIDIEvent.Data1);
				USBRXPush(RXMIDIEvent.Data2);
			}
			break;
		case 0x5: case 0xF:
			USBRXPush(RXMIDIEvent.Data1);
			break;
		default:
			break;
		}
	}
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void) {}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void) {}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void) { 	MIDI_Device_ConfigureEndpoints(&MIDI_Device); }

/** Event handler for the library USB Unhandled Control Request event. */
void EVENT_USB_Device_UnhandledControlRequest(void) { MIDI_Device_ProcessControlRequest(&MIDI_Device); }
