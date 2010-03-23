#ifndef MEGAGUARD_H
#define MEGAGUARD_H

#include "singleton.h"
#include <QtCore>

enum Privileges
{
    User,
    Admin
};

class MegaGuard : public Singleton<MegaGuard>
{
protected:
    MegaGuard();
    friend class Singleton<MegaGuard>;
public:
    void setPassword(const QString&);
    QString getPassword();
    Privileges getMode();
    void setMode(const Privileges&);
private:
    Privileges mode;
    QString password;
};

#endif // MEGAGUARD_H
