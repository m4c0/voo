#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import casein;
import mtx;
import rng;
import sith;
import vee;
import voo;

struct inst {
  float x, y;
};

void create_instances(voo::h2l_buffer *insts) {
  voo::mapmem m{insts->host_memory()};
  static_cast<inst *>(*m)[0] = {rng::randf(), rng::randf()};
  static_cast<inst *>(*m)[1] = {-1, -1};
}

static struct : public voo::casein_thread {
  void run() override {
    voo::device_and_queue dq{"voo-poc"};

    voo::one_quad quad{dq};

    vee::pipeline_layout pl = vee::create_pipeline_layout();

    // TODO: fix validation issues while resizing
    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      constexpr const unsigned sz = 2 * sizeof(inst);
      auto u = voo::updater{dq.queue(), &create_instances, dq, sz};
      sith::run_guard ut{&u};

      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = dq.render_pass(),
          .shaders{
              voo::shader("poc.vert.spv").pipeline_vert_stage(),
              voo::shader("poc.frag.spv").pipeline_frag_stage(),
          },
          .bindings{
              quad.vertex_input_bind(),
              vee::vertex_input_bind_per_instance(sizeof(inst)),
          },
          .attributes{
              quad.vertex_attribute(0),
              vee::vertex_attribute_vec2(1, 0),
          },
      });

      auto q = dq.queue();
      extent_loop(q, sw, [&] {
        sw.queue_one_time_submit(q, [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_bind_vertex_buffers(*scb, 1, u.data().local_buffer());
          quad.run(scb, 0, 2);
        });
      });
    }
  }
} t;
