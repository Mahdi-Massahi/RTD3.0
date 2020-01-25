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
#include "sim-transport.h"

namespace ns3{

  std::string trasportPackGetString(Ptr<Packet> inp_packet);
  std::string appendParityBit(std::string str);
  Ptr<Packet> trasnportCreatePackage(std::string data);
  bool checkParity(std::string str);
  std::string detachParityBit(std::string str);
  unsigned int getSize(std::string str);
  std::string detachSequenceNumber(std::string str);
  std::string appendSeqenceNumber(std::string str);
  void timeredFunctuin();

  int RTT = 300;
  char packetNumber = '0';
  Ptr<Packet> bufferedPacket;
  int bufferedId;

  NS_LOG_COMPONENT_DEFINE ("SimTransport");
  NS_OBJECT_ENSURE_REGISTERED (SimTransport);

  SimTransport::SimTransport (){

  }
  SimTransport::~SimTransport (){

  }

  void SimTransport::SetProcess (Ptr<SimProcess> inp_process){
    m_process = inp_process;
  }

  void SimTransport::SetChannel (Ptr<SimChannel> inp_channel){
    m_channel = inp_channel;
    m_channel-> Add (this);
  }

  void SimTransport::SendToChannel (Ptr<Packet> inp_packet, int id)
  {
    //std::cout << trasportPackGetString(inp_packet)[0] << '\n';
    if (trasportPackGetString(inp_packet)[0] != 'A') {
      // packet is data
      my_event.Cancel();
      std::cout << "[" << Now() << "]\t[Transport " << id << "]\t"
       << " Timer restarted." << std::endl;

      m_channel->SendPacket (this, inp_packet);
      my_event = Simulator::Schedule(MicroSeconds(RTT), &SimTransport::SendToChannel, this, inp_packet, id);
      std::cout << "[" << Now() << "]\t[Transport " << id << "]\t\'"
       << trasportPackGetString(inp_packet)<<"\' is sent to channel." << std::endl;

      std::cout << "[" << Now() << "]\t[Transport " << id << "]\t"
      << " Timer started." << std::endl;
    }
    else
    {
      // packet is ACK/NACK
      m_channel->SendPacket (this, inp_packet);
      std::cout << "[" << Now() << "]\t[Transport " << id << "]\t\'"
       << trasportPackGetString(inp_packet)<<"\' is sent to channel." << std::endl;
    }
  }

  void SimTransport::ReceiveFromChannel (Ptr<Packet> inp_packet, int id)
  {
    std::cout << "[" << Now() << "]\t[Transport " << id << "]\t\'"
     << trasportPackGetString(inp_packet)<<"\' is received from channel." << std::endl;

    if (trasportPackGetString(inp_packet)[0] == 'A') {
      //cancle timer
      my_event.Cancel();
      std::cout << "[" << Now() << "]\t[Transport " << id << "]\t"
       << " Timer canceled." << std::endl;
    }

    if (trasportPackGetString(inp_packet)[0] == 'A') {
      //Paket is ACK or NACK
      if (trasportPackGetString(inp_packet)[3] == packetNumber)
      {
        //ACKed add sequence number
        if (packetNumber == '0') {
          packetNumber = '1';
        }
        else
          packetNumber = '0';
      }
      else if(trasportPackGetString(inp_packet)[3] != packetNumber){
        // Send again

        if (trasportPackGetString(inp_packet)[0] != 'A') {
          bufferedPacket = inp_packet;
          bufferedId = id;
        }

        std::cout << "[" << Now() << "]\t[Transport "<< id << "]\t\'"
         << trasportPackGetString(bufferedPacket)<<"\' Resending buffered packet." << std::endl;

        Ptr<Packet> new_packet = trasnportCreatePackage(appendSeqenceNumber(trasportPackGetString(bufferedPacket)));
        std::cout << "[" << Now() << "]\t[Transport "<< id << "]\t\'"
         << trasportPackGetString(new_packet)<<"\' Sequence number added: "<< packetNumber << std::endl;

        // Apply parity bit
        new_packet = trasnportCreatePackage(appendParityBit(trasportPackGetString(new_packet)));
        std::cout << "[" << Now() << "]\t[Transport "<< id << "]\t\'"
          << trasportPackGetString(new_packet)<<"\' Parity bit added." << std::endl;

        SimTransport::SendToChannel (new_packet, id);

      }
    }
    else
    {
      // check Parity
      if (checkParity(trasportPackGetString(inp_packet))) {
        // parity is ok
        // detach parity bit
        Ptr<Packet> new_packet = trasnportCreatePackage(detachParityBit(trasportPackGetString(inp_packet)));
        std::cout << "[" << Now() << "]\t[Transport " << id << "]\t\'"
          << trasportPackGetString(new_packet)<<"\' Parity bit cheked and detached." << std::endl;

        // Extraxt sequance number from packet
        new_packet = trasnportCreatePackage(detachSequenceNumber(trasportPackGetString(new_packet)));
        std::cout << "[" << Now() << "]\t[Transport " << id << "]\t\'"
          << trasportPackGetString(new_packet)<<"\' Sequence number detached: " << packetNumber << std::endl;

        // Pass it to processor
        SendToProcess (new_packet, id);

        // Send back ACK
        char buff[4] = {'A','C','K','0'};
        if (packetNumber=='1') {
          buff[3] = '1';
        }
        Ptr<Packet> ACK = trasnportCreatePackage(buff);
        SendToChannel(ACK, id);

      }
      else
      {
        // parity is NOT ok
        std::cout << "[" << Now() << "]\t[Transport " << id << "]\t" << " Parity worng." << std::endl;

        // Send back NACK
        char buff[4] = {'A','C','K','0'};
        if (packetNumber=='1') {
          buff[3] = '1';
        }
        if (buff[3] == '0')
        {
          buff[3] = '1';
        }
        else
          buff[3] = '0';
        Ptr<Packet> NACK = trasnportCreatePackage(buff);
        SendToChannel(NACK, id);
      }
    }
  }

  void SimTransport::SendToProcess (Ptr<Packet> inp_packet, int id)
  {
    std::cout << "[" << Now() << "]\t[Transport " << id << "]\t\'"
     << trasportPackGetString(inp_packet)<<"\' is sent to proccess." << std::endl;
    m_process->ReceiveFromTransport (inp_packet, id);
  }

  void SimTransport::ReceiveFromProcess (Ptr<Packet> inp_packet, int id)
  {
    bufferedPacket = inp_packet;
    bufferedId = id;
    std::cout << "[" << Now() << "]\t[Transport "<< id << "]\t\'"
     << trasportPackGetString(inp_packet)<<"\' is received from processor." << std::endl;

    Ptr<Packet> new_packet = trasnportCreatePackage(appendSeqenceNumber(trasportPackGetString(inp_packet)));
    std::cout << "[" << Now() << "]\t[Transport "<< id << "]\t\'"
     << trasportPackGetString(new_packet)<<"\' Sequence number added: "<< packetNumber << std::endl;

    // Apply parity bit
    new_packet = trasnportCreatePackage(appendParityBit(trasportPackGetString(new_packet)));
    std::cout << "[" << Now() << "]\t[Transport "<< id << "]\t\'"
      << trasportPackGetString(new_packet)<<"\' Parity bit added." << std::endl;

    SendToChannel (new_packet, id);
  }

  std::string trasportPackGetString(Ptr<Packet> inp_packet){
    // getting string form <Packet>
    uint8_t * buffer = new uint8_t[inp_packet -> GetSize()];
    inp_packet -> CopyData(buffer, inp_packet -> GetSize());
    buffer[inp_packet->GetSize()] = '\0';
    return std::string((char *)buffer);
  }

  std::string appendParityBit(std::string str){
    // appending parity bit
    int ones = 0;
    for (unsigned int i = 0; i < sizeof(str); i++)
      if (str[i] == '1')
        ones++;
    if (ones%2 == 1)
      return(str + '1');
    return(str + '0');
  }

  std::string appendSeqenceNumber(std::string str){
    // appending sequance number
    if (packetNumber == '1'){
        return(str + '1');
    }
    else{
        return(str + '0');
    }
  }

  Ptr<Packet> trasnportCreatePackage(std::string data){
    // creating package
    std::stringstream temp_str;
    temp_str << data;
    Ptr<Packet> pkt = Create<Packet>((uint8_t*)temp_str.str().c_str(), sizeof(temp_str));
    return pkt;
  }

  bool checkParity(std::string str){
    // check parity bit
    int ones = 0;
    for (unsigned int i = 0; i < getSize(str); i++)
      if (str[i] == '1')
        ones++;
    if (ones%2 == 1)
      return(false);
    return(true);
  }

  std::string detachParityBit(std::string str){
    // detach parity bit
    str[getSize(str)-1] = '\0';
    return(str);
  }

  unsigned int getSize(std::string str){
    int i = 0;
    while (str[i] != '\0') {
      i++;
    }
    return i;
  }

  std::string detachSequenceNumber(std::string str){
    // Detach sequence number
    str[getSize(str)-1] = '\0';
    return str;
  }

  void timeredFunctuin(){
    std::cout << "Time has ended." << '\n';
  }
}
