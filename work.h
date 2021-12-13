#ifndef _WORK_H
#define _WORK_H

#include "tao_forward.h"
using namespace tao;

namespace drumlin {

class WorkDone
{
    enum WorkReportType {
        Elapsed  = 1,
        Jobs     = 2,
        Memory   = 4,
        All = Elapsed|Jobs|Memory,
    };
public:
    typedef WorkReportType ReportType;
    virtual ~WorkDone(){}
    virtual void stop(){}
    virtual void report(json::value *obj,ReportType type)const=0;
};
typedef WorkDone WorkObject;

} // namespace drumlin

#endif
