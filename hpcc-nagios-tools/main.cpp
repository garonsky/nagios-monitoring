#include "HPCCNagiosToolSet.hpp"
#include "jstring.hpp"
#include <iostream>

void usage()
{
    std::cout << "HPCC Systems Nagios configuration file generator\n\
Usage: hpcc-nagios-tools -env <environment file> -out <output path> [options]\n";
    std::cout << "  -c or -cfggen     : The path to the configgen.  (Default: /opt/HPCCSystems/sbin/configgen)\n";
    std::cout << "  -g or -hostgroup  : generate host group file\n";
    std::cout << "  -s or -service    : generate service and host file\n";
    std::cout << "  -p or -nrpe       : generate client plugin cfgs for nrpe\n";
    std::cout << "  -m or -commands   : generate nrpe command config\n";
    std::cout << "  -e or -env        : hpcc environment configuration file (Default: /etc/HPCCSystems/environment.xml)\n";
    std::cout << "  -o or -output     : outpfile where the generated configuration will be written\n";
    std::cout << "  -r or -retry      : keep attempting to resolve IP to hostnames (Default: will stop resolution after 1st failure)\n";
    std::cout << "Advanced Options (when using -nrpe) :\n";
    //std::cout << "
}

int main(int argc, char *argv[])
{
    int i = 1;
    StringBuffer strConfigGenPath;
    StringBuffer strOutputPath;
    StringBuffer strEnvFilePath;
    bool bGenerateHostGroup         = false;
    bool bGenerateServiceAndHost    = false;
    bool bGenerateNRPECommands      = false;

    if (argc == 1)
    {
        usage();
        return 0;
    }

    while (i < argc)
    {
        if (argc == 1 || stricmp(argv[i],"-h") == 0 || stricmp(argv[i],"-help") == 0)
        {
            usage();
            return 0;
        }
        else if (stricmp(argv[i], "-c") == 0 || stricmp(argv[i], "-cfggen") == 0)
        {
            i++;
            strConfigGenPath.set(argv[i]);
        }
        else if (stricmp(argv[i], "-g") == 0 || stricmp(argv[i], "-hostgroup") == 0)
        {
            bGenerateHostGroup = true;
        }
        else if (stricmp(argv[i], "-s") == 0 || stricmp(argv[i], "-service") == 0)
        {
            bGenerateServiceAndHost = true;
        }
        else if (stricmp(argv[i], "-p") == 0 || stricmp(argv[i], "-nrpe") == 0)
        {
            CHPCCNagiosToolSet::m_bUseNPRE = true;
        }
        else if (stricmp(argv[i], "-o") == 0 || stricmp(argv[i], "-output") == 0 || stricmp(argv[i], "-out") == 0)
        {
            i++;
            strOutputPath.set(argv[i]);
        }
        else if (stricmp(argv[i], "-e") == 0 || stricmp(argv[i], "-env") == 0)
        {
            i++;
            strEnvFilePath.set(argv[i]);
        }
        else if (stricmp(argv[i], "-r") == 0 || stricmp(argv[i], "-retry") == 0)
        {
            CHPCCNagiosToolSet::m_retryHostNameLoookUp = true;
        }
        else if (stricmp(argv[i], "-m") == 0 || stricmp(argv[i], "-commands") == 0)
        {
            bGenerateNRPECommands = true;
        }

        i++;
    }

    try
    {
        if (strOutputPath.length() == 0)
        {
            std::cout << "Missing output file path! (-output)\n";
            return 0;
        }
        else if ( bGenerateServiceAndHost + bGenerateHostGroup + bGenerateNRPECommands != 1) //(bGenerateServiceAndHost^bGenerateHostGroup)^bGenerateNRPECommands) == false)
        {
            std::cout << "Select one (1) type of config per invocation (e.g. -hostgroup xor -service xor -nrpe)\n";
            return 0;
        }
        else if (bGenerateHostGroup == true)
        {
            std::cout << "Generating hostgroup --> " << strOutputPath.str();
            std::flush(std::cout);

            if (CHPCCNagiosToolSet::generateHostGroupsConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
            {
                std::cout << "\nError generating configuration!. Verify input.\n";
                return 0;
            }
        }
        else if (bGenerateServiceAndHost == true)
        {
            std::cout << "Generating service and host config --> " << strOutputPath.str();
            std::flush(std::cout);

            if (CHPCCNagiosToolSet::generateServerAndHostConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
            {
                std::cout << "\nError generating service and host configuration!. Verify input.\n";
                return 0;
            }
        }
        else if (bGenerateNRPECommands == true)
        {
            std::cout << "Generating nrpe client command config --> " << strOutputPath.str();
            std::flush(std::cout);


        }

        std::cout << "\nDone!\n";
    }
    catch (...)
    {
        std::cout << "\nException caught!\n  -->Check that files and directories exists and that permissions are set properly.\n";
    }

    return 0;
}
