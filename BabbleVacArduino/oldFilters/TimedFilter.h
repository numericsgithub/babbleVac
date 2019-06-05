
template <typename T> 
class TimedFilter
{
  private:
    const int SIZE = 200;
    T values[SIZE];
    unsigned long times[SIZE];
    int curIndex;
    bool isReady;
  public:
    Filter()
    {
      curIndex = 0;
      for(int i = 0; i < SIZE; i++)
      {
        values[i] = 0;
        times[i] = 0;
      }
      isReady = true;
    }

    bool IsReady()
    {
      return isReady;
    }

    void Feed(T value)
    {
      times[curIndex] = millis();
      values[curIndex] = value;
      curIndex++;
      if(curIndex == SIZE)
      {
        isReady = true;
        curIndex = 0;
      }
    }

    
};

