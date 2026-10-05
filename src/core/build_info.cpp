#include <veyr/build_info.hpp>

namespace veyr {

std::string_view build_version() noexcept {
  return VEYR_BUILD_VERSION;
}

}  // namespace veyr
