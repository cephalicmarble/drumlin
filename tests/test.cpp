#include "gtest/gtest.h"

#include <iostream>
#include <utility>

#include "../main_tao.hpp"
#include "../application.h"
#include "../signalhandler.h"
#include "../thread.h"
#include "../thread_worker.h"
#include "../thread_accessor.h"
#include "../terminator.h"
#define TAOJSON
#include "../tao_forward.h"
#include "../../gremlin/compat.h"

namespace drumlin {

class ApplicationWorker
    : public ThreadWorker {
public:
    int argc;
    char **argv;
    ApplicationWorker(Type _type, int _argc, char **_argv)
    : ThreadWorker(_type), argc(_argc), argv(_argv)
    {
        APLATE;
    }
    ~ApplicationWorker()
    {
        BPLATE;
    }
    virtual void writeToStream(std::ostream &stream)const {
        stream << "ApplicationWorker" << std::endl;
    }
    virtual void writeToObject(json::value *obj)const {
        obj->get_object().insert({std::string("ApplicationWorker"),std::string("writeToObject")});
    }
    virtual void getStatus(json::value *obj)const{
        obj->get_object().insert({std::string("ApplicationWorker"),std::string("getStatus")});
    }
    virtual void shutdown() {
        signalTermination();
    }
    virtual void report(json::value *obj/*,ReportType type*/)const {
        obj->get_object().insert({std::string("ApplicationWorker"),std::string("report")});
    }
    virtual void work(Object *,Event *pevent){
        if (pevent->getName() == "run-tests") {
            ThreadAccessor access;
            access.named("terminal");
            access.getNamed();
            if(0 == RUN_ALL_TESTS()) {
                access(SendEvent(event::make_event(DrumlinEventThreadNotify, "test-success")));
            }else{
                access(SendEvent(event::make_event(DrumlinEventThreadNotify, "test-failure")));
            }
        }
    }
    virtual bool event(std::shared_ptr<Event>)
    {
        return false;
    }
};

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
    ASSERT_EQ(1,1);
}

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

int main(int argc, char **argv) {
    Application a;
    ::testing::InitGoogleTest(&argc, argv);
    drumlin::iapp = dynamic_cast<ApplicationBase*>(&a);
    //a.addThread(new Thread("test-worker", new ApplicationWorker(ThreadType_test, argc, argv)), false);
    a.addThread(new Thread("terminal", new Terminator()), true);
    // json::value status(json::empty_object);
    // a.getStatus(&status);
    // json::to_stream(std::cout, status);
    a.exec();
    Debug() << "returning from main";
}
