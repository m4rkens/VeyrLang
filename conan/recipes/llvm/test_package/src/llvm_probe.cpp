#include <llvm/ADT/SmallVector.h>
#include <llvm/Config/abi-breaking.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/Support/raw_ostream.h>

#if LLVM_VERSION_MAJOR != 23 || LLVM_VERSION_MINOR != 1 || LLVM_VERSION_PATCH != 2
#error "The probe requires LLVM 23.1.2 headers"
#endif
#if LLVM_ENABLE_ABI_BREAKING_CHECKS
#error "The probe requires ABI-breaking checks disabled"
#endif

int main() {
  int total = 0;
  {
    llvm::SmallVector<int, 0> values;
    for (int value = 0; value < 256; ++value) {
      values.push_back(value);
    }
    for (const int value : values) {
      total += value;
    }
  } // Exercises Support allocation, growth and destruction across the package boundary.
  if (total != 32640) {
    return 1;
  }
  llvm::outs() << "LLVM " << LLVM_VERSION_STRING << ": Support allocation/destruction passed\n";
  return 0;
}
