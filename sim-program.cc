#include "ns3/core-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/simulator.h"
#include "ns3/command-line.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include "ns3/sim-module.h"


using namespace ns3;


int main ()
{
  ns3::RngSeedManager::SetSeed(4);
  ns3::RngSeedManager::SetRun(2);

  ns3::PacketMetadata::Enable();

  Time::SetResolution (Time::US);

  Ptr<SimProcess> node1_process, node2_process;
  Ptr<SimTransport> node1_transport, node2_transport;
  Ptr<SimChannel> my_channel;

  node1_process = CreateObject <SimProcess> ();
  node2_process = CreateObject <SimProcess> ();
  node1_transport = CreateObject <SimTransport> ();
  node2_transport = CreateObject <SimTransport> ();
  my_channel = CreateObject <SimChannel> ();

  node1_process->SetId (1);
  node2_process->SetId (2);

  node1_process->SetTransport (node1_transport);
  node2_process->SetTransport (node2_transport);

  node1_transport->SetProcess (node1_process);
  node2_transport->SetProcess (node2_process);

  node1_transport->SetChannel (my_channel);
  node2_transport->SetChannel (my_channel);

  node1_process->GeneratePacket ();


  Simulator::Stop (Seconds (10));

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;

}
