#include "Comm.h"

CommMode comm_mode = BOTH_PARALLEL, current_comm;

uint8_t usart_message[MAX_MESSAGE_LENGTH],
	usart_message_length = 0,
	usb_message[MAX_MESSAGE_LENGTH],
	usb_message_length = 0;

extern void ProcessMidiData(uint8_t data, uint8_t *message, uint8_t *message_length);

inline void SetCommMode(CommMode mode)
{
	comm_mode = mode;
}

inline CommMode GetCommMode()
{
	return comm_mode;
}

inline void USARTUserTask(uint8_t data)
{
	current_comm = USART_ONLY;
	ProcessMidiData(data, usart_message, &usart_message_length);
}

inline void USBUserTask(uint8_t data)
{
	current_comm = USB_ONLY;
	ProcessMidiData(data, usb_message, &usb_message_length);
}
	
void SendByte(uint8_t data)
{
	if ((comm_mode == USART_ONLY) || (comm_mode == BOTH_MIX) || ((comm_mode == BOTH_PARALLEL) && (current_comm == USART_ONLY)))
		USARTSendByte(data);
	if ((comm_mode == USB_ONLY) || (comm_mode == BOTH_MIX) || ((comm_mode == BOTH_PARALLEL) && (current_comm == USB_ONLY)))
		USBSendByte(data);
}

void Send(uint8_t d1, uint8_t d2, uint8_t d3)
{	
	if ((comm_mode == USART_ONLY) || (comm_mode == BOTH_MIX) || ((comm_mode == BOTH_PARALLEL) && (current_comm == USART_ONLY)))
		USARTSend(d1, d2, d3);
	if ((comm_mode == USB_ONLY) || (comm_mode == BOTH_MIX) || ((comm_mode == BOTH_PARALLEL) && (current_comm == USB_ONLY)))
		USBSend(d1, d2, d3);
}

void SendSysex(unsigned char *data, unsigned char length)
{
	if ((comm_mode == USART_ONLY) || (comm_mode == BOTH_MIX) || ((comm_mode == BOTH_PARALLEL) && (current_comm == USART_ONLY)))
		USARTSendSysex(data, length);
	if ((comm_mode == USB_ONLY) || (comm_mode == BOTH_MIX) || ((comm_mode == BOTH_PARALLEL) && (current_comm == USB_ONLY)))
		USBSendSysex(data, length);
}

void SendP(const uint8_t *data, unsigned char length)
{
	if ((comm_mode == USART_ONLY) || (comm_mode == BOTH_MIX) || ((comm_mode == BOTH_PARALLEL) && (current_comm == USART_ONLY)))
		USARTSendP(data, length);
	if ((comm_mode == USB_ONLY) || (comm_mode == BOTH_MIX) || ((comm_mode == BOTH_PARALLEL) && (current_comm == USB_ONLY)))
		USBSendP(data, length);
}
