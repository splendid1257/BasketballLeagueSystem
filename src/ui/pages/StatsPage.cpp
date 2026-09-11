#include "ui/pages/StatsPage.h"

#include "ui/widgets/UiUtils.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

StatsPage::StatsPage(DataStore *store, QWidget *parent)
    : QWidget(parent)
    , m_store(store)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("数据排行榜"), this);
    title->setObjectName(QStringLiteral("H1"));
    m_subtitle = new QLabel(this);
    m_subtitle->setObjectName(QStringLiteral("Muted"));
    root->addWidget(title);
    root->addWidget(m_subtitle);

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(10);
    auto *label = new QLabel(QStringLiteral("榜单类型"), this);
    m_board = new QComboBox(this);
    m_board->setMinimumWidth(180);
    m_board->addItem(QStringLiteral("得分榜"), int(DataStore::Board::Points));
    m_board->addItem(QStringLiteral("三分榜"), int(DataStore::Board::ThreePointers));
    m_board->addItem(QStringLiteral("篮板榜"), int(DataStore::Board::Rebounds));
    m_board->addItem(QStringLiteral("扣篮榜"), int(DataStore::Board::Dunks));
    m_board->addItem(QStringLiteral("抢断榜"), int(DataStore::Board::Steals));
    toolbar->addWidget(label);
    toolbar->addWidget(m_board);
    toolbar->addStretch();
    root->addLayout(toolbar);

    m_table = new QTableWidget(this);
    ui::setupTable(m_table, {QStringLiteral("排名"), QStringLiteral("球员"),
                             QStringLiteral("球队"), QStringLiteral("位置"),
                             QStringLiteral("年龄"), QStringLiteral("身高"),
                             QStringLiteral("体重"), QStringLiteral("出场"),
                             QStringLiteral("数值")});
    for (int c = 0; c < m_table->columnCount(); ++c)
        ui::alignHeader(m_table, c, (c == 1 || c == 2) ? Qt::AlignLeft : Qt::AlignCenter);
    ui::autoSizeColumns(m_table, {1, 2});
    root->addWidget(m_table, 1);

    connect(m_board, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { reload(); });
    connect(m_store, &DataStore::changed, this, &StatsPage::refresh);

    refresh();
}

DataStore::Board StatsPage::currentBoard() const
{
    return static_cast<DataStore::Board>(m_board->currentData().toInt());
}

void StatsPage::refresh()
{
    reload();
}

void StatsPage::reload()
{
    const DataStore::Board board = currentBoard();
    QString unit;
    switch (board) {
    case DataStore::Board::Points: unit = QStringLiteral("生涯总得分（三分×3 + 扣篮×2）"); break;
    case DataStore::Board::ThreePointers: unit = QStringLiteral("生涯三分球命中总数"); break;
    case DataStore::Board::Rebounds: unit = QStringLiteral("生涯篮板球总数"); break;
    case DataStore::Board::Dunks: unit = QStringLiteral("生涯扣篮成功总数"); break;
    case DataStore::Board::Steals: unit = QStringLiteral("生涯抢断总数"); break;
    }
    m_subtitle->setText(unit + QStringLiteral(" · 按降序排列"));

    const auto rows = m_store->leaderboard(board);
    m_table->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const Player p = m_store->findPlayer(rows.at(r).first);
        const int games = m_store->playerMatchLog(p.id).size();
        const QColor medal = (r == 0) ? ui::gold() : (r < 3 ? ui::green() : ui::dim());

        m_table->setItem(r, 0, ui::item(QString::number(r + 1), Qt::AlignCenter, medal));
        m_table->setItem(r, 1, ui::item(p.name, Qt::AlignLeft));
        m_table->setItem(r, 2, ui::item(p.team, Qt::AlignLeft, ui::dim()));
        m_table->setItem(r, 3, ui::item(p.position.isEmpty() ? QStringLiteral("-") : p.position));
        m_table->setItem(r, 4, ui::item(QString::number(p.age)));
        m_table->setItem(r, 5, ui::item(p.heightCm > 0 ? QString::number(p.heightCm) : QStringLiteral("-")));
        m_table->setItem(r, 6, ui::item(p.weightKg > 0 ? QString::number(p.weightKg) : QStringLiteral("-")));
        m_table->setItem(r, 7, ui::item(QString::number(games)));
        m_table->setItem(r, 8, ui::item(QString::number(rows.at(r).second), Qt::AlignCenter, ui::gold()));
    }
}
