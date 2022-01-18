#include "work.h"

#include <boost/uuid/uuid_io.hpp>
#include "drumlin.h"
#include "application.h"
#include "exception.h"

namespace drumlin {

namespace Work {

Token::Token(std::string target, std::string uuid, guint64 part)
: m_target(target), m_part(part)
{
    boost::uuids::string_generator gen;
    m_uuid = gen(uuid);
}

Token::Token(std::string target, boost::uuids::uuid uuid, guint64 part)
: m_target(target), m_uuid(uuid), m_part(part)
{}

Token::Token(Token const& rhs)
: m_target(rhs.m_target), m_uuid(rhs.m_uuid), m_part(rhs.m_part)
{}

Token::~Token()
{}

bool Token::operator ==(Token const& rhs)const
{
    return
        m_target == rhs.m_target
        && m_uuid == rhs.m_uuid
        && m_part == rhs.m_part;
}

Token Token::clone() const
{
    Token wt(*this);
    wt.m_part++;
    return wt;
}

std::string Token::toString()
{
    std::stringstream ss;
    ss << m_target << ":" << m_uuid << ":" << m_part;
    return ss.str();
}

Object::Object()
{}

Store::Store()
{}

Store::~Store()
{}

work_access_type Store::setWork(Token workToken)
{
    THREADSLOCK;
    if (m_workMap.find(workToken) != m_workMap.end()) {
        throw Exception("Already seen " + workToken.toString() + ".");
    }
    auto pair = make_pair(workToken, new drumlin::Work::Object());
    m_workMap.insert(pair);
    m_workQueue.push(workToken);
    return work_access_type(new Access(workToken, m_workMap[workToken]));
}

work_access_type Store::getWork(Token workToken)
{
    THREADSLOCK;
    if (m_workMap.find(workToken) == m_workMap.end()) {
        throw Exception("Canot find " + workToken.toString() + ".");
    }
    return work_access_type(new Access(workToken, m_workMap[workToken]));
}

Promises::Promises()
{}
Promises::~Promises()
{}

void Promises::queuePromise(std::string name, Work::workPromise &&promise)
{
    THREADSLOCK;
    if (m_promises.find(name) != m_promises.end()) {
        throw Exception("Already promised "+ name +".");
    }
    m_promises[name] = std::move(promise);
}

Access::Access(Token token, std::shared_ptr<Object> object)
:m_token(token), m_object(object)
{
}

Access::~Access()
{
}

Access* Access::addDescriptor(std::string descriptor)
{
    m_object->m_descriptors << descriptor;
    return this;
}

Access* Access::queuePromise(std::string name, workPromise && promise)
{
    iapp->queuePromise(name, std::move(promise));
    return this;
}

} // namespace Work

} // namespace drumlin
