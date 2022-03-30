#ifndef HELLO_PUBLISHER_H
#define HELLO_PUBLISHER_H

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/PublisherListener.h>

#include <hello_world.h>
#include <hello_worldPubSubTypes.h>

class HelloPublisher {
public:
    HelloPublisher();
    ~HelloPublisher();

    bool init();
    bool publish(bool waitForListener = true);

    void run(uint32_t number, uint32_t sleep);
private:
    HelloWorld m_hello_;
    eprosima::fastrtps::Participant *mp_participant_;
    eprosima::fastrtps::Publisher *mp_publisher_;
    bool stop_;

    class PubListener: public eprosima::fastrtps::PublisherListener {
    public:
		PubListener(): n_matched_(0), firstConnected_(false){};
		~PubListener(){};
		void onPublicationMatched(eprosima::fastrtps::Publisher* pub, eprosima::fastrtps::rtps::MatchingInfo& info);
		int n_matched_;
        bool firstConnected_;
    } m_publistener_;

    void runThread(uint32_t number, uint32_t sleep);
    HelloWorldPubSubType m_type_;
};


#endif