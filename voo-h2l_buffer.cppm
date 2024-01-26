export module voo:h2l_buffer;
import :device_and_queue;
import :guards;
import :host_buffer;
import sith;
import vee;

namespace voo {
export class h2l_buffer {
  fenced_host_buffer m_hbuf;

  vee::buffer m_buf;
  vee::device_memory m_mem;

public:
  h2l_buffer() = default;
  explicit h2l_buffer(vee::physical_device pd, vee::command_pool::type cp,
                      unsigned sz)
      : m_hbuf{pd, cp, sz} {
    m_buf =
        vee::create_buffer(sz, vee::vertex_buffer, vee::transfer_dst_buffer);
    m_mem = vee::create_local_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem);

    voo::cmd_buf_sim_use pcb{m_hbuf.cmd_buf()};
    vee::cmd_pipeline_barrier(*pcb, *m_buf, vee::from_host_to_transfer);
    vee::cmd_copy_buffer(*pcb, m_hbuf.buffer(), *m_buf, sz);
    vee::cmd_pipeline_barrier(*pcb, *m_buf, vee::from_transfer_to_vertex);
  }
  explicit h2l_buffer(const voo::device_and_queue &dq, unsigned sz)
      : h2l_buffer{dq.physical_device(), dq.command_pool(), sz} {}

  [[nodiscard]] auto mapmem(sith::thread *t) { return m_hbuf.mapmem(t); }

  void submit(const vee::queue &q) { m_hbuf.submit(q); }
  void submit(const voo::device_and_queue &dq) { submit(dq.queue()); }

  [[nodiscard]] auto buffer() const noexcept { return *m_buf; }
};
} // namespace voo
