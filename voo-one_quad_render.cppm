export module voo:one_quad_render;
import :device_and_queue;
import :one_quad;
import :shader;
import jute;
import silog;
import vee;

namespace voo {
  export class one_quad_render {
    using blends_t = decltype(vee::gr_pipeline_params::blends);

    voo::one_quad m_quad;
    vee::gr_pipeline m_pipeline;

  public:
    one_quad_render(jute::view shader, const voo::device_and_queue * dq, const vee::pipeline_layout::type pl)
      : one_quad_render { shader, dq->physical_device(), dq->render_pass(), pl } {}

    one_quad_render(
        jute::view vert, jute::view frag,
        vee::physical_device pd, vee::render_pass::type rp, const vee::pipeline_layout::type pl,
        blends_t blends = { vee::colour_blend_classic() })
        : m_quad { pd }
        , m_pipeline {
          vee::create_graphics_pipeline({
            .pipeline_layout = pl,
            .render_pass = rp,
            .blends = blends,
            .shaders {
              voo::shader(vert).pipeline_vert_stage(),
              voo::shader(frag).pipeline_frag_stage(),
            },
            .bindings { m_quad.vertex_input_bind() },
            .attributes { m_quad.vertex_attribute(0) },
          })
        } {}

    one_quad_render(jute::heap shader, vee::physical_device pd, vee::render_pass::type rp, const vee::pipeline_layout::type pl,
                    blends_t blends = { vee::colour_blend_classic() })
      : one_quad_render(*(shader + ".vert.spv"), *(shader + ".frag.spv"), pd, rp, pl, blends) {}

    one_quad_render(jute::view shader, vee::physical_device pd, vee::render_pass::type rp, const vee::pipeline_layout::type pl,
                    blends_t blends = { vee::colour_blend_classic() })
      : one_quad_render(jute::heap { shader }, pd, rp, pl, blends) {}

    void run(vee::command_buffer cb, auto fn) {
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
      fn();
      m_quad.run(cb, 0, 1);
    }
    void run(vee::command_buffer cb) {
      run(cb, [] {});
    }
  };
} // namespace voo
