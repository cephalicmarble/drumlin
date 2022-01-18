#ifndef JSONCONFIG_H
#define JSONCONFIG_H

#include <sstream>
#include <iostream>
#include <list>
#include <iostream>
#include <fstream>
#include <memory>
using namespace std;
#include "tao_forward.h"
using namespace tao;
#include "object.h"
#include "logger.h"

namespace drumlin {

namespace Config {

class JsonConfig;

typedef map<string,JsonConfig*> json_map_type;

class json_map_clearer
{
    json_map_type s_jsons;
public:
    json_map_type &getJsons();
    ~json_map_clearer();
};

extern json_map_clearer klaar;

/**
 * @brief The JsonConfig class
 */
class JsonConfig
{
    std::unique_ptr<json::value> m_json;
    JsonConfig();
    JsonConfig(const JsonConfig &rhs);
    JsonConfig(json::value *);
public:
    json::value *object();
    static json_map_type &getJsons();
    static void reload(std::set<string>);
    static JsonConfig* fromJson(string const& _json);
    static JsonConfig* fromFile(string const& path);
    virtual ~JsonConfig();

    void setDefaultValue(json::value *parent,const json::object_initializer &&);
    void setKey(const json::object_initializer && l);
    json::value *getKey(string const& key);
    json::value operator[](string const& key)const;
    json::value &at(string const& key)const;
    json::value getJson()const;
    void save(ostream &device);
    void save(string const& path);
public:
    friend logger &operator<<(logger &stream,const JsonConfig &rel);
    friend json_map_clearer::~json_map_clearer();
};

#define JSON_OBJECT_PROP(parent,name) (*parent.find(name)).toObject()

#define JSON_PROPERTY(parent,name) (*parent.find(name))

extern string devices_config_file;
extern string gstreamer_config_file;
extern string files_config_file;

json::value *object(json::value *obj = 0);
json::value *array(json::value *array = 0);
size_t length(json::value *value);

extern logger &operator<<(logger &stream,const JsonConfig &rel);

} // namespace Config

} // namespace drumlin

#endif // JSONCONFIG_H
