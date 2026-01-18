#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import casein;
import rng;
import sith;
import voo;

struct inst {
  float x, y;
};

void create_instances(voo::bound_buffer * insts) {
  voo::mapmem m { *insts->memory };
  static_cast<inst *>(*m)[0] = { rng::randf(), rng::randf() };
  static_cast<inst *>(*m)[1] = { -1, -1 };
}

// Use "vapp" of "vinyl" for more standardised control
struct thread : public sith::thread {
  sith::run_guard m_run{};

  thread() {
    using namespace casein;
    handle(CREATE_WINDOW, [this] { m_run = sith::run_guard{this}; });
    handle(KEY_DOWN, K_Q, [] { interrupt(IRQ_QUIT); });
    handle(QUIT, [this] { m_run = {}; });
  }

  void run() {
    voo::device_and_queue dq { "poc-voo", casein::native_ptr };
    auto rp = voo::single_att_render_pass(dq);
    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq, *rp };
      voo::one_quad quad {};

      constexpr const unsigned sz = 2 * sizeof(inst);
      auto insts = voo::bound_buffer::create_from_host(sz, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

      vee::pipeline_layout pl = vee::create_pipeline_layout();
      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = *rp,
          .shaders {
              *voo::vert_shader("poc.vert.spv"),
              *voo::frag_shader("poc.frag.spv"),
          },
          .bindings {
              quad.vertex_input_bind(),
              vee::vertex_input_bind_per_instance(sizeof(inst)),
          },
          .attributes {
              quad.vertex_attribute(0),
              vee::vertex_attribute_vec2(1, 0),
          },
      });

      while (!interrupted()) {
        // TODO: sync CPU+GPU
        create_instances(&insts);
        
        sw.acquire_next_image();
        sw.queue_one_time_submit([&] {
          auto scb = sw.cmd_render_pass();
          auto cb = sw.command_buffer();
          vee::cmd_set_viewport(cb, sw.extent());
          vee::cmd_set_scissor(cb, sw.extent());
          vee::cmd_bind_gr_pipeline(cb, *gp);
          vee::cmd_bind_vertex_buffers(cb, 1, *insts.buffer);
          quad.run(cb, 0, 2);
        });
        sw.queue_present();
      }

      dq.queue()->device_wait_idle();
    }
  }
} t;
