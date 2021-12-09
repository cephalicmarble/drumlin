#define TAOJSON
#include "thread_worker.h"
#include "event.h"

namespace drumlin {

void ThreadWorker::stop()
{
    shutdown();
    signalTermination();
}

ThreadWorker::ThreadWorker(Type _type,Object *parent = nullptr) : Object(parent),m_type(_type)
{
}

ThreadWorker::ThreadWorker(Type _type,string task) : Object(),m_type(_type)
{
    std::lock_guard<std::recursive_mutex> l(m_critical_section);
    m_thread = new Thread(task);
    m_thread->setWorker(this);
}

/**
 * @brief ThreadWorker::ThreadWorker : worker constructor
 * connects thread->finished() to punt event back to application for removal.
 * Sets worker to this, and moves the worker to the (already created) thread.
 * @param _thread Thread*
 */
ThreadWorker::ThreadWorker(Type _type,Thread *_thread) : Object(),m_thread(_thread),m_type(_type)
{
    std::lock_guard<std::recursive_mutex> l(m_critical_section);
    m_thread = _thread;
    m_thread->setWorker(this);
}

/**
 * @brief ThreadWorker::~ThreadWorker : removes the event filter
 */
ThreadWorker::~ThreadWorker()
{
    if(m_thread)
        m_thread->m_worker = nullptr;
}

/**
 * @brief ThreadWorker::signalTermination
 */
void ThreadWorker::signalTermination()
{
    if(!getThread()->isTerminated())
        getThread()->terminate();
}

void ThreadWorker::report(json::value *obj,ReportType type)const
{
    auto &map(obj->get_object());
    map.insert({"task",getThread()->getTask()});
    map.insert({"type",string(gremlin::metaEnum<Type>().toString(this->m_type))});
    if(type & WorkObject::ReportType::Elapsed){
        map.insert({"elapsed",getThread()->elapsed()});
    }
    if(type & WorkObject::ReportType::Memory){
        map.insert({"memory",0});
    }
    if(type & WorkObject::ReportType::Jobs){
        if(map.end()==map.find("jobs"))
            map.insert({"jobs",json::empty_object});
        for(jobs_type::value_type const& job : m_jobs){
            json::value job_obj(json::empty_object);
            job.second->report(&job_obj,type);
            map.at("jobs").get_object().insert({job.first,job_obj});
        }
    }
}

void ThreadWorker::postWork(Object *sender)
{
    make_event(DrumlinEventThreadWork,__func__,sender)->send(getThread());
}

/* STREAM OPERATORS */

void ThreadWorker::writeToObject(json::value *obj)const
{
    report(obj,WorkObject::ReportType::All);
    obj->get_object().insert({"task",getThread()->getTask()});
}

void ThreadWorker::writeToStream(std::ostream &stream)const
{
    json::value obj(json::empty_object);
    writeToObject(&obj);
    json::to_stream(stream,obj);
}

} // namespace drumlin
