#pragma once

#include <QFrame>

class QLabel;

// 指标卡片：强调色条 + 标题/数值/副标题，用于概览页与详情页的数据磁贴
class StatCard : public QFrame
{
    Q_OBJECT

public:
    explicit StatCard(const QString &title,
                      const QString &accentColor = QStringLiteral("#C8102E"),
                      QWidget *parent = nullptr);

    // 数值仅通过 setter 更新，内部 QLabel 不对外暴露
    void setValue(const QString &value);
    void setTitle(const QString &title);
    void setSubtitle(const QString &subtitle);

private:
    QLabel *m_title = nullptr;
    QLabel *m_value = nullptr;
    QLabel *m_sub = nullptr;
};
