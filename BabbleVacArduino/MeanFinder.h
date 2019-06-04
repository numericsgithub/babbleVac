class MeanFinder
{
  private:
    int lastValue;
    int mean;
    bool isInit;

    int diff(int x, int y)
    {
      int diff = 0;
      if (x>y) { 
        diff = (x-y);
      } else if (y>x) {
         diff = (y-x);
      } else if (x==y) {
        diff = 0;
      }
      return diff;
    }
  public:
    MeanFinder()
    {
      lastValue = 0;
      mean = 0;
      isInit = false;
    }

    int Mean()
    {
      return mean;
    }

    int Get()
    {
      return -1 * (mean - lastValue);
    }

    int Feed(int value)
    {
      lastValue = value;
      if(!isInit)
      {
        isInit = true;
        mean = value;
        return diff(mean, value); 
      }
      int rdif = random(diff(mean, value));
      if(rdif < 20 || rdif > 50)
      {
        return diff(mean, value);
      }
      if(value > mean)
        mean++;
      else if(value < mean) mean--;
      return diff(mean, value);
    }
};

