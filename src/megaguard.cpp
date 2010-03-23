#include "megaguard.h"

MegaGuard::MegaGuard()
{
    mode = User;
}

void MegaGuard::setPassword(const QString &pass)
{
    password = pass;
}

QString MegaGuard::getPassword()
{
    return password;
}

Privileges MegaGuard::getMode()
{
    return mode;
}

void MegaGuard::setMode(const Privileges &m)
{
    mode = m;
}
