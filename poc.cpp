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

class updater : public sith::thread {
  voo::device_and_queue *m_dq;
  voo::h2l_buffer insts{*m_dq, 2 * sizeof(inst)};

public:
  updater(voo::device_and_queue &dq) : m_dq{&dq} { start(); }

  [[nodiscard]] constexpr auto local_buffer() const noexcept {
    return insts.local_buffer();
  }

  void run() {
    auto cp = vee::create_command_pool(m_dq->queue_family());
    auto cb = vee::allocate_primary_command_buffer(*cp);

    voo::fence f{voo::fence::signaled{}};
    while (!interrupted()) {
      f.wait_and_reset();

      {
        voo::mapmem m{insts.host_memory()};
        static_cast<inst *>(*m)[0] = {rng::randf(), rng::randf()};
        static_cast<inst *>(*m)[1] = {-1, -1};
      }

      {
        voo::cmd_buf_one_time_submit pcb{cb};
        insts.setup_copy(*pcb);
      }
      m_dq->queue_submit({
          .fence = *f,
          .command_buffer = cb,
      });
    }
  }
};

class thread : public voo::casein_thread {
public:
  void run() override {
    voo::device_and_queue dq{"voo-poc", native_ptr()};

    voo::one_quad quad{dq};

    vee::pipeline_layout pl = vee::create_pipeline_layout();

    updater u{dq};

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

      extent_loop(dq, sw, [&] {
        {
          voo::cmd_buf_one_time_submit pcb{sw.command_buffer()};
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_bind_vertex_buffers(*scb, 1, u.local_buffer());
          quad.run(scb, 0, 2);
        }
        sw.queue_submit(dq);
      });
    }
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static thread t{};
  t.handle(e);
}
