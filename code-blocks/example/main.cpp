#include <iostream>

#if !defined( WIN32 )
    #ifdef UNIT_TESTS
        #define BOOST_TEST_MAIN
        #define BOOST_TEST_DYN_LINK
        #include <boost/test/auto_unit_test.hpp>
    #endif
#endif

#include <obfy/obfy.hpp>

int simple_if()
{
    int n = 42;
    OBFY_BEGIN_CODE
       OBFY_IF( n == 42)
            n = 43;
       OBFY_ELSE
            n = 44;
       OBFY_END
    OBFY_END_CODE
    return n;
}

int numeric_wrapper_returner()
{
    int n;
    OBFY_BEGIN_CODE
       n = OBFY_N(42);
    OBFY_END_CODE
    return n;
}

int simple_variable_wrapper_returner()
{
	int n;
	OBFY_BEGIN_CODE
		OBFY_V(n) = OBFY_N(42);
	OBFY_END_CODE
	return n;
}

int simple_variable_wrapper_min_eq()
{
	int n = 64;
	OBFY_BEGIN_CODE
		OBFY_V(n) -= n << OBFY_N(2);
	OBFY_END_CODE
	return n;
}


int variable_wrapper_returner()
{
    int n = numeric_wrapper_returner();
    OBFY_BEGIN_CODE
        OBFY_V(n) ++;
        OBFY_V(n) --;
    OBFY_END_CODE
    return n;
}

int variable_wrapper_operations()
{
    int n = numeric_wrapper_returner();
    int v;
    OBFY_BEGIN_CODE
        OBFY_V(v) = OBFY_V(n) + OBFY_N(1);
        OBFY_V(v) = OBFY_V(v) - OBFY_N(1);
        OBFY_V(v) = OBFY_V(v) * OBFY_N(2);
        OBFY_V(v) = OBFY_V(v) / OBFY_N(2);
        OBFY_V(v) = OBFY_V(v) | OBFY_N(1);
        OBFY_V(v) = OBFY_V(v) & OBFY_N(42);
        OBFY_V(v) = OBFY_V(v) << OBFY_N(1);
        OBFY_V(v) = OBFY_V(v) >> OBFY_N(1);
        OBFY_V(v) = OBFY_V(v) ^ OBFY_N(55);
        OBFY_V(v) = OBFY_V(v) ^ OBFY_N(55);
        OBFY_V(v) = OBFY_V(v) % OBFY_N(41);

        OBFY_V(v) += OBFY_N(41);
        OBFY_V(v) -= OBFY_N(1);
        OBFY_V(v) *= OBFY_N(2);
        OBFY_V(v) /= OBFY_N(2);
        OBFY_V(v) |= OBFY_N(1);
        OBFY_V(v) &= OBFY_N(42);
        OBFY_V(v) <<= OBFY_N(1);
        OBFY_V(v) >>= OBFY_N(1);
        OBFY_V(v) ^= OBFY_N(55);
        OBFY_V(v) ^= OBFY_N(55);
        OBFY_V(v) %= OBFY_N(41);
        OBFY_V(v) += OBFY_N(2);

    OBFY_END_CODE
    return v;
}

int for_loop_test()
{
    int a;
    int n = 0;
    int x;
    OBFY_V(x) = OBFY_N(556);
    OBFY_BEGIN_CODE

        OBFY_FOR(OBFY_V(a) = 0, OBFY_V(a) < OBFY_N(10), OBFY_V(a)++)
            n ++;
        OBFY_ENDFOR

    OBFY_END_CODE
    return n;
}

int for_loop_test_break()
{
    int a;
    int n = 0;
    OBFY_BEGIN_CODE

        OBFY_FOR(OBFY_V(a) = 0, OBFY_V(a) < OBFY_N(10), OBFY_V(a)++)
            OBFY_V(n) ++;
            OBFY_IF(OBFY_V(a) == OBFY_N(5))
                OBFY_BREAK;
            OBFY_END
        OBFY_ENDFOR

    OBFY_END_CODE
    return n;
}

int for_loop_test_continue()
{
    int a;
    int n = 0;
    OBFY_BEGIN_CODE

        OBFY_FOR(OBFY_V(a) = 0, OBFY_V(a) < OBFY_N(10), OBFY_V(a)++)
            OBFY_IF(OBFY_V(a) == OBFY_N(5))
                OBFY_CONTINUE;
            OBFY_END
            OBFY_V(n) ++;
        OBFY_ENDFOR

    OBFY_END_CODE
    return n;
}

int while_loop_test()
{
    int a = 0;
    int n = 0;
    OBFY_BEGIN_CODE

        OBFY_WHILE(OBFY_V(a) < 10)
            OBFY_V(n) ++;
            OBFY_V(a) = OBFY_V(n);
        OBFY_ENDWHILE

    OBFY_END_CODE
    return n;
}

int while_loop_test_break()
{
    int a = 0;
    int n = 0;
    OBFY_BEGIN_CODE

        OBFY_WHILE(OBFY_V(a) < 10)
            OBFY_V(n) ++;
            OBFY_V(a) = OBFY_V(n);
            OBFY_IF (OBFY_V(a) > OBFY_N(5) )
                OBFY_BREAK;
            OBFY_END
        OBFY_ENDWHILE

    OBFY_END_CODE
    return n;
}

int while_loop_test_continue()
{
    int a = 0;
    int n = 0;
    OBFY_BEGIN_CODE

        OBFY_WHILE(OBFY_V(a) < 10)
            OBFY_V(a) ++;
            OBFY_IF (OBFY_V(a) >= OBFY_N(5) )
                OBFY_CONTINUE;
            OBFY_END
            OBFY_V(n) ++;

        OBFY_ENDWHILE

    OBFY_END_CODE
    return n;
}

int repeat_loop_test()
{
    int a = 0;
    int n = 0;

    OBFY_BEGIN_CODE

        OBFY_REPEAT
            ++OBFY_V(a);
            OBFY_V(n) = OBFY_V(a);
        OBFY_AS_LONG_AS ( OBFY_V(a) <= OBFY_N(10))

    OBFY_END_CODE
    return n;
}

int repeat_loop_test_break()
{
    int a = 0;
    int n = 0;

    OBFY_BEGIN_CODE

        OBFY_REPEAT
            OBFY_IF (OBFY_V(n) > OBFY_N(5))
                OBFY_BREAK;
            OBFY_ELSE
                ++OBFY_V(a);
                OBFY_V(n) = OBFY_V(a);
        OBFY_END
        OBFY_AS_LONG_AS ( OBFY_V(a) <= OBFY_N(10))

    OBFY_END_CODE
    return n;
}

int repeat_loop_test_continue()
{
    int a = 0;
    int n = 0;

    OBFY_BEGIN_CODE

        OBFY_REPEAT
            ++OBFY_V(a);

            OBFY_IF (OBFY_V(a) < OBFY_N(7))
                OBFY_CONTINUE;
            OBFY_END

            OBFY_V(n) ++;
        OBFY_AS_LONG_AS ( OBFY_V(a) < OBFY_N(10))

    OBFY_END_CODE
    return n;
}

int case_tester()
{
    const std::string something = "B";
    int n =


#ifndef OBF_DEBUG
            0;

    OBFY_BEGIN_CODE

        OBFY_CASE ( something )

            OBFY_WHEN("Y") OBFY_DO
                OBFY_BREAK;
            OBFY_DONE

            OBFY_WHEN("X") OBFY_DO
                OBFY_BREAK;
            OBFY_DONE

            OBFY_WHEN("A") OBFY_OR OBFY_WHEN("B") OBFY_DO
                OBFY_V(n) = OBFY_N(42);
                OBFY_BREAK;
            OBFY_DONE

            OBFY_WHEN("C") OBFY_DO
                OBFY_V(n) = OBFY_N(43);
                OBFY_BREAK;
            OBFY_DONE

            OBFY_DEFAULT
                OBFY_V(n) = 44;
            OBFY_DONE

        OBFY_ENDCASE

    OBFY_END_CODE
#else
            42;
        #endif
    return n;
}

int case_tester_fallthrough()
{
    std::string something = "B";
    int n =

#ifndef OBF_DEBUG
            0;
    OBFY_BEGIN_CODE

        OBFY_CASE (something)

            OBFY_WHEN( "A" ) OBFY_OR OBFY_WHEN("B") OBFY_DO
                OBFY_V(n) = OBFY_N(42);
            OBFY_DONE

            OBFY_WHEN("C") OBFY_DO
                OBFY_V(n) ++;
            OBFY_DONE

            OBFY_DEFAULT
                OBFY_V(n) ++;
            OBFY_DONE

        OBFY_ENDCASE

    OBFY_END_CODE
#else
    44;
#endif

    return n;
}

int returner()
{
    OBFY_BEGIN_CODE
        OBFY_RETURN(42);
    OBFY_END_CODE
}

struct ATest
{
    int x = 42;
};

ATest class_test(int& a)
{
    OBFY_BEGIN_CODE
        OBFY_IF(OBFY_V(a) == 5)
            OBFY_V(a) = 42;
        OBFY_END
        OBFY_RETURN (ATest()) ;
    OBFY_END_CODE
}

#if !defined (WIN32) && defined (UNIT_TESTS)

#include <stdint.h>

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

#else
#include <iostream>
int main()
{
	std::cout
        << "42:" << numeric_wrapper_returner() << std::endl
        << "-192:" << simple_variable_wrapper_min_eq() << std::endl
        << "42:" << variable_wrapper_returner() << std::endl
        << "42:" << variable_wrapper_operations() << std::endl
        << "11:" << repeat_loop_test() << std::endl
        << "6:" << repeat_loop_test_break() << std::endl
        << "4:" << repeat_loop_test_continue() << std::endl
        << "42:" << case_tester() << std::endl
        << "44:" << case_tester_fallthrough() << std::endl
        << "42:" << returner() << std::endl;

    int a = 5;
    ATest x = class_test(a);
    std::cout << "42:" << x.x << std::endl;

    int64_t bigNumber;
    OBFY_V(bigNumber) = OBFY_N(1537232811123);
    std::cout << "1537232811123:" << bigNumber << std::endl;
}

#endif
