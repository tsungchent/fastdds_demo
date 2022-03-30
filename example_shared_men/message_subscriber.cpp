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
 * @file MessageSubscriber.cpp
 *
 */

#include "message_subscriber.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/Domain.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace eprosima::fastdds::rtps;

MessageSubscriber::MessageSubscriber()
    : mp_participant_(nullptr), mp_subscriber(nullptr)
{
}

bool MessageSubscriber::init()
{
    ParticipantAttributes PParam;
    PParam.rtps.builtin.discovery_config.discoveryProtocol = DiscoveryProtocol_t::SIMPLE;
    PParam.rtps.builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.discovery_config.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant_sub");

    // SharedMem transport configuration
    PParam.rtps.useBuiltinTransports = false;

    auto sm_transport = std::make_shared<SharedMemTransportDescriptor>();
    sm_transport->segment_size(2 * 2048 * 2048);
    PParam.rtps.userTransports.push_back(sm_transport);

    // UDP
    auto udp_transport = std::make_shared<UDPv4TransportDescriptor>();
    // udp_transport->interfaceWhiteList.push_back("127.0.0.1");
    PParam.rtps.userTransports.push_back(udp_transport);

    mp_participant_ = Domain::createParticipant(PParam);
    if (mp_participant_ == nullptr)
    {
        return false;
    }

    // REGISTER THE TYPE
    Domain::registerType(mp_participant_, &m_type_);

    // CREATE THE SUBSCRIBER
    SubscriberAttributes Rparam;
    Rparam.topic.topicKind = NO_KEY;
    Rparam.topic.topicDataType = m_type_.getName();
    Rparam.topic.topicName = "imageSharedMemTopic";
    Rparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Rparam.topic.historyQos.depth = 30;
    Rparam.topic.resourceLimitsQos.max_samples = 50;
    Rparam.topic.resourceLimitsQos.allocated_samples = 20;
    Rparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Rparam.qos.m_durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;

    // this is imported setting for dynamic msg, for example sequence and map
    Rparam.historyMemoryPolicy = rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

    mp_subscriber = Domain::createSubscriber(mp_participant_, Rparam, (SubscriberListener *)&m_listener_);

    if (mp_subscriber == nullptr)
    {
        return false;
    }

    return true;
}

MessageSubscriber::~MessageSubscriber()
{
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant_);
}

void MessageSubscriber::SubListener::onSubscriptionMatched(
    Subscriber * /*sub*/,
    MatchingInfo &info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "MessageSubscriber matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "MessageSubscriber unmatched" << std::endl;
    }
}

void MessageSubscriber::SubListener::onNewDataMessage(
    Subscriber *sub)
{
    // std::cout << "onNewDataMessage." << std::endl;
    if (sub->takeNextData((void *)m_image_.get(), &m_info))
    {
        if (m_info.sampleKind == ALIVE)
        {
            this->n_samples++;
            if (message_callback_)
            {
                message_callback_(m_image_);
            }
            else
            {
                std::cout << "Message " << m_image_->message() << " " << m_image_->timestamp() << std::endl;
                std::cout << "Message is not processed cause message callback function is not set." << std::endl;
            }
        } else {
            std::cout << "sample Kind is not alive." << std::endl;
        }
    }
    else
    {

        std::cout << "failed to take message." << std::endl;
    }
}

void MessageSubscriber::run()
{
    std::cout << "MessageSubscriber running. Please press enter to stop the MessageSubscriber" << std::endl;
    std::cin.ignore();
}

void MessageSubscriber::run(
    uint32_t number)
{
    std::cout << "MessageSubscriber running until " << number << "samples have been received" << std::endl;
    while (number > this->m_listener_.n_samples)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
