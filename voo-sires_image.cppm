export module voo:sires_image;
import :device_and_queue;
import :guards;
import :h2l_image;
import :mapmem;
import :updater;
import :queue;
import jute;
import silog;
import sith;
import stubby;
import traits;
import vee;

namespace voo {
  export auto load_image_file_as_buffer(const char * file, vee::physical_device pd) {
    return stbi::load(file).map([file, pd](auto && img) {
      auto res = host_buffer_for_image(pd, img.width, img.height, 4);
      {
        mapmem m { res.memory() };
        auto *c = static_cast<unsigned char *>(*m);
        for (auto i = 0; i < img.width * img.height * 4; i++) {
          c[i] = (*img.data)[i];
        }
      }
      silog::log(silog::info, "Pre-loaded %dx%d image [%s]", img.width, img.height, file);
      return res;
    })
    .trace("pre-loading image")
    .log_error();
  }

  export auto load_image_file(const char * file, vee::physical_device pd) {
    return stbi::load(file)
        .map([file, pd](auto &&img) {
          unsigned w = img.width;
          unsigned h = img.height;
          auto m_img = h2l_image { pd, w, h, VK_FORMAT_R8G8B8A8_SRGB };

          {
            mapmem m{m_img.host_memory()};
            auto *c = static_cast<unsigned char *>(*m);
            for (auto i = 0; i < img.width * img.height * 4; i++) {
              c[i] = (*img.data)[i];
            }
          }
          silog::log(silog::info, "Loaded %dx%d image [%s]",
              img.width, img.height, file);
          return traits::move(m_img);
        })
        .trace("loading image")
        .log_error();
  }
export auto load_sires_image(jute::view file, vee::physical_device pd) {
  return stbi::load_from_resource(file)
      .map([file, pd](auto &&img) {
        unsigned w = img.width;
        unsigned h = img.height;
        auto m_img = h2l_image { pd, w, h, VK_FORMAT_R8G8B8A8_SRGB };

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
      .trace("loading resource image")
      .log_error();
}
export constexpr auto load_sires_image(jute::view file) {
  return [=](vee::physical_device pd) { return load_sires_image(file, pd); };
}
} // namespace voo
