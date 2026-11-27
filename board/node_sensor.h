/* node_sensor.h - the two functions the sensor part provides (node_sensor.c). */
#ifndef NODE_SENSOR_H_
#define NODE_SENSOR_H_

#include <stdbool.h>

/* Called once at start-up. */
void node_sensor_init(void);

/* Called every 2 s. true = someone is in the room. */
bool node_is_occupied(void);

#endif /* NODE_SENSOR_H_ */
