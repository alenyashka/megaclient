#ifndef SINGLETON_H
#define SINGLETON_H

#include <QMutex>

static QMutex mutex;

template <class T> class Singleton
{
    static T *_self;
    static int _refcount;
protected:
    Singleton(){}
    virtual ~Singleton(){_self = NULL;}
public:
    static T* Instance()
    {
        mutex.lock();
        if (!_self) _self = new T;
        _refcount++;
        mutex.unlock();
        return _self;
    }
    void FreeInst()
    {
        if (--_refcount == 0) delete this;
    }
};

template <class T> T* Singleton<T>::_self = NULL;
template <class T> int Singleton<T>::_refcount = 0;

#endif // SINGLETON_H
