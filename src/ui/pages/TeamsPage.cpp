#include "ui/pages/TeamsPage.h"

#include "storage/DataStore.h"
#include "ui/widgets/UiUtils.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QTableWidget>
#include <QVBoxLayout>

TeamsPage::TeamsPage(DataStore *store, QWidget *parent)
    : QWidget(parent)
    , m_store(store)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("球队与花名册"), this);
    title->setObjectName(QStringLiteral("H1"));
    auto *sub = new QLabel(QStringLiteral("选择球队，查看该队全部球员信息"), this);
    sub->setObjectName(QStringLiteral("Muted"));
    root->addWidget(title);
    root->addWidget(sub);

    auto *columns = new QHBoxLayout();
    columns->setSpacing(16);

    // 左：球队列表
    auto *leftCard = new QFrame(this);
    leftCard->setObjectName(QStringLiteral("Card"));
    leftCard->setFixedWidth(260);
    auto *lv = new QVBoxLayout(leftCard);
    lv->setContentsMargins(14, 14, 14, 14);
    lv->setSpacing(10);
    auto *lTitle = new QLabel(QStringLiteral("球队"), leftCard);
    lTitle->setObjectName(QStringLiteral("H3"));
    m_teamList = new QListWidget(leftCard);
    m_teamList->setObjectName(QStringLiteral("TeamList"));
    lv->addWidget(lTitle);
    lv->addWidget(m_teamList, 1);
    columns->addWidget(leftCard);

    // 右：花名册
    auto *rightCard = new QFrame(this);
    rightCard->setObjectName(QStringLiteral("Card"));
    auto *rv = new QVBoxLayout(rightCard);
    rv->setContentsMargins(16, 14, 16, 14);
    rv->setSpacing(6);
    m_teamTitle = new QLabel(QStringLiteral("花名册"), rightCard);
    m_teamTitle->setObjectName(QStringLiteral("H2"));
    m_teamMeta = new QLabel(QStringLiteral("请选择左侧球队"), rightCard);
    m_teamMeta->setObjectName(QStringLiteral("Muted"));
    m_roster = new QTableWidget(rightCard);
    ui::setupTable(m_roster, {QStringLiteral("编号"), QStringLiteral("姓名"),
                              QStringLiteral("号码"), QStringLiteral("位置"),
                              QStringLiteral("年龄"), QStringLiteral("身高"),
                              QStringLiteral("体重"), QStringLiteral("国籍"),
                              QStringLiteral("出场"), QStringLiteral("总得分")});
    for (int c = 0; c < m_roster->columnCount(); ++c)
        ui::alignHeader(m_roster, c, c == 1 ? Qt::AlignLeft : Qt::AlignCenter);
    ui::autoSizeColumns(m_roster, {1});
    rv->addWidget(m_teamTitle);
    rv->addWidget(m_teamMeta);
    rv->addSpacing(6);
    rv->addWidget(m_roster, 1);
    columns->addWidget(rightCard, 1);

    root->addLayout(columns, 1);

    connect(m_teamList, &QListWidget::currentRowChanged, this, [this](int) { onTeamSelected(); });
    connect(m_store, &DataStore::changed, this, &TeamsPage::refresh);

    refresh();
}

void TeamsPage::refresh()
{
    const QString previous = m_teamList->currentItem() ? m_teamList->currentItem()->text() : QString();
    m_teamList->clear();

    for (const QString &team : m_store->teams()) {
        int count = 0;
        for (const Player &p : m_store->players())
            if (p.team == team)
                ++count;
        auto *it = new QListWidgetItem(QStringLiteral("%1   (%2)").arg(team).arg(count));
        it->setData(Qt::UserRole, team);
        m_teamList->addItem(it);
    }

    if (m_teamList->count() == 0) {
        m_teamTitle->setText(QStringLiteral("花名册"));
        m_teamMeta->setText(QStringLiteral("暂无球队，请先在球员管理中添加球员并填写所属球队"));
        m_roster->setRowCount(0);
        return;
    }

    // 恢复之前选中的球队
    int row = 0;
    for (int i = 0; i < m_teamList->count(); ++i) {
        if (m_teamList->item(i)->data(Qt::UserRole).toString() == previous) {
            row = i;
            break;
        }
    }
    m_teamList->setCurrentRow(row);
    onTeamSelected();
}

void TeamsPage::onTeamSelected()
{
    auto *current = m_teamList->currentItem();
    if (!current) {
        m_roster->setRowCount(0);
        return;
    }
    const QString team = current->data(Qt::UserRole).toString();

    QVector<Player> roster;
    for (const Player &p : m_store->players())
        if (p.team == team)
            roster.append(p);

    m_teamTitle->setText(team);

    int totalPoints = 0;
    for (const Player &p : roster)
        totalPoints += m_store->careerTotals(p.id).points();
    m_teamMeta->setText(QStringLiteral("共 %1 名球员 · 生涯合计 %2 分")
                            .arg(roster.size())
                            .arg(totalPoints));

    m_roster->setRowCount(roster.size());
    for (int r = 0; r < roster.size(); ++r) {
        const Player &p = roster.at(r);
        const PlayerStats t = m_store->careerTotals(p.id);
        const int games = m_store->playerMatchLog(p.id).size();
        m_roster->setItem(r, 0, ui::item(p.id, Qt::AlignCenter, ui::gold()));
        m_roster->setItem(r, 1, ui::item(p.name, Qt::AlignLeft));
        m_roster->setItem(r, 2, ui::item(p.number > 0 ? QString::number(p.number) : QStringLiteral("-")));
        m_roster->setItem(r, 3, ui::item(p.position.isEmpty() ? QStringLiteral("-") : p.position));
        m_roster->setItem(r, 4, ui::item(QString::number(p.age)));
        m_roster->setItem(r, 5, ui::item(p.heightCm > 0 ? QString::number(p.heightCm) : QStringLiteral("-")));
        m_roster->setItem(r, 6, ui::item(p.weightKg > 0 ? QString::number(p.weightKg) : QStringLiteral("-")));
        m_roster->setItem(r, 7, ui::item(p.country.isEmpty() ? QStringLiteral("-") : p.country,
                                         Qt::AlignCenter, ui::dim()));
        m_roster->setItem(r, 8, ui::item(QString::number(games)));
        m_roster->setItem(r, 9, ui::item(QString::number(t.points()), Qt::AlignCenter, ui::green()));
    }
}
