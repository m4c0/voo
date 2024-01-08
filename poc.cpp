#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import casein;
import vee;
import voo;

struct inst {
  float x, y;
};

class thread : public voo::casein_thread {
public:
  void run() override {
    voo::device_and_queue dq{"winnipeg", native_ptr()};

    voo::one_quad quad{dq};
    vee::command_buffer cb =
        vee::allocate_primary_command_buffer(dq.command_pool());
    vee::command_buffer cb1 =
        vee::allocate_primary_command_buffer(dq.command_pool());

    vee::pipeline_layout pl = vee::create_pipeline_layout();

    voo::h2l_buffer insts{dq, 2 * sizeof(inst)};

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = sw.render_pass(),
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

      {
        auto m = insts.mapmem();
        *static_cast<inst *>(*m) = {-0.5, -0.5};
      }

      extent_loop([&] {
        sw.acquire_next_image();

        insts.submit(cb1, dq);

        sw.one_time_submit(dq, cb, [&](auto &pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          insts.cmd_bind_vertex_buffer(scb, 1);
          quad.run(scb, 0);
        });
        sw.queue_present(dq);
      });
    }
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static thread t{};
  t.handle(e);
}
