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

void create_instances(voo::h2l_buffer * insts) {
  voo::mapmem m { insts->host_memory() };
  static_cast<inst *>(*m)[0] = { rng::randf(), rng::randf() };
  static_cast<inst *>(*m)[1] = { -1, -1 };
}

// Use "vapp" of "vinyl" for more standardised control
struct thread : public sith::thread {
  sith::run_guard m_run{};

  thread() {
    casein::handle(casein::CREATE_WINDOW, [this] { m_run = sith::run_guard{this}; });
    casein::handle(casein::QUIT, [this] { m_run = {}; });
  }

  void run() {
    voo::device_and_queue dq { "poc-voo", casein::native_ptr };
    auto rp = voo::single_att_render_pass(dq);
    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq, *rp };
      voo::one_quad quad { dq };

      constexpr const unsigned sz = 2 * sizeof(inst);
      auto u = voo::updater { dq.queue(), &create_instances, dq, sz };
      sith::run_guard ut { &u };

      vee::pipeline_layout pl = vee::create_pipeline_layout();
      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = *rp,
          .shaders {
              voo::shader("poc.vert.spv").pipeline_vert_stage(),
              voo::shader("poc.frag.spv").pipeline_frag_stage(),
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
        sw.acquire_next_image();
        sw.queue_one_time_submit(dq.queue(), [&] {
          auto scb = sw.cmd_render_pass();
          auto cb = sw.command_buffer();
          vee::cmd_set_viewport(cb, sw.extent());
          vee::cmd_set_scissor(cb, sw.extent());
          vee::cmd_bind_gr_pipeline(cb, *gp);
          vee::cmd_bind_vertex_buffers(cb, 1, u.data().local_buffer());
          quad.run(cb, 0, 2);
        });
        sw.queue_present();
      }

      dq.queue()->device_wait_idle();
    }
  }
} t;
