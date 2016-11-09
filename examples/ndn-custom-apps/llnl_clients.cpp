/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// llnl_clients.cpp
#include <fstream>
#include <iostream>

#include "llnl_clients.hpp"

//#include "model/ndn-app-face.hpp"

#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-fib-helper.hpp"

#include "ns3/random-variable-stream.h"
 #include <iostream>
 #include <string>
 #include <fstream>
 #include <vector>
 #include <boost/algorithm/string.hpp>
 
                

NS_LOG_COMPONENT_DEFINE("LlnlSim");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(LlnlSim);

// register NS-3 type
TypeId
LlnlSim::GetTypeId()
{
  static TypeId tid = TypeId("ns3::ndn::LlnlSim").
                      SetGroupName("Ndn").
					  SetParent<ndn::App>().
					  AddConstructor<LlnlSim>().
					  AddAttribute("IP", "IP of Client node", StringValue("none"),
                      MakeStringAccessor(&LlnlSim::SetIP, &LlnlSim::GetIP), MakeStringChecker());
						
  return tid;
}

LlnlSim::LlnlSim()
  : IP("")
{
}

LlnlSim::~LlnlSim()
{
}


void
LlnlSim::SetIP(const std::string& value)
{
    IP = value;
}

std::string 
LlnlSim::GetIP() const
{
 return IP; 
}

// Processing upon start of the application
void
//LlnlSim::StartApplication(std::string IpAddr)
LlnlSim::StartApplication()
{
  // initialize ndn::App
  // read the ip, read corresponding file, schedule interests
   std::cout << "IP = " << IP << std::endl;

  ndn::App::StartApplication();
  // Add entry to FIB for `/prefix/sub`
  //ndn::FibHelper::AddRoute(GetNode(), "/cmip5", m_face, 0);
//  ndn::FibHelper::AddRoute(GetNode(), "/", m_face, 0);


  // Schedule send of first interest
  auto fileName = "src/ndnSIM/examples/data/" + IP + ".txt.txt";
  std::ifstream is(fileName);
  std::string line;
  std::vector<std::string> parts;
  long count = 0;
  while(getline(is, line)) {
	if (count < 2000) {
     boost::split(parts, line, boost::is_any_of(","));
	 auto IntName = "/cmip5/" + parts[2];
     auto time = boost::lexical_cast<double> ((boost::lexical_cast<double>(parts[0])-1326440142)/1000000.0);
     NS_LOG_DEBUG( "Scheduling " << IntName << "at time = " << time <<  " at " << IP);
	 Simulator::Schedule(Seconds(time), &LlnlSim::SendInterest, this, IntName);
     count++;
    }
  }
}

// Processing when application is stopped
void
LlnlSim::StopApplication()
{
  // cleanup ndn::App
  ndn::App::StopApplication();
}

void
LlnlSim::SendInterest(std::string IntName)
{
  /////////////////////////////////////
  // Sending one Interest packet out //
  /////////////////////////////////////

  // Create and configure ndn::Interest
//  auto interest = std::make_shared<ndn::Interest>("/prefix/sub");
  auto interest = std::make_shared<ndn::Interest>(IntName);
  Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
  interest->setNonce(rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
  interest->setInterestLifetime(ndn::time::seconds(1));

  NS_LOG_DEBUG("Sending Interest packet for " << *interest << " from " << IP);

  // Call trace (for logging purposes)
  m_transmittedInterests(interest, this, m_face);

  m_appLink->onReceiveInterest(*interest);
}

// Callback that will be called when Interest arrives
void
LlnlSim::OnInterest(std::shared_ptr<const ndn::Interest> interest)
{
  ndn::App::OnInterest(interest);

  NS_LOG_DEBUG("Received Interest packet for " << interest->getName() << " at " <<  IP);

  // Note that Interests send out by the app will not be sent back to the app !

  auto data = std::make_shared<ndn::Data>(interest->getName());
//  data->setFreshnessPeriod(ndn::time::milliseconds(10000/1000000));
//  data->setContent(std::make_shared< ::ndn::Buffer>(1024));
//  ndn::StackHelper::getKeyChain().sign(*data);
//
  NS_LOG_DEBUG("aSending Data packet for " << data->getName() << " from " << IP);
//
//  // Call trace (for logging purposes)
//  m_transmittedDatas(data, this, m_face);
//
//  m_appLink->onReceiveData(*data);
}

// Callback that will be called when Data arrives
void
LlnlSim::OnData(std::shared_ptr<const ndn::Data> data)
{
  NS_LOG_DEBUG("Receiving Data packet for " << data->getName() << "at " << IP);

//  std::cout << "DATA received for name " << data->getName() << std::endl;
}

} // namespace ns3
