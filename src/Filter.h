#ifndef Filter_h
#define Filter_h 

template <class T>
class Filter
{

public:

    //---------------------------------------------------------
    //
    //  inputs:     theSize - # of items in the buffer
    //  outputs:    none
    //  notes:      The sorting algorithm is a simple insertion short.
    //              for sizes > 9 or so it would be advisable to 
    //              implement a better algorithm.
    //
    Filter(int theSize);
     ~Filter(void);

    //---------------------------------------------------------
    //
    //  inputs:     none
    //  outputs:    returns newest value in buffer
    //  notes:
    //
    T aNewest(void);

    //---------------------------------------------------------
    //
    //  inputs:     none
    //  outputs:    returns mean of values in the buffer
    //  notes:
    //
    T aMean(void);
    T aAverage(void);

    //---------------------------------------------------------
    //
    //  inputs: adds an item to the buffer, removing the oldest
    //  outputs:
    //  notes:
    //
     void add(T value);


    //---------------------------------------------------------
    //
    //  inputs:     newValue
    //  outputs:
    //  notes:      Sets all data in the buffer to the value
    //
    void setAllTo(const T& newValue);


    //---------------------------------------------------------
    //
    //  inputs:
    //  outputs:
    //  notes:      returns # of spaces in  buffer
    //
    int aSize(void);


protected:

    int size;
    int head;
    int meanIndex;
    T *data;
    int *indexSortedByValue;
};

template <class T>
T Filter<T>::aNewest(void)
{
    return data[head];
}

template <class T>
T Filter<T>::aMean(void)
{
    return data[indexSortedByValue[meanIndex]];
}


template <class T>
T Filter<T>::aAverage(void)
{
    T sum = 0;
    for ( int i=0; i<size; i++)
    {
        sum = sum + data[i];
    }
    return sum/size;
}

template <class T>
void Filter<T>::add(T newItem)
{
    head = (++head % size );
    data[head] = newItem;

    //now sort
    int *lowerBound=indexSortedByValue;
    int *upperBound=indexSortedByValue+(size-1);
    int value;
    int *nextElementIndex;
    int *shiftIndex;
    int *shiftIndexMin=lowerBound-1;
    
    for (nextElementIndex = lowerBound+1; nextElementIndex <= upperBound; 
            nextElementIndex++)
    {
        value= *nextElementIndex;
        for (shiftIndex=nextElementIndex-1; shiftIndex != shiftIndexMin && 
                (data[*shiftIndex]>data[value]); shiftIndex--)
        {
            *(shiftIndex+1)=*shiftIndex;
        }
        *(shiftIndex+1)=value;
    }
}


template <class T>
Filter<T>::~Filter(void)
{
    delete [] data;
    delete [] indexSortedByValue;
}

template <class T>
Filter<T>::Filter(int theSize) :
    size(theSize),
    head(0)
{
    //data = new [] T[size];
    data = new  T[size];
    //indexSortedByValue = new [] int[size];
    indexSortedByValue = new int[size];

    for (int i=0; i<size; i++)
    {
        indexSortedByValue[i]=i;
    }

    meanIndex = size/2;

    setAllTo(0);
}


template <class T>
void Filter<T>::setAllTo(const T& newValue)
{
    for (int i=0; i<size; i++)
    {
        data[i] = newValue;
    }
}

template <class T>
int Filter<T>::aSize(void)
{
    return size;
}


#endif




