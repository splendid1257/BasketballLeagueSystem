#pragma once

#include <QFrame>

class QLabel;

// 数据概览卡片：左侧彩色竖条 + 标题 + 大数值 + 副标题
class StatCard : public QFrame
{
    Q_OBJECT

public:
    explicit StatCard(const QString &title,
                      const QString &accentColor = QStringLiteral("#C8102E"),
                      QWidget *parent = nullptr);

    void setValue(const QString &value);
    void setTitle(const QString &title);
    void setSubtitle(const QString &subtitle);

private:
    QLabel *m_title = nullptr;
    QLabel *m_value = nullptr;
    QLabel *m_sub = nullptr;
};
