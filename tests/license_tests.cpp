#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <obfy/obfy.hpp>
#include <cstring>
#include <string>

static const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

std::string generate_license(const char* user) {
    if (!user) return "";
    char result[17] = {0};
    size_t l = strlen(user), lic_ctr = 0;
    int add = 0;
    while (lic_ctr < 16) {
        size_t i = lic_ctr;
        i %= l;
        int current = 0;
        while (i < l) { current += user[i]; i++; }
        current += add;
        add++;
        result[lic_ctr] = letters[current % sizeof letters];
        lic_ctr++;
    }
    return std::string(result);
}

bool check_license(const char* user, const char* users_license) {
    std::string license;
    size_t ll = strlen(users_license);
    size_t l = strlen(user), lic_ctr = 0;
    int add = 0;

    for (size_t i = 0; i < ll; i++)
        if (users_license[i] != '-')
            license += users_license[i];

    while (lic_ctr < license.length()) {
        size_t i = lic_ctr;
        i %= l;
        int current = 0;
        while (i < l) current += user[i++];
        current += add;
        add++;
        if (license[lic_ctr] != letters[current % sizeof letters])
            return false;
        lic_ctr++;
    }
    return true;
}

bool check_license1(const char* user, const char* users_license) {
    OBFY_BEGIN_CODE
    std::string license;
    size_t ll = strlen(users_license);
    size_t l = strlen(user), lic_ctr = OBFY_N(0);

    size_t add = OBFY_N(0), i = OBFY_N(0);

    OBFY_FOR(OBFY_V(i) = OBFY_N(0), OBFY_V(i) < OBFY_V(ll), OBFY_V(i)++)
        OBFY_IF (OBFY_V(users_license[i]) != OBFY_N(45))
            license += users_license[i];
        OBFY_ENDIF
    OBFY_ENDFOR

    OBFY_WHILE (OBFY_V(lic_ctr) < license.length())
        size_t i = lic_ctr;
        OBFY_V(i) %= l;
        int current = 0;
        OBFY_WHILE (OBFY_V(i) < OBFY_V(l))
            OBFY_V(current) += user[OBFY_V(i)];
            OBFY_V(i)++;
        OBFY_ENDWHILE
        OBFY_V(current) += OBFY_V(add);
        ++OBFY_V(add);

        OBFY_IF ((license[lic_ctr] != letters[current % sizeof letters]))
            OBFY_RETURN(false);
        OBFY_ENDIF

        lic_ctr++;
    OBFY_ENDWHILE

    OBFY_RETURN(true);
    OBFY_END_CODE
}

BOOST_AUTO_TEST_CASE(license_example)
{
    const char* user = "obfy";
    const std::string lic = generate_license(user);
    BOOST_CHECK(check_license(user, lic.c_str()));
    BOOST_CHECK(check_license1(user, lic.c_str()));
    BOOST_CHECK_EQUAL(check_license(user, "XXXX"), check_license1(user, "XXXX"));
}

