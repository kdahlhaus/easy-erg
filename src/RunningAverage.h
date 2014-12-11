#ifndef RunningAverage_h
#define RunningAverage_h

// template to keep a running average for things like heart rate, power

template <class AVERAGE_TYPE, class RUNNING_TOTAL_TYPE>
class RunningAverage
{
    public:

        RunningAverage() { reset(); }

        // access the current average
        AVERAGE_TYPE aAverage(void) { return (AVERAGE_TYPE)(numb_of_items != 0 ? total/numb_of_items : 0); }
        void reset() { total=(RUNNING_TOTAL_TYPE)0; numb_of_items=0; }

        void add(AVERAGE_TYPE value) { total = total += (RUNNING_TOTAL_TYPE)value; numb_of_items += 1; }

    private:
        RUNNING_TOTAL_TYPE total;
        unsigned short numb_of_items;
};

#endif 
