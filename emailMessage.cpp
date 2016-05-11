//
// Created by timothy on 5/9/16.
//
#include <cstdlib>
#include <string>

using namespace std;

class emailMessage {
private:
    string smtpServer;          //"mail.example.com:587"
    string smtpAuthUser;        //"someone@example.com"
    string smtpPassword;        //"abc123"
    string senderAddress;       //"someone@example.com"
    string recipientAddress;    //someone@example.com
    string subject;             //"Subject"
    string body;                //"Body"

    void quoteSurround(string &s)
    {
        if (s[0] != '\"')
            s.insert(0, "\"");
        if (s[s.length() - 1] != '\"')
            s.append("\"");
    }

public:
    emailMessage(string smtpServer = "", string smtpAuthUser = "", string smtpPassword = "",
                 string senderAddress = "", string recipientAddress = "", string subject = "", string body = "")
    {
        setServer(smtpServer);
        setAuthUser(smtpAuthUser);
        setPassword(smtpPassword);
        setSenderAddress(senderAddress);
        setRecipientAddress(recipientAddress);
        setSubject(subject);
        setBody(body);
    }

    void setServer(string smtpServer)
    {
        quoteSurround(smtpServer);
        this->smtpServer = smtpServer;
    }

    string getServer() const
    {
        return smtpServer;
    }

    void setAuthUser(string smtpAuthUser)
    {
        quoteSurround(smtpAuthUser);
        this->smtpAuthUser = smtpAuthUser;
    }

    string getAuthUser() const
    {
        return smtpAuthUser;
    }

    void setPassword(string smtpPassword)
    {
        quoteSurround(smtpPassword);
        this->smtpPassword = smtpPassword;
    }

    void setSenderAddress(string senderAddress)
    {
        quoteSurround(senderAddress);
        this->senderAddress = senderAddress;
    }

    string getSenderAddress() const
    {
        return senderAddress;
    }

    void setRecipientAddress(string recipientAddress)
    {
        this->recipientAddress = recipientAddress;
    }

    string getRecipientAddress() const
    {
        return recipientAddress;
    }

    void setSubject(string subject)
    {
        quoteSurround(subject);
        this->subject = subject;
    }

    string getSubject() const
    {
        return subject;
    }

    void setBody(string body)
    {
        quoteSurround(body);
        this->body = body;
    }

    string getBody() const
    {
        return body;
    }

    int send() const
    {
        if (system(NULL) == 0) //Fail if no shell
            return -1;
        string command = "echo " + getBody() + " | mailx -v -r " + getSenderAddress() + " -s " + getSubject()
                        + " -S smtp=" + getServer() + " -S smtp-use-starttls -S smtp-auth=login -S smtp-auth-user="
                        + getAuthUser() + "-S smtp-auth-password=" + smtpPassword + "-S ssl-verify=ignore "
                        + getRecipientAddress();

        return system(command.c_str());
    }
};