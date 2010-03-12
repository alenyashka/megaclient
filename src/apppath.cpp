#include "apppath.h"

const QString AppPath::getL10nPath()
{
    QString path;
    path.append(QApplication::applicationDirPath() + QDir::separator() +
                "l10n");

    QDir defaultDir(path);
    if (defaultDir.exists())
    {
        return defaultDir.absolutePath();
    }

    path.clear();
    path.append(QApplication::applicationDirPath() + QDir::separator() +
                "../share/l10n");
    return path;
}
