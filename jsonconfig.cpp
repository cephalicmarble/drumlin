#include <tao/json.hpp>
using namespace tao;
#include <iostream>
#include <fstream>
using namespace std;
#include "byte_array.h"
using namespace drumlin;
#include "jsonconfig.h"
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include "socket.h"

namespace Config {

json_map_clearer klaar;

json_map_type JsonConfig::s_jsons;

void reload()
{
    for(json_map_type::value_type &config : JsonConfig::s_jsons){
        delete config.second;
        config.second = load(config.first).object();
    }
}

JsonConfig load(string path)
{
    return JsonConfig(path);
}

JsonConfig::JsonConfig() : Object(0),temporaryFlag(true)
{
    json = fromJson("{}");
}

/**
 * @brief JsonConfig::JsonConfig
 */
JsonConfig::JsonConfig(std::string const& path) : Object(0),temporaryFlag(false)
{
    json = fromFile(path);
}

/**
 * @brief JsonConfig::JsonConfig : copy constructor
 * @param rhs JsonConfig&
 */
JsonConfig::JsonConfig(const JsonConfig &rhs) :
    Object(0),json(rhs.json),temporaryFlag(rhs.temporaryFlag)
{
}

/**
 * @brief JsonConfig::~JsonConfig
 */
JsonConfig::~JsonConfig()
{
    if(temporaryFlag){
        delete json;
    }
}

/**
 * @brief JsonConfig::object : return the config hash
 * @return QJsonObject
 */
json::value *JsonConfig::object()
{
    return json;
}

/**
 * @brief JsonConfig::getKey : return a key from the config
 * @param key tring
 * @return QJsonValue
 */
json::value *JsonConfig::getKey(std::string const& key)const
{
    return json->find(key);
}

/**
 * @brief JsonConfig::has
 * @param key string
 * @return bool
 */
bool JsonConfig::has(std::string const& key)const
{
    return !!getKey(key);
}

/**
 * @brief JsonConfig::operator []
 * @param key tring
 * @return json::value*
 */
json::value &JsonConfig::operator[](std::string const& key)const
{
    return at(key);
}

/**
 * @brief JsonConfig::at
 * @param key string
 * @return json::value&
 */
json::value &JsonConfig::at(std::string const& key)const
{
    json::pointer ptr(key.c_str());
    return json->at(ptr);
}

/**
 * @brief JsonConfig::setKey : set a key in the config
 * @param initializer json::object_initializer
 */
void JsonConfig::setKey(const json::object_initializer && l)
{
    json->get_object().insert(std::move(l));
}

json::value *JsonConfig::fromJson(std::string const& _json)
{
    json = new json::value(json::from_string(_json));
    return json;
}

/**
 * @brief JsonConfig::load : load from file
 * @param path tring
 */
json::value *JsonConfig::fromFile(std::string const& path)
{
    json_map_type::iterator it(s_jsons.find(path));
    if(s_jsons.end()!=it){
        json = it->second;
        return json;
    }
    filesystem::path p(path);
    if(!filesystem::exists(p))
        return nullptr;
    ifstream strm(path.c_str());
    stringstream ss;
    ss << strm.rdbuf();
    json = new json::value(json::from_string(ss.str()));
    s_jsons.insert({path,json});
    return json;
}

/**
 * @brief JsonConfig::save : save to device
 * @param device ostream*
 */
void JsonConfig::save(ostream &device)
{
    json::to_stream(device,*json,2);
}

/**
 * @brief JsonConfig::save : save to file
 * @param path tring
 */
void JsonConfig::save(std::string const& path)
{
    filesystem::path p(path);
    if(!filesystem::exists(p))
        return;
    ofstream strm(path.c_str());
    if(!strm.is_open()){
        Debug() << "could not write config '" << path.c_str() << "'";
        return;
    }
    save(strm);
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
    json::to_stream(stream,*rel.json);
    return stream;
}

json_map_clearer::~json_map_clearer()
{
    for(json_map_type::value_type & pair : JsonConfig::s_jsons){
        delete pair.second;
    }
}

} // namespace Config
