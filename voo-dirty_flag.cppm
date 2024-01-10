export module voo:dirty_flag;
import vee;

namespace voo {
class dirt_guard {
  vee::mapmem m_mem;
  bool *m_dirty;

public:
  dirt_guard(vee::device_memory::type m, bool *flag)
      : m_mem{vee::mapmem{m}}, m_dirty{flag} {}
  ~dirt_guard() { *m_dirty = true; }

  [[nodiscard]] auto operator*() { return *m_mem; }
};
class dirty_flag {
  bool m_dirty{true};

public:
  [[nodiscard]] constexpr bool get_and_clear() noexcept {
    auto r = m_dirty;
    m_dirty = false;
    return r;
  }

  [[nodiscard]] auto guard(vee::device_memory::type m) {
    return dirt_guard{m, &m_dirty};
  }
};
} // namespace voo