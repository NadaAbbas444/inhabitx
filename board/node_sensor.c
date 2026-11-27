/*
 * node_sensor.c - YOUR PART: read the sensor and decide "someone there or not".
 *
 * This is the only file you need to fill in. Everything else (Wi-Fi, the
 * secure connection to the broker, sending the message every 2 s,
 * reconnecting) is already done in publisher_task.c and Infineon's example.
 *
 * How it is used:
 *   node_sensor_init()    is called once, when the board starts.
 *   node_is_occupied()    is called every 2 seconds. Whatever it returns is
 *                         sent to the cloud as "occupied": true / false.
 *
 * Rules (README §4):
 *   - return true  after about 3 s of presence
 *   - return false only after about 20 s without presence (demo value;
 *     the real value is 2 min), so a person sitting still or quietly
 *     doesn't "disappear"
 *   - only true/false leaves the board: never send raw sensor data or audio
 *
 * Tip: node_is_occupied() must return quickly (well under 2 s). If your
 * sensor needs to be read more often than every 2 s, read it in your own
 * task or timer and let node_is_occupied() just return the latest decision.
 */
#include "node_sensor.h"

/* ---- add your includes here (sensor driver, your logic, ...) ---------- */


/* ---- add your variables here ------------------------------------------ */


void node_sensor_init(void)
{
    /* TODO: set up your sensor here (pins, I2C/SPI/PDM, your filters, ...).
     *       Leave it empty if your sensor needs no setup. */
}

bool node_is_occupied(void)
{
    /* TODO: read your sensor and run your occupancy logic here.
     *       return true  = someone is in the room
     *       return false = the room is empty
     *
     * Until you add your code, the room is always reported as empty. */
    return false;
}
