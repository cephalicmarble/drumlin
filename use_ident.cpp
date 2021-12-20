#define TAOJSON
#include "use_ident.h"

#include <memory>
#include <string>
#include <utility>

#include "uses_allocator.h"
#include "string_list.h"
#include "crypto.h"

namespace drumlin {

/**
 * @brief UseIdent::UseIdent : the default UseIdent is falsy
 * @param untrue
 */
UseIdent::UseIdent()
{
}

UseIdent::UseIdent(UsesAllocator* source)
: m_source(source)
{
    m_source_name = source->getToken();
}

/**
 * @brief UseIdent::UseIdent : move constructor
 * @param rel UseIdent const&&
 */
UseIdent::UseIdent(UseIdent &&rel)
{
    operator =(std::move(rel));
}

/**
 * @brief UseIdent::UseIdent : copy constructor
 * @param rhs UseIdent const&
 */
UseIdent::UseIdent(const UseIdent &rhs)
{
    operator =(rhs);
}

UseIdent UseIdent::withAttr(attribute_list const& attributes)
{
    UseIdent ret(*this);
    for(attribute_list::value_type const& attr : attributes) {
        ret.m_specification << make_pair(attr.first, attr.second);
    }
    return ret;
}

UseIdent UseIdent::withTick(guint32 tick)
{
    UseIdent ret(*this);
    ret.m_tick = tick;
    return ret;
}

string_list const& UseIdent::getSpecification()const
{
    return m_specification;
}

std::string UseIdent::getHash()const
{
    std::string spec(m_specification.toString());
    return crypto::Crypto::sha256(spec);
}

void UseIdent::setUse(UsesAllocator*uses)
{
    m_source = uses;
    m_source_name = m_source->getToken();
}

UsesAllocator* UseIdent::getUse()const
{
    return m_source;
}

bool UseIdent::hasComponent(std::string const& name)const{
    return m_specification.end() !=
        std::find_if(m_specification.begin(), m_specification.end(), [name](auto & attribute){
            return name == attribute.substr(0, attribute.find('='));
        });
}

/**
 * @brief UseIdent::operator = : copy assignment
 * @param rhs UseIdent const&
 */
void UseIdent::operator=(const UseIdent &rhs)
{
    m_specification = rhs.m_specification;
    m_source_name = rhs.m_source_name;
    m_source = rhs.m_source;
    m_tick = rhs.m_tick;
}

/**
 * @brief UseIdent::operator = : copy assignment
 * @param rhs UseIdent const&
 */
void UseIdent::operator=(UseIdent && rhs)
{
    std::swap(m_specification, rhs.m_specification);
    std::swap(m_source_name, rhs.m_source_name);
    std::swap(m_source, rhs.m_source);
    std::swap(m_tick, rhs.m_tick);
}

bool UseIdent::operator<(UseIdent &rhs)
{
    return m_source < rhs.m_source &&
        m_tick < rhs.m_tick &&
        std::distance(m_specification.begin(), m_specification.end()) <
            std::distance(rhs.m_specification.begin(), rhs.m_specification.end());
}

bool UseIdent::operator==(UseIdent const& rhs) {
    return m_source == rhs.m_source &&
        m_tick == rhs.m_tick &&
        m_specification == rhs.m_specification;
}

bool UseIdent::operator==(UseIdent const& rhs) const {
    return m_source == rhs.m_source &&
        m_tick == rhs.m_tick &&
        m_specification == rhs.m_specification;
}

void UseIdent::toJson(json::value *object)const
{
    json::object_t &obj(object->get_object());
    obj.insert({std::string("source"),m_source_name});
    stringstream ss;
    ss << m_specification;
    obj.insert({std::string("spec"),ss.str()});
    obj.insert({std::string("hash"),getHash()});
}

// /**
//  * @brief fromArguments : derive UseIdent from parsed URI
//  * @param c_arguments UriParseFunc::arguments_type
//  * @return UseIdent
//  */
// UseIdent UseIdent::fromArguments(const UriParseFunc::arguments_type &c_arguments,bool argParity)
// {
//     typedef UriParseFunc::arguments_type arguments_type;
//     arguments_type &arguments = const_cast<arguments_type&>(c_arguments);
//     UseIdent ret(true);
//     if(arguments.end()!=arguments.find("source")){
//         ret = arguments.at("source").c_str();
//     }
//     if(arguments.end()!=arguments.find("uuid")){
//         ret.setUuid(arguments.at("uuid"));
//     }
//     for(auto arg : arguments){
//         ret.arguments.insert(arg);
//     }
//     ret.argParity = argParity;
//     return ret;
// }
UseIdentFilter::operator UseIdent()const
{
    return m_ident;
}
UseIdentFilter::UseIdentFilter(){}
UseIdentFilter::UseIdentFilter(UseIdent &lhs):m_ident(lhs){}
bool UseIdentFilter::operator==(UseIdent const&)const{ return false; }
LogicalAndFilter UseIdentFilter::operator&&(const UseIdentFilter &rhs)const
{
    return LogicalAndFilter(*this, rhs);
}
LogicalOrFilter UseIdentFilter::operator||(const UseIdentFilter &rhs)const
{
    return LogicalOrFilter(*this, rhs);
}
LogicalNotFilter UseIdentFilter::operator!()const
{
    return LogicalNotFilter(*this);
}
LogicalAndFilter::LogicalAndFilter(UseIdentFilter const& _lhs, UseIdentFilter const& _rhs)
: UseIdentFilter(), m_lhs(_lhs), m_rhs(_rhs) {}
bool LogicalAndFilter::operator==(UseIdent const& rhs)const
{
    return m_lhs == rhs && m_rhs == rhs;
}
LogicalOrFilter::LogicalOrFilter(UseIdentFilter const& _lhs, UseIdentFilter const& _rhs)
: UseIdentFilter(), m_lhs(_lhs), m_rhs(_rhs) {}
bool LogicalOrFilter::operator==(UseIdent const& rhs)const
{
    return m_lhs == rhs || m_rhs == rhs;
}
LogicalNotFilter::LogicalNotFilter(UseIdentFilter const& _rhs)
: UseIdentFilter(), m_rhs(_rhs) {}
bool LogicalNotFilter::operator==(UseIdent const& rhs)const
{
    return !(m_rhs == rhs);
}
SourcesOnly::SourcesOnly(UseIdent &use):UseIdentFilter(use){}
bool SourcesOnly::operator==(UseIdent const& rhs)const
{
    return m_ident.m_source_name == rhs.m_source_name;
}
HashOnly::HashOnly(UseIdent &use):UseIdentFilter(use){}
bool HashOnly::operator==(UseIdent const& rhs)const
{
    return m_ident.getHash() == rhs.getHash();
}
NamesOnly::NamesOnly(UseIdent &use)
: UseIdentFilter(use)
{
    m_list.clear();
}
NamesOnly::NamesOnly(string_list &list)
{
    m_list = list;
}
NamesOnly& NamesOnly::strict()
{
    m_strict = true;
    return *this;
}
bool NamesOnly::operator==(UseIdent const& rhs)const
{
    return m_list.empty() ?
        m_ident.getSpecification().namesOnly(m_strict)
        == rhs.getSpecification().namesOnly(m_strict) :
        m_list.namesOnly(m_strict)
        == rhs.getSpecification().namesOnly(m_strict);
}
TickRange::TickRange(guint32 min, guint32 max)
: UseIdentFilter(),m_min(min), m_max(max){}
TickRange &TickRange::exclusive()
{
    m_exc = true;
    return *this;
}
bool TickRange::operator==(UseIdent const& rhs)const
{
    return m_exc ?
        (rhs.m_tick > m_min && rhs.m_tick < m_max)
    :
        (rhs.m_tick >= m_min && rhs.m_tick <= m_max);
}

UseIdent &UseIdent::operator<<(std::pair<std::string,std::string> & pair)
{
    m_specification << pair;
    return *this;
}

UseIdent &UseIdent::operator<<(std::pair<std::string,boost::any &> & pair)
{
    m_specification << pair;
    return *this;
}

/**
 * @brief operator << : stream operator
 * @param stream std::ostream &
 * @param rhs UseIdent const&
 * @return std::ostream &
 */
logger &operator<<(logger &stream,UseIdent & use)
{
    stream << "{"
           << "name:" << use.m_source_name
           << ",hash:" << use.getHash()
           << ",spec:" << use.m_specification
           << "}";
    return stream;
}

} // namespace drumlin
