#include "gtest/gtest.h"

#include <iostream>
#include <utility>

#include "../drumlin/main_tao.h"
#include "../drumlin/application.h"
#include "../drumlin/signalhandler.h"
#include "../drumlin/thread.h"
#include "../drumlin/thread_worker.h"
#include "../drumlin/thread_accessor.h"
#include "../drumlin/tao_forward.h"
#include "../gremlin/compat.h"
#include "terminator.h"

namespace drumlin {

// template <typename T>
// bool IsEven(T value) { return (value % 2) == 0; }
//
// // Tests that Foo does Xyz.
// TEST_F(FooTest, DoesXyz) {
//   // Exercises the Xyz feature of Foo.
//   EXPECT_TRUE(::testing::internal::String::EndsWithCaseInsensitive("blargle", "argle"));
//   EXPECT_TRUE(IsEven(4));
// }

} // namespace drumlin

#define TAOJSON
#include "exception.h"
#include "threads.h"

int main(int argc, char **argv) {
    Application a;
    drumlin::iapp = dynamic_cast<ApplicationBase*>(&a);
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    a.exec();
    Debug() << "returning from main";
}
