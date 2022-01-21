#ifndef _PROGRAMS_H_
#define _PROGRAMS_H_

#include <string>
#include <gtest/gtest.h>
#include "../drumlin/program.h"
#include "../drumlin/string_list.h"

class ProgramTest : public ::testing::Test {
protected:

    ProgramTest() {
        // You can do set-up work for each test here.
    }

    ~ProgramTest() override {
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
TEST_F(ProgramTest, ProgramParses) {
    drumlin::string_list text;
    text << "TEST";
    text << "Thread:terminator";
    text << "[<basename>]=file://../tests/test/test.json";
    text << "";
    text << "Thread:json-test-parser";
    text << "[anchor]=terminator";
    text << "~wait[%]->work:json-length-check(<>)";
    text << "~each[*]->[~]=work:json-test-parse(<>)";
    text << "";
    text << "Thread:json-test-output";
    text << "[anchor]=test-output";
    text << "~every[*]->[~]=text:<>";
    text << "[*]->work:test-output(<>)";
    auto parser = drumlin::Work::ProgramParser::parse(text.join('\n'));
    ASSERT_EQ(parser.isParsedOkay(), true);
}

#endif // _PROGRAMS_H_
