#ifndef _BYTEARRAYS_H_
#define _BYTEARRAYS_H_

#include <future>
#include <string>
#include <gtest/gtest.h>
#include "../drumlin/tao_forward.h"
#include "../drumlin/event.h"
#include "../drumlin/uses_allocator.h"
#include "../drumlin/byte_array.h"
#include "terminator.h"

class ByteArrayTest : public ::testing::Test {
protected:

    ByteArrayTest() {
        // You can do set-up work for each test here.
    }

    ~ByteArrayTest() override {
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

std::string pc[] = {
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
};

TEST_F(ByteArrayTest, Initializer) {
    byte_array b(byte_array::fromData(pc[1].c_str(), 0, pc[1].length()));
    //byte_array nb(std::move(b));
    byte_array c(byte_array::fromData(pc[1]));
    //ASSERT_STREQ(nb.cdata(), "b");
    ASSERT_STREQ(c.cdata(), b.cdata());
    //byte_array t = c;
    //ASSERT_STREQ(t.cdata(), b.cdata());
    c.append(pc[3]).append(pc[4]);
    ASSERT_STREQ(c.cdata(), "b\0d\0e");
    ASSERT_STREQ(string_list(c).join(',').c_str(), "b,d,e");
}

TEST_F(ByteArrayTest, Conversions) {
    byte_array a(byte_array::fromData(std::string("one,two,three")));
    a.truncate(7);
    ASSERT_STREQ(a.cdata(), "one,two");
    ASSERT_STREQ(std::string(a).c_str(), "one,two");
    a.clear();
    ASSERT_EQ(a.empty(), true);
}

#endif // _THREADS_H_
