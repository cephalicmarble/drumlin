#include "gtest/gtest.h"

#include <iostream>
#include <utility>

#include "../main_tao.hpp"
#include "../application.h"
#include "../thread.h"
#include "../thread_worker.h"
#define TAOJSON
#include "../tao_forward.h"
#include "../../gremlin/compat.h"

namespace drumlin {

class ApplicationWorker
    : public ThreadWorker {
public:
    int argc;
    char **argv;
    ApplicationWorker(Type _type, int _argc, char **_argv) : ThreadWorker(_type), argc(_argc), argv(_argv) {}
    virtual void writeToStream(std::ostream &stream)const {
        stream << "ApplicationWorker" << std::endl;
    }
    virtual void writeToObject(json::value *obj)const {
        obj->get_object().insert({std::string("ApplicationWorker"),json::from_string("writeToObject")});
    }
    virtual void getStatus(json::value *obj)const{
        obj->get_object().insert({std::string("ApplicationWorker"),json::from_string("getStatus")});
    }
    virtual void shutdown() {
        ;
    }
    virtual void report(json::value *obj/*,ReportType type*/)const {
        obj->get_object().insert({std::string("ApplicationWorker"),json::from_string("report")});
    }
    virtual void work(Object *,Event *){
        //(void)RUN_ALL_TESTS();
    }
};

Application a;


class ApplicationTest : public ::testing::Test {
protected:

    ApplicationTest() {
        // You can do set-up work for each test here.
    }

    ~ApplicationTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
    }

    void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
    }
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(ApplicationTest, ThreadDoesWork) {
}

// template <typename T>
// bool IsEven(T value) { return (value % 2) == 0; }
//
// // Tests that Foo does Xyz.
// TEST_F(FooTest, DoesXyz) {
//   // Exercises the Xyz feature of Foo.
//   EXPECT_TRUE(::testing::internal::String::EndsWithCaseInsensitive("blargle", "argle"));
//   EXPECT_TRUE(IsEven(4));
//   ASSERT_EQ(1,1);
// }

} // namespace drumlin

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  a.addThread(new Thread("test-worker", new ApplicationWorker(ThreadType_test, argc, argv)));
  a.exec();
}
