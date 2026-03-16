export module voo:images;
import vee;
import wagen;

namespace voo {
  export struct bound_image {
    vee::image img {};
    vee::device_memory mem {};
    vee::image_view iv {};

    static bound_image create(wagen::VkImageCreateInfo info, wagen::VkImageViewCreateInfo iv) {
      bound_image res {};
      res.img = vee::image { &info };
      res.mem = vee::create_local_image_memory(wagen::physical_device(), *res.img);
      vee::bind_image_memory(*res.img, *res.mem);

      iv.image = *res.img;
      iv.format = info.format;
      res.iv = vee::image_view { &iv };

      return res;
    }

    static bound_image create(wagen::VkImageCreateInfo info, wagen::VkImageSubresourceRange sub) {
      return create(info, VkImageViewCreateInfo {
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .subresourceRange = sub,
      });
    }

    static bound_image create(wagen::VkImageCreateInfo info, wagen::VkImageAspectFlags aspect) {
      return create(info, vee::image_subresource_range(aspect));
    }

    static bound_image create_depth(vee::extent ext, wagen::VkImageUsageFlags usage) {
      return create(vee::depth_image_create_info(ext, usage), VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    static bound_image create(vee::extent ext, vee::format fmt, wagen::VkImageUsageFlags usage) {
      return create(vee::image_create_info(ext, fmt, usage), VK_IMAGE_ASPECT_COLOR_BIT);
    }
  };
}
