#include <iostream>
#include "build-config.h"
#include "jlib.hpp"
#include "componentstatus.hpp"
#include "ws_machine.hpp"

void usage()
{
    std::cout << "send_status sends component process health to the ws_machine web services\n";
    std::cout << "usage: send_status <host:port> <status> <process type> <process name> <process port> <status message> <reporter> <url>\n";
    std::cout << "e.g. send_status localhost:8010 1 DaliServer mydali 7100 \"down, no response\" \"http://localhost/nagios3\" \n";
}


enum eSendStatusParams
{
    eHostIDParam = 1,   //  <-- host:port of eclwatch ws_machine service
    eStatusParam,       //  <-- status
    eProcessType,       //  <-- process type
    eProcessName,       //  <-- process name
    eProcessPort,       //  <-- process port
    eMessage,           //  <-- status message
    eReporter,          //  <-- reporter
    eLast
};

int main(int argc, char* argv[])
{

    if (argc != eLast)
    {
        usage();
        return 0;
    }

    InitModuleObjects();

    Owned<IClientws_machine> pMachine = createws_machineClient();

    StringBuffer strURL("http://");
    strURL.appendf("%s/ws_machine",argv[eHostIDParam]);

    pMachine->addServiceUrl(strURL.str());

//    pMachine->setSoapAction("");

    Owned<IComponentStatusFactory> factory = getComponentStatusFactory();
    IArrayOf<IConstComponentStatus> statusList;
    Owned<IEspComponentStatus> cs1 = createComponentStatus();

    cs1->setComponentStatusID(atoi(argv[eStatusParam]));
    cs1->setComponentType(argv[eProcessType]);
    cs1->setReporter(argv[eReporter]);

    IArrayOf<IConstStatusReport> statusReports1;
    Owned<IEspStatusReport> statusReport1 = createStatusReport();

    statusReport1->setStatusTypeID(atoi(argv[eStatusParam]));
    statusReport1->setURL("");
    statusReport1->setStatusDetails(argv[eMessage]);
    statusReport1->setReporter(argv[eReporter]);
    statusReport1->setStatusType(argv[eStatusParam]);
    statusReport1->setStatusTypeID(atoi(argv[eStatusParam]));

    statusReports1.append(*statusReport1.getClear());
    cs1->setStatusReports(statusReports1);
    statusList.append(*cs1.getClear());
    factory->updateComponentStatus("me", statusList);

    Owned<IClientUpdateComponentStatusRequest> req = createClientUpdateComponentStatusRequest();

    req->setReporter(argv[eReporter]);
    req->setComponentStatusList(statusList);

    pMachine->UpdateComponentStatus(req);

    return 0;
}
