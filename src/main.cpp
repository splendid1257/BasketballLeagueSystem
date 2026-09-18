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
    // 退出登录通过隐藏主窗口实现；禁用“最后窗口关闭即退出”，改由 closeEvent 显式退出
    app.setQuitOnLastWindowClosed(false);
    loadTheme(app);

    DataStore store;
    store.load();
    AuthManager auth(&store);

    LoginDialog login(&auth);
    if (login.exec() != QDialog::Accepted)
        return 0;

    MainWindow window(&store, login.username());

    // 退出登录：隐藏主窗口并重新显示登录框
    QObject::connect(&window, &MainWindow::logoutRequested, &app, [&]() {
        // 用 hide() 而非 close()：close() 会触发 closeEvent 中的显式退出，导致进程终止
        window.hide();
        // 最初的登录框已在 main() 作用域内析构，这里须重新构造一份
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
