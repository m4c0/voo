export module voo:sires_image;
import :guards;
import :h2l_image;
import jute;
import silog;
import sith;
import stubby;
import traits;
import vee;

namespace voo {
export auto load_sires_image(jute::view file, vee::physical_device pd,
                             vee::command_pool::type cp, sith::thread *t) {
  return stbi::load_from_resource(file)
      .map([file, pd, cp, t](auto &&img) {
        unsigned w = img.width;
        unsigned h = img.height;
        auto m_img = h2l_image{pd, cp, w, h};

        return m_img.mapmem(t)
            .map([&](auto &&m) {
              auto *c = static_cast<unsigned char *>(*m);
              for (auto i = 0; i < img.width * img.height * 4; i++) {
                c[i] = (*img.data)[i];
              }
            })
            .map([&] {
              silog::log(silog::info, "Loaded %dx%d image [%.*s]", img.width,
                         img.height, static_cast<int>(file.size()),
                         file.data());
              return traits::move(m_img);
            });
      })
      .take([file](auto msg) {
        silog::log(silog::error, "Failed loading resource image [%.*s]: %s",
                   static_cast<int>(file.size()), file.data(), msg);
      });
}
} // namespace voo
