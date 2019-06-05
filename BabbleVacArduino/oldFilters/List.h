#include "ListElement.h"
template <typename T> 
class List
{
  private:
    ListElement<T>* first;
    ListElement<T>* last;
    int len;
    
    ListElement<T>* GetEle(int index)
    {
      ListElement<T>* cur = first;
      for(int i = 0; i < index && i < len; i++)
        cur = cur->next;
      return cur;
    }
    
  public:
    List()
    {
      len = 0;
    }
  
    T Get(int index)
    {
      return GetEle(index)->value;
    }

    void Add(T value)
    {
      ListElement<T>* ele = new ListElement<T>();
      ele->value = value;
      if(len == 0)
      {
        first = ele;
        last = ele;
      }
      else {
        last->next = ele;
        last = ele;
      }
      len++;
    }

    int Length()
    {
      return len;
    }
};

