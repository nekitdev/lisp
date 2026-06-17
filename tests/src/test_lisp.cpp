#include <gtest/gtest.h>

#include "lisp.hpp"

using namespace lisp;

class TestLisp: public ::testing::Test {
    protected:
        EnvPtr env;

        void SetUp() override {
            env = default_env();
        }
};

// test 1: run expression

TEST_F(TestLisp, TestRun) {
    auto result = run(env, "42");

    EXPECT_TRUE(result);

    auto value = result.value();

    EXPECT_EQ(value, Value(42));
}

// test 2: add three integers

TEST_F(TestLisp, TestAdd) {
    auto result = run(env, "(+ 1 2 3)");

    EXPECT_TRUE(result);

    if (result) {
        auto value = result.value();

        EXPECT_EQ(value, Value(6));
    }
}

// test 3: define and begin

TEST_F(TestLisp, TestDefine) {
    auto result = run(env, "(begin (define n 69) n)");

    EXPECT_TRUE(result);

    if (result) {
        auto value = result.value();

        EXPECT_EQ(value, Value(69));
    }
}

// test 4: lambda increment

TEST_F(TestLisp, TestLambdaIncrement) {
    auto result = run(env, "((lambda (x) (+ x 1)) 6)");

    EXPECT_TRUE(result);

    if (result) {
        auto value = result.value();

        EXPECT_EQ(value, Value(7));
    }
}

// test 5: defun factorial

TEST_F(TestLisp, TestDefunFactorial) {
    auto result = run(env, "(begin (defun f (n) (if (= n 0) 1 (* n (f (- n 1))))) (f 4))");

    EXPECT_TRUE(result);

    if (result) {
        auto value = result.value();

        EXPECT_EQ(value, Value(24));
    }
}

// test 6: car-cdr-cons

TEST_F(TestLisp, TestCarCdr) {
    auto result = run(env, "(car (cdr (cons 13 (cons 42 nil))))");

    EXPECT_TRUE(result);

    if (result) {
        auto value = result.value();

        EXPECT_EQ(value, Value(42));
    }
}

// test 7: not

TEST_F(TestLisp, TestNot) {
    auto result = run(env, "(not true)");

    EXPECT_TRUE(result);

    if (result) {
        auto value = result.value();

        EXPECT_EQ(value, Value(false));
    }
}
