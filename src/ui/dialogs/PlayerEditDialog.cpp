#include "ui/dialogs/PlayerEditDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

PlayerEditDialog::PlayerEditDialog(const QStringList &teams, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("球员信息"));
    setModal(true);
    setMinimumWidth(420);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(22, 20, 22, 18);
    root->setSpacing(14);

    auto *title = new QLabel(this);
    title->setObjectName(QStringLiteral("H2"));
    title->setText(QStringLiteral("新增球员"));
    m_titleLabel = title;
    root->addWidget(title);

    auto *form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_id = new QLineEdit(this);
    m_id->setPlaceholderText(QStringLiteral("例如 L001"));
    m_name = new QLineEdit(this);
    m_name->setPlaceholderText(QStringLiteral("球员姓名"));
    m_team = new QComboBox(this);
    m_team->setEditable(true);
    m_team->addItems(teams);
    m_number = new QSpinBox(this);
    m_number->setRange(0, 99);
    m_position = new QComboBox(this);
    m_position->setEditable(true);
    m_position->addItems({QStringLiteral("PG"), QStringLiteral("SG"), QStringLiteral("SF"),
                          QStringLiteral("PF"), QStringLiteral("C")});
    m_age = new QSpinBox(this);
    m_age->setRange(10, 80);
    m_age->setValue(20);
    m_height = new QSpinBox(this);
    m_height->setRange(150, 250);
    m_height->setSuffix(QStringLiteral(" cm"));
    m_height->setValue(195);
    m_weight = new QSpinBox(this);
    m_weight->setRange(50, 200);
    m_weight->setSuffix(QStringLiteral(" kg"));
    m_weight->setValue(90);
    m_country = new QLineEdit(this);
    m_country->setPlaceholderText(QStringLiteral("例如 美国"));

    form->addRow(QStringLiteral("编号"), m_id);
    form->addRow(QStringLiteral("姓名"), m_name);
    form->addRow(QStringLiteral("所属球队"), m_team);
    form->addRow(QStringLiteral("球衣号码"), m_number);
    form->addRow(QStringLiteral("场上位置"), m_position);
    form->addRow(QStringLiteral("年龄"), m_age);
    form->addRow(QStringLiteral("身高"), m_height);
    form->addRow(QStringLiteral("体重"), m_weight);
    form->addRow(QStringLiteral("国籍"), m_country);
    root->addLayout(form);
    root->addStretch();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setObjectName(QStringLiteral("Primary"));
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("保存"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    connect(buttons, &QDialogButtonBox::accepted, this, &PlayerEditDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &PlayerEditDialog::reject);
    root->addWidget(buttons);
}

void PlayerEditDialog::setPlayer(const Player &p)
{
    m_editMode = true;
    m_id->setText(p.id);
    m_name->setText(p.name);
    m_age->setValue(p.age);
    const int idx = m_team->findText(p.team);
    if (idx >= 0)
        m_team->setCurrentIndex(idx);
    else
        m_team->setEditText(p.team);
    m_number->setValue(p.number);
    const int posIdx = m_position->findText(p.position);
    if (posIdx >= 0)
        m_position->setCurrentIndex(posIdx);
    else
        m_position->setEditText(p.position);
    m_height->setValue(p.heightCm > 0 ? p.heightCm : 195);
    m_weight->setValue(p.weightKg > 0 ? p.weightKg : 90);
    m_country->setText(p.country);
    if (m_titleLabel)
        m_titleLabel->setText(QStringLiteral("编辑球员"));
}

Player PlayerEditDialog::player() const
{
    Player p;
    p.id = m_id->text().trimmed();
    p.name = m_name->text().trimmed();
    p.age = m_age->value();
    p.team = m_team->currentText().trimmed();
    p.number = m_number->value();
    p.position = m_position->currentText().trimmed();
    p.heightCm = m_height->value();
    p.weightKg = m_weight->value();
    p.country = m_country->text().trimmed();
    return p;
}

void PlayerEditDialog::accept()
{
    if (m_id->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写球员编号"));
        return;
    }
    if (m_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写球员姓名"));
        return;
    }
    QDialog::accept();
}
