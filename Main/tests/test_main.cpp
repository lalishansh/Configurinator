#define BOOST_TEST_MODULE HelloWorldTests
#include <boost/test/included/unit_test.hpp>
#include <fmt/core.h>
#include <version.h>

BOOST_AUTO_TEST_CASE(hello_world_test) {
    std::string output = fmt::format("{}.{}.{}",
                                     HelloWorldProject_VERSION_MAJOR,
                                     HelloWorldProject_VERSION_MINOR,
                                     HelloWorldProject_VERSION_PATCH);
    BOOST_TEST(output == HelloWorldProject_VERSION_STRING);
}
