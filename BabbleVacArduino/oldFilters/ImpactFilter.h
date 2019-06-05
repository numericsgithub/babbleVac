class ImpactFilter
{
  private:
    int lastFeed;

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
    int peekVal;
    ImpactFilter()
    {
      lastFeed = 0;
      peekVal = 0;
    }

    int Empty()
    {
      return peekVal;
    }

    bool Feed(int value)
    {
      if(diff(value, peekVal) > 1600)
      {
        Serial.print("Impact: ");
        Serial.println(diff(peekVal, value));
        peekVal = value;
        return true;
      }
      //if(diff(peekVal, 0) > diff(value, 0))
      //  peekVal = value;
      return false;
      /*if(value > lastFeed)
      {
        Serial.println("900");
        peekVal = lastFeed;
        return false;
      }
      return true;*/
      /*if(value > peekVal)
        peekVal = value;
      if(curDirection == 1)
      {
        if(value > lastFeed)
        {
          lastFeed = value;
          return false;
        }
        lastFeed = value;
        curDirection = 0;
        return true;
      }
      if(curDirection == -1)
      {
        if(value < peekVal)
          peekVal = value;
        if(value < lastFeed)
        {
          lastFeed = value;
          return false;
        }
        lastFeed = value;
        curDirection = 0;
        return true;
      }
      if(curDirection == 0)
      {
        if(value == lastFeed)
        {
          return false;
        }
        if(value > lastFeed)
          curDirection = 1;
        else curDirection = -1;
        lastFeed = value;
        peekVal = value;
        return false;
      }*/
    }
};

