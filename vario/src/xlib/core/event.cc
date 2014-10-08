#include "event.h"

void EventSetSource(uint8_t event_channel, uint8_t event_source)
{
	if (event_channel > 7) return;

	volatile uint8_t * chMux;
	chMux = &EVSYS.CH0MUX + event_channel;
	*chMux = event_source;
}
