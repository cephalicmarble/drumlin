#ifndef _JSONCONFIGS_H_
#define _JSONCONFIGS_H_

#include <iostream>
#include <future>
#include <string>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>
#define TAOJSON
#include "../drumlin/tao_forward.h"
#include "../drumlin/event.h"
#include "../drumlin/uses_allocator.h"
#include "../drumlin/byte_array.h"
#include "../drumlin/jsonconfig.h"
#include "terminator.h"

class JsonConfigTest : public ::testing::Test {
protected:
    JsonConfigTest() {
        // You can do set-up work for each test here.
    }

    ~JsonConfigTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
    }

    void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
    }
public:
    static std::string jsons;
    static std::string tempFilePath;
};

std::string JsonConfigTest::jsons("{ \
    \"a\":\"one\", \
    \"b\":\"two\", \
    \"c\":[ \
        1, \
        2 \
    ], \
    \"d\":{ \
        \"a\":\"one\", \
        \"b\":\"two\" \
    } \
}");

TEST_F(JsonConfigTest, String) {
    std::unique_ptr<drumlin::Config::JsonConfig> cfg(drumlin::Config::JsonConfig::fromJson(JsonConfigTest::jsons));
    std::string d_a = cfg->at("/d/a").get_string();
    //ASSERT_EQ(d_a->is_string(), true);
    ASSERT_STREQ(d_a.c_str(), "one");
    int c = 0;
    for(auto item : cfg->at("/c").get_array())
    {
        ASSERT_EQ(item.get_unsigned(), ++c);
    }
}

TEST_F(JsonConfigTest, File) {
    std::string path;
    drumlin::Config::JsonConfig *cfg;
    {
        temp_file tmp;
        tmp.get_ofstream() << JsonConfigTest::jsons << std::endl;
        cfg = drumlin::Config::JsonConfig::fromFile(path = tmp.get_path().string());
    }
    drumlin::Config::JsonConfig *config = drumlin::Config::JsonConfig::getJsons().at(path);
    ASSERT_STREQ(json::to_string(config->getJson()).c_str(), json::to_string(json::from_string(JsonConfigTest::jsons.c_str())).c_str());
    ASSERT_STREQ(cfg->at("/a").get_string().c_str(), "one");
}

TEST_F(JsonConfigTest, Reload) {
    std::string path;
    {
        temp_file tmp;
        {
            std::ofstream ostr = tmp.get_ofstream();
            ostr << JsonConfigTest::jsons;
            ostr.flush();
        }
        path = tmp.get_path().string();
        std::cout << path << std::endl;
        drumlin::Config::JsonConfig::reload({path});
    }
    drumlin::Config::JsonConfig *config = drumlin::Config::JsonConfig::getJsons().at(path);
    ASSERT_STREQ(json::to_string(config->getJson()).c_str(), json::to_string(json::from_string(JsonConfigTest::jsons.c_str())).c_str());
    ASSERT_STREQ(config->at("/b").get_string().c_str(), "two");
}

#endif // _JSONCONFIGS_H_
