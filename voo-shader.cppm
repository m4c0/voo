export module voo:shader;
import jute;
import silog;
import sires;
import vee;

namespace voo {
export class shader {
  vee::shader_module m_mod;

  static auto create_shader(jute::view from) {
    if (from.subview(4).before == "\x03\x02\x23\x07") {
      return vee::create_shader_module(from);
    } else {
      silog::log(silog::info, "Loading shader %s", from.cstr().begin());
      return vee::create_shader_module(sires::slurp(from));
    }
  }

protected:
  [[nodiscard]] constexpr auto mod() const { return *m_mod; }

public:
  constexpr shader() = default;
  explicit shader(const void * data, unsigned size) : m_mod{vee::create_shader_module(data, size)} {}
  explicit shader(jute::view src_or_res) : m_mod { create_shader(src_or_res) } {}

  explicit constexpr operator bool() const { return *m_mod; }
};

  export struct comp_shader : shader {
    using shader::shader;
    [[nodiscard]] auto operator*() const { return mod(); }
  };

  export struct frag_shader : shader {
    using shader::shader;
  
    [[nodiscard]] auto pipeline_stage(const char *fn = "main") const {
      return vee::pipeline_frag_stage(mod(), fn);
    }
    [[nodiscard]] auto pipeline_stage(const char *fn, const auto & k) const {
      return vee::pipeline_frag_stage(mod(), fn, k);
    }
    [[nodiscard]] auto operator*() const { return pipeline_stage(); }
  };

  export struct vert_shader : shader {
    using shader::shader;
  
    [[nodiscard]] auto pipeline_stage(const char *fn = "main") const {
      return vee::pipeline_vert_stage(mod(), fn);
    }
    [[nodiscard]] auto pipeline_stage(const char *fn, const auto & k) const {
      return vee::pipeline_vert_stage(mod(), fn, k);
    }

    [[nodiscard]] auto operator*() const { return pipeline_stage(); }
  };
}
