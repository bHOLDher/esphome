#pragma once

#include "remote_base.h"
#include <vector>
//#include "esphome/core/component.h"
//#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace remote_base {

struct Wh1080Data {
  std::vector<uint8_t> data;

  bool operator==(const Wh1080Data &rhs) const { return data == rhs.data; }
};

class Wh1080Protocol : public RemoteProtocol<Wh1080Data> {
 public:
  void encode(RemoteTransmitData *dst, const Wh1080Data &data) override;
  optional<Wh1080Data> decode(RemoteReceiveData src) override;
  void dump(const Wh1080Data &data) override;
  // void set_deviceid_sensor(sensor::Sensor *deviceid_sensor) { deviceId_sensor_ = deviceid_sensor; }
  // void set_temp_sensor(sensor::Sensor *temp_sensor) { temp_sensor_ = temp_sensor; }
  // void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }
  // void set_windavg_sensor(sensor::Sensor *windavg_sensor) { windAvg_sensor_ = windavg_sensor; }
  // void set_windgust_sensor(sensor::Sensor *windgust_sensor) { windGust_sensor_ = windgust_sensor; }
  // void set_rain_sensor(sensor::Sensor *rain_sensor) { rain_sensor_ = rain_sensor; }
  // void set_batteryflag_sensor(sensor::Sensor *batteryflag_sensor) { batteryFlag_sensor_ = batteryflag_sensor; }
  // void set_winddirection_sensor(sensor::Sensor *winddirection_sensor) { windDirection_sensor_ = winddirection_sensor; }

  // void setup() override;
  // void loop() override;
  // void update() override;
  // void dump_config() override;
  
 protected:
  // void encode_byte_(RemoteTransmitData *dst, uint8_t item);
  bool expect_sync(RemoteReceiveData &src);
  // sensor::Sensor *deviceId_sensor_{nullptr};
  // sensor::Sensor *temp_sensor_{nullptr};
  // sensor::Sensor *humidity_sensor_{nullptr};
  // sensor::Sensor *windAvg_sensor_{nullptr};
  // sensor::Sensor *windGust_sensor_{nullptr};
  // sensor::Sensor *rain_sensor_{nullptr};
  // sensor::Sensor *batteryFlag_sensor_{nullptr};
  // sensor::Sensor *windDirection_sensor_{nullptr};

};

DECLARE_REMOTE_PROTOCOL(Wh1080)

template<typename... Ts> class Wh1080Action : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(std::vector<uint8_t>, code)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    Wh1080Data data{};
    data.data = this->code_.value(x...);
    Wh1080Protocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
