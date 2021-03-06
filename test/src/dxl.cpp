#include "dxl.h"
#include "utils.h"

Dynamixel dxl(PIN_RTS);

void dxl_setup() {
  DXL_SERIAL.begin(3000000, SERIAL_HALF_DUPLEX);
  dxl.attach(DXL_SERIAL, 3000000);
  for (uint8_t id: joint_ids) {
    dxl.addModel<DxlModel::X>(id);
  }
  delay(2000);

  bool connected = true;
  for (uint8_t id: joint_ids) {
    if (!dxl.ping(id)) { connected = false; break; }
  }

  if (connected) {
    DEBUG_SERIAL.println("DXL setup done!");
  } else {
    DEBUG_SERIAL.println("DXL setup failed!");
  }
}

void set_dxl_parameters() {
  for (uint8_t id: joint_ids) {
    dxl.positionPGain(id, 400);
    dxl.positionDGain(id, 500);
    dxl.positionIGain(id, 40);
    dxl.profileVelocity(id, 300);
    dxl.profileAcceleration(id, 300);
    delay(10);
  }
}

void set_joint_position_limit(std::vector<uint8_t> id_set, int min, int max) {
  for (uint8_t id: id_set) {
    dxl.minPositionLimit(id, min);
    dxl.maxPositionLimit(id, max);
    dxl.torqueEnable(id, true);
    dxl.verbose(id);
    delay(10);
  }
}

void initialize_joint_positions(std::vector<uint8_t> id_set, int position) {
  for (uint8_t id: id_set) {
    dxl.goalPosition(id, position);
    dxl.verbose(id);
    delay(10);
  }
  while (1) {
    bool initialized = true;
    for (uint8_t id: id_set) {
      initialized = initialized && !dxl.moving(id);
    }
    if (initialized) { break; }
  }
}

void print_joint_positions() {
  for (uint8_t id: joint_ids) {
    int pos = dxl.presentPosition(id);
    if (pos == -1) {
      DEBUG_SERIAL.printf("MOTOR%d ERROR: %d\t", id, dxl.hardwareErrorStatus(id));
      continue;
    }
    DEBUG_SERIAL.printf("MOTOR%d: %d\t", id, pos);
    dxl.verbose(id);
  }
  DEBUG_SERIAL.println();
}
