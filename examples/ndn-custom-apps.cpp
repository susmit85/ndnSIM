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

// ndn-custom-apps.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

/**
 * This scenario simulates a one-node two-custom-app scenario:
 *
 *   +------+ <-----> (CustomApp)
 *   | Node |
 *   +------+ <-----> (Hijacker)
 *
 *     NS_LOG=CustomApp ./waf --run=ndn-custom-apps
 */
int
main(int argc, char* argv[])
{
  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  //read the topology
  AnnotatedTopologyReader topologyReader("", 1); 
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topology_ndnsim.txt");
  topologyReader.Read();

  // install NDN on nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/cmip5", "/localhost/nfd/strategy/best-route");

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  std::vector<std::string> ipAddresses{"103.37.201.21","103.37.201.31","130.206.30.86","136.172.30.61","140.172.240.95","146.83.8.135","149.171.147.29","157.82.156.76","159.226.234.18","222.195.137.73"};

  // Getting containers for the consumer/producer
  Ptr<Node> producer = Names::Find<Node>("198.128.52.131");
  Ptr<Node> consumers[10];
  int index = 0;
  for (const auto x: ipAddresses) {
	consumers[index] = Names::Find<Node>(x);
    index++;
  }

//  Ptr<Node> consumers[10] = {Names::Find<Node>("149.171.147.29"), Names::Find<Node>("136.172.30.61"),
//                            Names::Find<Node>("146.83.8.135"), Names::Find<Node>("130.206.30.86")};

  // Install NDN stack on all nodes


  // App1
  ndn::AppHelper app1("ns3::ndn::LlnlSim");
  int pos = 0;
  for (const auto y: ipAddresses) {
    app1.SetAttribute("IP" , StringValue(y));
    app1.Install(consumers[pos]);
    pos++;
  }


  // App2
  ndn::AppHelper app2("Hijacker");
  app2.Install(producer); // last node


  // Register /root prefix with global routing controller and
  // install producer that will satisfy Interests in /root namespace
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

  ndnGlobalRoutingHelper.AddOrigins("/cmip5", producer);
  producerHelper.SetPrefix("/cmip5");
  producerHelper.Install(producer).Start(Seconds(0));

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  //21793704
//start: 1326440142
//end: 1458114218 
//40208263
  Simulator::Stop(Seconds(200));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
