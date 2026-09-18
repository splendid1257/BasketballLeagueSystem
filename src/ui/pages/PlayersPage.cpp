#include "ui/pages/PlayersPage.h"

#include "storage/DataStore.h"
#include "ui/dialogs/PlayerEditDialog.h"
#include "ui/widgets/EmptyStateLabel.h"
#include "ui/widgets/UiUtils.h"

#include <QComboBox>
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
    root->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("球员管理"), this);
    title->setObjectName(QStringLiteral("H1"));
    auto *sub = new QLabel(QStringLiteral("维护球员档案，双击查看生涯数据与出场记录"), this);
    sub->setObjectName(QStringLiteral("Muted"));
    root->addWidget(title);
    root->addWidget(sub);

    auto *filters = new QHBoxLayout();
    filters->setSpacing(10);
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(QStringLiteral("搜索编号 / 姓名 / 球队 / 位置…"));
    m_search->setClearButtonEnabled(true);
    m_search->setMinimumWidth(260);
    filters->addWidget(m_search);

    m_teamFilter = new QComboBox(this);
    m_teamFilter->setMinimumWidth(150);
    m_positionFilter = new QComboBox(this);
    m_positionFilter->setMinimumWidth(110);
    filters->addWidget(m_teamFilter);
    filters->addWidget(m_positionFilter);
    filters->addStretch();
    root->addLayout(filters);

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(10);
    m_countLabel = new QLabel(this);
    m_countLabel->setObjectName(QStringLiteral("Muted"));
    toolbar->addWidget(m_countLabel);
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
                             QStringLiteral("球队"), QStringLiteral("号码"),
                             QStringLiteral("位置"), QStringLiteral("年龄"),
                             QStringLiteral("身高(cm)"), QStringLiteral("体重(kg)"),
                             QStringLiteral("国籍"), QStringLiteral("出场"),
                             QStringLiteral("得分")});
    for (int c = 0; c < m_table->columnCount(); ++c)
        ui::alignHeader(m_table, c, (c == 1 || c == 2) ? Qt::AlignLeft : Qt::AlignCenter);
    ui::autoSizeColumns(m_table, {1, 2});
    root->addWidget(m_table, 1);

    auto *pager = new QHBoxLayout();
    pager->setSpacing(10);
    pager->addStretch();
    m_prevBtn = new QPushButton(QStringLiteral("‹ 上一页"), this);
    m_prevBtn->setObjectName(QStringLiteral("Ghost"));
    m_nextBtn = new QPushButton(QStringLiteral("下一页 ›"), this);
    m_nextBtn->setObjectName(QStringLiteral("Ghost"));
    pager->addWidget(m_prevBtn);
    pager->addWidget(m_nextBtn);
    root->addLayout(pager);

    // 空态提示覆盖在表格之上，仅在结果集为空时可见
    m_empty = new EmptyStateLabel(m_table, this);

    connect(addBtn, &QPushButton::clicked, this, &PlayersPage::onAdd);
    connect(editBtn, &QPushButton::clicked, this, &PlayersPage::onEdit);
    connect(delBtn, &QPushButton::clicked, this, &PlayersPage::onDelete);
    connect(detailBtn, &QPushButton::clicked, this, &PlayersPage::onDetail);
    connect(m_table, &QTableWidget::doubleClicked, this, &PlayersPage::onDetail);
    connect(m_search, &QLineEdit::textChanged, this, &PlayersPage::applyFilter);
    connect(m_teamFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) { applyFilter(); });
    connect(m_positionFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) { applyFilter(); });
    // 上一页显式做下限保护；下一页不设上限，越界交给 renderPage 内的 qBound 钳制
    connect(m_prevBtn, &QPushButton::clicked, this, [this]() {
        if (m_page > 0) { --m_page; renderPage(); }
    });
    connect(m_nextBtn, &QPushButton::clicked, this, [this]() {
        ++m_page; renderPage();
    });
    // 数据源变化即走 refresh：重建筛选并重绘当前页，分页也随之重置
    connect(m_store, &DataStore::changed, this, &PlayersPage::refresh);

    refresh();
}

// 重建两个筛选下拉的选项并尽量恢复原选中项；期间屏蔽信号以免触发多余的过滤
void PlayersPage::rebuildFilters()
{
    const QString prevTeam = m_teamFilter->currentData().toString();
    const QString prevPos = m_positionFilter->currentData().toString();

    m_teamFilter->blockSignals(true);
    m_positionFilter->blockSignals(true);

    m_teamFilter->clear();
    m_teamFilter->addItem(QStringLiteral("全部球队"), QString());
    for (const QString &t : m_store->teams())
        m_teamFilter->addItem(t, t);

    QStringList positions;
    for (const Player &p : m_store->players()) {
        if (!p.position.isEmpty() && !positions.contains(p.position))
            positions.append(p.position);
    }
    positions.sort();
    m_positionFilter->clear();
    m_positionFilter->addItem(QStringLiteral("全部位置"), QString());
    for (const QString &pos : positions)
        m_positionFilter->addItem(pos, pos);

    const int ti = m_teamFilter->findData(prevTeam);
    if (ti >= 0)
        m_teamFilter->setCurrentIndex(ti);
    const int pi = m_positionFilter->findData(prevPos);
    if (pi >= 0)
        m_positionFilter->setCurrentIndex(pi);

    m_teamFilter->blockSignals(false);
    m_positionFilter->blockSignals(false);
}

// 数据变化后的统一入口：先重建筛选项再套用过滤
void PlayersPage::refresh()
{
    rebuildFilters();
    applyFilter();
}

// 按关键词+球队+位置三重条件筛出内存候选集，并重置到第 0 页
void PlayersPage::applyFilter()
{
    const QString key = m_search->text().trimmed();
    const QString team = m_teamFilter->currentData().toString();
    const QString pos = m_positionFilter->currentData().toString();

    m_filtered.clear();
    for (const Player &p : m_store->players()) {
        if (!team.isEmpty() && p.team != team)
            continue;
        if (!pos.isEmpty() && p.position != pos)
            continue;
        if (!key.isEmpty()) {
            // 多个字段拼成单一字符串后做不区分大小写的包含匹配
            const QString hay = QStringLiteral("%1 %2 %3 %4 %5 %6")
                                    .arg(p.id, p.name, p.team, p.position, p.country,
                                         QString::number(p.number));
            if (!hay.contains(key, Qt::CaseInsensitive))
                continue;
        }
        m_filtered.append(p);
    }
    m_page = 0;
    renderPage();
}

// 按当前页从候选集切出一页渲染到表格，并更新计数与翻页按钮状态
void PlayersPage::renderPage()
{
    const int total = m_filtered.size();
    // 向上取整求总页数，且至少 1 页，避免空结果时除零
    const int pageCount = qMax(1, (total + m_pageSize - 1) / m_pageSize);
    m_page = qBound(0, m_page, pageCount - 1);

    const int start = m_page * m_pageSize;
    const int rows = qMin(m_pageSize, total - start);

    m_table->setRowCount(qMax(0, rows));
    // 批量填充期间关闭逐格自适应列宽，结束后统一计算
    ui::beginTableFill(m_table);
    for (int i = 0; i < rows; ++i) {
        const Player &p = m_filtered.at(start + i);
        const PlayerStats t = m_store->careerTotals(p.id);
        const int games = m_store->playerGameCount(p.id);

        m_table->setItem(i, 0, ui::item(p.id, Qt::AlignCenter, ui::gold()));
        m_table->setItem(i, 1, ui::item(p.name, Qt::AlignLeft));
        m_table->setItem(i, 2, ui::item(p.team, Qt::AlignLeft, ui::dim()));
        // 号码/身高/体重等为 0 或空视为未填写，统一显示「-」
        m_table->setItem(i, 3, ui::item(p.number > 0 ? QString::number(p.number) : QStringLiteral("-")));
        m_table->setItem(i, 4, ui::item(p.position.isEmpty() ? QStringLiteral("-") : p.position));
        m_table->setItem(i, 5, ui::item(QString::number(p.age)));
        m_table->setItem(i, 6, ui::item(p.heightCm > 0 ? QString::number(p.heightCm) : QStringLiteral("-")));
        m_table->setItem(i, 7, ui::item(p.weightKg > 0 ? QString::number(p.weightKg) : QStringLiteral("-")));
        m_table->setItem(i, 8, ui::item(p.country.isEmpty() ? QStringLiteral("-") : p.country,
                                        Qt::AlignCenter, ui::dim()));
        m_table->setItem(i, 9, ui::item(QString::number(games)));
        m_table->setItem(i, 10, ui::item(QString::number(t.points()), Qt::AlignCenter, ui::green()));
    }
    ui::endTableFill(m_table, {1, 2});

    m_countLabel->setText(QStringLiteral("共 %1 名球员 · 第 %2/%3 页")
                              .arg(total)
                              .arg(m_page + 1)
                              .arg(pageCount));
    m_prevBtn->setEnabled(m_page > 0);
    m_nextBtn->setEnabled(m_page + 1 < pageCount);

    if (total == 0) {
        // 区分「无数据」与「筛选后无结果」，给出不同的空态文案
        const bool filtered = !m_search->text().trimmed().isEmpty()
                              || !m_teamFilter->currentData().toString().isEmpty()
                              || !m_positionFilter->currentData().toString().isEmpty();
        m_empty->setMessage(filtered
                                ? QStringLiteral("未找到匹配的球员\n试试更换关键词，或调整球队 / 位置筛选")
                                : QStringLiteral("暂无球员\n点击右上角「＋ 新增球员」开始录入"));
    }
    m_empty->refresh();
}

// 返回选中行第 0 列的编号；行号随分页/过滤漂移，故以 id 而非行号定位数据
QString PlayersPage::selectedPlayerId() const
{
    const int row = m_table->currentRow();
    if (row < 0 || !m_table->item(row, 0))
        return {};
    return m_table->item(row, 0)->text();
}

// 弹窗录入新球员；编号重复时放弃，成功写库后由 changed 信号刷新界面
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

// 编辑后编号若被改动，需按新编号复查唯一性
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

// 删除前二次确认，并明确提示会连带移除该球员在所有场次中的数据
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

// 向上层发信号请求详情，导航职责留给主窗口
void PlayersPage::onDetail()
{
    const QString id = selectedPlayerId();
    if (id.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一名球员"));
        return;
    }
    emit playerDetailRequested(id);
}
