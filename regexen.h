#ifndef REGEX_H
#define REGEX_H

#include <string>
#include <functional>
#include <memory>
using namespace std;
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
using namespace boost;
#include "drumlin.h"
#include "string_list.h"

namespace drumlin {

using namespace boost::regex_constants;

class CRegex;
class SRegex;
template <class Regex> struct iterate_impl;

template <class Match,class Iterator,class T>
class drumlin_regex
{
public:
    typedef boost::regex regex_type;
    typedef Match match_type;
    typedef Iterator iterator_type;
    typedef typename iterator_type::value_type capture_type;
    typedef typename iterator_type::value_type::reference sub_match_type;
    typedef boost::regex_constants::match_flag_type match_flag_type;
    typedef boost::regex_constants::syntax_option_type syntax_option_type;
    typedef iterate_impl<drumlin_regex<match_type,iterator_type,T> > impl_type;
protected:
    drumlin_regex(string rx,syntax_option_type option)
        : m_rx(rx,option),m_cap()
    {
        APLATE;
    }
    drumlin_regex(drumlin_regex const& rhs)
    {
        APLATE;
        m_rx = rhs.m_rx;
        m_cap = rhs.m_cap;
    }
    virtual ~drumlin_regex()
    {
        BPLATE;
    }
public:
    iterator_type make_regex_iterator(std::string &haystack,match_flag_type flags)
    {
        return iterator_type(haystack.begin(),haystack.end(),m_rx,flags);
    }
    bool match(T haystack,match_flag_type flags = boost::regex_constants::match_default)
    {
        if(boost::regex_match(haystack,m_cap,m_rx,flags)) {
            m_impl.reset(new impl_type(haystack, *this, flags));
            m_impl->operator()(getCap());
            return true;
        }
        return false;
    }
    bool search(T haystack,match_flag_type flags = boost::regex_constants::match_default)
    {
        if(boost::regex_search(haystack,m_cap,m_rx,flags)) {
            m_impl.reset(new impl_type(haystack, *this, flags));
            m_impl->operator()(getCap());
            return true;
        }
        return false;
    }
    std::string replace(std::string haystack,size_t n,std::string subst)const
    {
        drumlin::string_list list({
            haystack.substr(0,m_cap.position(n)),
            subst,
            haystack.c_str() + m_cap.position(n) + m_cap.length(n),
        });
        return list.join();
    }
    match_type const& getCap()const{ return m_cap; }
protected:
    regex_type m_rx;
    match_type m_cap;
    std::unique_ptr<impl_type> m_impl;
};

template <class Regex>
struct iterate_impl
{
    struct replacement {
        std::string::difference_type position;
        std::string::difference_type length;
    };
    typedef typename Regex::iterator_type iterator_type;
    typedef typename Regex::capture_type capture_type;
    typedef typename Regex::match_type match_type;
    typedef std::vector<std::pair<replacement,std::string>> captures_type;
    typedef std::function<bool(iterate_impl<Regex> &,capture_type &)> func_type;

    // iterate_impl()
    // {
    //     APLATE;
    // }
    iterate_impl(string haystack,Regex &rx,typename Regex::match_flag_type flags = boost::regex_constants::match_default)
        : m_haystack(haystack),m_iter(rx.make_regex_iterator(m_haystack,flags))
    {
        APLATE;
    }
    ~iterate_impl(){
        BPLATE;
    }
    int operator()(match_type const& cap)
    {
        size_t c;
        for(c=0;c<cap.size();c++)
        {
            m_captures.push_back({
                replacement{cap.position(c), cap.length(c)},
                m_haystack.substr(cap.position(c), cap.length(c))
            });
        }
        return c;
    }
    auto &operator[](long unsigned idx)const
    {
        return m_captures[idx];
    }
    int length()const
    {
        return m_captures.size();
    }
    void substitute(size_t n,std::string subst)
    {
        m_captures[n].second = subst;
    }
    std::string replace()const
    {
        std::string::size_type pos(0);
        std::stringstream ss;
        for(typename captures_type::size_type i=1; i < m_captures.size();i++) {
            typename captures_type::value_type const& replacement(m_captures[i]);

            ss << m_haystack.substr(pos,replacement.first.position - pos);
            pos += replacement.first.position - pos;
            ss << replacement.second;
            pos += replacement.first.length;
        }
        if(pos < m_haystack.length()) {
            ss << m_haystack.substr(pos);
        }
        return ss.str();
    }
    std::string m_haystack;
    iterator_type m_iter;
    captures_type m_captures;
    captures_type m_replacements;
};

struct SRegex : public drumlin_regex<boost::smatch,boost::sregex_iterator,string>
{
    typedef drumlin_regex<boost::smatch,boost::sregex_iterator,string> Base;
    typedef iterate_impl<SRegex> iterate_type;
    typedef typename iterate_type::func_type func_type;
    SRegex clone()const{
        return *this;
    }
    SRegex()
    : Base("",boost::regex_constants::extended)
    {
        APLATE;
    }
    SRegex(string rx,syntax_option_type option = boost::regex_constants::extended)
    : Base(rx,option)
    {
        APLATE;
    }
    SRegex(SRegex const& rhs)
    : Base(rhs)
    {
        APLATE;
    }
    ~SRegex()
    {
        BPLATE;
    }
    impl_type &getImpl(){ return *m_impl; }
};

struct CRegex : public drumlin_regex<boost::cmatch,boost::cregex_iterator,const char*>
{
    typedef drumlin_regex<boost::cmatch,boost::cregex_iterator,const char*> Base;
    typedef iterate_impl<CRegex> iterate_type;
    typedef typename iterate_type::func_type func_type;
    CRegex clone()const{
        return *this;
    }
    CRegex()
    : Base("",boost::regex_constants::extended)
    {
        APLATE;
    }
    CRegex(string rx,syntax_option_type option = boost::regex_constants::extended)
    : Base(rx,option)
    {
        APLATE;
    }
    CRegex(CRegex const& rhs) : Base(rhs)
    {
        APLATE;
    }
    ~CRegex()
    {
        BPLATE;
    }
    impl_type &getImpl(){ return *m_impl; }
};

typedef boost::iterator_range<typename std::string::iterator> iterator_range_type;
inline iterator_range_type make_iterator_range(typename std::remove_const<typename boost::smatch::reference>::type match)
{
    typedef typename std::string::iterator iterator;
    return boost::make_iterator_range(iterator(const_cast<char*>(&*match.first)),iterator(const_cast<char*>(&*match.second)));
}

typedef boost::iterator_range<typename std::add_pointer<const typename std::string::value_type>::type> char_iterator_range_type;
inline char_iterator_range_type make_iterator_range(typename std::remove_const<typename boost::cmatch::reference>::type match)
{
    return boost::make_iterator_range(const_cast<char*>(&*match.first),const_cast<char*>(&*match.second));
}

} // namespace drumlin

#endif // REGEX_H
