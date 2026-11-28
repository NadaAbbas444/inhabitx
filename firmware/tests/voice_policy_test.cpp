#include "../ClassroomOccupancy/VoicePolicy.h"
#include "../ClassroomOccupancy/NodeMessage.h"
#include <assert.h>
#include <string.h>

int main() {
    VoicePolicy policy;
    assert(!policy.command(VoiceCommand::Leave, 100)); // No wake: no effect.
    assert(!policy.active(100));
    policy.command(VoiceCommand::Wake, 1000);
    assert(policy.listening(1001));
    assert(policy.command(VoiceCommand::Leave, 2000));
    assert(policy.state(RoomState::Occupied, 2001) == RoomState::Unoccupied);
    assert(policy.active(601999));
    assert(!policy.active(602000)); // Exactly 10 minutes from accepted command.
    assert(policy.state(RoomState::Occupied, 602000) == RoomState::Occupied);
    assert(!policy.command(VoiceCommand::Arrive, 3000)); // Wake is consumed.

    policy.command(VoiceCommand::Wake, 10000);
    assert(!policy.command(VoiceCommand::Leave, 18001)); // Expired command window.
    policy.command(VoiceCommand::Wake, 20000);
    policy.cancelListening();
    assert(!policy.command(VoiceCommand::Leave, 20001)); // Capture fault cancels listening.

    policy.command(VoiceCommand::Wake, 30000);
    assert(policy.command(VoiceCommand::Leave, 31000));
    policy.command(VoiceCommand::Wake, 32000);
    assert(policy.command(VoiceCommand::Arrive, 33000)); // Arrival cancels departure.
    assert(policy.state(RoomState::Unoccupied, 33001) == RoomState::Occupied);
    assert(policy.active(152999));
    assert(!policy.active(153000));

    VoicePolicy wrap;
    wrap.command(VoiceCommand::Wake, UINT32_MAX - 1000);
    assert(wrap.command(VoiceCommand::Leave, 500));
    assert(wrap.active(600499));
    assert(!wrap.active(600500));

    char payload[256];
    assert(nodeMessage("Z.1.09", RoomState::Occupied, payload, sizeof payload, true));
    assert(strstr(payload, "\"occupied\":true"));
    assert(strstr(payload, "\"source\":\"voice\""));
    assert(strstr(payload, "Room occupied confirmed by user"));
    assert(nodeMessage("Z.1.09", RoomState::Unoccupied, payload, sizeof payload, true));
    assert(strstr(payload, "\"occupied\":false"));
    assert(strstr(payload, "Room marked empty by user"));
    puts("Voice policy tests passed");
}
