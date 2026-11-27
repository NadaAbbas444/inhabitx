/*
 * main_rooms.h - the rooms of building Z and the main's last will.
 *
 * If you change the rooms, change MAIN_WILL_PAYLOAD to match (every room
 * "offline", with its seats and floor). test_main_app.c checks that they do.
 */
#ifndef MAIN_ROOMS_H_
#define MAIN_ROOMS_H_

/* id, floor, seats (seats from the AAU campus plan) */
#define MAIN_ROOMS \
    { "Z.1.08", 1, 40 }, { "Z.1.09", 1, 48 }, { "Z.1.10", 1, 173 }, \
    { "Z.1.11", 1, 86 }, { "Z.1.12", 1, 4 },

#define MAIN_WILL_TOPIC     "inhabitx/Z/rooms"
#define MAIN_WILL_PAYLOAD   "{\"ts\":0,\"main\":\"offline\",\"rooms\":[{\"id\":\"Z.1.08\",\"state\":\"offline\",\"seats\":40,\"floor\":1},{\"id\":\"Z.1.09\",\"state\":\"offline\",\"seats\":48,\"floor\":1},{\"id\":\"Z.1.10\",\"state\":\"offline\",\"seats\":173,\"floor\":1},{\"id\":\"Z.1.11\",\"state\":\"offline\",\"seats\":86,\"floor\":1},{\"id\":\"Z.1.12\",\"state\":\"offline\",\"seats\":4,\"floor\":1}]}"

#endif /* MAIN_ROOMS_H_ */
