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

  export template<typename T>
  class memiter {
    mapmem m_mm;
    T * m_ptr;
    unsigned * m_count;

  public:
    explicit memiter(vee::device_memory::type m, unsigned * c)
      : m_mm { m }
      , m_ptr { static_cast<T *>(*m_mm) }
      , m_count { c }
    {
      *m_count = 0;
    }

    auto & operator+=(T i) {
      *m_ptr++ = i;
      ++*m_count;
      return *this;
    }

    auto count() { return *m_count; }
  };
} // namespace voo
