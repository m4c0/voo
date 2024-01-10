export module voo:sires_image;
import :guards;
import :h2l_image;
import silog;
import stubby;
import vee;

namespace voo {
export auto load_sires_image(const char *file, vee::physical_device pd,
                             vee::command_pool::type cp) {
  return stbi::load(file)
      .map([pd, cp](auto &&img) {
        auto m_img = h2l_image{pd, cp, img.width, img.height};

        auto m = m_img.mapmem();
        auto *c = static_cast<unsigned char *>(*m);
        for (auto i = 0; i < img.width * img.height * 4; i++) {
          c[i] = (*img.data)[i];
        }

        return m_img;
      })
      .take([file](auto msg) {
        silog::log(silog::error, "Failed loading resource image [%s]: %s", file,
                   msg);
      });
}
} // namespace voo
