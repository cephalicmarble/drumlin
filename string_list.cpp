#include "string_list.h"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#include "drumlin.h"

namespace drumlin {

string string_list::join(string str)
{
    return algorithm::join(*this,str);
}

string string_list::join(const char*pc)
{
    return join(string(pc ? pc : ""));
}

string_list string_list::fromString(string const& toSplit,const char* delim,bool all,algorithm::token_compress_mode_type flags)
{
    return string_list::fromString(toSplit,string(delim),all,flags);
}

string_list string_list::fromString(string const& toSplit,const char delim,bool all,algorithm::token_compress_mode_type flags)
{
    return string_list::fromString(toSplit,string(&delim),all,flags);
}

string_list string_list::fromString(string const& toSplit,string delim,bool all,algorithm::token_compress_mode_type flags)
{
    string_list list;
    if(!all){
        algorithm::split(list,toSplit,algorithm::is_any_of(delim),flags);
    }else{
        string::size_type pos(string::npos);
        string s(toSplit);
        while((pos=s.find(delim))!=string::npos)s.replace(pos,delim.length(),"¬");
        algorithm::split(list,s,algorithm::is_any_of("¬"),flags);
    }
    return list;
}

string_list &string_list::operator=(string_list const& rhs)
{
    clear();
    std::copy(rhs.begin(),rhs.end(),back_inserter(*this));
    return *this;
}

string_list::string_list(string_list const& rhs) : base()
{
    *this = rhs;
}

string_list::operator attribute_list()const
{
    attribute_list map;
    int ordinal = 0;
    for(auto &item : *this) {
        auto lpos(item.find('='));
        if(item.begin() + lpos != item.end()) {
            map.insert({item.substr(0, lpos), item.substr(lpos + 1)});
        } else {
            map.insert({boost::lexical_cast<std::string>(ordinal++), item});
        }
    }
    return map;
}

std::string string_list::toString()const
{
    std::stringstream ss;
    std::for_each(begin(), end(), [&ss](auto & str){ ss << str;});
    return ss.str();
}

string_list string_list::namesOnly(bool strict)const
{
    string_list names;
    std::for_each(begin(), end(),
        [&names, strict](std::string const& item){
        auto lpos(item.find('='));
        if(item.begin() + lpos != item.end()) {
            names.push_back(item.substr(0, lpos));
        } else if(!strict) {
            names.push_back(item);
        }
    });
    return names;
}

bool string_list::operator==(string_list &rhs)
{
    std::vector<std::string> diff;
    std::set_difference(
        begin(), end(),
        rhs.begin(), rhs.end(),
        std::back_inserter(diff)
    );
    return diff.empty();
}

string_list &operator<< (string_list &vecS,string const& str)
{
    vecS.push_back(str);
    return vecS;
}

string_list &operator<< (string_list &vecS,string & str)
{
    vecS.push_back(str);
    return vecS;
}

string_list &operator<< (string_list &vecS,const char* str)
{
    vecS.push_back(string(str));
    return vecS;
}

string_list &operator<< (string_list &vecS,char* str)
{
    vecS.push_back(string(str));
    return vecS;
}

string_list & operator<< (string_list &vecS,std::pair<std::string,boost::any const&> const& pair)
{
    std::stringstream ss;
    ss << pair.first << "=";
    boost::mpl::for_each<value_types>(drumlin::stream_operator_impl(ss,pair.second));
    vecS.push_back(ss.str());
    return vecS;
}

string_list & operator<< (string_list &vecS,std::pair<std::string,std::string> const& pair)
{
    std::stringstream ss;
    ss << pair.first + "=" + pair.second;
    vecS.push_back(ss.str());
    return vecS;
}

ostream& operator <<(ostream &stream, string_list const& list)
{
    for_each(list.begin(), list.end(),[&stream](string_list::value_type const& line){
        stream << line << std::endl;
    });
    return stream;
}

}
