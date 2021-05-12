#ifndef STATIC_LINK_VW
#  define BOOST_TEST_DYN_LINK
#endif

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include <string>

#include "io/owning_stream.h"
#include "io/custom_streambuf.h"
#include "io/io_adapter.h"
#include "memory.h"

BOOST_AUTO_TEST_CASE(test_custom_ostream)
{
  auto output_func = [](void* context, const char* buffer, size_t num_bytes) -> ssize_t {
    std::string input(buffer, num_bytes);
    BOOST_CHECK(context == nullptr);
    BOOST_CHECK_EQUAL(input, "This is the test input, 123\n");
    return 0;
  };

  vw::io::owning_ostream stream{
      vw::make_unique<vw::io::writer_stream_buf>(vw::io::create_custom_writer(nullptr, output_func))};

  stream << "This is the test input, " << 123 << std ::endl;
}
