//
// Created by timothy on 5/9/16.
//
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include "emailMessage.cpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;

int main(int argc, char *argv[])
{
    //Begin Daemon Construction
    pid_t pid, sid;

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    else
        exit(EXIT_SUCCESS);

    umask(0);

    sid = setsid();
    if (sid < 0)
        exit(EXIT_FAILURE);

    if ((chdir("/")) < 0)
        exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    //End Daemon Construction

    //Actual program
    //Initialization
    //Config members
    int failedPingsAllowed, messageRetryCount;
    string target, smtpServer, smtpAuthUser, smtpPassword, recipientAddress;
    //Internal members
    int failedPings = 0, totalFailedPings = 0;
    bool down = false;
    //Load config
    try
    {
        if(argc != 8)
            throw exception();
        failedPingsAllowed =    int(argv[1]);
        messageRetryCount =     int(argv[2]);
        target =                    argv[3];
        smtpServer =                argv[4];
        smtpAuthUser =              argv[5];
        smtpPassword =              argv[6];
        recipientAddress =          argv[7];
    }
    catch (exception e)
    {
        syslog(LOG_MAKEPRI(LOG_USER, LOG_ERR), "Invalid Config. IsAlive exiting.");
        return -1;
    }
    //Create email message holders
    emailMessage downMessage(smtpServer, smtpAuthUser, smtpPassword, smtpAuthUser, recipientAddress, target + " DOWN"),
            upMessage(smtpServer, smtpAuthUser, smtpPassword, smtpAuthUser, recipientAddress, target + " UP");
    //Fail if no shell
    if (system(NULL) == 0)
    {
        syslog(LOG_MAKEPRI(LOG_USER, LOG_ERR), "Shell undefined. IsAlive exiting.");
        return -1;
    }

    while (true) //Running loop
    {
        if (system(("ping -c 1 -q " + target).c_str()) == 0)
        {
            //Perfect ping
            if (down) //If we were down
            {
                syslog(LOG_MAKEPRI(LOG_USER, LOG_NOTICE), ("Target " + target + " back up after " +
                                                           to_string(totalFailedPings) + " pings.").c_str());
                down = false;

                syslog(LOG_MAKEPRI(LOG_USER, LOG_NOTICE), "Attempting to send email notification.");
                for (int i = 0; i < messageRetryCount; i++) //Try to send
                    if (upMessage.send() == 0)
                        break;
                    else
                        syslog(LOG_MAKEPRI(LOG_USER, LOG_NOTICE), "Failed to send email notification.");

                failedPings = 0; //Reset failed counters
                totalFailedPings = 0;
            }
        }
        else
        {
            //Failed ping
            syslog(LOG_MAKEPRI(LOG_USER, LOG_NOTICE), ("Target " + target + " is down for " +
                                                       to_string(totalFailedPings + 1) + " pings.").c_str());
            if (!down || failedPings >= failedPingsAllowed) //If we weren't down or we passed the failed ping allowance
            {
                down = true; //Consider target down

                syslog(LOG_MAKEPRI(LOG_USER, LOG_NOTICE), "Attempting to send email notification.");
                downMessage.setBody("Ping to " + target + " failed!\r\n\tPings failed: " + to_string(totalFailedPings));
                for (int i = 0; i < messageRetryCount; i++) //Try to send
                    if (downMessage.send() == 0)
                        break;
                    else
                        syslog(LOG_MAKEPRI(LOG_USER, LOG_NOTICE), "Failed to send email notification.");

                failedPings = 0; //Reset failed pings b/c we sent a message
            }
            else
            {
                failedPings++; //Increment failed pings otherwise
                totalFailedPings++;
            }
        }
    }
}

#pragma clang diagnostic pop
#pragma clang diagnostic pop