export module voo:one_quad_render;
import :device_and_queue;
import :one_quad;
import :shader;
import jute;
import sitime;
import sith;
import vee;

namespace voo {
  export class one_quad_render {
    voo::one_quad m_quad;
    vee::gr_pipeline m_pipeline;
    sith::memfn_thread<one_quad_render> m_shd_mon;
    sith::run_guard m_rg;

    void mon_shd(sith::thread * t) {
      while (!t->interrupted()) {
        sitime::sleep(1);
      }
    }

  public:
    one_quad_render(jute::view shader, const voo::device_and_queue * dq, const vee::pipeline_layout::type pl)
      : m_quad { *dq }
      , m_pipeline { vee::create_graphics_pipeline({
        .pipeline_layout = pl,
        .render_pass = dq->render_pass(),
        .shaders {
            voo::shader((shader + ".vert.spv").cstr()).pipeline_vert_stage(),
            voo::shader((shader + ".frag.spv").cstr()).pipeline_frag_stage(),
        },
        .bindings { m_quad.vertex_input_bind() },
        .attributes { m_quad.vertex_attribute(0) },
      }) }
      , m_shd_mon { this, &one_quad_render::mon_shd }
      , m_rg { &m_shd_mon } {}

    void run(vee::command_buffer cb, vee::extent ext) {
      vee::cmd_set_viewport(cb, ext);
      vee::cmd_set_scissor(cb, ext);
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
      m_quad.run(cb, 0, 1);
    }
  };
}
