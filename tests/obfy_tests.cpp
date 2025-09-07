#define BOOST_TEST_DYN_LINK
#ifdef OBFY_DEBUG
#define BOOST_TEST_MODULE obfy_debug_tests
#else
#define BOOST_TEST_MODULE obfy_tests
#endif
#include <boost/test/unit_test.hpp>
#define BOOST_AUTO_TEST_OBFY_CASE BOOST_AUTO_TEST_CASE

#define OBFY_ENABLE_FSM_CALL
#include <obfy/obfy_call.hpp>
#include <obfy/obfy_bytes.hpp>
#include <stdint.h>
#include <limits>
#include <memory>

template<template<class> class Extra, typename T>
static void roundtrip(T value)
{
    T original = value;
    { Extra<T> op(value); }
    BOOST_CHECK_EQUAL(value, original);
}

template<template<class> class Extra, typename T>
static void test_type(T rnd)
{
    roundtrip<Extra>(static_cast<T>(0));
    roundtrip<Extra>(std::numeric_limits<T>::max());
    roundtrip<Extra>(rnd);
}

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

BOOST_AUTO_TEST_OBFY_CASE(ratio_and_bitcast)
{
    double d = OBFY_RATIO_D(314, 100);
    float f = OBFY_RATIO_F(157, 50);
    BOOST_CHECK(d > 3.13 && d < 3.15);
    BOOST_CHECK(f > 3.13f && f < 3.15f);

    const uint32_t b32 = 0x4048F5C3u;
    const uint64_t b64 = 0x40091EB851EB851FULL;
    float rf = obfy::obfy_bit_cast<float>(b32);
    double rd = obfy::obfy_bit_cast<double>(b64);
    BOOST_CHECK_EQUAL(obfy::obfy_bit_cast<uint32_t>(rf), b32);
    BOOST_CHECK_EQUAL(obfy::obfy_bit_cast<uint64_t>(rd), b64);
}

BOOST_AUTO_TEST_OBFY_CASE(bit_cast_macro)
{
    const uint32_t b32 = 0x4048F5C3u;
    const uint64_t b64 = 0x40091EB851EB851FULL;
    float rf = OBFY_BIT_CAST(float, b32);
    double rd = OBFY_BIT_CAST(double, b64);
    BOOST_CHECK_EQUAL(OBFY_BIT_CAST(uint32_t, rf), b32);
    BOOST_CHECK_EQUAL(OBFY_BIT_CAST(uint64_t, rd), b64);
}

BOOST_AUTO_TEST_OBFY_CASE(string_literal)
{
    BOOST_CHECK_EQUAL(std::string(OBFY_STR("test")), "test");
    BOOST_CHECK_EQUAL(std::string(OBFY_STR("")), "");
    BOOST_CHECK_EQUAL(std::string(OBFY_STR_ONCE("test once")), "test once");
    BOOST_CHECK(std::wstring(OBFY_WSTR(L"wide")) == L"wide");
}

BOOST_AUTO_TEST_OBFY_CASE(byte_block)
{
    const unsigned char* data = OBFY_BYTES("\x01\x02\x03\x04");
    BOOST_CHECK_EQUAL(data[0], 0x01);
    BOOST_CHECK_EQUAL(data[3], 0x04);

    auto once = OBFY_BYTES_ONCE("\x05\x06");
    BOOST_CHECK_EQUAL(once.size(), 2u);
    BOOST_CHECK_EQUAL(once.data()[0], 0x05);
    BOOST_CHECK_EQUAL(once.data()[1], 0x06);
}

BOOST_AUTO_TEST_OBFY_CASE(float_variable_wrapper)
{
    float f = 0.0f;
    OBFY_V(f) = 1.5f;
    OBFY_V(f) += 2.25f;
    BOOST_CHECK_CLOSE(static_cast<double>(f), 3.75, 0.001);

    double d = 0.0;
    OBFY_V(d) = 2.0;
    OBFY_V(d) *= 1.5;
    BOOST_CHECK_CLOSE(d, 3.0, 0.001);
}

BOOST_AUTO_TEST_OBFY_CASE(refholder_move_only)
{
    std::unique_ptr<int> src(new int(42));
    std::unique_ptr<int> dest;

    obfy::refholder<std::unique_ptr<int>> dest_holder(dest);
    dest_holder = std::move(src);
    BOOST_CHECK(!src);
    BOOST_CHECK(dest && *dest == 42);

    std::unique_ptr<int> src2(new int(5));
    obfy::refholder<std::unique_ptr<int>> src_holder(src2);
    dest_holder = std::move(src_holder);
    BOOST_CHECK(!src2);
    BOOST_CHECK(dest && *dest == 5);

    obfy::refholder<std::unique_ptr<int>> moved_holder(std::move(dest_holder));
    moved_holder = std::unique_ptr<int>(new int(7));
    BOOST_CHECK(dest && *dest == 7);
}

BOOST_AUTO_TEST_OBFY_CASE(extra_operations_roundtrip)
{
    test_type<obfy::extra_rot, uint8_t>(static_cast<uint8_t>(0xA5));
    test_type<obfy::extra_rot, uint16_t>(static_cast<uint16_t>(0xBEEF));
    test_type<obfy::extra_rot, uint32_t>(0xDEADBEEFu);
    test_type<obfy::extra_rot, uint64_t>(0x0123456789ABCDEFULL);

    test_type<obfy::extra_bswap, uint8_t>(static_cast<uint8_t>(0xA5));
    test_type<obfy::extra_bswap, uint16_t>(static_cast<uint16_t>(0xBEEF));
    test_type<obfy::extra_bswap, uint32_t>(0xDEADBEEFu);
    test_type<obfy::extra_bswap, uint64_t>(0x0123456789ABCDEFULL);

    test_type<obfy::extra_affine, uint8_t>(static_cast<uint8_t>(0xA5));
    test_type<obfy::extra_affine, uint16_t>(static_cast<uint16_t>(0xBEEF));
    test_type<obfy::extra_affine, uint32_t>(0xDEADBEEFu);
    test_type<obfy::extra_affine, uint64_t>(0x0123456789ABCDEFULL);
}

OBFY_NOINLINE static int add_obf(int a, int b) { return a + b; }
OBFY_NOINLINE static void inc_obf(int& x) { ++x; }

BOOST_AUTO_TEST_OBFY_CASE(obfuscated_call)
{
    BOOST_CHECK_EQUAL(OBFY_CALL_RET(int, add_obf, 40, 2), 42);
    int v = 1;
    OBFY_CALL(inc_obf, v);
    BOOST_CHECK_EQUAL(v, 2);
}

