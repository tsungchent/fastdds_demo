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
 * @file HelloWorldSubscriber.cpp
 *
 */

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/Domain.h>

#include "hello_subscriber.h"

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

HelloSubscriber::HelloSubscriber()
    : mp_participant_(nullptr)
    , mp_subscriber_(nullptr)
{
}

bool HelloSubscriber::init()
{
    ParticipantAttributes PParam;
    PParam.rtps.builtin.discovery_config.discoveryProtocol = DiscoveryProtocol_t::SIMPLE;
    PParam.rtps.builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.discovery_config.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant_sub");
    mp_participant_ = Domain::createParticipant(PParam);
    if (mp_participant_ == nullptr)
    {
        return false;
    }

    //REGISTER THE TYPE

    Domain::registerType(mp_participant_, &m_type_);
    //CREATE THE SUBSCRIBER
    SubscriberAttributes Rparam;
    Rparam.topic.topicKind = NO_KEY;
    Rparam.topic.topicDataType = "HelloWorld";
    
    Rparam.topic.topicName = "HelloWorldTopic";

    Rparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Rparam.topic.historyQos.depth = 30;
    Rparam.topic.resourceLimitsQos.max_samples = 50;
    Rparam.topic.resourceLimitsQos.allocated_samples = 20;
    Rparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Rparam.qos.m_durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;
    mp_subscriber_ = Domain::createSubscriber(mp_participant_, Rparam, (SubscriberListener*)&m_sublistener_);

    if (mp_subscriber_ == nullptr)
    {
        return false;
    }


    return true;
}

HelloSubscriber::~HelloSubscriber()
{
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant_);
}

void HelloSubscriber::SubListener::onSubscriptionMatched(
        Subscriber* /*sub*/,
        MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched_ ++;
        std::cout << "Subscriber matched" << std::endl;
    }
    else
    {
        n_matched_ --;
        std::cout << "Subscriber unmatched" << std::endl;
    }
}

void HelloSubscriber::SubListener::onNewDataMessage(
        Subscriber* sub)
{
    if (sub->takeNextData((void*)&m_hello_, &m_info_))
    {
        if (m_info_.sampleKind == ALIVE)
        {
            this->n_samples_++;
            // Print your structure data here.
            std::cout << "Message " << m_hello_.message() << " " << m_hello_.index() << " RECEIVED" << std::endl;
        }
    }

}

void HelloSubscriber::run()
{
    std::cout << "Subscriber running. Please press enter to stop the Subscriber" << std::endl;
    std::cin.ignore();
}

void HelloSubscriber::run(
        uint32_t number)
{
    std::cout << "Subscriber running until " << number << "samples have been received" << std::endl;
    while (number > this->m_sublistener_.n_samples_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
