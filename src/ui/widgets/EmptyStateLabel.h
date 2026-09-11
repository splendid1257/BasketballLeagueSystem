#pragma once

#include <QLabel>

class QTableWidget;

// 表格空状态提示：当表格没有任何可见行时，在表格区域居中显示提示文字。
// 自动跟随表格视口大小变化，且不拦截鼠标事件。
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
