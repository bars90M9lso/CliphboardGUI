#include "ClipboardDaemon.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("CliphistGUI");

    ClipboardDaemon clipboardDaemon;

    if (app.arguments().contains("--capture"))
    {
        QString state = qEnvironmentVariable("CLIPBOARD_STATE");

        if (state != "data")
            return 0;

        clipboardDaemon.AddItem();
        return 0;
    }

    clipboardDaemon.StartMonitoring();

    return app.exec();
}