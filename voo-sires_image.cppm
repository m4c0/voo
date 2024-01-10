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
  explicit sires_image(vee::physical_device pd, vee::command_pool::type cp)
      : m_img{pd, cp, 16, 16} {}
  sires_image(const char *file, vee::physical_device pd,
              vee::command_pool::type cp) {
    stbi::load(file)
        .map([this, pd, cp](auto &&img) {
          m_img = h2l_image{pd, cp, img.width, img.height};

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

  [[nodiscard]] constexpr auto &image() noexcept { return m_img; }
};
} // namespace voo
