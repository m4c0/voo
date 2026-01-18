export module voo:one_quad;
import :buffers;
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
  bound_buffer m_b = bound_buffer::create_from_host(sizeof(quad), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

public:
  explicit one_quad() {
    mapmem mem { *m_b.memory };
    *static_cast<quad *>(*mem) = {};
  }

  [[nodiscard]] constexpr auto buffer() const { return *m_b.buffer; }

  void run(vee::command_buffer cb, unsigned idx, unsigned inst = 1,
           unsigned first_inst = 0) const {
    vee::cmd_bind_vertex_buffers(cb, idx, *m_b.buffer);
    vee::cmd_draw(cb, quad::v_count, inst, first_inst);
  }

  static auto vertex_input_bind() {
    return vee::vertex_input_bind(sizeof(float) * quad::v_size);
  }
  static auto vertex_attribute(unsigned binding) {
    return vee::vertex_attribute_vec2(binding, 0);
  }
};
} // namespace voo
