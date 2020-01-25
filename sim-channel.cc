#include <iostream>
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "sim-channel.h"
#include <sys/time.h>

namespace ns3{
  std::string applyNoise(std::string str);
  Ptr<Packet> channelCreatePackage(std::string data);
  std::string channelPacketGetString(Ptr<Packet> inp_packet);
  long getMicrotime();
  unsigned int _getSize(std::string str);

  int probDrop = 100;     // P(Drop) = 1/probDrop
  int probFlip = 2;     // P(Flip) = 1/probFlip
  int propDeviatin = 200; // PropagationDelay = 30us + [0,200]

  NS_LOG_COMPONENT_DEFINE ("SimChannel");
  NS_OBJECT_ENSURE_REGISTERED (SimChannel);

  SimChannel::SimChannel ()
  {

  }
  SimChannel::~SimChannel ()
  {

  }

  void
  SimChannel::Add (Ptr<SimTransport> inp_transport)
  {
    m_transport_list.push_back(inp_transport);
  }

  void
  SimChannel::SendPacket (Ptr<SimTransport> inp_stransport, Ptr<Packet> inp_packet)
  {
    Time propagation_delay;
    int j = 0;
    for (TransportList::const_iterator i = m_transport_list.begin (); i != m_transport_list.end (); i++)
    {
      long randomVar = getMicrotime();
      propagation_delay = MicroSeconds (30 + randomVar%200);
      if (inp_stransport != (*i) && randomVar%(probDrop)!=0)
      {
        Simulator::Schedule (propagation_delay , &SimChannel::ReceivePacket, this, inp_packet, j);
      }
      j++;
    }
  }

  void
  SimChannel::ReceivePacket (Ptr<Packet> inp_packet, int j)
  {
    // applying noise by flipping one bit of inp_packet randomly
    Ptr<Packet> bpkt = channelCreatePackage(applyNoise(channelPacketGetString(inp_packet)));
    std::cout << "[" << Now() << "]\t[Channel]\t Tansmition is done."<< std::endl;
    m_transport_list[j] ->ReceiveFromChannel (bpkt, j+1);
  }

  std::string applyNoise(std::string str){
    std::string my_str = str;
    Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();

    if (str[0] != 'A') {
      int digit = getMicrotime() % _getSize(str);

      if(getMicrotime()%(probFlip) == 0)
      {
        std::cout << "-Noise is applied." << '\n';
        if (my_str[digit] == '0')
        {
          my_str [digit] = '1';
        }
        else if (my_str[digit] == '1')
        {
          my_str [digit] = '0';
        }
      }
      else
        std::cout << "-No noise is applied." << '\n';

      return my_str;
    }
    else
      return str;
  }

  std::string channelPacketGetString(Ptr<Packet> inp_packet){
    // getting string form <Packet>
    uint8_t * buffer = new uint8_t[inp_packet -> GetSize()];
    inp_packet -> CopyData(buffer, inp_packet -> GetSize());
    buffer[inp_packet->GetSize()] = '\0';
    return std::string((char *)buffer);
  }

  Ptr<Packet> channelCreatePackage(std::string data){
    // creating package
    std::stringstream temp_str;
    temp_str << data;
    Ptr<Packet> pkt = Create<Packet>((uint8_t*)temp_str.str().c_str(), sizeof(temp_str));
    return pkt;
  }

  long getMicrotime()
  {
  	struct timeval currentTime;
  	gettimeofday(&currentTime, NULL);
  	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
  }

  unsigned int _getSize(std::string str){
    int i = 0;
    while (str[i] != '\0') {
      i++;
    }
    return i;
  }
}
