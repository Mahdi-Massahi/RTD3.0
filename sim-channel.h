#ifndef SIMP_CHANNEL_H
#define SIMP_CHANNEL_H
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/object-factory.h"
#include "ns3/sim-process.h"
#include "ns3/sim-transport.h"

namespace ns3{
  class SimTransport;
      class SimChannel: public Object
      {
      public:
      SimChannel ();
      virtual ~SimChannel ();
      typedef std::vector<Ptr<SimTransport>> TransportList;
      TransportList m_transport_list;
      void Add (Ptr<SimTransport> inp_transport);
      void SendPacket (Ptr<SimTransport> inp_stransport, Ptr<Packet> inp_packet);
      void ReceivePacket (Ptr<Packet> inp_packet, int j);
      };
  }
#endif
