#include <QApplication>
#include <QFile>

#include "storage/AuthManager.h"
#include "storage/DataStore.h"
#include "ui/LoginDialog.h"
#include "ui/MainWindow.h"

static void loadTheme(QApplication &app)
{
    QFile f(QStringLiteral(":/style/theme.qss"));
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(f.readAll()));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("BasketballLeagueSystem"));
    app.setOrganizationName(QStringLiteral("CourseDesign"));
    app.setQuitOnLastWindowClosed(false);
    loadTheme(app);

    DataStore store;
    store.load();
    AuthManager auth(&store);

    LoginDialog login(&auth);
    if (login.exec() != QDialog::Accepted)
        return 0;

    MainWindow window(&store, login.username());

    // 退出登录：隐藏主窗口，重新弹出登录框
    QObject::connect(&window, &MainWindow::logoutRequested, &app, [&]() {
        window.hide();
        LoginDialog again(&auth);
        if (again.exec() == QDialog::Accepted) {
            window.setUser(again.username());
            window.show();
        } else {
            app.quit();
        }
    });

    window.show();
    return app.exec();
}
