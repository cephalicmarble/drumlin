#define TAOJSON
#include "jsonconfig.h"

#include <iostream>
#include <fstream>
#include <set>
using namespace std;
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
//#include "bluetooth.h"
//#include "lowenergy.h"
//#include "device.h"
#include "byte_array.h"
#include "drumlin.h"

namespace drumlin {

namespace Config {

json_map_clearer klaar;

json_map_type &json_map_clearer::getJsons()
{
    return s_jsons;
}

json_map_clearer::~json_map_clearer()
{
    BPLATE;
    for(json_map_type::value_type &config : s_jsons)
    {
        delete config.second;
    }
    s_jsons.clear();
}

JsonConfig::JsonConfig()
{
    APLATE;
    m_json.reset(new json::value(json::from_string(std::string("{}"))));
}

/**
 * @brief JsonConfig::JsonConfig : copy constructor
 * @param rhs JsonConfig&
 */
JsonConfig::JsonConfig(const JsonConfig &rhs)
{
    APLATE;
    m_json.reset(new json::value(*rhs.m_json.get()));
}

JsonConfig::JsonConfig(json::value *_json)
{
    APLATE;
    m_json.reset(_json);
}

/**
 * @brief JsonConfig::object : return the config hash
 * @return QJsonObject
 */
json::value *JsonConfig::object()
{
    return m_json.get();
}

json_map_type &JsonConfig::getJsons()
{
    return klaar.getJsons();
}

void JsonConfig::reload(std::set<string> list)
{
    std::set<string> visible;
    for(json_map_type::value_type &config : getJsons())
    {
        if(config.second != nullptr)
            delete config.second;
        config.second = nullptr;
        if(boost::filesystem::exists(boost::filesystem::path(config.first))) {
            visible.insert(config.first);
        }
    }
    getJsons().clear();
    for(auto & li : visible)
    {
        if(list.find(li) == list.end())
        {
            getJsons().insert(make_pair(li, fromFile(li)));
        }
    }
    for(auto & li : list)
    {
        getJsons().insert(make_pair(li, fromFile(li)));
    }
}

JsonConfig *JsonConfig::fromJson(std::string const& _json)
{
    return new JsonConfig(new json::value(json::from_string(_json)));
}

/**
 * @brief JsonConfig::load : load from file
 * @param path tring
 */
JsonConfig *JsonConfig::fromFile(std::string const& path)
{
    json_map_type::iterator it(getJsons().find(path));
    if(getJsons().end()!=it)
    {
        return it->second;
    }
    boost::filesystem::path p(path);
    if(!boost::filesystem::exists(p))
        return nullptr;
    ifstream strm(path.c_str());
    stringstream ss;
    ss << strm.rdbuf();
    JsonConfig *ret(new JsonConfig(new json::value(json::from_string(ss.str()))));
    getJsons().insert({path,ret});
    return ret;
}

/**
 * @brief JsonConfig::~JsonConfig
 */
JsonConfig::~JsonConfig()
{
    BPLATE;
}

/**
 * @brief JsonConfig::setDefaultValue : convenience function
 * @param parent QJsonObject
 * @param key tring
 * @param _default QJsonValue
 */
void JsonConfig::setDefaultValue(json::value *parent, const json::object_initializer && l)
{
    for(auto _l : l){
        if(!parent->find(_l.first))
            parent->get_object().insert(_l);
    }
}

/**
 * @brief JsonConfig::setKey : set a key in the config
 * @param key tring
 * @param value QJsonValue
 */
void JsonConfig::setKey(const json::object_initializer && l)
{
    m_json->get_object().insert(std::move(l));
}

/**
 * @brief JsonConfig::getKey : return a key from the config
 * @param key tring
 * @return QJsonValue
 */
json::value *JsonConfig::getKey(std::string const& key)
{
    return m_json->find(key);
}

/**
 * @brief JsonConfig::operator []
 * @param key tring
 * @return json::value*
 */
json::value JsonConfig::operator[](std::string const& key)const
{
    json::pointer ptr(key.c_str());
    return m_json->at(ptr);
}

/**
 * @brief JsonConfig::operator []
 * @param key tring
 * @return json::value*
 */
json::value &JsonConfig::at(std::string const& key)const
{
    json::pointer ptr(key.c_str());
    return m_json->at(ptr);
}

json::value JsonConfig::getJson()const
{
    return *m_json.get();
}

/**
 * @brief JsonConfig::save : save to device
 * @param device ostream*
 */
void JsonConfig::save(ostream &device)
{
    json::to_stream(device,*m_json,2);
}

/**
 * @brief JsonConfig::save : save to file
 * @param path tring
 */
void JsonConfig::save(std::string const& path)
{
    boost::filesystem::path p(path);
    if(!boost::filesystem::exists(p))
        return;
    ofstream strm(path.c_str());
    if(!strm.is_open()){
        Debug() << "could not write config '" << path.c_str() << "'";
        return;
    }
    save(strm);
}

std::string devices_config_file;
std::string gstreamer_config_file;
std::string files_config_file;

json::value *object(json::value *obj)
{
    if(!obj) obj = new json::value;
    if(!*obj) obj->prepare_object();
    return obj;
}
json::value *array(json::value *array)
{
    if(!array) array = new json::value;
    if(!*array) array->prepare_array();
    return array;
}
size_t length(json::value *value)
{
    if(value->is_array()){
        return std::distance(value->get_array().begin(),value->get_array().end());
    }else if(value->is_object()){
        return std::distance(value->get_object().begin(),value->get_object().end());
    }else{
        return value->empty()?0:1;
    }
}

/**
 * @brief operator << : stream operator
 * @param stream std::ostream
 * @param rel JsonConfig
 * @return std::ostream
 */
logger &operator<<(logger &stream,const JsonConfig &rel)
{
    json::to_stream(stream.getStream(),*rel.m_json);
    return stream;
}

} // namespace Config

} // namespace drumlin
