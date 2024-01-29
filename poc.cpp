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

class thread : public voo::casein_thread {
  voo::device_and_queue *m_dq;
  voo::h2l_buffer *m_insts;

public:
  [[nodiscard]] auto dq() {
    wait_init();
    return m_dq;
  }
  [[nodiscard]] auto instances() {
    wait_init();
    return m_insts;
  }

  void queue_submit(vee::submit_info s) {
    wait_init();
    m_dq->queue_submit(s);
  }

  void run() override {
    voo::device_and_queue dq{"voo-poc", native_ptr()};
    m_dq = &dq;

    voo::one_quad quad{dq};

    vee::pipeline_layout pl = vee::create_pipeline_layout();

    voo::h2l_buffer insts{dq, 2 * sizeof(inst)};
    m_insts = &insts;
    release_init_lock();
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
          vee::cmd_bind_vertex_buffers(*scb, 1, insts.local_buffer());
          quad.run(scb, 0, 2);
        }
        sw.queue_submit(dq);
      });
    }

    m_insts = nullptr;
    m_dq = nullptr;
  }
};

class main {
  ::thread m_thr{};
  sith::memfn_thread<main> m_mt{this, &main::run};

  void run(sith::thread *t) {
    auto cp = vee::create_command_pool(m_thr.dq()->queue_family());
    auto cb = vee::allocate_primary_command_buffer(*cp);

    voo::fence f{voo::fence::signaled{}};
    while (!t->interrupted()) {
      auto buf = m_thr.instances();
      if (!buf)
        continue;

      f.wait_and_reset();

      {
        voo::mapmem m{buf->host_memory()};
        static_cast<inst *>(*m)[0] = {rng::randf(), rng::randf()};
        static_cast<inst *>(*m)[1] = {-1, -1};
      }

      {
        voo::cmd_buf_one_time_submit pcb{cb};
        buf->setup_copy(*pcb);
      }
      m_thr.queue_submit({
          .fence = *f,
          .command_buffer = cb,
      });
    }
  }

public:
  main() { m_mt.start(); }

  void handle(const casein::event &e) { m_thr.handle(e); }
};

extern "C" void casein_handle(const casein::event &e) {
  static main m{};
  m.handle(e);
}
