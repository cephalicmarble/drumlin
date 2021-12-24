#include "gtest/gtest.h"
#include "drumlin/exception.h"

namespace my {
namespace project {
namespace {

// The fixture for testing class Foo.
class ExceptionTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  ExceptionTest() {
     // You can do set-up work for each test here.
  }

  ~ExceptionTest() override {
     // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
  }

  void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
  }

  // Class members declared here can be used by all tests in the test suite
  // for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(ExceptionTest, ExceptionsRaiseAndAreCaught) {
    try {
        throw drumlin::Exception("drumlin");
    } catch (drumlin::Exception &e) {
        EXPECT_EQ(0, strncmp("drumlin", e.what(), 7));
    }
}

// Tests that Foo does Xyz.
// TEST_F(FooTest, DoesXyz) {
//   // Exercises the Xyz feature of Foo.
//   EXPECT_TRUE(::testing::internal::String::EndsWithCaseInsensitive("blargle", "argle"));
//   EXPECT_TRUE(IsEven(4));
//   ASSERT_EQ(1,1);
// }

}  // namespace
}  // namespace project
}  // namespace my
