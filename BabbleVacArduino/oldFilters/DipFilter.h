class DipFilter
{
  private:
    int pitch;
    int lastFeed;

    /*bool NewDirection(int value)
    {
      if(value < lastFeed)
        return -1;
      else if(value > lastFeed) return 1;
      else if(value == lastFeed)
        return 0;
      return curDirection;
    }*/
  public:
    int curDirection;
    
    DipFilter()
    {
      //pitchStartValue = 0;
      lastFeed = 0;
      curDirection = 0;
    }

    int Empty()
    {
      return pitch;
    }

    bool Feed(int value)
    {
      if(value > lastFeed) // We go up
      {
        if(curDirection == 1)
        {
          lastFeed = value;
          return false; // All fine
        }
        if(value != lastFeed)
        {
          curDirection = 1;
        }
        else curDirection = 0;
        pitch = lastFeed;
        lastFeed = value;
        return true; // All changed
      }
      if(value < lastFeed) // We go down
      {
        if(curDirection == -1)
        {
          lastFeed = value;
          return false; // All fine
        }
        if(value != lastFeed)
          curDirection = -1;
        else curDirection = 0;
        pitch = lastFeed;
        lastFeed = value;
        return true; // All changed
      }
      if(value == lastFeed) // We stay
      {
        if(curDirection == 0)
        {
          lastFeed = value;
          return false; // All fine
        }
        if(value > lastFeed)
          curDirection = 1;
        else curDirection = -1;
        pitch = lastFeed;
        lastFeed = value;
        return true; // All changed
      }
      
      /*if(NewDirection(value) != curDirection)
      {
        Serial.println("900");
        curDirection = NewDirection(value);
        lastFeed = value;
        disturbance = 0;
        return true;
      }
      lastFeed = value;
      return false;*/
      
      /*switch(direction)
      {
        case 0: 
        {
          if(value == 0)
            return false;
          pitchStartValue = 0;
          ChangeDirection(value);
        } break;
        case 1: 
        {
          if(value > lastFeed)
          {
            lastFeed = value;
            return false;
          }
          pitchStartValue = value;
          ChangeDirection(value);
        } break;
      }
      lastFeed = value;
      return true;*/
    }
};

