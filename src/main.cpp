#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QMainWindow>

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
    loadTheme(app);

    QMainWindow w;
    w.setWindowTitle(QStringLiteral("篮球联赛管理系统"));
    w.resize(1280, 800);
    auto *lbl = new QLabel(QStringLiteral("Basketball League System — scaffold OK"), &w);
    lbl->setAlignment(Qt::AlignCenter);
    w.setCentralWidget(lbl);
    w.show();

    return app.exec();
}
