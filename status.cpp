#include <tao/json.hpp>
using namespace tao;
#include "status.h"

namespace drumlin {

Status::Status()
{
    m_json = new json::value(json::empty_object);
}

Status::~Status()
{
    delete m_json;
}

void Status::visit(StatusProvider *provider)
{
    provider->getStatus(m_json);
}

ostream &operator<<(ostream &str,Status const& status)
{
    json::to_stream(str,*status.m_json,2);
    return str;
}

}
