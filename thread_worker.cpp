#define TAOJSON
#include "thread_worker.h"

#include <sstream>
#include "event.h"
#include "exception.h"
#include "thread_accessor.h"
#include "metatypes.h"
#include "../gremlin/compat.h"

namespace drumlin {

void ThreadWorker::stop()
{
    shutdown();
    signalTermination();
}

/**
 * @brief ThreadWorker::~ThreadWorker : removes the event filter
 */
ThreadWorker::ThreadWorker(Type _type) : Object(), m_type(_type)
{
}

/**
 * @brief ThreadWorker::~ThreadWorker : removes the event filter
 */
ThreadWorker::~ThreadWorker()
{
}

/**
 * @brief ThreadWorker::signalTermination
 */
void ThreadWorker::signalTermination()
{
    ThreadAccessor access;
    access.getWorkered(this);
    if(access.selectionEmpty()) {
        std::stringstream ss;
        ss << "no thread for " << getType() << " cannot signalTermination.";
        throw Exception(ss);
    }
    if(!access.first()->isTerminated())
        access.first()->terminate();
}

void ThreadWorker::report(json::value *obj/*,ReportType type*/)const
{
    auto &map(obj->get_object());
    //map.insert({"task",getThread()->getTask()});
    map.insert({std::string("type"),gremlin::metaEnum<gremlin::ThreadType>().toString(this->m_type)});
/*
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
*/
}

/* STREAM OPERATORS */

void ThreadWorker::writeToObject(json::value *obj)const
{
    report(obj/*,WorkObject::ReportType::All*/);
    obj->get_object().insert({std::string("type"),gremlin::metaEnum<gremlin::ThreadType>().toString(this->m_type)});
}

void ThreadWorker::writeToStream(std::ostream &stream)const
{
    json::value obj(json::empty_object);
    writeToObject(&obj);
    json::to_stream(stream,obj);
}

} // namespace drumlin
