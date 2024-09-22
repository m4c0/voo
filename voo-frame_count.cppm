export module voo:frame_count;
import silog;
import sitime;

namespace voo {
class frame_count {
  sitime::stopwatch m_time{};
  int m_count{};

public:
  auto &operator++() {
    m_count++;
    return *this;
  }

  void print() {
    float t = m_time.millis() / 1000.0f;
    float fps = static_cast<float>(m_count) / t;
    silog::log(silog::info, "%d frames in %.3f seconds (%.2f FPS average)",
               m_count, t, fps);
  }
};
} // namespace voo
