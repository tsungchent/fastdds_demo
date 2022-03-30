#include <stdio.h>
#include <hello_world.h>
#include <hello_worldPubSubTypes.h>

#include <fastrtps/Domain.h>
#include <fastrtps/log/Log.h>

#include <hello_publisher.h>
#include <hello_subscriber.h>

#include <cxxopts.hpp>

using namespace eprosima;
using namespace fastrtps;
using namespace rtps;
using namespace eprosima::fastdds::dds;

int main(int argc, char *argv[])
{
    // set log level: info
    Log::SetVerbosity(Log::Kind::Info);
    // logInfo(HelloWorld, "This is an info log.");
    // logWarning(HelloWorld, "This is an warning log.");
    // logError(HelloWorld, "This is an error log.");

    std::cout << "Starting " << std::endl;

    int count = 10;
    long sleep = 100;

    cxxopts::Options options("example_simple", "this is a simple fastdds demo");
    options.add_options()("t, type", "publisher or subscriber", cxxopts::value<std::string>()->default_value("subscriber"), "publisher/subscriber")("c, count", "count", cxxopts::value<int>()->default_value("20"), "message count")("s, sleep", "sleep", cxxopts::value<int>()->default_value("100"), "sleep gap");

    std::cout << "begin to parse: " << std::endl;
    auto result = options.parse(argc, argv);
    std::string mode = result["type"].as<std::string>();

    if (mode == "publisher")
    {
        std::cout << "this is a publisher: " << std::endl;
        count = result["count"].as<int>();
        sleep = result["sleep"].as<int>();
        
        std::cout << "count = " << count << std::endl;
        std::cout << "sleep = " << sleep << std::endl;

        HelloPublisher pub;
        if (pub.init())
        {
            pub.run(count, sleep);
        }
    }
    else if (mode == "subscriber")
    {
        std::cout << "this is a subscriber: " << std::endl;
        HelloSubscriber sub;
        if (sub.init())
        {
            sub.run();
        }
    }
    else
    {
        std::cout << mode << " is not support" << std::endl;
        std::cout << "only support subscriber or publisher" << std::endl;
    }

    return 0;
}