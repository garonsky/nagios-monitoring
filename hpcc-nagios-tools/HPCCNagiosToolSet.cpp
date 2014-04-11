#include <cstdio>
#include "jstream.ipp"
#include "jfile.hpp"
#include "jbuff.hpp"
#include "HPCCNagiosToolSet.hpp"
#include "HPCCNagiosToolSetCommon.hpp"
#include <cstring>

bool CHPCCNagiosToolSet::generateHostGroupsFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
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

    int nCount = 0;
    char pProcess[1024] = "";
    bool bAdd = false;
    char *pch = NULL;
    char pHostName[1024] = "";
    StringBuffer strHostConfig;
    int i = 0;

    pch = strtok(pOutput, ",\n");

    while (pch != NULL)
    {
        if (nCount % 6 ==  0) // Process name
        {
            if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strHostConfig.append(P_NAGIOS_HOSTS_GROUP_CONFIG_4);
                strHostConfig.append("\n");
                strncpy(pProcess, pch, sizeof(pProcess));
                bAdd = true;
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                bAdd = true;
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
                bAdd = false;
            }
        }
        else if (nCount % 6 == 2) // IP address
        {
            static bool bDoLookUp = true;
            hostent *hp;

            memset(pHostName,0,sizeof(pHostName));

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp = false;
                if (strcmp(pch, pHostName) == 0)
                {
                    pch = strtok(NULL, ",\n");
                    nCount++;

                    continue;
                }
                strcpy(pHostName, pch);
            }
            else
            {
                if (strcmp(pch, pHostName) == 0)
                {
                    pch = strtok(NULL, ",\n");
                    nCount++;

                    continue;
                }
                strcpy(pHostName,hp->h_name);
            }

            static char pLastHostName[1024] = "";

            if (bAdd == true)
            {
                strHostConfig.append(P_NAGIOS_HOSTS_GROUP_CONFIG_1).append(pProcess).append("-servers").append(P_NAGIOS_HOSTS_GROUP_CONFIG_2).append(pProcess).append(" servers")\
                                                                        .append(P_NAGIOS_HOSTS_GROUP_CONFIG_3).append(pHostName);

                strcpy(pLastHostName,pHostName);
                bAdd = false;
            }
            else
            {
                if (strcmp(pLastHostName,pHostName) != 0)
                {
                    strHostConfig.append(", ").append(pHostName);
                    strcpy(pLastHostName,pHostName);
                }
            }
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    io->write(0, strHostConfig.length(), strHostConfig.str());
    io->close();

    delete pOutput;

    return true;
}

bool CHPCCNagiosToolSet::generateHostConfigurationsFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
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

    int nCount = 0;
    char pProcess[1024] = "";

    MapIPtoNode mapIPtoHostName;

    StringBuffer strServiceConfig;

    CHPCCNagiosToolSet::generateNagiosHostConfig(strServiceConfig, mapIPtoHostName, pEnvXML);

    int i = -1;
    char *pch = NULL;
    bool bAdd = false;
    pch = strtok(pOutput, ",\n");

    while (pch != NULL)
    {
        if (nCount % 6 ==  0) // Process name
        {
            if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                bAdd = true;
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                bAdd = true;
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
                bAdd = false;
            }
        }
        else if (nCount % 6 == 2) // IP address
        {
            if (bAdd == true)
            {
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(mapIPtoHostName.getValue(pch)->strHostName).append(P_NAGIOS_SERVICE_CONFIG_2).append("")\
                        .append(P_NAGIOS_SERVICE_CONFIG_3).append("COMMAND_TO_DO").append(P_NAGIOS_SERVICE_CONFIG_4).append(pch).append(P_NAGIOS_SERVICE_CONFIG_5);

                strServiceConfig.append("\n");
                bAdd = false;
            }
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    io->write(0, strServiceConfig.length(), strServiceConfig.str());
    io->close();

    delete pOutput;
}

bool CHPCCNagiosToolSet::generateNagiosHostConfig(StringBuffer &strHostConfig, MapIPtoNode &mapIPtoHostName, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(PCONFIGGEN_PARAM_MACHINES).append(PCONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

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

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    strOutput.replace('\377',',');

    char *pOutput = strdup(strOutput.str());

    int nCount = 0;
    char pIP[1024] = "";

    char *pch = NULL;
    pch = strtok(pOutput, ",\n");

    int i = 0;
    while (pch != NULL)
    {
        if (nCount % 2 ==  0) // Process name
        {
            char pHostName[1024] = "";
            struct hostent* hp = NULL;

            static bool bDoLookUp = true;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp = false;
                strcpy(pHostName, "Server");
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }

            strHostConfig.append(P_NAGIOS_HOST_CONFIG_1).append(pHostName).append(i).append(P_NAGIOS_HOST_CONFIG_2).append(pHostName).append(" ")\
                                                                    .append(i).append(P_NAGIOS_HOST_CONFIG_3).append(pch).append(P_NAGIOS_HOST_CONFIG_4);
            strHostConfig.append("\n");

            struct NodeName nm;
            nm.strHostName.setf("%s%d",pHostName, i);
            nm.strHostAlias.setf("%s %d", pHostName, i);
            mapIPtoHostName.setValue(pch, nm);

            i++;
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    return true;
}

bool CHPCCNagiosToolSet::generateServiceDefinitionFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath )
{
    return true;
}
