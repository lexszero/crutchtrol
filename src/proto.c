#include "common.h"

#define MAX_EVENTS 16

typedef struct {
	uint8_t chan;
	int8_t val;
} __attribute__((packed)) ev_t;

ev_t ev_buf[MAX_EVENTS];
uint8_t ev_count;

void task_events(void *arg)
{
	(void)arg;
	while (1) {
		
	}
}
