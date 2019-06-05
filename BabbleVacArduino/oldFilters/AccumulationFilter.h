

class AccumulationFilter
{
  private:
    long long accValue;
    unsigned int valueCount;
    unsigned long lastStart;
    unsigned int intervall;
    int maxVal;
    int minVal;

    bool isIntervallReady()
    {
      return (millis() - lastStart >= intervall) && valueCount > 2;
    }
  public:
    bool UseExtreme;
  
    AccumulationFilter(unsigned int intervall)
    {
      maxVal = -16000;
      minVal = 16000;
      accValue = 0;
      valueCount = 0;
      lastStart = millis();
      this->intervall = intervall;
      UseExtreme = false;
    }

    AccumulationFilter(unsigned int intervall, bool useExtreme)
    {
      maxVal = -16000;
      minVal = 16000;
      accValue = 0;
      valueCount = 0;
      lastStart = millis();
      this->intervall = intervall;
      UseExtreme = useExtreme;
    }

    bool IsReady()
    {
      if(!UseExtreme)
        return isIntervallReady();
      return true;
    }

    int Empty()
    {
      if(isIntervallReady())
      {
        lastStart = millis();
        int value = ((accValue ) / (valueCount ));
        accValue = 0;
        valueCount = 0;
        maxVal = -16000;
        minVal = 16000;
        return value;
      }
      else if(IsReady())
        return abs(maxVal) > abs(minVal)?maxVal:minVal;
      return -1;
    }

    void Feed(int value)
    {
      if(value > maxVal)
        maxVal = value;
      if(value < minVal)
        minVal = value;
      accValue += value;
      valueCount++;
    }
};

