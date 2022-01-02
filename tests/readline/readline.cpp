#include <gtest/gtest.h>

#include <iostream>
#include <utility>

#include "../drumlin/main_tao.h"
#include "../drumlin/application.h"
#include "../drumlin/signalhandler.h"
#include "../drumlin/thread.h"
#include "../drumlin/thread_worker.h"
#include "../drumlin/thread_accessor.h"
#define TAOJSON
#include "../drumlin/tao_forward.h"
#include "../gremlin/compat.h"
#include "behaviour.h"
#include "terminator.h"

namespace drumlin {

class ApplicationWorker
    : public ThreadWorker {
public:
    int argc;
    char **argv;
    ApplicationWorker(int _argc, char **_argv)
    : ThreadWorker(ThreadType_test), argc(_argc), argv(_argv)
    {
        APLATE;
    }
    ~ApplicationWorker()
    {
        BPLATE;
    }
    virtual void shutdown() {
        PLATE;
    }
    virtual void work(Object *,std::shared_ptr<Event> pevent) {
        if (pevent->getName() == "run-tests") {
            EVENTLOG1(pevent, "GTEST work function...");
            ThreadAccessor access;
            access.named("terminal");
            access.getNamed();

            if(0 == RUN_ALL_TESTS()) {
                access(SendEvent(event::make_event(DrumlinEventThreadNotify, "test-success")));
            }else{
                access(SendEvent(event::make_event(DrumlinEventThreadNotify, "test-failure")));
            }
        } else if(pevent->getName() == "beforeWork") {
            EVENTLOG1(pevent, "waiting...");
        } else if(pevent->getName() == "work") {
            EVENTLOG1(pevent, "working...");
        }
    }
    virtual bool event(std::shared_ptr<Event> pevent)
    {
        switch(pevent->type())
        {
            case DrumlinEventThreadWork:
                if (pevent->getName() == "tick") {
                    EVENTLOG1(pevent, "ticking...");
                    return true;
                }
                return false;
            case DrumlinEventThreadNotify:
                if (pevent->getName() == "beforeStart") {
                    EVENTLOG1(pevent, "preparing...");
                    return true;
                } else if (pevent->getName() == "beforeWork"){
                    EVENTLOG(pevent);
                    return true;
                }
                {
                    ThreadAccessor()
                    .named("terminal")
                    .getNamed()
                    (SendEvent(event::make_event(DrumlinEventThreadWork, "next-char")));
                }
                return true;
            default:
                return false;
        }
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
    std::string str;
    for(int i=1; i < argc; i++) {
        str += std::string(argv[i]);
    }
    std::cout << "main.str: " << str << std::endl;
    std::cout << grammar::parse_as_string<std::string, grammar::sourceRange >(str);
    return 0;
    // Application a;
    // ::testing::InitGoogleTest(&argc, argv);
    // drumlin::iapp = dynamic_cast<ApplicationBase*>(&a);
    // a.addThread(new Thread("test-worker", new ApplicationWorker(argc, argv)), true);
    // a.addThread(new Thread("terminal", new Terminator()), true);
    // // json::value status(json::empty_object);
    // // a.getStatus(&status);
    // // json::to_stream(std::cout, status);
    // a.exec();
    // Debug() << "returning from main";
}
