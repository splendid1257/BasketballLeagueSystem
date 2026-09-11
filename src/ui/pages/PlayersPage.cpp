#include "ui/pages/PlayersPage.h"

#include "storage/DataStore.h"
#include "ui/dialogs/PlayerEditDialog.h"
#include "ui/widgets/UiUtils.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PlayersPage::PlayersPage(DataStore *store, QWidget *parent)
    : QWidget(parent)
    , m_store(store)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("球员管理"), this);
    title->setObjectName(QStringLiteral("H1"));
    auto *sub = new QLabel(QStringLiteral("维护球员档案，双击查看生涯数据与出场记录"), this);
    sub->setObjectName(QStringLiteral("Muted"));
    root->addWidget(title);
    root->addWidget(sub);

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(10);
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(QStringLiteral("搜索编号 / 姓名 / 球队…"));
    m_search->setClearButtonEnabled(true);
    m_search->setMinimumWidth(280);
    toolbar->addWidget(m_search);
    toolbar->addStretch();

    auto *detailBtn = new QPushButton(QStringLiteral("查看详情"), this);
    auto *editBtn = new QPushButton(QStringLiteral("编辑"), this);
    auto *delBtn = new QPushButton(QStringLiteral("删除"), this);
    delBtn->setObjectName(QStringLiteral("Danger"));
    auto *addBtn = new QPushButton(QStringLiteral("＋ 新增球员"), this);
    addBtn->setObjectName(QStringLiteral("Primary"));
    toolbar->addWidget(detailBtn);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(delBtn);
    toolbar->addWidget(addBtn);
    root->addLayout(toolbar);

    m_table = new QTableWidget(this);
    ui::setupTable(m_table, {QStringLiteral("编号"), QStringLiteral("姓名"),
                             QStringLiteral("年龄"), QStringLiteral("球队"),
                             QStringLiteral("出场"), QStringLiteral("总得分"),
                             QStringLiteral("三分"), QStringLiteral("篮板"),
                             QStringLiteral("扣篮"), QStringLiteral("抢断")});
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    root->addWidget(m_table, 1);

    connect(addBtn, &QPushButton::clicked, this, &PlayersPage::onAdd);
    connect(editBtn, &QPushButton::clicked, this, &PlayersPage::onEdit);
    connect(delBtn, &QPushButton::clicked, this, &PlayersPage::onDelete);
    connect(detailBtn, &QPushButton::clicked, this, &PlayersPage::onDetail);
    connect(m_table, &QTableWidget::doubleClicked, this, &PlayersPage::onDetail);
    connect(m_search, &QLineEdit::textChanged, this, &PlayersPage::applyFilter);
    connect(m_store, &DataStore::changed, this, &PlayersPage::refresh);

    refresh();
}

void PlayersPage::refresh()
{
    const auto &players = m_store->players();
    m_table->setRowCount(players.size());
    for (int r = 0; r < players.size(); ++r) {
        const Player &p = players.at(r);
        const PlayerStats t = m_store->careerTotals(p.id);
        const int games = m_store->playerMatchLog(p.id).size();

        m_table->setItem(r, 0, ui::item(p.id, Qt::AlignCenter, ui::gold()));
        m_table->setItem(r, 1, ui::item(p.name, Qt::AlignLeft));
        m_table->setItem(r, 2, ui::item(QString::number(p.age)));
        m_table->setItem(r, 3, ui::item(p.team, Qt::AlignLeft, ui::dim()));
        m_table->setItem(r, 4, ui::item(QString::number(games)));
        m_table->setItem(r, 5, ui::item(QString::number(t.points()), Qt::AlignCenter, ui::green()));
        m_table->setItem(r, 6, ui::item(QString::number(t.threePointers)));
        m_table->setItem(r, 7, ui::item(QString::number(t.rebounds)));
        m_table->setItem(r, 8, ui::item(QString::number(t.dunks)));
        m_table->setItem(r, 9, ui::item(QString::number(t.steals)));
    }
    applyFilter();
}

void PlayersPage::applyFilter()
{
    const QString key = m_search->text().trimmed();
    for (int r = 0; r < m_table->rowCount(); ++r) {
        bool match = key.isEmpty();
        for (int c = 0; c < m_table->columnCount() && !match; ++c) {
            if (m_table->item(r, c) && m_table->item(r, c)->text().contains(key, Qt::CaseInsensitive))
                match = true;
        }
        m_table->setRowHidden(r, !match);
    }
}

QString PlayersPage::selectedPlayerId() const
{
    const int row = m_table->currentRow();
    if (row < 0 || !m_table->item(row, 0))
        return {};
    return m_table->item(row, 0)->text();
}

void PlayersPage::onAdd()
{
    PlayerEditDialog dlg(m_store->teams(), this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    const Player p = dlg.player();
    if (m_store->playerExists(p.id)) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("球员编号 %1 已存在").arg(p.id));
        return;
    }
    if (!m_store->addPlayer(p))
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("新增失败"));
}

void PlayersPage::onEdit()
{
    const QString id = selectedPlayerId();
    if (id.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一名球员"));
        return;
    }
    PlayerEditDialog dlg(m_store->teams(), this);
    dlg.setPlayer(m_store->findPlayer(id));
    if (dlg.exec() != QDialog::Accepted)
        return;
    const Player p = dlg.player();
    if (p.id != id && m_store->playerExists(p.id)) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("球员编号 %1 已存在").arg(p.id));
        return;
    }
    if (!m_store->updatePlayer(id, p))
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("保存失败"));
}

void PlayersPage::onDelete()
{
    const QString id = selectedPlayerId();
    if (id.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一名球员"));
        return;
    }
    if (QMessageBox::question(this, QStringLiteral("确认"),
                              QStringLiteral("确定删除球员 %1 吗？其在所有场次中的数据也会被移除。").arg(id))
        != QMessageBox::Yes)
        return;
    m_store->removePlayer(id);
}

void PlayersPage::onDetail()
{
    const QString id = selectedPlayerId();
    if (id.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一名球员"));
        return;
    }
    emit playerDetailRequested(id);
}
