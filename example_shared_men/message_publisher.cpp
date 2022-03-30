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
 * @file MessagePublisher.cpp
 *
 */

#include "message_publisher.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/Domain.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>

#include <thread>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace eprosima::fastdds::rtps;

MessagePublisher::MessagePublisher()
    : mp_participant_(nullptr), mp_publisher_(nullptr)
{
}

bool MessagePublisher::init(int width, int height, int chn)
{
    // m_hello_ = std::make_shared<HelloWorld>();
    // m_hello_->index(0);
    // m_hello_->message("HelloWorld");
    m_image_ = std::make_shared<image>();
    m_image_->timestamp(0);
    m_image_->message("image");
    m_image_->width(width);
    m_image_->height(width);
    m_image_->chn(width);
    m_image_->data(std::vector<uint8_t>(width * height * chn));

    // m_image_->data().resize(width * height * chn);
    // Create a DynamicTypeBuilder for a sequence of two elements of type inte32
    // uint32_t length = 2;
    // DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_int32_type();
    // DynamicTypeBuilder_ptr builder =
    // DynamicTypeBuilderFactory::get_instance()->create_sequence_builder(base_type, length);
    // Create the data instance
    // DynamicType_ptr sequence_type = builder->build();
    // DynamicData_ptr data(DynamicDataFactory::get_instance()->create_data(sequence_type));
    // Insert and remove elements
    // MemberId newId, newId2;
    // data->insert_int32_value(10, newId);
    // data->insert_int32_value(12, newId2);
    // data->remove_sequence_data(newId);

    ParticipantAttributes PParam;
    PParam.rtps.builtin.discovery_config.discoveryProtocol = DiscoveryProtocol_t::SIMPLE;
    PParam.rtps.builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.discovery_config.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant_pub");

    // SharedMem transport configuration
    PParam.rtps.useBuiltinTransports = false;

    auto shm_transport = std::make_shared<SharedMemTransportDescriptor>();
    shm_transport->segment_size(2 * 2048 * 2048);

    PParam.rtps.userTransports.push_back(shm_transport);

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
    Domain::registerType(mp_participant_, &m_image_type_);

    // CREATE THE PUBLISHER
    // hello publisher
    PublisherAttributes Wparam;
    Wparam.topic.topicKind = NO_KEY;
    Wparam.topic.topicDataType = m_image_type_.getName();
    Wparam.topic.topicName = "imageSharedMemTopic";

    Wparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Wparam.topic.historyQos.depth = 30;
    Wparam.qos.m_publishMode.kind = ASYNCHRONOUS_PUBLISH_MODE;
    // this is imported setting for dynamic msg, for example sequence and map
    Wparam.historyMemoryPolicy = rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

    Wparam.topic.resourceLimitsQos.max_samples = 50;
    Wparam.topic.resourceLimitsQos.allocated_samples = 20;
    Wparam.times.heartbeatPeriod.seconds = 2;
    Wparam.times.heartbeatPeriod.nanosec = 200 * 1000 * 1000;
    Wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Wparam.qos.m_publishMode.kind = ASYNCHRONOUS_PUBLISH_MODE;

    mp_publisher_ = Domain::createPublisher(mp_participant_, Wparam, (PublisherListener *)&m_listener_);
    if (mp_publisher_ == nullptr)
    {
        return false;
    }
    return true;
}

MessagePublisher::~MessagePublisher()
{
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant_);
}

void MessagePublisher::PubListener::onPublicationMatched(
    Publisher * /*pub*/,
    MatchingInfo &info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        firstConnected = true;
        std::cout << "MessagePublisher matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "MessagePublisher unmatched" << std::endl;
    }
}

void MessagePublisher::runThread(
    uint32_t samples,
    uint32_t sleep)
{
    if (samples == 0)
    {
        while (!stop_)
        {
            if (publish(false))
            {
                std::cout << "Message: " << m_image_->message() << " with timestamp: " << m_image_->timestamp() << " SENT" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        }
    }
    else
    {
        for (uint32_t i = 0; i < samples; ++i)
        {
            if (!publish())
            {
                --i;
            }
            else
            {
                std::cout << "Message: " << m_image_->message() << " with timestamp: " << m_image_->timestamp() << " SENT" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        }
    }
}

void MessagePublisher::run_test(
    uint32_t samples,
    uint32_t sleep)
{
    stop_ = false;
    std::thread thread(&MessagePublisher::runThread, this, samples, sleep);
    if (samples == 0)
    {
        std::cout << "MessagePublisher running. Please press enter to stop_ the MessagePublisher at any time." << std::endl;
        std::cin.ignore();
        stop_ = true;
    }
    else
    {
        std::cout << "MessagePublisher running " << samples << " samples." << std::endl;
    }
    thread.join();
}

bool MessagePublisher::publish(const std::shared_ptr<image> &_image)
{
    eprosima::fastrtps::rtps::WriteParams params;
    bool ret = mp_publisher_->write((void *)_image.get(), params);
    return ret;
}

bool MessagePublisher::publish(
    bool waitForListener)
{
    if (m_listener_.firstConnected || !waitForListener || m_listener_.n_matched > 0)
    {
        m_image_->timestamp(m_image_->timestamp() + 1);
        // size_t data_size = m_image_->data().size();
        std::string s = "BigData" + std::to_string(m_image_->timestamp() % 10);
        // strcpy((char *)&m_image_->data()[data_size - s.length() - 1], s.c_str());
        eprosima::fastrtps::rtps::WriteParams params;
        bool ret = mp_publisher_->write((void *)m_image_.get(), params);

        if (!ret)
            std::cout << "failed to publish!!!" << std::endl;
        return ret;
    }
    return false;
}
