export module voo:bound_buffer;
import vee;

namespace voo {
  class bound_buffer {
    vee::buffer m_buf;
    vee::device_memory m_mem;

  public:
    bound_buffer() = default;

    bound_buffer(unsigned mem_type_idx, unsigned sz)
      : bound_buffer { mem_type_idx, sz, vee::buffer_usage::transfer_src_buffer }
    {}

    bound_buffer(unsigned mem_type_idx, unsigned sz, vee::buffer_usage usage, auto... usages)
      : m_buf { vee::create_buffer(sz, usage, usages...) }
      , m_mem { vee::create_memory(mem_type_idx, sz) }
    { vee::bind_buffer_memory(*m_buf, *m_mem); }
    
    [[nodiscard]] auto buffer() const { return *m_buf; }
    [[nodiscard]] auto memory() const { return *m_mem; }
  };
}
