#ifndef SIMP_PROCESS_H
#define SIMP_PROCESS_H
#include <map>
#include "ns3/nstime.h"
#include "ns3/callback.h"
#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/vector.h"
#include <set>
#include <fstream>
#include "ns3/random-variable-stream.h"
#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/object-factory.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/sim-transport.h"

namespace ns3 {
  class SimTransport;
  class SimProcess: public Object
  {
    public:
    SimProcess ();
    virtual ~SimProcess ();
    int m_id;
    Ptr<SimTransport> m_transport;
    void SetId (int inp_id);
    void SetTransport (Ptr<SimTransport> inp_transport);
    void GeneratePacket ();
    void ShowPacket (Ptr<Packet> inp_packet);
    void SendToTransport (Ptr<Packet> inp_packet);
    void ReceiveFromTransport (Ptr<Packet> inp_packet, int id);
  };
}
#endif
