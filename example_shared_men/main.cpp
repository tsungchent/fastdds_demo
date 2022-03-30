// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorld_main.cpp
 *
 */

#include "message_publisher.h"
#include "message_subscriber.h"

#include <fastrtps/Domain.h>
#include <fastrtps/log/Log.h>

#include <cxxopts.hpp>

using namespace eprosima;
using namespace fastrtps;
using namespace rtps;

int main(int argc, char **argv)
{
    std::cout << "Starting " << std::endl;

    int type = 1;
    int count = 10;
    long sleep = 100;

    cxxopts::Options options("example_shared_men", "this is a simple fastdds sahred memory demo");
    options.add_options()("t, type", "publisher or subscriber or both", cxxopts::value<std::string>()->default_value("subscriber"), "publisher/subscriber/both")("c, count", "count", cxxopts::value<int>()->default_value("20"), "message count")("s, sleep", "sleep", cxxopts::value<int>()->default_value("100"), "sleep gap")("h,help", "Print usage");
    try
    {
        std::cout << "begin to parse: " << std::endl;
        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        std::string mode = result["type"].as<std::string>();

        if (mode == "publisher")
        {
            std::cout << "this is a publisher: " << std::endl;
            count = result["count"].as<int>();
            sleep = result["sleep"].as<int>();

            std::cout << "count = " << count << std::endl;
            std::cout << "sleep = " << sleep << std::endl;

            MessagePublisher pub;
            if (pub.init(1920, 1080, 1))
            {
                pub.run_test(count, sleep);
            }
        }
        else if (mode == "subscriber")
        {
            std::cout << "this is a subscriber: " << std::endl;
            MessageSubscriber sub;
            if (sub.init())
            {
                sub.run();
            }
        }
        else if (mode == "both")
        {
            std::thread thread_sub([]
                                   {
                        MessageSubscriber mysub;
                        if (mysub.init())
                        {
                            mysub.run();
                        } });

            std::thread thread_pub([&]
                                   {
                        MessagePublisher mypub;
                        if (mypub.init(1920,1080, 1))
                        {
                            mypub.run_test(count, sleep);
                        } });

            thread_sub.join();
            thread_pub.join();
        }
        else
        {
            std::cout << mode << " is not support" << std::endl;
            std::cout << "only support subscriber or publisher" << std::endl;
        }

        Domain::stopAll();
        Log::Reset();
    } catch(...) {
        std::cout << options.help() << std::endl;
        exit(-1);
    }

    return 0;
}
