export module voo:fence;
import vee;

namespace voo {
export class fence {
  vee::fence m_f{};

public:
  fence() = default;

  explicit fence(bool signaled)
    : m_f { signaled ? vee::create_fence_signaled() : vee::create_fence_reset() }
  {}

  bool get() { return vee::get_fence_status(*m_f); }
  void wait_and_reset(unsigned timeout_ms = ~0U) {
    vee::wait_and_reset_fence(*m_f, timeout_ms);
  }
  void wait(unsigned timeout_ms = ~0U) {
    vee::wait_for_fence(*m_f, timeout_ms);
  }
  void reset() { vee::reset_fence(*m_f); }

  [[nodiscard]] constexpr operator vee::fence::type() const { return *m_f; }
};
} // namespace voo
