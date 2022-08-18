#include "evccDataClass.h"

void evccDataClass::addAvgData(AvgData *avgdata, int newdata)
{
    avgdata->sumraw = avgdata->sumraw + newdata;
    if (avgdata->count <= avgdata->maxcount)
    {
        avgdata->count++;
    }
    else
    {
        avgdata->sumraw = avgdata->sumraw - avgdata->avgraw;
    }
    avgdata->avgraw = avgdata->sumraw / avgdata->count;
    avgdata->avg = avgdata->avgraw;
    avgdata->last = newdata;
}