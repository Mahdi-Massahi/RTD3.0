#include "ns3/queue.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <iostream>
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "sim-process.h"

#include "sim-process.h"

namespace ns3 {

  std::string processorPackGetString(Ptr<Packet> inp_packet);
  NS_LOG_COMPONENT_DEFINE ("SimProcess");
  NS_OBJECT_ENSURE_REGISTERED (SimProcess);

  SimProcess::SimProcess ()
  {

  }
  SimProcess::~SimProcess ()
  {

  }

  void
  SimProcess::SetId (int inp_id)
  {
    m_id = inp_id;
  }

  void
  SimProcess::SetTransport (Ptr<SimTransport> inp_transport)
  {
    m_transport = inp_transport;
  }

  void
  SimProcess::GeneratePacket ()
  {
    std::stringstream temp_str;
    temp_str << "1010101010";
    uint16_t packetSize = temp_str.str().length();
    Ptr<Packet> pkt = Create<Packet> ((uint8_t*)temp_str.str().c_str(), packetSize);

    std::cout << "[" << Now() << "]\t[Processor "  << m_id  <<"]\t\'"
     << temp_str.str().c_str() << "\' is sent to transport layer." << std::endl;
    SendToTransport (pkt);
  }

  void
  SimProcess::SendToTransport (Ptr<Packet> inp_packet)
  {
    m_transport->ReceiveFromProcess(inp_packet, m_id);
  }

  void
  SimProcess::ReceiveFromTransport (Ptr<Packet> inp_packet, int id)
  {
    ShowPacket (inp_packet);
  }

  void
  SimProcess::ShowPacket (Ptr<Packet> inp_packet)
  {
    std::cout << "[" << Now() << "]\t[Processor " << m_id << "]\t\'"
     << processorPackGetString(inp_packet) << "\' is received from transport layer." << std::endl;
  }

  std::string processorPackGetString(Ptr<Packet> inp_packet){
    // getting string form <Packet>
    uint8_t * buffer = new uint8_t[inp_packet -> GetSize()];
    inp_packet -> CopyData(buffer, inp_packet -> GetSize());
    buffer[inp_packet->GetSize()] = '\0';
    return std::string((char *)buffer);
  }
}
