#ifndef STATUS_H
#define STATUS_H

#include "tao_forward.h"
using namespace tao;
#include <iostream>
using namespace std;

namespace drumlin {

class StatusProvider
{
public:
    virtual void getStatus(json::value *status)const=0;
    virtual ~StatusProvider(){}
};

class Status
{
public:
    Status();
    ~Status();
    void visit(StatusProvider *provider);
    json::value &getJson(){ return *m_json; }
protected:
    json::value *m_json;
    friend std::ostream &operator<<(std::ostream &str,Status const& status);
};

} // namespace drumlin

#endif // STATUS_H
