export module voo:shader;
import jute;
import sires;
import vee;

namespace voo {
export class shader {
  vee::shader_module m_mod;

  static auto create_shader(jute::view from) {
    if (from.subview(4).before == "\x03\x02\x23\x07") {
      return vee::create_shader_module(from);
    } else {
      return vee::create_shader_module(sires::jojo_cstr(from));
    }
  }

public:
  explicit shader(const void * data, unsigned size) : m_mod{vee::create_shader_module(data, size)} {}
  explicit shader(jute::view src_or_res) : m_mod { create_shader(src_or_res) } {}

  [[nodiscard]] auto pipeline_frag_stage(const char *fn = "main") {
    return vee::pipeline_frag_stage(*m_mod, fn);
  }
  [[nodiscard]] auto pipeline_frag_stage(const char *fn, const auto & k) {
    return vee::pipeline_frag_stage(*m_mod, fn, k);
  }
  [[nodiscard]] auto pipeline_vert_stage(const char *fn = "main") {
    return vee::pipeline_vert_stage(*m_mod, fn);
  }
  [[nodiscard]] auto pipeline_vert_stage(const char *fn, const auto & k) {
    return vee::pipeline_vert_stage(*m_mod, fn, k);
  }
};
} // namespace voo
