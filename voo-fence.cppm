export module voo:fence;
import vee;

namespace voo {
class fence {
  vee::fence m_f{};

public:
  struct signaled {};

  fence() = default;
  fence(signaled s) : m_f{vee::create_fence_signaled()} {}

  bool get() { return vee::get_fence_status(*m_f); }
  void wait_and_reset(unsigned timeout_ms = ~0U) {
    vee::wait_and_reset_fence(*m_f, timeout_ms);
  }
  void wait(unsigned timeout_ms = ~0U) {
    vee::wait_for_fence(*m_f, timeout_ms);
  }
  void reset() { vee::reset_fence(*m_f); }

  [[nodiscard]] constexpr auto operator*() const noexcept { return *m_f; }
};
} // namespace voo
