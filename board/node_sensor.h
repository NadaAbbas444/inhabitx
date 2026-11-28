/* node_sensor.h - what the Wi-Fi core needs from the sensor part (node_sensor.c). */
#ifndef NODE_SENSOR_H_
#define NODE_SENSOR_H_

#include <stdbool.h>

/* Called once at start-up. */
void node_sensor_init(void);

/* Called every 2 s, before node_is_occupied(). false = no reliable answer
 * right now (warming up, audio gap, audio core silent): nothing is sent,
 * and after ~10 s the main shows the room as offline instead of empty. */
bool node_sensor_ready(void);

/* Called every 2 s when ready. true = someone is in the room. */
bool node_is_occupied(void);

#endif /* NODE_SENSOR_H_ */
