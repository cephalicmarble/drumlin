#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <vector>
#include <string>
using namespace std;
//#include <boost/asio.hpp> TODO:remove these
//#include <boost/thread/mutex.hpp>
//#include <boost/thread/recursive_mutex.hpp>
//#include <boost/thread/lock_guard.hpp>
#include <boost/algorithm/string.hpp>
using namespace boost;

namespace drumlin {

class string_list : public vector<string>
{
public:
    typedef vector<string> base;
    friend string_list& operator<< (string_list &vecS,string const& str);
    friend string_list& operator<< (string_list &vecS,string & str);
    friend string_list& operator<< (string_list &vecS,const char* str);
    friend string_list& operator<< (string_list &vecS,char *str);
    string join(string str);
    string join(const char*pc);
    static string_list fromString(string const& toSplit,string delim,bool all = false,algorithm::token_compress_mode_type = algorithm::token_compress_on);
    static string_list fromString(string const& toSplit,const char* delim,bool all = false,algorithm::token_compress_mode_type = algorithm::token_compress_on);
    static string_list fromString(string const& toSplit,const char delim,bool all = false,algorithm::token_compress_mode_type = algorithm::token_compress_on);
    string_list &operator=(string_list const& rhs);
    string_list():base(){}
    string_list(string_list const&);
    template<class transform_iter_type>
    string_list(transform_iter_type &begin,transform_iter_type &end)
    {
        std::copy(begin,end,back_inserter(*this));
    }
};

extern string_list & operator<< (string_list&,string const& str);
extern string_list & operator<< (string_list&,const char* str);
extern string_list & operator<< (string_list&,string & str);
extern string_list & operator<< (string_list&,char* str);

} // namespace drumlin

#endif // STRING_LIST_H
