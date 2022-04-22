struct Aqi {
  uint16_t value;
  unsigned long at_millis;
};

// A ring buffer of AQI values.
class AqiBuffer {
public:
  AqiBuffer(int size) { aqis.resize(size); }

  // Inserts a new value, overwriting the oldest value if the ring buffer is
  // full.
  //
  // Inserted values should always be younger than all values already in
  // the buffer.
  void insert(Aqi new_aqi) {
    latest = (latest + 1) % aqis.size();
    aqis[latest] = new_aqi;
  }

  // Returns the average of all values younger than maxAgeMs.
  int average(unsigned long max_age_ms) {
    int sum = 0;
    int count = 0;
    int i = latest;
    unsigned long now_millis = millis();
    while (aqis[i].at_millis != 0 &&
           now_millis - aqis[i].at_millis < max_age_ms) {
      sum += aqis[i].value;
      ++count;

      // Maybe I forgot how negative modulo works, but it wasn't doing what I
      // expected, so I just add aqis.size() to keep everything positive.
      i = (i - 1 + aqis.size()) % aqis.size();
      if (i == latest) {
        break;
      }
    }
    return sum / count;
  }

private:
  std::vector<Aqi> aqis;
  int latest = 0;
};
