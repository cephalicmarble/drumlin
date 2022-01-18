#ifndef _STRINGLISTS_H_
#define _STRINGLISTS_H_

#include <future>
#include <string>
#include <gtest/gtest.h>
#include "../drumlin/tao_forward.h"
#include "../drumlin/event.h"
#include "../drumlin/uses_allocator.h"
#include "../drumlin/string_list.h"
#include "terminator.h"

class StringListTest : public ::testing::Test {
protected:

    StringListTest() {
        // You can do set-up work for each test here.
    }

    ~StringListTest() override {
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
};

TEST_F(StringListTest, Initializer) {
    string_list list = {
        "Here is",
        "a string_list."
    };
    ASSERT_EQ(list[0], "Here is");
    ASSERT_EQ(list[1], string_list{"a string_list."}.join());
    ASSERT_EQ(list[0], string_list::fromString("one,two,Here is,a string_list", ',')[2]);
}

TEST_F(StringListTest, Conversions) {
    string_list list = {
        "one=Here is",
        "two=a string_list."
    };
    attribute_list al(list);
    ASSERT_EQ(al["one"], "Here is");
    ASSERT_EQ(al["two"], "a string_list.");
    ASSERT_EQ(list.toString(), "one=Here istwo=a string_list.");
    ASSERT_EQ(list.join('\n'), "one=Here is\ntwo=a string_list.");
    ASSERT_EQ(list.namesOnly(true)[1], "two");
}

#endif // _THREADS_H_
