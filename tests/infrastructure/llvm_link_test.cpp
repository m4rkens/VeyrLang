#include <gtest/gtest.h>

#include <llvm/ADT/SmallVector.h>
#include <llvm/Config/abi-breaking.h>
#include <llvm/Config/llvm-config.h>

static_assert(LLVM_VERSION_MAJOR == 23 && LLVM_VERSION_MINOR == 1 && LLVM_VERSION_PATCH == 2);
static_assert(LLVM_ENABLE_ABI_BREAKING_CHECKS == 0);

TEST(LLVMIntegration, SupportAllocationUseAndDestruction) {
  // Exceed inline capacity: allocation/growth/destruction crosses libSupport.
  llvm::SmallVector<int, 2> values;
  for (int value = 0; value < 128; ++value) {
    values.push_back(value);
  }
  ASSERT_EQ(values.size(), 128U);
  EXPECT_EQ(values.front(), 0);
  EXPECT_EQ(values.back(), 127);
  values.resize(3);
  EXPECT_EQ(values[2], 2);
}
