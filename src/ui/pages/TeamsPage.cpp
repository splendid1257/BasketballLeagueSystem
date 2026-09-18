#include "ui/pages/TeamsPage.h"

#include "storage/DataStore.h"
#include "ui/dialogs/PlayerEditDialog.h"
#include "ui/dialogs/TeamEditDialog.h"
#include "ui/widgets/UiUtils.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
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
    auto *sub = new QLabel(QStringLiteral("管理球队档案，选择球队查看该队全部球员信息"), this);
    sub->setObjectName(QStringLiteral("Muted"));
    root->addWidget(title);
    root->addWidget(sub);

    // 左右两栏：左栏固定宽度放球队列表，右栏自适应展示当前球队花名册
    auto *columns = new QHBoxLayout();
    columns->setSpacing(16);

    auto *leftCard = new QFrame(this);
    leftCard->setObjectName(QStringLiteral("Card"));
    leftCard->setFixedWidth(268);
    auto *lv = new QVBoxLayout(leftCard);
    lv->setContentsMargins(14, 14, 14, 14);
    lv->setSpacing(10);

    auto *lHead = new QHBoxLayout();
    auto *lTitle = new QLabel(QStringLiteral("球队"), leftCard);
    lTitle->setObjectName(QStringLiteral("H3"));
    auto *addBtn = new QPushButton(QStringLiteral("＋ 新增球队"), leftCard);
    addBtn->setObjectName(QStringLiteral("Primary"));
    addBtn->setCursor(Qt::PointingHandCursor);
    lHead->addWidget(lTitle);
    lHead->addStretch();
    lHead->addWidget(addBtn);
    lv->addLayout(lHead);

    m_teamList = new QListWidget(leftCard);
    m_teamList->setObjectName(QStringLiteral("TeamList"));
    lv->addWidget(m_teamList, 1);

    auto *lActions = new QHBoxLayout();
    lActions->setSpacing(8);
    auto *editBtn = new QPushButton(QStringLiteral("编辑球队"), leftCard);
    auto *delBtn = new QPushButton(QStringLiteral("删除球队"), leftCard);
    delBtn->setObjectName(QStringLiteral("Danger"));
    lActions->addWidget(editBtn);
    lActions->addWidget(delBtn);
    lv->addLayout(lActions);
    columns->addWidget(leftCard);

    auto *rightCard = new QFrame(this);
    rightCard->setObjectName(QStringLiteral("Card"));
    auto *rv = new QVBoxLayout(rightCard);
    rv->setContentsMargins(16, 14, 16, 14);
    rv->setSpacing(6);
    m_teamTitle = new QLabel(QStringLiteral("花名册"), rightCard);
    m_teamTitle->setObjectName(QStringLiteral("H2"));
    m_teamMeta = new QLabel(QStringLiteral("请选择左侧球队"), rightCard);
    m_teamMeta->setObjectName(QStringLiteral("Muted"));
    m_teamMeta->setWordWrap(true);
    m_roster = new QTableWidget(rightCard);
    ui::setupTable(m_roster, {QStringLiteral("编号"), QStringLiteral("姓名"),
                              QStringLiteral("号码"), QStringLiteral("位置"),
                              QStringLiteral("年龄"), QStringLiteral("身高(cm)"),
                              QStringLiteral("体重(kg)"), QStringLiteral("国籍"),
                              QStringLiteral("出场"), QStringLiteral("总得分")});
    for (int c = 0; c < m_roster->columnCount(); ++c)
        ui::alignHeader(m_roster, c, c == 1 ? Qt::AlignLeft : Qt::AlignCenter);
    ui::autoSizeColumns(m_roster, {1});
    rv->addWidget(m_teamMeta);
    rv->addSpacing(6);
    rv->addWidget(m_roster, 1);

    auto *rHead = new QHBoxLayout();
    rHead->addWidget(m_teamTitle);
    rHead->addStretch();
    auto *editPlayerBtn = new QPushButton(QStringLiteral("编辑球员"), rightCard);
    rHead->addWidget(editPlayerBtn);
    rv->insertLayout(0, rHead);
    connect(editPlayerBtn, &QPushButton::clicked, this, &TeamsPage::onEditPlayer);
    connect(m_roster, &QTableWidget::doubleClicked, this, &TeamsPage::onEditPlayer);
    columns->addWidget(rightCard, 1);

    root->addLayout(columns, 1);

    connect(m_teamList, &QListWidget::currentRowChanged, this, [this](int) { onTeamSelected(); });
    connect(addBtn, &QPushButton::clicked, this, &TeamsPage::onAddTeam);
    connect(editBtn, &QPushButton::clicked, this, &TeamsPage::onEditTeam);
    connect(delBtn, &QPushButton::clicked, this, &TeamsPage::onDeleteTeam);
    // 数据源变化即重建左侧球队列表，并尽量恢复原选中球队
    connect(m_store, &DataStore::changed, this, &TeamsPage::refresh);

    refresh();
}

// 重建球队列表并恢复选中：先用当前项文本做记号，清空重填后再找回原位置
void TeamsPage::refresh()
{
    const QString previous = m_teamList->currentItem() ? m_teamList->currentItem()->text() : QString();
    m_teamList->clear();

    for (const QString &team : m_store->teams()) {
        int count = 0;
        for (const Player &p : m_store->players())
            if (p.team == team)
                ++count;
        auto *it = new QListWidgetItem(
            QStringLiteral("%1   (%2)%3")
                .arg(team)
                .arg(count)
                .arg(count < 5 ? QStringLiteral("  ⚠") : QString()));
        // 显示文本混入了人数与警示符号，故将纯队名另存到 UserRole 供后续取用
        it->setData(Qt::UserRole, team);
        m_teamList->addItem(it);
    }

    // 无球队时的空态：清空花名册并提示创建
    if (m_teamList->count() == 0) {
        m_teamTitle->setText(QStringLiteral("花名册"));
        m_teamMeta->setText(QStringLiteral("暂无球队，点击左上角「＋ 新增球队」创建"));
        m_roster->setRowCount(0);
        return;
    }

    // 在重建后的列表中找回原选中行，未命中则保留默认的第 0 行
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

// 选中球队后聚合该队球员，计算生涯合计得分并渲染右侧花名册
void TeamsPage::onTeamSelected()
{
    auto *current = m_teamList->currentItem();
    if (!current) {
        m_roster->setRowCount(0);
        return;
    }
    const QString team = current->data(Qt::UserRole).toString();

    // 按球队筛选花名册成员（球员与球队通过队名字符串关联，而非外键 id）
    QVector<Player> roster;
    for (const Player &p : m_store->players())
        if (p.team == team)
            roster.append(p);

    m_teamTitle->setText(team);

    int totalPoints = 0;
    for (const Player &p : roster)
        totalPoints += m_store->careerTotals(p.id).points();

    const Team info = m_store->findTeam(team);
    // 仅拼接非空字段，避免出现无意义的空占位
    QStringList bits;
    if (!info.city.isEmpty())
        bits << QStringLiteral("城市 %1").arg(info.city);
    if (!info.coach.isEmpty())
        bits << QStringLiteral("主教练 %1").arg(info.coach);
    if (!info.arena.isEmpty())
        bits << QStringLiteral("主场 %1").arg(info.arena);
    bits << QStringLiteral("共 %1 名球员").arg(roster.size());
    bits << QStringLiteral("生涯合计 %1 分").arg(totalPoints);
    // 阵容不足 5 人时以警示色高亮元信息
    if (roster.size() < 5) {
        m_teamMeta->setStyleSheet(QStringLiteral("color:#FDB927;"));
        bits << QStringLiteral("⚠ 不足 5 人，建议补充至至少 5 人");
    } else {
        m_teamMeta->setStyleSheet(QString());
    }
    m_teamMeta->setText(bits.join(QStringLiteral(" · ")));

    m_roster->setRowCount(roster.size());
    // 批量填充期间关闭逐格自适应列宽，结束后统一计算
    ui::beginTableFill(m_roster);
    for (int r = 0; r < roster.size(); ++r) {
        const Player &p = roster.at(r);
        const PlayerStats t = m_store->careerTotals(p.id);
        const int games = m_store->playerGameCount(p.id);
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
    ui::endTableFill(m_roster, {1});
}

// 从右侧花名册取选中球员编号后编辑；编号改动时复查唯一性
void TeamsPage::onEditPlayer()
{
    const int row = m_roster->currentRow();
    if (row < 0 || !m_roster->item(row, 0)) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一名球员"));
        return;
    }
    const QString id = m_roster->item(row, 0)->text();
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

// 弹窗录入新球队；同名时放弃，成功写库后由 changed 信号刷新列表
void TeamsPage::onAddTeam()
{
    TeamEditDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    const Team t = dlg.team();
    if (m_store->teamExists(t.name)) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("球队「%1」已存在").arg(t.name));
        return;
    }
    if (!m_store->addTeam(t)) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("新增失败"));
        return;
    }
    // 创建成功后：阵容不足 5 人时给出提示（不阻止创建）
    int count = 0;
    for (const Player &p : m_store->players())
        if (p.team == t.name)
            ++count;
    if (count < 5) {
        QMessageBox::information(
            this, QStringLiteral("球队已创建"),
            QStringLiteral("球队「%1」已创建成功。\n\n"
                           "当前有 %2 名球员，不足 5 人。\n"
                           "建议到「球员管理」中新增或改派球员并选择该球队，"
                           "使阵容至少达到 5 人。")
                .arg(t.name)
                .arg(count));
    }
}

// 编辑球队；改名时按新名字复查唯一性
void TeamsPage::onEditTeam()
{
    auto *current = m_teamList->currentItem();
    if (!current) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一支球队"));
        return;
    }
    const QString name = current->data(Qt::UserRole).toString();
    TeamEditDialog dlg(this);
    dlg.setTeam(m_store->findTeam(name));
    if (dlg.exec() != QDialog::Accepted)
        return;
    const Team t = dlg.team();
    if (t.name != name && m_store->teamExists(t.name)) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("球队「%1」已存在").arg(t.name));
        return;
    }
    if (!m_store->updateTeam(name, t))
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("保存失败"));
}

// 删除前先查该队下是否仍有球员——有则拒绝删除，要求先改派或删除球员
void TeamsPage::onDeleteTeam()
{
    auto *current = m_teamList->currentItem();
    if (!current) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中一支球队"));
        return;
    }
    const QString name = current->data(Qt::UserRole).toString();

    int count = 0;
    for (const Player &p : m_store->players())
        if (p.team == name)
            ++count;
    if (count > 0) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("球队「%1」下还有 %2 名球员，请先在「球员管理」中改派或删除后再删除球队。")
                                 .arg(name)
                                 .arg(count));
        return;
    }
    if (QMessageBox::question(this, QStringLiteral("确认"),
                              QStringLiteral("确定删除球队「%1」吗？").arg(name))
        != QMessageBox::Yes)
        return;
    m_store->removeTeam(name);
}
