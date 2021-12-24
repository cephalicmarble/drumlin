#ifndef _WORK_H
#define _WORK_H

#include "metatypes.h"
#include "tao_forward.h"
using namespace tao;
// system, process & thread clocks : 10, 50, 100 ticks.
// rating fr WorkerThread, completion, ETA, lateness etc.
// allocator usage by this thread
#define WorkReportTypes ( \
    (WorkReport_first)(0), \
    (WorkReport_Elapsed)(1), \
    (WorkReport_Jobs)(2), \
    (WorkReport_Heap)(4), \
    (WorkReport_All)(7) \
)

ENUMN(ReportType, WorkReportTypes);

namespace drumlin {


class WorkDone
{
    enum WorkReportType {
        Elapsed  = 1,               // system, process & thread clocks : 10, 50, 100 ticks.
        Jobs     = 2,               // rating fr WorkerThread, completion, ETA, lateness etc.
        Memory   = 4,               // allocator usage by this thread
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
