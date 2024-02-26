export module voo:sires_image;
import :device_and_queue;
import :guards;
import :h2l_image;
import :mapmem;
import :update_thread;
import :queue;
import jute;
import silog;
import sith;
import stubby;
import traits;
import vee;

namespace voo {
export auto load_sires_image(jute::view file, vee::physical_device pd) {
  return stbi::load_from_resource(file)
      .map([file, pd](auto &&img) {
        unsigned w = img.width;
        unsigned h = img.height;
        auto m_img = h2l_image{pd, w, h};

        {
          mapmem m{m_img.host_memory()};
          auto *c = static_cast<unsigned char *>(*m);
          for (auto i = 0; i < img.width * img.height * 4; i++) {
            c[i] = (*img.data)[i];
          }
        }
        silog::log(silog::info, "Loaded %dx%d image [%.*s]", img.width,
                   img.height, static_cast<int>(file.size()), file.data());
        return traits::move(m_img);
      })
      .take([file](auto msg) {
        silog::log(silog::error, "Failed loading resource image [%.*s]: %s",
                   static_cast<int>(file.size()), file.data(), msg);
      });
}

export class sires_image : public voo::update_thread {
  voo::h2l_image m_img;

  void build_cmd_buf(vee::command_buffer cb) override {
    voo::cmd_buf_one_time_submit pcb{cb};
    m_img.setup_copy(cb);
  }

public:
  sires_image(jute::view name, voo::device_and_queue *dq)
      : sires_image{name, dq->physical_device(), dq->queue()} {}
  sires_image(jute::view name, vee::physical_device pd, voo::queue *q)
      : update_thread{q}
      , m_img{voo::load_sires_image(name, pd)} {}

  [[nodiscard]] constexpr auto iv() const noexcept { return m_img.iv(); }
  [[nodiscard]] constexpr auto width() const noexcept { return m_img.width(); }
  [[nodiscard]] constexpr auto height() const noexcept {
    return m_img.height();
  }

  using update_thread::run_once;
};
} // namespace voo
