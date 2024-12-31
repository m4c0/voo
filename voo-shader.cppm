export module voo:shader;
import jute;
import vee;

namespace voo {
export class shader {
  vee::shader_module m_mod;

public:
  explicit shader(jute::view res_name)
      : m_mod{vee::create_shader_module_from_resource(res_name)} {}

  [[nodiscard]] auto pipeline_frag_stage(const char *fn = "main") {
    return vee::pipeline_frag_stage(*m_mod, fn);
  }
  [[nodiscard]] auto pipeline_frag_stage(const char *fn, auto k) {
    return vee::pipeline_frag_stage(*m_mod, fn, k);
  }
  [[nodiscard]] auto pipeline_vert_stage(const char *fn = "main") {
    return vee::pipeline_vert_stage(*m_mod, fn);
  }
  [[nodiscard]] auto pipeline_vert_stage(const char *fn, auto k) {
    return vee::pipeline_vert_stage(*m_mod, fn, k);
  }
};
} // namespace voo
