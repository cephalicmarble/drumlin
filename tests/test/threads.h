#ifndef _THREADS_H_
#define _THREADS_H_

#include <future>
#include <string>
#include <gtest/gtest.h>
#include "../drumlin/thread_worker.h"
#include "../drumlin/tao_forward.h"
#include "../drumlin/event.h"
#include "../drumlin/thread_accessor.h"

class TestWorker
    : public ThreadWorker {
public:
    int argc;
    char **argv;
    TestWorker()
    : ThreadWorker(ThreadType_test)
    {
        APLATE;
    }
    ~TestWorker()
    {
        BPLATE;
    }
    virtual void writeToStream(std::ostream &stream)const {
        stream << "ApplicationWorker" << std::endl;
    }
    virtual void writeToObject(json::value *obj)const {
        obj->get_object().insert({std::string("ApplicationWorker"),std::string("writeToObject")});
    }
    virtual void getStatus(json::value *obj)const {
        obj->get_object().insert({std::string("ApplicationWorker"),std::string("getStatus")});
    }
    virtual void shutdown() {
        PLATE;
    }
    virtual void report(json::value *obj/*,ReportType type*/)const {
        obj->get_object().insert({std::string("ApplicationWorker"),std::string("report")});
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
        // {
        //     ThreadAccessor()
        //     .named("terminal")
        //     .getNamed()
        //     (SendEvent(event::make_event(DrumlinEventThreadWork, "next-char")));
        // }
    }
    virtual bool event(std::shared_ptr<Event> pevent)
    {
        switch(pevent->type())
        {
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
    Thread *testWorker = new Thread("test-worker", new TestWorker());
    Thread *terminator = new Thread("terminal", new Terminator());
    std::string workToken("tell-test-worker:00000000-0000-0000-000001:1");
    std::promise<typeof workToken> barrier;
    iapp->queuePromise("promise-name", std::move(barrier));
    // test shell interface thread 'terminator' requires 'testWorker' to do work
    // uses 2-ary function as there is no data to pass in to the WorkObject
    terminator->setWork({std::string(workToken), std::string("promise-name")});
    // sends event "workHere" workToken to testWorker
    // which looks inside and resolves the promise by name
    a.addThread(testWorker, true);
    a.addThread(terminator, true);
    barrier.get_future().wait();
    // testWorker
    ASSERT_EQ(barrier.get_future().get(), workToken);
    ASSERT_EQ(testWorker->doneWork(workToken), true);
    ASSERT_EQ(testWorker->doneWork(workToken + "blargle", false);
    ASSERT_EQ(terminator->doneWork(workToken), true);
    ASSERT_EQ(terminator->doneWork(workToken + "blargle", false);
}

#endif // _THREADS_H_