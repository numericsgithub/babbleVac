class MeanFinder
{
  private:
    
    int lastValue;
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
    int mean;
    MeanFinder()
    {
      lastValue = 0;
      mean = 0;
      isInit = false;
    }

    void SetMean(int value)
    {
      mean = value;
      Serial.println("Mean Set");
    }

    /*int Mean()
    {
      return mean;
    }*/

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
        Serial.println("Inited");
        return diff(mean, value); 
      }
      int rdif = random(diff(mean, value));
      if(rdif > 50||rdif < 20)
      {
        return diff(mean, value);
      }
      if(value > mean)
        mean++;
      else if(value < mean) mean--;
      return diff(mean, value);
    }
};

