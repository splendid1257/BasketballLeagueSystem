#pragma once

#include <QLabel>

class QTableWidget;

class EmptyStateLabel : public QLabel
{
    Q_OBJECT

public:
    explicit EmptyStateLabel(QTableWidget *table, QWidget *parent = nullptr);

    void setMessage(const QString &message);
    // 根据当前“可见行数”决定显示还是隐藏；每次筛选/刷新后调用
    void refresh();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QTableWidget *m_table = nullptr;
};
