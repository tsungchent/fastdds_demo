#ifndef HELLO_SUBSCRIBER_HPP
#define HELLO_SUBSCRIBER_HPP

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include <hello_worldPubSubTypes.h>
#include <hello_world.h>

class HelloSubscriber
{
public:
    HelloSubscriber();
    ~HelloSubscriber();

    bool init();

    void run();

    void run(uint32_t number);

private:
    eprosima::fastrtps::Participant *mp_participant_;
    eprosima::fastrtps::Subscriber *mp_subscriber_;

public:
    class SubListener : public eprosima::fastrtps::SubscriberListener
    {
    public:
        SubListener() : n_matched_(0), n_samples_(0){};
        ~SubListener(){};
        void onSubscriptionMatched(eprosima::fastrtps::Subscriber *sub, eprosima::fastrtps::rtps::MatchingInfo &info);
        void onNewDataMessage(eprosima::fastrtps::Subscriber *sub);
        HelloWorld m_hello_;
        eprosima::fastrtps::SampleInfo_t m_info_;
        int n_matched_;
        uint32_t n_samples_;
    } m_sublistener_;

private:
    HelloWorldPubSubType m_type_;
};

#endif