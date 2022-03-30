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
 * @file Publisher.h
 *
 */

#ifndef SHARED_MEM_PUBLISHER_H_
#define SHARED_MEM_PUBLISHER_H_

#include <imagePubSubTypes.h>

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/PublisherListener.h>

// #include <hello_worldPubSubTypes.h>
// #include "hello_world.h"
#include "image.h"

class MessagePublisher
{
public:
    MessagePublisher();
    virtual ~MessagePublisher();

    //! Initialize
    bool init(int width, int height, int chn);

    //! Publish a sample
    bool publish(
        bool waitForListener = true);

    // publish a image
    bool publish(const std::shared_ptr<image> &_image);

    //! Run for number samples for test
    void run_test(
        uint32_t number,
        uint32_t sleep);

    bool is_connected() {
        return m_listener_.firstConnected || m_listener_.n_matched > 0;
    }

private:
    // std::shared_ptr<HelloWorld> m_hello_;
    std::shared_ptr<image> m_image_;

    eprosima::fastrtps::Participant *mp_participant_;
    eprosima::fastrtps::Publisher *mp_publisher_;
    bool stop_;

    class PubListener : public eprosima::fastrtps::PublisherListener
    {
    public:
        PubListener()
            : n_matched(0), firstConnected(false){};

        ~PubListener(){};

        void onPublicationMatched(
            eprosima::fastrtps::Publisher *pub,
            eprosima::fastrtps::rtps::MatchingInfo &info);

        int n_matched;
        bool firstConnected;

    } m_listener_;

    void runThread(
        uint32_t number,
        uint32_t sleep);

    // HelloWorldPubSubType m_type_;
    imagePubSubType m_image_type_;
};

#endif /* PUBLISHER_H_ */
