#ifndef SIMP_TRANSPORT_H
#define SIMP_TRANSPORT_H
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/sim-process.h"
#include "ns3/sim-channel.h"
#include "ns3/object.h"
#include "ns3/object-factory.h"
#include "ns3/event-id.h"

namespace ns3{
  class SimChannel;
  class SimProcess;
  class SimTransport: public Object
  {
    public:
      SimTransport ();
      virtual ~SimTransport ();
      EventId my_event;
      Ptr<SimProcess> m_process;
      Ptr<SimChannel> m_channel;
      void SetProcess (Ptr<SimProcess> inp_process);
      void SetChannel (Ptr<SimChannel> inp_channel);
      void SendToChannel (Ptr<Packet> inp_packet, int id);

      void ReceiveFromChannel(Ptr<Packet> inp_packet, int id);
      void SendToProcess (Ptr<Packet> inp_packet, int id);
      void ReceiveFromProcess (Ptr<Packet> inp_packet, int id);

    };
}
#endif
