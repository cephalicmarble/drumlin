#ifndef _THREADS_H_
#define _THREADS_H_

#include <future>
#include <string>
#include <gtest/gtest.h>
#include "../drumlin/thread_worker.h"
#include "../drumlin/tao_forward.h"
#include "../drumlin/event.h"
#include "../drumlin/thread_accessor.h"
#include "../drumlin/uses_allocator.h"
#include "../drumlin/regexen.h"
#include "terminator.h"

class RegexTest : public ::testing::Test {
protected:

    RegexTest() {
        // You can do set-up work for each test here.
    }

    ~RegexTest() override {
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

// Tests that the Foo::Bar() method does Abc.
TEST_F(RegexTest, RegexMatch) {
    ASSERT_EQ(
        SRegex("^\\[(\\d+)\\]\\s*(=)\\s*([-_a-zA-Z0-9]+)$")
            .match("[0]=blargle"), true);
    ASSERT_EQ(
        SRegex("([-_a-zA-Z0-9]+\\s*(=)\\s*([-_a-zA-Z0-9]+)(\\[(\\d+)\\]))?$")
            .match("result=test-worker[0]"), true);
}

// Tests that the Foo::Bar() method does Abc.
TEST_F(RegexTest, RegexCapture) {
    SRegex regex("^\\[(\\d+)\\]\\s*(=)\\s*([-_a-zA-Z0-9]+)$");
    std::string str("[0]=blargle");
    ASSERT_EQ(regex.match(str), true);
    ASSERT_EQ(regex.getImpl().length(), 3+1);
    ASSERT_EQ(regex.getImpl()[1].second, "0");
    ASSERT_EQ(regex.getImpl()[2].second, "=");
    ASSERT_EQ(regex.getImpl()[3].second, "blargle");
    regex.getImpl().substitute(1, "1");
    regex.getImpl().substitute(2, "<>");
    regex.getImpl().substitute(3, "argle");
    ASSERT_EQ(regex.getImpl().replace(), "[1]<>argle");
}

#endif // _THREADS_H_
