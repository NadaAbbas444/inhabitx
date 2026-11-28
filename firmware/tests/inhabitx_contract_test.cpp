#include "../ClassroomOccupancy/NodeMessage.h"
#include <assert.h>
#include <string.h>

int main() {
    char payload[128];
    size_t length = nodeMessage("Z.1.09", RoomState::Occupied, payload, sizeof payload);
    assert(length == strlen(payload));
    assert(strcmp(payload, "{\"room\":\"Z.1.09\",\"occupied\":true,\"ts\":0}") == 0);
    assert(nodeMessage("Z.1.09", RoomState::Unoccupied, payload, sizeof payload));
    assert(strcmp(payload, "{\"room\":\"Z.1.09\",\"occupied\":false,\"ts\":0}") == 0);
    assert(nodeMessage("Z.1.09", RoomState::Unknown, payload, sizeof payload) == 0);
    assert(nodeMessage("Z.1.09", RoomState::Occupied, payload, 8) == 0);

    NodeSnapshot snapshot = {RoomState::Occupied, 1000, 7};
    assert(nodeSnapshotValid(snapshot, 3000, 7, 2000));
    assert(!nodeSnapshotValid(snapshot, 3001, 7, 2000));
    assert(!nodeSnapshotValid(snapshot, 1001, 8, 2000)); // capture fault invalidates old prediction
    snapshot.state = RoomState::Unknown;
    assert(!nodeSnapshotValid(snapshot, 1001, 7, 2000));
    snapshot = {RoomState::Unoccupied, UINT32_MAX - 499, 7};
    assert(nodeSnapshotValid(snapshot, 500, 7, 2000)); // millis wrap
    assert(!nodeSnapshotValid(snapshot, 2000, 7, 2000));
    puts("InhabitX contract tests passed");
}
