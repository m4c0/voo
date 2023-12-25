#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import casein;
import vee;
import voo;

class thread : public voo::casein_thread {
public:
  void run() override {
    voo::device_and_queue dq{"winnipeg", native_ptr()};

    voo::one_quad quad{dq};
    vee::command_buffer cb =
        vee::allocate_primary_command_buffer(dq.command_pool());

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};
      vee::pipeline_layout pl = vee::create_pipeline_layout();
      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = sw.render_pass(),
          .shaders{
              voo::shader("poc.vert.spv").pipeline_vert_stage(),
              voo::shader("poc.frag.spv").pipeline_frag_stage(),
          },
          .bindings{
              quad.vertex_input_bind(),
          },
          .attributes{
              quad.vertex_attribute(0),
          },
      });

      resized() = false;
      while (!interrupted() && !resized()) {
        sw.acquire_next_image();
        {
          voo::cmd_buf_one_time_submit pcb{cb};
          auto scb = sw.cmd_render_pass(cb);
          vee::cmd_bind_gr_pipeline(cb, *gp);
          quad.run(scb, 0);
        }
        sw.queue_submit(dq, cb);
        sw.queue_present(dq);
      }

      vee::device_wait_idle();
    }
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static thread t{};
  t.handle(e);
}
