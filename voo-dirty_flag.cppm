export module voo:dirty_flag;
import :mapmem;
import no;
import traits;
import vee;

namespace voo {
class dirt_guard : no::copy {
  mapmem m_mem{};
  bool *m_dirty{};

public:
  dirt_guard() = default;
  dirt_guard(vee::device_memory::type m, bool *flag)
      : m_mem{mapmem{m}}, m_dirty{flag} {}
  ~dirt_guard() {
    if (m_dirty)
      *m_dirty = true;
  }

  dirt_guard(dirt_guard &&) = default;

  [[nodiscard]] auto operator*() { return *m_mem; }
};
class dirty_flag {
  bool m_dirty{false};

public:
  [[nodiscard]] constexpr bool get_and_clear() {
    auto r = m_dirty;
    m_dirty = false;
    return r;
  }

  [[nodiscard]] auto guard(vee::device_memory::type m) {
    return dirt_guard{m, &m_dirty};
  }
};
} // namespace voo
