#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-VRI");
    BOOST_CHECK(GetBoolArg("-VRI"));
    BOOST_CHECK(GetBoolArg("-VRI", false));
    BOOST_CHECK(GetBoolArg("-VRI", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-VRIo"));
    BOOST_CHECK(!GetBoolArg("-VRIo", false));
    BOOST_CHECK(GetBoolArg("-VRIo", true));

    ResetArgs("-VRI=0");
    BOOST_CHECK(!GetBoolArg("-VRI"));
    BOOST_CHECK(!GetBoolArg("-VRI", false));
    BOOST_CHECK(!GetBoolArg("-VRI", true));

    ResetArgs("-VRI=1");
    BOOST_CHECK(GetBoolArg("-VRI"));
    BOOST_CHECK(GetBoolArg("-VRI", false));
    BOOST_CHECK(GetBoolArg("-VRI", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noVRI");
    BOOST_CHECK(!GetBoolArg("-VRI"));
    BOOST_CHECK(!GetBoolArg("-VRI", false));
    BOOST_CHECK(!GetBoolArg("-VRI", true));

    ResetArgs("-noVRI=1");
    BOOST_CHECK(!GetBoolArg("-VRI"));
    BOOST_CHECK(!GetBoolArg("-VRI", false));
    BOOST_CHECK(!GetBoolArg("-VRI", true));

    ResetArgs("-VRI -noVRI");  // -VRI should win
    BOOST_CHECK(GetBoolArg("-VRI"));
    BOOST_CHECK(GetBoolArg("-VRI", false));
    BOOST_CHECK(GetBoolArg("-VRI", true));

    ResetArgs("-VRI=1 -noVRI=1");  // -VRI should win
    BOOST_CHECK(GetBoolArg("-VRI"));
    BOOST_CHECK(GetBoolArg("-VRI", false));
    BOOST_CHECK(GetBoolArg("-VRI", true));

    ResetArgs("-VRI=0 -noVRI=0");  // -VRI should win
    BOOST_CHECK(!GetBoolArg("-VRI"));
    BOOST_CHECK(!GetBoolArg("-VRI", false));
    BOOST_CHECK(!GetBoolArg("-VRI", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--VRI=1");
    BOOST_CHECK(GetBoolArg("-VRI"));
    BOOST_CHECK(GetBoolArg("-VRI", false));
    BOOST_CHECK(GetBoolArg("-VRI", true));

    ResetArgs("--noVRI=1");
    BOOST_CHECK(!GetBoolArg("-VRI"));
    BOOST_CHECK(!GetBoolArg("-VRI", false));
    BOOST_CHECK(!GetBoolArg("-VRI", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VRI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VRI", "eleven"), "eleven");

    ResetArgs("-VRI -bar");
    BOOST_CHECK_EQUAL(GetArg("-VRI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VRI", "eleven"), "");

    ResetArgs("-VRI=");
    BOOST_CHECK_EQUAL(GetArg("-VRI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VRI", "eleven"), "");

    ResetArgs("-VRI=11");
    BOOST_CHECK_EQUAL(GetArg("-VRI", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-VRI", "eleven"), "11");

    ResetArgs("-VRI=eleven");
    BOOST_CHECK_EQUAL(GetArg("-VRI", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-VRI", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VRI", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-VRI", 0), 0);

    ResetArgs("-VRI -bar");
    BOOST_CHECK_EQUAL(GetArg("-VRI", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-VRI=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-VRI", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-VRI=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-VRI", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--VRI");
    BOOST_CHECK_EQUAL(GetBoolArg("-VRI"), true);

    ResetArgs("--VRI=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-VRI", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noVRI");
    BOOST_CHECK(!GetBoolArg("-VRI"));
    BOOST_CHECK(!GetBoolArg("-VRI", true));
    BOOST_CHECK(!GetBoolArg("-VRI", false));

    ResetArgs("-noVRI=1");
    BOOST_CHECK(!GetBoolArg("-VRI"));
    BOOST_CHECK(!GetBoolArg("-VRI", true));
    BOOST_CHECK(!GetBoolArg("-VRI", false));

    ResetArgs("-noVRI=0");
    BOOST_CHECK(GetBoolArg("-VRI"));
    BOOST_CHECK(GetBoolArg("-VRI", true));
    BOOST_CHECK(GetBoolArg("-VRI", false));

    ResetArgs("-VRI --noVRI");
    BOOST_CHECK(GetBoolArg("-VRI"));

    ResetArgs("-noVRI -VRI"); // VRI always wins:
    BOOST_CHECK(GetBoolArg("-VRI"));
}

BOOST_AUTO_TEST_SUITE_END()
