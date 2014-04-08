#include <cstdio>
#include "jstream.ipp"
#include "jfile.hpp"
#include "jbuff.hpp"
#include "HPCCNagiosToolSet.hpp"
#include <cstring>

static const char *PCONFIGGEN_PARAM_LIST(" -list ");
static const char *PCONFIGGEN_PARAM_LIST_ALL(" -listall ");
static const char *PCONFIGGEN_PARAM_ENVIRONMENT(" -env ");
static const char *PCONFIGGEN_PARAM_MACHINES(" -machines ");

/*
define host{
        use                     generic-host  ; Name of host template to use
        host_name               server02
        alias                   Server 02
        address                 172.18.100.101
}
*/
static const char *P_NAGIOS_HOST_CONFIG_1("define host\n\
    {\n\
        use         generic-host\n\
        host_name   ");
static const char *P_NAGIOS_HOST_CONFIG_2("\
        alias       ");
static const char *P_NAGIOS_HOST_CONFIG_3("\
        address     ");
static const char *P_NAGIOS_HOST_CONFIG_4("\n\
    }\n");


/*
define service {
          use                             generic-service
          host_name                       server02
          service_description             DNS
          check_command                   check_dns!172.18.100.101
}
*/
static const char *P_NAGIOS_SERVICE_CONFIG_1("define service\n\
    {\n\
        use                 generic-service\n\
        host_name   ");
static const char *P_NAGIOS_SERVICE_CONFIG_2("\
        service_description ");
static const char *P_NAGIOS_SERVICE_CONFIG_3("\
        check_command       ");
static const char *P_NAGIOS_SERVICE_CONFIG_4("!");  // NOT SURE IF THIS IS USEFUL
static const char *P_NAGIOS_SERVICE_CONFIG_5("\n\
    }\n");

/*
define service {
        hostgroup_name        mysql-servers
        service_description   MySQL
        check_command         check_mysql_cmdlinecred!nagios!secret!$HOSTADDRESS
        use                   generic-service
        notification_interval 0 ; set > 0 if you want to be renotified
}
*/
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_1("define service\n\
    {\n\
        hostgroup_name      ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_2("\
        service_description ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_3("\
        check_command       ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_4("\
        use                 generic-service\n\
        notification_interval 0\n\
    }\n");


/*
define hostgroup {
        hostgroup_name  mysql-servers
                alias           MySQL servers
                members         localhost, server02
        }
*/



bool CHPCCNagiosToolSet::generateHostGroupFile(const char* pOutputFilePath, const char* pConfigGenPath)
{

}

bool CHPCCNagiosToolSet::generateServiceDefinitionFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pOutputFilePath == NULL || *pOutputFilePath == 0 || pConfigGenPath == NULL || *pConfigGenPath == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(PCONFIGGEN_PARAM_LIST_ALL).append(PCONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

    FILE *fp = popen(strConfiggenCmdLine.str(), "r");

    if (fp == NULL)
    {
        return false;
    }

    int nCharacter = -1;
    CFileInputStream cfgInputStream(fileno(fp));

    memBuff.clear();

    do
    {
        nCharacter = cfgInputStream.readNext();

        memBuff.append(static_cast<unsigned char>(nCharacter));
    }
    while(nCharacter != -1);

    memBuff.append('\0');

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    char *pOutput = strdup(strOutput.str());

    char *pch = NULL;

    pch = strtok(pOutput, ",\n");

    int nCount = 0;

    StringArray arrProcessTypes;
    MapStringTo<StringArray> mapProcessToNodes;

    char pProcess[1024] = "";

    while (pch != NULL)
    {
        if (nCount % 6 ==  0) // Process name
        {
            strncpy(pProcess, pch, sizeof(pProcess));

        }
        else if (nCount % 6 == 2) // IP address
        {
            StringArray *pArrIPList = mapProcessToNodes.getValue(pProcess);

            if (pArrIPList == NULL)
            {
                pArrIPList = new StringArray();
                arrProcessTypes.append(pProcess);
                mapProcessToNodes.setValue(pProcess, *pArrIPList);
            }

            pArrIPList->append(pch);
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    StringBuffer strServiceConfig;

    for (int n = 0; n < arrProcessTypes.length(); n++)
    {
        StringArray *pMapedList = mapProcessToNodes.getValue(arrProcessTypes.item(n));

        strServiceConfig.append()

        delete pMapedList;
    }

    io->write(0, strServiceConfig.length(), strServiceConfig.str());
    io->close();

    delete pOutput;
}

bool CHPCCNagiosToolSet::generateServiceDefinitionFile(StringBuffer &strOutput, const char* pEnvXML, const char* pConfigGenPath)
{
    return true;
}

bool CHPCCNagiosToolSet::generateHostGroupFile(StringBuffer &strOutput, const char* pConfigGenPath)
{
    return true;
}

void CHPCCNagiosToolSet::createHostGroupString(StringArray &pIP, StringBuffer &strHostGroup)
{

}
