#include "ui/pages/MatchesPage.h"

#include "storage/DataStore.h"
#include "ui/dialogs/MatchEditDialog.h"
#include "ui/widgets/EmptyStateLabel.h"
#include "ui/widgets/UiUtils.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

MatchesPage::MatchesPage(DataStore *store, QWidget *parent)
    : QWidget(parent)
    , m_store(store)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("场次管理"), this);
    title->setObjectName(QStringLiteral("H1"));
    auto *sub = new QLabel(QStringLiteral("创建与维护比赛场次，双击可查看详情与球员数据"), this);
    sub->setObjectName(QStringLiteral("Muted"));
    root->addWidget(title);
    root->addWidget(sub);

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(10);
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(QStringLiteral("搜索编号 / 球队 / 地点…"));
    m_search->setClearButtonEnabled(true);
    m_search->setMinimumWidth(280);
    toolbar->addWidget(m_search);
    toolbar->addStretch();

    auto *detailBtn = new QPushButton(QStringLiteral("查看详情"), this);
    auto *editBtn = new QPushButton(QStringLiteral("编辑"), this);
    auto *delBtn = new QPushButton(QStringLiteral("删除"), this);
    delBtn->setObjectName(QStringLiteral("Danger"));
    auto *addBtn = new QPushButton(QStringLiteral("＋ 新增场次"), this);
    addBtn->setObjectName(QStringLiteral("Primary"));
    toolbar->addWidget(detailBtn);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(delBtn);
    toolbar->addWidget(addBtn);
    root->addLayout(toolbar);

    m_table = new QTableWidget(this);
    ui::setupTable(m_table, {QStringLiteral("编号"), QStringLiteral("比赛时间"),
                             QStringLiteral("地点"), QStringLiteral("球队一"),
                             QStringLiteral("球队二"), QStringLiteral("比分"),
                             QStringLiteral("胜者")});
    ui::autoSizeColumns(m_table, {2, 3, 4});
    for (int c = 0; c < m_table->columnCount(); ++c)
        ui::alignHeader(m_table, c, (c == 2 || c == 3 || c == 4 || c == 6) ? Qt::AlignLeft : Qt::AlignCenter);
    root->addWidget(m_table, 1);

    m_empty = new EmptyStateLabel(m_table, this);

    connect(addBtn, &QPushButton::clicked, this, &MatchesPage::onAdd);
    connect(editBtn, &QPushButton::clicked, this, &MatchesPage::onEdit);
    connect(delBtn, &QPushButton::clicked, this, &MatchesPage::onDelete);
    connect(detailBtn, &QPushButton::clicked, this, &MatchesPage::onDetail);
    connect(m_table, &QTableWidget::doubleClicked, this, &MatchesPage::onDetail);
    connect(m_search, &QLineEdit::textChanged, this, &MatchesPage::applyFilter);
    connect(m_store, &DataStore::changed, this, &MatchesPage::refresh);

    refresh();
}

void MatchesPage::refresh()
{
    const auto &matches = m_store->matches();
    ui::beginTableFill(m_table);
    m_table->setRowCount(matches.size());
    for (int r = 0; r < matches.size(); ++r) {
        const Match &m = matches.at(r);
        m_table->setItem(r, 0, ui::item(m.id, Qt::AlignCenter, ui::gold()));
        m_table->setItem(r, 1, ui::item(m.dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm"))));
        m_table->setItem(r, 2, ui::item(m.location, Qt::AlignLeft, ui::dim()));
        m_table->setItem(r, 3, ui::item(m.team1Name, Qt::AlignLeft));
        m_table->setItem(r, 4, ui::item(m.team2Name, Qt::AlignLeft));
        m_table->setItem(r, 5, ui::item(m.scoreText(), Qt::AlignCenter, ui::green()));
        m_table->setItem(r, 6, ui::item(m.winnerText(), Qt::AlignLeft));
    }
    ui::endTableFill(m_table, {2, 3, 4});
    applyFilter();
}

void MatchesPage::applyFilter()
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

    if (m_empty) {
        m_empty->setMessage(m_table->rowCount() == 0
                                ? QStringLiteral("暂无场次\n点击右上角「＋ 新增场次」开始录入")
                                : QStringLiteral("未找到匹配的场次\n试试更换编号 / 球队 / 地点关键词"));
        m_empty->refresh();
    }
}

QString MatchesPage::selectedMatchId() const
{
    const int row = m_table->currentRow();
    if (row < 0 || !m_table->item(row, 0))
        return {};
    return m_table->item(row, 0)->text();
}

void MatchesPage::onAdd()
{
    MatchEditDialog dlg(m_store->teams(), this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    const Match m = dlg.match();
    if (m_store->matchExists(m.id)) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("场次编号 %1 已存在").arg(m.id));
        return;
    }
    if (!m_store->addMatch(m))
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("新增失败"));
}

void MatchesPage::onEdit()
{
    const QString id = selectedMatchId();
    if (id.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一个场次"));
        return;
    }
    MatchEditDialog dlg(m_store->teams(), this);
    dlg.setMatch(m_store->findMatch(id));
    if (dlg.exec() != QDialog::Accepted)
        return;
    const Match m = dlg.match();
    if (m.id != id && m_store->matchExists(m.id)) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("场次编号 %1 已存在").arg(m.id));
        return;
    }
    if (!m_store->updateMatch(id, m))
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("保存失败"));
}

void MatchesPage::onDelete()
{
    const QString id = selectedMatchId();
    if (id.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一个场次"));
        return;
    }
    if (QMessageBox::question(this, QStringLiteral("确认"),
                              QStringLiteral("确定删除场次 %1 吗？该操作不可恢复。").arg(id))
        != QMessageBox::Yes)
        return;
    m_store->removeMatch(id);
}

void MatchesPage::onDetail()
{
    const QString id = selectedMatchId();
    if (id.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一个场次"));
        return;
    }
    emit matchDetailRequested(id);
}
