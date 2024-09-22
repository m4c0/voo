export module voo:mapmem;
import vee;

namespace voo {
export class mapmem {
  vee::device_memory::type m_dm{};
  void *m_ptr{};

public:
  mapmem() = default;
  explicit mapmem(vee::device_memory::type m)
      : m_dm{m}, m_ptr{vee::map_memory(m)} {}
  ~mapmem() {
    if (m_ptr != nullptr)
      vee::unmap_memory(m_dm);
  }

  mapmem(const mapmem &) = delete;
  mapmem &operator=(const mapmem &) = delete;

  mapmem(mapmem &&o) : m_dm{o.m_dm}, m_ptr{o.m_ptr} { o.m_ptr = nullptr; }
  mapmem &operator=(mapmem &&o) = delete;

  [[nodiscard]] constexpr auto *operator*() { return m_ptr; }
};
} // namespace voo
