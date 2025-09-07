#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#define BOOST_AUTO_TEST_OBFY_CASE BOOST_AUTO_TEST_CASE

#include <obfy/obfy.hpp>
#include <stdint.h>

int numeric_wrapper_returner();
int simple_variable_wrapper_min_eq();
int variable_wrapper_returner();
int variable_wrapper_operations();
int for_loop_test();
int for_loop_test_break();
int for_loop_test_continue();
int while_loop_test();
int while_loop_test_break();
int while_loop_test_continue();
int repeat_loop_test();
int repeat_loop_test_break();
int repeat_loop_test_continue();
int case_tester();
int case_tester_fallthrough();
int returner();

struct ATest { int x = 42; };
ATest class_test(int& a);

BOOST_AUTO_TEST_OBFY_CASE(test_wrappers)
{
    BOOST_CHECK_EQUAL(numeric_wrapper_returner(), 42);
    BOOST_CHECK_EQUAL(simple_variable_wrapper_min_eq(), -192);

    BOOST_CHECK_EQUAL(variable_wrapper_returner(), 42);
    BOOST_CHECK_EQUAL(variable_wrapper_operations(), 42);
}

BOOST_AUTO_TEST_OBFY_CASE(for_loops)
{
    BOOST_CHECK_EQUAL(for_loop_test(), 10);
    BOOST_CHECK_EQUAL(for_loop_test_break(), 6);
    BOOST_CHECK_EQUAL(for_loop_test_continue(), 9);
}

BOOST_AUTO_TEST_OBFY_CASE(while_loops)
{
    BOOST_CHECK_EQUAL(while_loop_test(), 10);
    BOOST_CHECK_EQUAL(while_loop_test_break(), 6);
    BOOST_CHECK_EQUAL(while_loop_test_continue(), 4);
}

BOOST_AUTO_TEST_OBFY_CASE(repeat_loops)
{
    BOOST_CHECK_EQUAL(repeat_loop_test(), 11);
    BOOST_CHECK_EQUAL(repeat_loop_test_break(), 6);
    BOOST_CHECK_EQUAL(repeat_loop_test_continue(), 4);
}

BOOST_AUTO_TEST_OBFY_CASE(case_test)
{
    BOOST_CHECK_EQUAL(case_tester(), 42);
    BOOST_CHECK_EQUAL(case_tester_fallthrough(), 44);
}

BOOST_AUTO_TEST_OBFY_CASE(return_test)
{
    BOOST_CHECK_EQUAL(returner(), 42);
    int a = 5;
    ATest x = class_test(a);
    BOOST_CHECK_EQUAL(a, 42);
    BOOST_CHECK_EQUAL(x.x, 42);
}

BOOST_AUTO_TEST_OBFY_CASE(bignumber)
{
    int64_t bigNumber;
    OBFY_V(bigNumber) = OBFY_N(1537232811123);
    BOOST_CHECK_EQUAL(bigNumber,1537232811123);
}

