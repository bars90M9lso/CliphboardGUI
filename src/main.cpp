#include "Clipboard.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QProcess>
#include <QQmlContext>
#include <QWindow>
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QQuickWindow> 

int main(int argc, char *argv[])
{
    LayerShellQt::Shell::useLayerShell();
    QGuiApplication app(argc, argv);
    app.setApplicationName("CliphistGUI");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    Clipboard clipboard;

    engine.rootContext()->setContextProperty(
        "clipboard",
        &clipboard
    );

    
    int cursorX = 0;
    int cursorY = 0;

    QProcess process;
    process.start("hyprctl", {"cursorpos"});
    if (process.waitForFinished()) {
        QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
        QStringList coordinates = output.split(',');
        if (coordinates.size() >= 2) {
            cursorX = coordinates[0].trimmed().toInt();
            cursorY = coordinates[1].trimmed().toInt();
        }
    }

    engine.loadFromModule("Clipboard", "Main");

    auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
    auto *layerWindow = LayerShellQt::Window::get(window);

    if (layerWindow) {
        // 1. Используем LayerTop вместо LayerOverlay
        layerWindow->setLayer(LayerShellQt::Window::LayerTop);

        // 2. Привязка к левому верхнему углу
        layerWindow->setAnchors(
            LayerShellQt::Window::Anchor(
                LayerShellQt::Window::AnchorTop |
                LayerShellQt::Window::AnchorLeft
            )
        );

        // 3. Позиция через отступы от краев экрана
        int marginLeft = qMax(0, cursorX - 200);
        int marginTop = qMax(0, cursorY - 275);
        layerWindow->setMargins(QMargins(marginLeft, marginTop, 0, 0));

        // 4. Явно передаем размер и отключаем сдвиг других окон
        layerWindow->setDesiredSize(QSize(400, 550));
        layerWindow->setExclusiveZone(-1);

        // setKeyboardInteractivity не вызываем (остается OnDemand по умолчанию)
    }

    window->show();


    return app.exec();
}
