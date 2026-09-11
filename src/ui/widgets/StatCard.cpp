#include "ui/widgets/StatCard.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

StatCard::StatCard(const QString &title, const QString &accentColor, QWidget *parent)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("Card"));
    setMinimumHeight(104);

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *bar = new QFrame(this);
    bar->setFixedWidth(5);
    bar->setStyleSheet(QStringLiteral("background-color:%1;border-top-left-radius:12px;border-bottom-left-radius:12px;")
                           .arg(accentColor));
    root->addWidget(bar);

    auto *content = new QVBoxLayout();
    content->setContentsMargins(16, 14, 16, 14);
    content->setSpacing(2);

    m_title = new QLabel(title, this);
    m_title->setObjectName(QStringLiteral("CardTitle"));

    m_value = new QLabel(QStringLiteral("0"), this);
    m_value->setObjectName(QStringLiteral("CardValue"));

    m_sub = new QLabel(QString(), this);
    m_sub->setObjectName(QStringLiteral("Muted"));

    content->addWidget(m_title);
    content->addWidget(m_value);
    content->addStretch();
    content->addWidget(m_sub);
    root->addLayout(content, 1);
}

void StatCard::setValue(const QString &value)
{
    m_value->setText(value);
}

void StatCard::setTitle(const QString &title)
{
    m_title->setText(title);
}

void StatCard::setSubtitle(const QString &subtitle)
{
    m_sub->setText(subtitle);
}
