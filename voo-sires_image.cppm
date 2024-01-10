export module voo:sires_image;
import :guards;
import :h2l_image;
import silog;
import stubby;
import vee;

namespace voo {
export class sires_image {
  h2l_image m_img;

public:
  explicit sires_image(vee::physical_device pd) : m_img{pd, 16, 16} {}
  sires_image(const char *file, vee::physical_device pd) {
    stbi::load(file)
        .map([this, pd](auto &&img) {
          m_img = h2l_image{pd, img.width, img.height};

          auto m = m_img.mapmem();
          auto *c = static_cast<unsigned char *>(*m);
          for (auto i = 0; i < img.width * img.height * 4; i++) {
            c[i] = (*img.data)[i];
          }
        })
        .take([file](auto msg) {
          silog::log(silog::error, "Failed loading resource image [%s]: %s",
                     file, msg);
        });
  }

  [[nodiscard]] auto iv() const noexcept { return m_img.iv(); }

  void run(const cmd_buf_one_time_submit &cb) { m_img.run(cb); }
};

} // namespace voo
