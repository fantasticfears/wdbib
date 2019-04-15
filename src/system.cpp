#include "system.h"

#include <optional>

#include <absl/strings/str_cat.h>
#include <fmt/format.h>

namespace wdbib {

namespace detail {

char const* kUnknown = "unknown";

#ifdef __APPLE__
#include <sys/sysctl.h>
std::optional<std::string> callSysctl(const char* name)
{
  char buf[256];
  size_t size = sizeof(buf);
  int ret = sysctlbyname(name, buf, &size, nullptr, 0);
  if (ret != 0) {
    return {};
  } else {
    return {buf};
  }
}

std::string getPlatform()
{
  std::string res;
  if (auto rel = callSysctl("kern.ostype"); rel) {
    absl::StrAppend(&res, *rel);
  } else {
    absl::StrAppend(&res, kUnknown);
  }
  if (auto arch = callSysctl("hw.machine"); arch) {
    absl::StrAppend(&res, "-", *arch);
  }
  if (auto ver = callSysctl("kern.osversion"); ver) {
    absl::StrAppend(&res, "/", *ver);
  }
  return res;
}

#elif __linux__ && !__ANDROID__
#include <sys/utsname.h>

std::string getPlatform()
{
  struct utsname buf;
  int ret = uname(&buf);
  if (ret != 0) {
    return kUnknown;
  } else {
    return absl::StrCat(buf.sysname, "-", buf.release, "/", buf.machine);
  }
}
#else
constexpr std::string getPlatform() { return kUnknown; }
#endif

#if __GNUC__ && !__clang__
std::string getBuildInfo()
{
  return absl::StrCat("g++-", __GNUC__, ".", __GNUC_MINOR__, ".",
                      __GNUC_PATCHLEVEL__
#ifdef __GLIBCXX__
                      ,
                      "/", __GLIBCXX__
#endif
  );
}
#elif __clang__
std::string getBuildInfo()
{
  return absl::StrCat("clang++-", __clang_major__, ".", __clang_minor__, ".",
                      __clang_patchlevel__
#ifdef _LIBCPP_VERSION
                      ,
                      "/", _LIBCPP_VERSION
#endif
  );
}
#else
constexpr char* getBuildInfo() { return kUnknown; }
#endif

}  // namespace detail

std::string GetUserAgent()
{
  return fmt::format("wdbib/{0} {1} {2}", kVersion, detail::getPlatform(),
                     detail::getBuildInfo());
}

} // namespace wdbib
