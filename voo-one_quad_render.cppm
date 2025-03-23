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
    using blends_t = decltype(vee::gr_pipeline_params::blends);
    jute::heap m_vert_name;
    jute::heap m_frag_name;
    blends_t m_blends;
    const vee::render_pass::type m_rp;
    const vee::pipeline_layout::type m_pl;

    voo::one_quad m_quad;
    vee::gr_pipeline m_pipeline;
    traits::ints::uint64_t m_last_updated;

    vee::gr_pipeline m_pipeline_old {};

    [[nodiscard]] auto current_mtime() {
      return dotz::max(sires::stat(*m_vert_name).unwrap(0), sires::stat(*m_frag_name).unwrap(0));
    }

    [[nodiscard]] auto create_pipeline() {
      m_last_updated = current_mtime();
      return vee::create_graphics_pipeline({
          .pipeline_layout = m_pl,
          .render_pass = m_rp,
          .blends = m_blends,
          .shaders {
              voo::shader(*m_vert_name).pipeline_vert_stage(),
              voo::shader(*m_frag_name).pipeline_frag_stage(),
          },
          .bindings { m_quad.vertex_input_bind() },
          .attributes { m_quad.vertex_attribute(0) },
      });
    }

  public:
    one_quad_render(jute::view shader, const voo::device_and_queue * dq, const vee::pipeline_layout::type pl)
      : one_quad_render { shader, dq->physical_device(), dq->render_pass(), pl } {}

    one_quad_render(
        jute::view vert, jute::view frag,
        vee::physical_device pd, vee::render_pass::type rp, const vee::pipeline_layout::type pl,
        blends_t blends = { vee::colour_blend_classic() })
        : m_vert_name { vert }
        , m_frag_name { frag }
        , m_blends { blends }
        , m_rp { rp }
        , m_pl { pl }
        , m_quad { pd }
        , m_pipeline { create_pipeline() } {}

    one_quad_render(jute::heap shader, vee::physical_device pd, vee::render_pass::type rp, const vee::pipeline_layout::type pl,
                    blends_t blends = { vee::colour_blend_classic() })
      : one_quad_render(*(shader + ".vert.spv"), *(shader + ".frag.spv"), pd, rp, pl, blends) {}

    one_quad_render(jute::view shader, vee::physical_device pd, vee::render_pass::type rp, const vee::pipeline_layout::type pl,
                    blends_t blends = { vee::colour_blend_classic() })
      : one_quad_render(jute::heap { shader }, pd, rp, pl, blends) {}

    void run(vee::command_buffer cb, vee::extent ext, auto fn) {
      vee::cmd_set_viewport(cb, ext);
      vee::cmd_set_scissor(cb, ext);
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
      fn();
      m_quad.run(cb, 0, 1);

      if (m_last_updated < current_mtime()) {
        silog::log(silog::debug, "refreshing pipeline");
        m_pipeline_old = traits::move(m_pipeline);
        m_pipeline = create_pipeline();
      }
    }
    void run(vee::command_buffer cb, vee::extent ext) {
      run(cb, ext, [] {});
    }
  };
} // namespace voo
