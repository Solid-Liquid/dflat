//"Main" for the Catch unit test library

#define CATCH_CONFIG_RUNNER // Allow us to define our own main
#include "catch2/catch.hpp"
#include "config.hpp"

//All tests are done in external .cpp files

int main(int argc, char** argv)
{
    using namespace Catch::clara;
    using namespace dflat;
    Catch::Session session;

    auto cli = session.cli() // Modify the included Catch CLI parser
        | Opt([&](bool) { config::trace = true; })
          ["--trace"]
          ("print parser trace output");

    session.cli(cli); // Use the new CLI parser.
    auto rc = session.applyCommandLine(argc, argv); // Parse the CLI.

    if (rc != 0)
    {
        return rc; // Exit with error.
    }

    return session.run();
}
