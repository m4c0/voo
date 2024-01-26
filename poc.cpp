#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import casein;
import rng;
import sith;
import vee;
import voo;

struct inst {
  float x, y;
};

class thread : public voo::casein_thread {
  voo::h2l_buffer *m_insts;

public:
  [[nodiscard]] auto instances() {
    wait_init();
    return m_insts;
  }

  void run() override {
    voo::device_and_queue dq{"winnipeg", native_ptr()};

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

      extent_loop([&] {
        sw.acquire_next_image();

        insts.submit(dq);

        sw.one_time_submit(dq, [&](auto &pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_bind_vertex_buffers(*scb, 1, insts.buffer());
          quad.run(scb, 0, 2);
        });
        sw.queue_present(dq);
      });
    }

    m_insts = nullptr;
  }
};

class loop {
  ::thread *m_thr;
  sith::memfn_thread<loop> m_mt{this, &loop::run};

  void run(sith::thread *t) {
    while (!t->interrupted()) {
      auto buf = m_thr->instances();
      if (!buf)
        continue;

      // There is a racing condition here. It can be triggered by disabling the
      // update of fence status in host_buffer
      buf->mapmem(t)
          .map([](auto &&m) {
            try {
              static_cast<inst *>(*m)[0] = {rng::randf(), rng::randf()};
              static_cast<inst *>(*m)[1] = {-1, -1};
            } catch (...) {
            }
          })
          .take([](auto err) {});
    }
  }

public:
  loop(::thread *t) : m_thr{t} { m_mt.start(); }
};

extern "C" void casein_handle(const casein::event &e) {
  static thread t{};
  static loop l{&t};
  t.handle(e);
}
