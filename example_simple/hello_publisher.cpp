#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/Domain.h>

#include "hello_publisher.h"
#include <thread>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

HelloPublisher::HelloPublisher()
    : mp_participant_(nullptr),
      mp_publisher_(nullptr) {}

HelloPublisher::~HelloPublisher()
{
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant_);
}

// how to set UDP/IP or IPC or TCP/IP or UART?
bool HelloPublisher::init()
{
    m_hello_.index(0);
    m_hello_.message("helloworld");

    //// Create a descriptor for the new transport.
    // std::shared_ptr<SharedMemTransportDescriptor> shm_transport = std::make_shared<SharedMemTransportDescriptor>();
    // Link the Transport Layer to the Participant.
    // qos.transport().user_transports.push_back(shm_transport);

    ParticipantAttributes PParam;
    PParam.rtps.builtin.discovery_config.discoveryProtocol = DiscoveryProtocol_t::SIMPLE;
    PParam.rtps.builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.discovery_config.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant_pub");

    mp_participant_ = Domain::createParticipant(PParam);
    if (mp_participant_ == nullptr)
    {
        return false;
    }
    
    Domain::registerType(mp_participant_, &m_type_);

    //CREATE THE PUBLISHER
    PublisherAttributes Wparam;
    Wparam.topic.topicKind = NO_KEY;
    Wparam.topic.topicDataType = "HelloWorld";
    Wparam.topic.topicName = "HelloWorldTopic";
    Wparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Wparam.topic.historyQos.depth = 30;
    Wparam.topic.resourceLimitsQos.max_samples = 50;
    Wparam.topic.resourceLimitsQos.allocated_samples = 20;
    Wparam.times.heartbeatPeriod.seconds = 2;
    Wparam.times.heartbeatPeriod.nanosec = 200 * 1000 * 1000;
    Wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    
    mp_publisher_ = Domain::createPublisher(mp_participant_, Wparam, (PublisherListener*)&m_publistener_);
    if (mp_publisher_ == nullptr)
    {
        return false;
    }
    return true;
}


void HelloPublisher::PubListener::onPublicationMatched(
        Publisher* /*pub*/,
        MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched_ ++;
        firstConnected_ = true;
        std::cout << "Publisher matched" << std::endl;
    }
    else
    {
        n_matched_ --;
        std::cout << "Publisher unmatched" << std::endl;
    }
}

void HelloPublisher::runThread(
        uint32_t samples,
        uint32_t sleep)
{
    if (samples == 0)
    {
        while (!stop_)
        {
            if (publish(false))
            {
                std::cout << "Message: " << m_hello_.message() << " with index: " << m_hello_.index() << " SENT" <<
                        std::endl;
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
                std::cout << "Message: " << m_hello_.message() << " with index: " << m_hello_.index() << " SENT" <<
                        std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        }
    }
}

void HelloPublisher::run(
        uint32_t samples,
        uint32_t sleep)
{
    stop_ = false;
    std::thread thread(&HelloPublisher::runThread, this, samples, sleep);
    if (samples == 0)
    {
        std::cout << "Publisher running. Please press enter to stop the Publisher at any time." << std::endl;
        std::cin.ignore();
        stop_ = true;
    }
    else
    {
        std::cout << "Publisher running " << samples << " samples." << std::endl;
    }
    thread.join();
}

bool HelloPublisher::publish(
        bool waitForListener)
{
    if (m_publistener_.firstConnected_ || !waitForListener || m_publistener_.n_matched_ > 0)
    {
        m_hello_.index(m_hello_.index() + 1);
        mp_publisher_->write((void*)&m_hello_);
        return true;
    }
    return false;
}


