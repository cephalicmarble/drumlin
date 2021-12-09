#ifndef METATYPES_H
#define METATYPES_H

#include <boost/preprocessor.hpp>

namespace gremlin {

template <typename Identifier = void> struct enum_traits {};
template <typename Identifier = void> typename enum_traits<Identifier>::meta_type metaEnum(){ return typename enum_traits<Identifier>::meta_type(); }

#define ENUM_TO_STRING_CASE(r,data,elem) case data(elem): return #elem;
#define STRING_TO_ENUM(r,data,elem) else if(s==#elem) {*ok = true;return data(elem);}
#define ENUM(Identifier,Tuple) \
  namespace gremlin { \
    typedef enum { BOOST_PP_TUPLE_ENUM(BOOST_PP_TUPLE_SIZE(Tuple),Tuple) } Identifier; \
    struct meta##Identifier { \
        typedef Identifier enum_type; \
        std::string toString(Identifier e) { \
            switch(e) { \
                BOOST_PP_LIST_FOR_EACH(ENUM_TO_STRING_CASE,Identifier, \
                    BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_SIZE(Tuple),Tuple) \
                ) \
                default: return ""; \
            } \
        } \
        Identifier toEnum(string s,bool *ok) { \
            if(false) {*ok = false;return (Identifier)0; } \
            BOOST_PP_LIST_FOR_EACH(STRING_TO_ENUM,Identifier, \
                BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_SIZE(Tuple),Tuple) \
            ) \
            else { *ok = false; return (Identifier)0; } \
        } \
    }; \
    template <> struct enum_traits<Identifier> { typedef meta##Identifier meta_type; }; \
    template <Identifier> typename enum_traits<Identifier>::meta_type metaEnum(){ return typename enum_traits<Identifier>::meta_type(); } \
  } // namespace gremlin


#define ENUMI_TO_STRING_CASE(r,data,elem) case data(data##elem): return #data#elem;
#define STRING_TO_ENUMI(r,data,elem) else if(s==#data#elem) {*ok = true;return data(data##elem);}
#define ENUMI_ELEM(r,data,elem) ,data##elem
#define ENUMI(Identifier,Tuple) \
  namespace gremlin { \
    typedef enum { Identifier##Unknown=0 BOOST_PP_LIST_FOR_EACH(ENUMI_ELEM,Identifier,BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_SIZE(Tuple),Tuple)) } Identifier; \
    struct meta##Identifier { \
        typedef Identifier enum_type; \
        std::string toString(Identifier e) { \
            switch(e) { \
                BOOST_PP_LIST_FOR_EACH(ENUMI_TO_STRING_CASE,Identifier, \
                    BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_SIZE(Tuple),Tuple) \
                ) \
                default: return ""; \
            } \
        } \
        Identifier toEnum(string s,bool *ok) { \
            if(false) {*ok = false;return (Identifier)0; } \
            BOOST_PP_LIST_FOR_EACH(STRING_TO_ENUMI,Identifier, \
                BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_SIZE(Tuple),Tuple) \
            ) \
            else { *ok = false; return (Identifier)0; } \
        } \
    }; \
    template <> struct enum_traits<Identifier> { typedef meta##Identifier meta_type; }; \
    template <Identifier> typename enum_traits<Identifier>::meta_type metaEnum(){ return typename enum_traits<Identifier>::meta_type(); } \
  } // namespace gremlin

#define ENUM_ELEM(r,data,elem) BOOST_PP_SEQ_ELEM(0,elem) BOOST_PP_SEQ_ELEM(1,elem)
#define ENUMN(Identifier,Tuple) \
  namespace gremlin { \
    typedef enum { BOOST_PP_LIST_FOR_EACH(ENUM_ELEM,,BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_SIZE(Tuple),Tuple)) } Identifier; \
    struct meta##Identifier { \
        typedef Identifier enum_type; \
        std::string toString(Identifier e) { \
            switch(e) { \
                BOOST_PP_LIST_FOR_EACH(ENUM_TO_STRING_CASE,Identifier, \
                    BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_SIZE(Tuple),Tuple) \
                ) \
                default: return ""; \
            } \
        } \
        Identifier toEnum(string s,bool *ok) { \
            if(false) {*ok = false;return (Identifier)0; } \
            BOOST_PP_LIST_FOR_EACH(STRING_TO_ENUM,Identifier, \
                BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_SIZE(Tuple),Tuple) \
            ) \
            else { *ok = false; return (Identifier)0; } \
        } \
    }; \
    template <> struct enum_traits<Identifier> { typedef meta##Identifier meta_type; }; \
    template <Identifier> typename enum_traits<Identifier>::meta_type metaEnum(){ return typename enum_traits<Identifier>::meta_type(); } \
  } // namespace gremlin

//struct lazy
//{
//public:
//    virtual bool isLazy()const { return !isLoaded; }
//    lazy(bool loaded):isLoaded(loaded){}
//    lazy():lazy(false){}
//protected:
//    virtual void load()=0;
//private:
//    bool isLoaded;
//};

} // namespace gremlin

using namespace gremlin;

#endif // METATYPES_H
