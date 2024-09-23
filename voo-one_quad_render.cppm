export module voo:one_quad_render;
import :device_and_queue;
import :one_quad;
import :shader;
import dotz;
import hai;
import jute;
import silog;
import sires;
import sitime;
import traits;
import vee;

namespace voo {
  export class one_quad_render {
    hai::cstr m_vert_name;
    hai::cstr m_frag_name;
    const voo::device_and_queue * m_dq;
    const vee::pipeline_layout::type m_pl;

    voo::one_quad m_quad;
    vee::gr_pipeline m_pipeline;
    traits::ints::uint64_t m_last_updated;

    vee::gr_pipeline m_pipeline_old {};

    [[nodiscard]] auto current_mtime() {
      return dotz::max(sires::stat(m_vert_name).unwrap(0), sires::stat(m_frag_name).unwrap(0));
    }

    [[nodiscard]] auto create_pipeline() {
      m_last_updated = current_mtime();
      return vee::create_graphics_pipeline({
          .pipeline_layout = m_pl,
          .render_pass = m_dq->render_pass(),
          .shaders {
              voo::shader(m_vert_name).pipeline_vert_stage(),
              voo::shader(m_frag_name).pipeline_frag_stage(),
          },
          .bindings { m_quad.vertex_input_bind() },
          .attributes { m_quad.vertex_attribute(0) },
      });
    }

  public:
    one_quad_render(jute::view shader, const voo::device_and_queue * dq, const vee::pipeline_layout::type pl)
        : m_vert_name { (shader + ".vert.spv").cstr() }
        , m_frag_name { (shader + ".frag.spv").cstr() }
        , m_dq { dq }
        , m_pl { pl }
        , m_quad { *dq }
        , m_pipeline { create_pipeline() } {}

    void run(vee::command_buffer cb, vee::extent ext) {
      vee::cmd_set_viewport(cb, ext);
      vee::cmd_set_scissor(cb, ext);
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
      m_quad.run(cb, 0, 1);

      if (m_last_updated < current_mtime()) {
        silog::log(silog::debug, "refreshing pipeline");
        m_pipeline_old = traits::move(m_pipeline);
        m_pipeline = create_pipeline();
      }
    }
  };
} // namespace voo
