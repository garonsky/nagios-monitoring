#include <iostream>
#include "build-config.h"
#include "jlib.hpp"
#include "componentstatus.hpp"
#include "ws_machine.hpp"

void usage()
{
    std::cout << "send_status sends component process health to the ws_machine web services\n";
}

int main(int argc, char* argv[])
{
    InitModuleObjects();

	Owned<IClientws_machine> pMachine = createws_machineClient();
	pMachine->addServiceUrl("http://localhost:8010/ws_machine");
	pMachine->setSoapAction("");

	Owned<IComponentStatusFactory> factory = getComponentStatusFactory();
 IArrayOf<IConstComponentStatus> statusList;
        Owned<IEspComponentStatus> cs1 = createComponentStatus();
        cs1->setComponentTypeID(1);

        IArrayOf<IConstStatusReport> statusReports1;
        Owned<IEspStatusReport> statusReport1 = createStatusReport();
        statusReport1->setStatusTypeID(0);
        statusReport1->setURL("www.yaho1o.com");
        statusReports1.append(*statusReport1.getClear());
        cs1->setStatusReports(statusReports1);
        statusList.append(*cs1.getClear());
	factory->updateComponentStatus("me", statusList);

	Owned<IClientUpdateComponentStatusRequest> req = createClientUpdateComponentStatusRequest();

	req->setReporter("gleb3");
	req->setComponentStatusList(statusList);


	pMachine->UpdateComponentStatus(req);

    return 0;
}
