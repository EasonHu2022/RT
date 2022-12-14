//
// Copyright (c) 2015 Artyom Beilis (Tonkikh)
// Copyright (c) 2021 Alexander Grund
//
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic ignored "-Wattributes"
#endif
#include <boost/nowide/filesystem.hpp>

#include <boost/nowide/convert.hpp>
#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/fstream.hpp>
#include "test.hpp"
#if defined(_MSC_VER)
#pragma warning(disable : 4714) // function marked as __forceinline not inlined
#endif
#include <boost/filesystem/operations.hpp>

// coverity[root_function]
void test_main(int, char** argv, char**)
{
    boost::nowide::nowide_filesystem();
    const std::string prefix = argv[0];
    const std::string utf8_name =
      prefix + "\xf0\x9d\x92\x9e-\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82-\xE3\x82\x84\xE3\x81\x82.txt";

    {
        boost::nowide::ofstream f(utf8_name.c_str());
        TEST(f);
        f << "Test" << std::endl;
    }

    TEST(boost::filesystem::is_regular_file(boost::nowide::widen(utf8_name)));
    TEST(boost::filesystem::is_regular_file(utf8_name));

    TEST(boost::nowide::remove(utf8_name.c_str()) == 0);

    TEST(!boost::filesystem::is_regular_file(boost::nowide::widen(utf8_name)));
    TEST(!boost::filesystem::is_regular_file(utf8_name));

    const boost::filesystem::path path = utf8_name;
    {
        boost::nowide::ofstream f(path);
        TEST(f);
        f << "Test" << std::endl;
        TEST(is_regular_file(path));
    }
    {
        boost::nowide::ifstream f(path);
        TEST(f);
        std::string test;
        f >> test;
        TEST(test == "Test");
    }
    {
        boost::nowide::fstream f(path);
        TEST(f);
        std::string test;
        f >> test;
        TEST(test == "Test");
    }
    boost::filesystem::remove(path);
}
