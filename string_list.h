#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <list>
#include <string>
using namespace std;
//#include <boost/asio.hpp> TODO:remove these
//#include <boost/thread/mutex.hpp>
//#include <boost/thread/recursive_mutex.hpp>
//#include <boost/thread/lock_guard.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/any.hpp>
using namespace boost;
#include <map>
#include <string>

namespace drumlin {

typedef std::map<std::string, std::string> attribute_list;

class string_list : public std::list<string>
{
public:
    typedef std::list<string> base;
    friend string_list& operator<< (string_list &vecS,string const& str);
    friend string_list& operator<< (string_list &vecS,string & str);
    friend string_list& operator<< (string_list &vecS,const char* str);
    friend string_list& operator<< (string_list &vecS,char *str);
    friend string_list& operator<< (string_list &vecS,std::pair<std::string,boost::any const&> const& pair);
    friend string_list& operator<< (string_list &vecS,std::pair<std::string,std::string> const& pair);
    string join(string str);
    string join(const char*pc = 0);
    static string_list fromString(string const& toSplit,string delim,bool all = false,algorithm::token_compress_mode_type = algorithm::token_compress_on);
    static string_list fromString(string const& toSplit,const char* delim,bool all = false,algorithm::token_compress_mode_type = algorithm::token_compress_on);
    static string_list fromString(string const& toSplit,const char delim,bool all = false,algorithm::token_compress_mode_type = algorithm::token_compress_on);
    string_list &operator=(string_list const& rhs);
    string_list():base(){}
    string_list(string_list const&);
    string_list(iterator_traits<std::pair<string_list::const_iterator, string_list::const_iterator>> const&);
    template<class transform_iter_type>
    string_list(transform_iter_type &begin,transform_iter_type &end)
    {
        std::copy(begin,end,back_inserter(*this));
    }
    operator attribute_list()const;
    std::string toString()const;
    string_list namesOnly(bool strict = true)const;
    bool operator==(string_list &rhs);
    friend ostream& operator <<(ostream &stream, string_list const& list);
};

extern string_list & operator<< (string_list&,string const& str);
extern string_list & operator<< (string_list&,const char* str);
extern string_list & operator<< (string_list&,string & str);
extern string_list & operator<< (string_list&,char* str);
extern string_list & operator<< (string_list &vecS,std::pair<std::string,boost::any const&> const& pair);
extern string_list & operator<< (string_list &vecS,std::pair<std::string,std::string> const& pair);

extern ostream& operator <<(ostream &stream, string_list const& list);

} // namespace drumlin

#endif // STRING_LIST_H
