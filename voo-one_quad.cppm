export module voo:one_quad;
import :device_and_queue;
import :guards;
import :mapmem;
import vee;

namespace {
struct quad {
  static constexpr const auto v_count = 6;
  static constexpr const auto v_size = 2;
  float p[v_count][v_size]{
      {0.0, 0.0}, {1.0, 1.0}, {1.0, 0.0},

      {1.0, 1.0}, {0.0, 0.0}, {0.0, 1.0},
  };
};
} // namespace

namespace voo {
  // TODO: make this "singleton-able"
  // TODO: make a "local" variant
export class one_quad {
  vee::buffer m_qbuf;
  vee::device_memory m_qmem;

public:
  explicit one_quad(vee::physical_device pd) {
    m_qbuf = vee::create_vertex_buffer(sizeof(quad));
    m_qmem = vee::create_host_memory(pd, sizeof(quad));
    vee::bind_buffer_memory(*m_qbuf, *m_qmem, 0);

    mapmem mem{*m_qmem};
    *static_cast<quad *>(*mem) = {};
  }
  explicit one_quad(const device_and_queue &dq)
      : one_quad(dq.physical_device()) {}

  void run(vee::command_buffer cb, unsigned idx, unsigned inst = 1,
           unsigned first_inst = 0) const {
    vee::cmd_bind_vertex_buffers(cb, idx, *m_qbuf);
    vee::cmd_draw(cb, quad::v_count, inst, first_inst);
  }
  void run(const cmd_render_pass &scb, unsigned idx, unsigned inst = 1,
           unsigned first_inst = 0) const {
    run(*scb, idx, inst, first_inst);
  }

  static auto vertex_input_bind() {
    return vee::vertex_input_bind(sizeof(float) * quad::v_size);
  }
  static auto vertex_attribute(unsigned binding) {
    return vee::vertex_attribute_vec2(binding, 0);
  }
};
} // namespace voo
