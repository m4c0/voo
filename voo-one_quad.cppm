export module voo:one_quad;
import :guards;
import vee;

namespace {
struct quad {
  static constexpr const auto v_count = 6;
  float p[v_count][2]{
      {0.0, 0.0}, {1.0, 1.0}, {1.0, 0.0},

      {1.0, 1.0}, {0.0, 0.0}, {0.0, 1.0},
  };
};
} // namespace

namespace voo {
export class one_quad {
  vee::buffer m_qbuf;
  vee::device_memory m_qmem;

public:
  explicit one_quad(vee::physical_device pd) {
    m_qbuf = vee::create_vertex_buffer(sizeof(quad));
    m_qmem = vee::create_host_buffer_memory(pd, sizeof(quad));
    vee::bind_buffer_memory(*m_qbuf, *m_qmem, 0);

    vee::mapmem mem{*m_qmem};
    *static_cast<quad *>(*mem) = {};
  }

  void cmd_bind_vertex_buffer(vee::command_buffer cb,
                              unsigned idx) const noexcept {
    vee::cmd_bind_vertex_buffers(cb, idx, *m_qbuf);
  }

  auto vertex_input_bind() const noexcept {
    return vee::vertex_input_bind(sizeof(float) * 2);
  }
  auto vertex_attribute(unsigned binding) const noexcept {
    return vee::vertex_attribute_vec2(binding, 0);
  }
};
} // namespace voo
