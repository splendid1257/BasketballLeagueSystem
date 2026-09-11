#include "ui/dialogs/MatchEditDialog.h"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

MatchEditDialog::MatchEditDialog(const QStringList &teams, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("场次信息"));
    setModal(true);
    setMinimumWidth(460);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(22, 20, 22, 18);
    root->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("新增场次"), this);
    title->setObjectName(QStringLiteral("H2"));
    m_titleLabel = title;
    root->addWidget(title);

    auto *form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_id = new QLineEdit(this);
    m_id->setPlaceholderText(QStringLiteral("例如 M007"));
    m_dt = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    m_dt->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm"));
    m_dt->setCalendarPopup(true);
    m_loc = new QLineEdit(this);
    m_loc->setPlaceholderText(QStringLiteral("比赛地点"));

    m_team1 = new QComboBox(this);
    m_team1->setEditable(true);
    m_team1->addItems(teams);
    m_team2 = new QComboBox(this);
    m_team2->setEditable(true);
    m_team2->addItems(teams);
    if (teams.size() > 1)
        m_team2->setCurrentIndex(1);

    form->addRow(QStringLiteral("场次编号"), m_id);
    form->addRow(QStringLiteral("比赛时间"), m_dt);
    form->addRow(QStringLiteral("比赛地点"), m_loc);
    form->addRow(QStringLiteral("球队一"), m_team1);
    form->addRow(QStringLiteral("球队二"), m_team2);
    root->addLayout(form);
    root->addStretch();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setObjectName(QStringLiteral("Primary"));
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("保存"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    connect(buttons, &QDialogButtonBox::accepted, this, &MatchEditDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MatchEditDialog::reject);
    root->addWidget(buttons);
}

void MatchEditDialog::setMatch(const Match &m)
{
    m_editMode = true;
    m_id->setText(m.id);
    m_dt->setDateTime(m.dateTime.isValid() ? m.dateTime : QDateTime::currentDateTime());
    m_loc->setText(m.location);

    auto selectOrEdit = [](QComboBox *box, const QString &text) {
        const int idx = box->findText(text);
        if (idx >= 0)
            box->setCurrentIndex(idx);
        else
            box->setEditText(text);
    };
    selectOrEdit(m_team1, m.team1Name);
    selectOrEdit(m_team2, m.team2Name);

    m_team1Players = m.team1Players;
    m_team2Players = m.team2Players;
    if (m_titleLabel)
        m_titleLabel->setText(QStringLiteral("编辑场次"));
}

Match MatchEditDialog::match() const
{
    Match m;
    m.id = m_id->text().trimmed();
    m.dateTime = m_dt->dateTime();
    m.location = m_loc->text().trimmed();
    m.team1Name = m_team1->currentText().trimmed();
    m.team2Name = m_team2->currentText().trimmed();
    m.team1Players = m_team1Players;
    m.team2Players = m_team2Players;
    return m;
}

void MatchEditDialog::accept()
{
    if (m_id->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写场次编号"));
        return;
    }
    if (m_team1->currentText().trimmed().isEmpty() || m_team2->currentText().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写两支球队名称"));
        return;
    }
    if (m_team1->currentText().trimmed() == m_team2->currentText().trimmed()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("两支球队不能相同"));
        return;
    }
    QDialog::accept();
}
