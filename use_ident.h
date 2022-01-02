#ifndef _USE_IDENT_H
#define _USE_IDENT_H

#include <iterator>
#include <memory>
#include <functional>
#include <boost/any.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/container_hash/hash.hpp>
#include "tao_forward.h"
using namespace tao;
#include "string_list.h"
#include "logger.h"
#include "gtypes.h"

namespace drumlin {

class UsesAllocator;

class SourcesOnly;
class TickRange;

/**
 * @brief The UseIdent class : for marking & comparing heap allocations
 */
class UseIdent
{
    UsesAllocator* m_source;
    string m_source_name = "";
    string_list m_specification;
    guint32 m_tick = 0;
public:
    UseIdent();
    UseIdent(UsesAllocator* source);
    UseIdent(const UseIdent &rhs);
    UseIdent(UseIdent &&rel);

    std::string const& getSourceName() const { return m_source_name; }

    UseIdent withAttr(attribute_list const& attributes);
    UseIdent withTick(guint32 tick);
    string_list const& getSpecification()const;
    std::string getHash()const;

    void setUse(UsesAllocator*);
    UsesAllocator* getUse()const;

    bool hasComponent(std::string const&)const;

    void operator=(const UseIdent &);
    void operator=(UseIdent &&);
    bool operator<(UseIdent &rhs);
    bool operator==(UseIdent const&);
    bool operator==(UseIdent const&)const;

    void toJson(json::value *object)const;

    friend logger &operator<<(logger &stream, UseIdent &rhs);
    UseIdent &operator<<(std::pair<std::string, std::string> & pair);
    UseIdent &operator<<(std::pair<std::string, boost::any &> &);

    friend SourcesOnly;
    friend TickRange;
    friend struct std::hash<UseIdent>;
};

//UseIdent use(UsesAllocator*)
//Buffers_Cache::relevant(SourcesOnly(use) && TickRange(0,100))

class LogicalAndFilter;
class LogicalOrFilter;
class LogicalNotFilter;

class UseIdentFilter {
protected:
    UseIdent m_ident;
public:
    UseIdentFilter();
    UseIdentFilter(UseIdent &lhs);
    UseIdentFilter(UseIdentFilter &lhs)=default;
    operator UseIdent()const;
    UseIdentFilter(UseIdentFilter const&)=default;
    virtual ~UseIdentFilter(){}
    virtual bool operator==(UseIdent const& rhs)const;
    LogicalAndFilter operator&&(const UseIdentFilter &rhs)const;
    LogicalOrFilter operator||(const UseIdentFilter &rhs)const;
    LogicalNotFilter operator!()const;
};

struct LogicalAndFilter : UseIdentFilter {
    LogicalAndFilter(UseIdentFilter const& _lhs, UseIdentFilter const& _rhs);
    LogicalAndFilter(LogicalAndFilter const&)=default;
    virtual bool operator==(UseIdent const& rhs)const;
private:
    UseIdentFilter m_lhs;
    UseIdentFilter m_rhs;
};

struct LogicalOrFilter : UseIdentFilter {
    LogicalOrFilter(UseIdentFilter const& _lhs, UseIdentFilter const& _rhs);
    LogicalOrFilter(LogicalOrFilter const&)=default;
    virtual bool operator==(UseIdent const& rhs)const;
private:
    UseIdentFilter m_lhs;
    UseIdentFilter m_rhs;
};

struct LogicalNotFilter : UseIdentFilter {
    LogicalNotFilter(UseIdentFilter const& _rhs);
    LogicalNotFilter(LogicalNotFilter const&)=default;
    virtual bool operator==(UseIdent const& rhs)const;
private:
    UseIdentFilter m_rhs;
};

struct SourcesOnly : UseIdentFilter {
    SourcesOnly(UseIdent &use);
    SourcesOnly(SourcesOnly const&)=default;
    virtual bool operator==(UseIdent const& rhs)const;
};

struct HashOnly : UseIdentFilter {
    HashOnly(UseIdent &use);
    HashOnly(HashOnly const&)=default;
    virtual bool operator==(UseIdent const& rhs)const;
};

struct NamesOnly : UseIdentFilter {
    NamesOnly(UseIdent &use);
    NamesOnly(string_list &list);
    NamesOnly(NamesOnly const&)=default;
    NamesOnly &strict();
    virtual bool operator==(UseIdent const& rhs)const;
private:
    string_list m_list;
    bool m_strict = false;
};

struct TickRange : UseIdentFilter {
    TickRange(guint32 min = 0, guint32 max = 0xffffffff);
    TickRange(TickRange const&)=default;
    TickRange &exclusive();
    virtual bool operator==(UseIdent const& rhs)const;
private:
    bool m_exc = false;
    guint32 m_min;
    guint32 m_max;
};

extern UseIdent &operator<<(UseIdent & use, std::pair<std::string, std::string> & pair);
extern UseIdent &operator<<(UseIdent & use, std::pair<std::string, boost::any const&> &);

extern logger &operator<<(logger &stream, UseIdent &rhs);

} // namespace drumlin

template<>
struct std::hash<drumlin::UseIdent>
{
    std::size_t operator()(drumlin::UseIdent const& s) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, std::hash<guint64>{}((guint64)s.m_source));
        boost::hash_combine(seed, std::hash<std::string>{}(s.m_source_name));
        boost::hash_combine(seed, std::hash<std::string>{}(s.m_specification.toString()));
        boost::hash_combine(seed, std::hash<guint32>{}(s.m_tick));
        return seed;
    }
};

#endif // _USE_IDENT_H
