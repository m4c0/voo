export module voo:h2l_buffer;
import :buffers;
import :device_and_queue;
import vee;

namespace voo {
export class h2l_buffer {
  bound_buffer m_hbuf;
  bound_buffer m_lbuf;
  unsigned m_sz;

public:
  h2l_buffer() = default;
  explicit h2l_buffer(
    vee::physical_device pd, unsigned sz,
    vee::buffer_usage usage = vee::buffer_usage::vertex_buffer
  )
    : m_hbuf { bound_buffer::create_from_host(pd, sz) }
    , m_lbuf { bound_buffer::create_from_device_local(pd, sz, vee::buffer_usage::transfer_dst_buffer, usage) }
    , m_sz { sz } {}

  explicit h2l_buffer(const voo::device_and_queue &dq, unsigned sz)
      : h2l_buffer{dq.physical_device(), sz} {}

  void setup_copy(vee::command_buffer cb) const {
    vee::cmd_pipeline_barrier(cb, *m_lbuf.buffer, vee::from_host_to_transfer);
    vee::cmd_copy_buffer(cb, *m_hbuf.buffer, *m_lbuf.buffer, m_sz);
    vee::cmd_pipeline_barrier(cb, *m_lbuf.buffer, vee::from_transfer_to_vertex);
  }

  [[nodiscard]] auto host_memory() const { return *m_hbuf.memory; }
  [[nodiscard]] auto local_buffer() const { return *m_lbuf.buffer; }
};
} // namespace voo
