#ifndef THREAD_WORKER_H_
#define THREAD_WORKER_H_

#include <iostream>
#include "tao_forward.h"
using namespace tao;
#include "drumlin.h"
#include "object.h"
//#include "registry.h"
#include "status.h"
#include "gtypes.h"
#include "../gremlin/compat.h"

namespace drumlin {

/**
 * @brief The ThreadWorker class
 */
class ThreadWorker :
    public Object,
//    public WorkObject,
    public StatusProvider
{
public:
    double elapsed;
    //typedef Registry<WorkObject> jobs_type;
protected:
    //jobs_type m_jobs;
public:
    typedef gremlin::ThreadType Type;
    /**
     * @brief writeToStream
     * @param stream std::ostream&
     */
    virtual void writeToStream(std::ostream &stream)const;
    virtual void writeToObject(json::value *obj)const;
    virtual void getStatus(json::value *)const{}
public:
    std::recursive_mutex m_critical_section;
    /**
     * @brief getType
     * @return ThreadType
     */
    Type getType()const{ return m_type; }
    /**
     * @brief start
     */
    //jobs_type const& getJobs()const{ return m_jobs; }
    void stop();
    ThreadWorker(Type _type);
    virtual ~ThreadWorker();
    virtual void shutdown()=0;
    void signalTermination();
    virtual void report(json::value *obj/*,ReportType type*/)const;
    virtual void work(Object *,Event *){}
    virtual bool event(Event *){return false;}
    friend class Server;
protected:
    Type m_type;
};

} // namespace drumlin

#endif // THREAD_WORKER_H_
