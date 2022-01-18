#ifndef _WORK_H
#define _WORK_H

#include <string>
#include <unordered_map>
#include <functional>
#include <future>
#include <queue>
#include <mutex>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <boost/container_hash/hash.hpp>
#include "gtypes.h"
#include "metatypes.h"
#include "tao_forward.h"
#include "buffer.h"
#include "allocator.h"
#include "string_list.h"
using namespace tao;

// system, process & thread clocks : 10, 50, 100 ticks.
// rating fr WorkerThread, completion, ETA, lateness etc.
// allocator usage by this thread
#define WorkReportTypes ( \
    (WorkReport_first)(0), \
    (WorkReport_Elapsed)(1), \
    (WorkReport_Jobs)(2), \
    (WorkReport_Heap)(4), \
    (WorkReport_All)(7) \
)

ENUMN(ReportType, WorkReportTypes);

namespace drumlin {

class Application;

namespace Work {

class Token {
    std::string m_target;
    boost::uuids::uuid m_uuid;
    guint64 m_part;
public:
    Token(std::string target, std::string uuid, guint64 part);
    Token(std::string target, boost::uuids::uuid uuid, guint64 part);
    Token(Token const& rhs);
    ~Token();
    bool operator ==(Token const& rhs)const;
    Token clone() const;
    std::string toString();
    friend struct std::hash<Token>;
};

} /* Work */ } /* drumlin */

namespace std {

template<>
struct hash<drumlin::Work::Token>
{
    size_t operator()(drumlin::Work::Token const& t) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, hash<string>{}(t.m_target));
        boost::hash_combine(seed, hash<boost::uuids::uuid>{}(t.m_uuid));
        boost::hash_combine(seed, hash<guint64>{}(t.m_part));
        return seed;
    }
};

}

namespace drumlin { namespace Work {

typedef std::promise<Token> workPromise;
typedef std::unordered_map<std::string, workPromise> promise_map_type;

class Item {
    guint32 m_index;
    Buffers::heap_key_type m_source;
    std::shared_ptr<HeapBuffer> m_item;
};

typedef std::map<int, Item> workItemIndex;
typedef std::vector<std::string> workIndex;

class Object {
    string_list m_descriptors; //[<name>=]<uses>\[<index>\], ...
    workIndex m_index;         //named work items
    workItemIndex m_itemindex; //every work item
    Object();
protected:
    void resolveDescriptors(); // functional style because of event bubbling later
    friend class Store;
    friend class Access;
};

class Access;

typedef std::unordered_map<Token, std::shared_ptr<Object>> work_map_type;
typedef std::queue<Token> work_queue_type;
typedef std::unique_ptr<Access> work_access_type;

class Store {
    work_map_type m_workMap;
    work_queue_type m_workQueue;
    std::recursive_mutex m_critical_section;
public:
    Store();
    virtual ~Store();

    Token next();
    virtual work_access_type setWork(Token workToken);
    virtual work_access_type getWork(Token workToken);
};

class Promises {
    promise_map_type m_promises;
    std::recursive_mutex m_critical_section;
public:
    Promises();
    virtual ~Promises();
    virtual void queuePromise(std::string name, Work::workPromise &&promise);
    friend class Application;
};

class Access {
    Token m_token;
    std::shared_ptr<Object> m_object;
public:
    Access(Token, std::shared_ptr<Object>);
    ~Access();

    Access* addDescriptor(std::string);
    Access* queuePromise(std::string, workPromise &&);
    // returned by Thread::setWork (queue add) and Thread::getWork (queue pull)
    // allows buffering control information (unresolved etc)
};

class Done
{
    Object m_work;
public:
    typedef gremlin::ReportType ReportType;
    virtual ~Done(){}
    virtual void stop(){}
    virtual void report(json::value *obj,ReportType type)const=0;
};

} // namespace Work

} // namespace drumlin

#endif
