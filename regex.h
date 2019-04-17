#ifndef REGEX_H
#define REGEX_H

#include <string>
#include <functional>
using namespace std;
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
using namespace boost;
#include "drumlin.h"

namespace drumlin {

using namespace boost::regex_constants;

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

    drumlin_regex(string rx,syntax_option_type option)
        : m_rx(rx,option),cap()
    {}
    drumlin_regex(drumlin_regex const& rhs) = default;
    virtual ~drumlin_regex() = default;
    bool match(T haystack,match_flag_type flags = boost::regex_constants::match_default)
    {
        return boost::regex_match(haystack,cap,m_rx,flags);
    }
    bool search(T haystack,match_flag_type flags = boost::regex_constants::match_default)
    {
        return boost::regex_search(haystack,cap,m_rx,flags);
    }
    std::string replace(std::string haystack,size_t n,std::string subst)const
    {
        drumlin::string_list list{
            haystack.substr(0,cap.position(n)),
            subst,
            haystack.c_str() + cap.position(n) + cap.length(n),
        };
        return list.join();
    }
    match_type const& getCap()const{ return cap; }
protected:
    regex_type m_rx;
    match_type cap;
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

    iterate_impl() {}
    iterate_impl(string &haystack,Regex &rx,typename Regex::match_flag_type flags = boost::regex_constants::match_default)
        : m_haystack(haystack),m_iter(rx.make_regex_iterator(m_haystack,flags))
    {}
    int operator()(func_type func = [](iterate_impl<Regex> &,capture_type &cap){ return true; })
    {
        int c = 0;
        for(;m_iter != iterator_type();++m_iter) {
            c++;
            capture_type &cap(const_cast<capture_type&>(*m_iter));
            if(!func(*this,cap))
                break;
        }
        return c;
    }
    void substitute(capture_type cap,size_t n,std::string subst)
    {
        m_captures.push_back(make_pair(replacement{cap.position(n),cap.length(n)},subst));
    }
    std::string replace()const
    {
        std::string::size_type pos(0);
        drumlin::string_list list;
        for(typename captures_type::size_type i=0; i < m_captures.size();i++) {
            typename captures_type::value_type const& replacement(m_captures[i]);

            list << m_haystack.substr(pos,replacement.first.position - pos);
            list << replacement.second;

            pos = replacement.first.position + replacement.first.length;
        }
        list << m_haystack.substr(pos);
        return list.join();
    }
    std::string m_haystack;
    iterator_type m_iter;
    captures_type m_captures;
};

struct SRegex : public drumlin_regex<boost::smatch,boost::sregex_iterator,string>
{
    typedef drumlin_regex<boost::smatch,boost::sregex_iterator,string> Base;
    typedef iterate_impl<SRegex> iterate_type;
    typedef typename iterate_type::func_type func_type;
    int iterate(string haystack,func_type func,match_flag_type flags = boost::regex_constants::match_default)
    {
        m_impl = iterate_type(haystack,*this,flags);
        int ret = m_impl(func);
        return ret;
    }
    iterator_type make_regex_iterator(std::string &haystack,match_flag_type flags)
    {
        return iterator_type(haystack.begin(),haystack.end(),m_rx,flags);
    }
    SRegex clone()const{
        return *this;
    }
    SRegex() : Base("",boost::regex_constants::extended) {}
    SRegex(string rx,syntax_option_type option = boost::regex_constants::extended) : Base(rx,option) {}
    SRegex(SRegex const& rhs) : Base(rhs){}
    iterate_impl<SRegex> &getImpl(){ return m_impl; }
private:
    iterate_impl<SRegex> m_impl;
};

struct CRegex : public drumlin_regex<boost::cmatch,boost::cregex_iterator,const char*>
{
    typedef drumlin_regex<boost::cmatch,boost::cregex_iterator,const char*> Base;
    typedef iterate_impl<CRegex> iterate_type;
    typedef typename iterate_type::func_type func_type;
    int iterate(string haystack,func_type func,match_flag_type flags = boost::regex_constants::match_default)
    {
        m_impl = iterate_type(haystack,*this,flags);
        int ret = m_impl(func);
        return ret;
    }
    iterator_type make_regex_iterator(std::string &haystack,match_flag_type flags)
    {
        return iterator_type(&*haystack.begin(),&*haystack.end(),m_rx,flags);
    }
    bool match(std::string &haystack,match_flag_type flags = boost::regex_constants::match_default)
    {
        return boost::regex_match(haystack.c_str(),cap,m_rx,flags);
    }
    bool search(std::string &haystack,match_flag_type flags = boost::regex_constants::match_default)
    {
        return boost::regex_search(haystack.c_str(),cap,m_rx,flags);
    }
    CRegex clone()const{
        return *this;
    }
    CRegex() : Base("",boost::regex_constants::extended) {}
    CRegex(string rx,syntax_option_type option = boost::regex_constants::extended) : Base(rx,option) {}
    CRegex(CRegex const& rhs) : Base(rhs){}
    iterate_impl<CRegex> &getImpl(){ return m_impl; }
private:
    iterate_impl<CRegex> m_impl;
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
