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

//#include "llnl_clients.hpp"

//#include "model/ndn-app-face.hpp"

//#include "ns3/callback.h"
//#include "ns3/string.h"
//#include "ns3/boolean.h"
//#include "ns3/uinteger.h"
//#include "ns3/integer.h"
//#include "ns3/double.h"
//#include "ns3/ptr.h"
#include "ns3/log.h"
//#include "ns3/simulator.h"
//#include "ns3/packet.h"
//
//#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
//#include "ns3/ndnSIM/helper/ndn-fib-helper.hpp"
//
//#include "ns3/random-variable-stream.h"
// #include <iostream 20 #include "ndn-producer.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-ns3.hpp"
#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/scheduler.hpp>

namespace app {
//namespace ndn {

NS_LOG_COMPONENT_DEFINE("ndn.LlnlConsumer");


class LlnlConsumerWithTimer
{
public:
  LlnlConsumerWithTimer(std::string PassedIP)
    : m_face(m_ioService) // Create face with io_service object
    , m_scheduler(m_ioService)

  {
      IP = PassedIP;
  }

  void
  run()
  {
  // read the ip, read corresponding file, schedule interests
   std::cout << "IP = " << IP << std::endl;

  // Schedule send of first interest
  auto fileName = "src/ndnSIM/examples/llnlsim/140.172.240.95.txt";
  //auto fileName = "/raid/llnl_simulation/ndnSIM_NEW/ns-3/data/" + IP + ".txt";
  std::ifstream is(fileName);
  std::string line;
  std::vector<std::string> parts;
  long count = 0;
  while(getline(is, line)) {
	if (count < 200) {
     boost::split(parts, line, boost::is_any_of(","));
	 auto IntName = "/cmip5/app/" + parts[2];
//     auto time = (boost::lexical_cast<double>(parts[0])-1326440142)/1000000.0;
     auto time = ((boost::lexical_cast<long>(parts[0]))-1326440142)*1000;
     std::cout <<  "Scheduling " << IntName << " at time = " << time <<  " nanoseconds at node " << IP << std::endl;
    ndn::Interest interest(IntName);
    interest.setInterestLifetime(ndn::time::seconds(40));
    interest.setMustBeFresh(true);
    m_scheduler.scheduleEvent(ndn::time::nanoseconds(time),
                              bind(&LlnlConsumerWithTimer::delayedInterest, this, interest));
     count++;
    }
  }

    // m_ioService.run() will block until all events finished or m_ioService.stop() is called
    //m_ioService.run();

    // Alternatively, m_face.processEvents() can also be called.
    // processEvents will block until the requested data received or timeout occurs.
     m_face.processEvents();
  
  }

private:
  void
  onData(const ndn::Interest& interest, const ndn::Data& data)
  {
    NS_LOG_INFO("Received data at " << IP <<  " Name " << data.getName());
  }

  void
  onNack(const ndn::Interest& interest, const ndn::lp::Nack& nack)
  {
    NS_LOG_INFO("Received Nack with reason " << nack.getReason()
              << " for interest " << interest.getName() << " at " << IP);
  }

  void
  onTimeout(const ndn::Interest& interest)
  {
    NS_LOG_INFO( "Timeout " << interest << " at " << IP );
    auto newInterest = ndn::Interest(interest.getName());
    newInterest.setInterestLifetime(ndn::time::seconds(4));
    newInterest.setMustBeFresh(true);
    newInterest.refreshNonce();
    NS_LOG_INFO("New Interest " << newInterest << " with refreshed nonce " << newInterest.getNonce() << " at " << IP);
    m_scheduler.scheduleEvent(ndn::time::seconds(1),
                              bind(&LlnlConsumerWithTimer::delayedInterest, this, newInterest));
  }

  void
  //delayedInterest(const std::string IntName)
  delayedInterest(const ndn::Interest& interest)
  {

    m_face.expressInterest(interest,
                           bind(&LlnlConsumerWithTimer::onData, this, _1, _2),
                           bind(&LlnlConsumerWithTimer::onNack, this, _1, _2),
                           bind(&LlnlConsumerWithTimer::onTimeout, this, _1));

    NS_LOG_INFO("Sending " << interest << " from " << IP);
  }

private:
  // Explicitly create io_service object, which can be shared between Face and Scheduler
  boost::asio::io_service m_ioService;
  ndn::Face m_face;
  ndn::Scheduler m_scheduler;
  //std::string IP = "140.172.240.95";
  std::string IP;
};

}//ndn

