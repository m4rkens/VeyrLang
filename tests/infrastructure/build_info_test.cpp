#include <veyr/build_info.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ostream>
#include <streambuf>
#include <string_view>

namespace {

class MockOutputBuffer : public std::streambuf {
 public:
  MOCK_METHOD(std::streamsize, xsputn, (const char*, std::streamsize), (override));
};

TEST(BuildInfo, StableNonemptyBuildIdentity) {
  const auto first = veyr::build_version();
  EXPECT_FALSE(first.empty());
  EXPECT_EQ(first, veyr::build_version());
  EXPECT_NE(first.find('.'), std::string_view::npos);
}

TEST(BuildInfo, VersionReachesOutputWithoutTruncation) {
  const auto version = veyr::build_version();
  testing::StrictMock<MockOutputBuffer> buffer;
  std::ostream output{&buffer};
  EXPECT_CALL(buffer, xsputn(testing::_, static_cast<std::streamsize>(version.size())))
      .WillOnce([version](const char* data, std::streamsize count) {
        EXPECT_EQ(std::string_view(data, static_cast<std::size_t>(count)), version);
        return count;
      });
  output << version;
  EXPECT_TRUE(output.good());
}

TEST(BuildInfo, FailedVersionOutputIsObservable) {
  testing::StrictMock<MockOutputBuffer> buffer;
  std::ostream output{&buffer};
  EXPECT_CALL(buffer, xsputn(testing::_, testing::_)).WillOnce(testing::Return(0));
  output << veyr::build_version();
  EXPECT_TRUE(output.bad());
}

}  // namespace
