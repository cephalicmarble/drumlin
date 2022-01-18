#include "gtest/gtest.h"
#define TAOJSON
#include "../drumlin/main_tao.h"
#include "../drumlin/thread_worker.h"
#include "../drumlin/event.h"
#include "../drumlin/thread_accessor.h"
#include "exception.h"
#include "regexes.h"
#include "string_lists.h"
#include "byte_arrays.h"
#include "jsonconfigs.h"
#include "tracers.h"

class GTest
    : public ThreadWorker {
public:
    GTest()
    : ThreadWorker(ThreadType_test)
    {
        APLATE;
    }
    ~GTest()
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
        static bool once = false;
        if(pevent->getName() == "beforeWork") {
            EVENTLOG1(pevent, "waiting...");
        } else if(pevent->getName() == "work" && !once) {
            once = true;
            EVENTLOG1(pevent, "GTEST work function...");
            RUN_ALL_TESTS();
        } else if(once) {
            signalTermination();
            //event::post(this, event::make_event(gremlin::DrumlinEventThreadShutdown, "done"));
        }
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
                return true;
            default:
                return false;
        }
    }
};


int main(int argc, char **argv) {
    Application a;
    ::testing::InitGoogleTest(&argc, argv);
    drumlin::iapp = &a;
    a.addThread(new Thread("run-all-tests", new GTest()), true);
    a.exec();
    Debug() << "returning from main";
}
