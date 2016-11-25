#include "ns3/application.h"
#include "llnl_clients.hpp"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/log.h"

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-fib-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-app-helper.hpp"
#include "model/ndn-ns3.hpp"

#include "ns3/random-variable-stream.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <ndn-cxx/face.hpp>
#include "ns3/core-module.h"



NS_LOG_COMPONENT_DEFINE("LlnlClientStarter");

namespace ns3 {


//NS_OBJECT_ENSURE_REGISTERED(LlnlClientStarter);    

// Class inheriting from ns3::Application
class LlnlClientStarter : public Application
{
public:
  static TypeId
  GetTypeId()
  {
    static TypeId tid = TypeId("LlnlClientStarter").
      SetParent<Application>().
      AddConstructor<LlnlClientStarter>().
      AddAttribute("IP", "IP of Client node", StringValue("none"),
      MakeStringAccessor(&LlnlClientStarter::SetIP, &LlnlClientStarter::GetIP), MakeStringChecker());

    return tid;
  }


 void
 SetIP(const std::string& value)
 {
  	IP = value;
 }
 
 std::string
 GetIP() const
 {
	return IP;
 }
 

protected:
  // inherited from Application base class.
  virtual void
  StartApplication()
  {
	
    // Create an instance of the app, and passing the dummy version of KeyChain (no real signing)
    m_instance.reset(new app::LlnlConsumerWithTimer(IP));
    m_instance->run(); // can be omitted
  }

  virtual void
  StopApplication()
  {
    // Stop and destroy the instance of the app
    m_instance.reset();
  }

private:
  std::unique_ptr<app::LlnlConsumerWithTimer> m_instance;
  std::string IP;

};

} // namespace ns3

