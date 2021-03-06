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
 * @file MessageSubscriber.h
 *
 */

#ifndef SHARED_MEM_SUBSCRIBER_H_
#define SHARED_MEM_SUBSCRIBER_H_

// #include <hello_worldPubSubTypes.h>
#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
// #include "hello_world.h"

#include <imagePubSubTypes.h>
#include <image.h>

class MessageSubscriber
{
public:
    MessageSubscriber();
    virtual ~MessageSubscriber();

    //! Initialize the subscriber
    bool init();

    //! RUN the subscriber
    void run();

    //! Run the subscriber until number samples have been received.
    void run(
        uint32_t number);

    void set_callback(std::function<bool(const std::shared_ptr<image> &)> &&callback)
    {
        m_listener_.message_callback_ = callback;
    }

private:
    eprosima::fastrtps::Participant *mp_participant_;
    eprosima::fastrtps::Subscriber *mp_subscriber;

public:
    class SubListener : public eprosima::fastrtps::SubscriberListener
    {
    public:
        SubListener()
            : n_matched(0), n_samples(0)
        {
            m_image_ = std::make_shared<image>();
        }

        ~SubListener()
        {
        }

        void onSubscriptionMatched(
            eprosima::fastrtps::Subscriber *sub,
            eprosima::fastrtps::rtps::MatchingInfo &info);

        void onNewDataMessage(
            eprosima::fastrtps::Subscriber *sub);

        std::function<bool(const std::shared_ptr<image> &)> message_callback_ = nullptr;

        std::shared_ptr<image> m_image_;
        eprosima::fastrtps::SampleInfo_t m_info;

        int n_matched;
        uint32_t n_samples;
    } m_listener_;

private:
    imagePubSubType m_type_;
};

#endif /* HELLOWORLDSUBSCRIBER_H_ */
