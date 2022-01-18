#ifndef _TRACERS_H_
#define _TRACERS_H_

#include <iostream>
#include <future>
#include <string>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>
#define TAOJSON
#include "../drumlin/tao_forward.h"
#include "../drumlin/cursor.h"
#include "terminator.h"

class TracerTest : public ::testing::Test {
protected:
    TracerTest() {
        // You can do set-up work for each test here.
    }

    ~TracerTest() override {
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

TEST_F(TracerTest, SingleStackEvent) {
    std::string path;
    {
        temp_file tmp;
        path = tmp.get_path().string();
        drumlin::Tracer::startTrace(path);
        drumlin::Tracer::Cursor::backtrace(8);
        drumlin::Tracer::Cursor::backtrace(8);
        drumlin::Tracer::endTrace();
        std::stringstream ss;
        ss << tmp.get_ifstream().rdbuf();
        json::value output(json::from_string(ss.str()));
        ASSERT_EQ(8, output.get_object().at("functions").get_array().size());
        ASSERT_EQ(2, output.get_object().at("events").get_array().size());
        json::array_t events(output.get_object().at("events").get_array());
        ASSERT_EQ(8, events[0].get_object().at("stack").get_array().size());
        ASSERT_EQ(8, events[1].get_object().at("stack").get_array().size());
    }
}

#endif // _TRACERS_H_
