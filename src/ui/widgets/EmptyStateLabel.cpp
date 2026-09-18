#include "ui/widgets/EmptyStateLabel.h"

#include <QEvent>
#include <QTableWidget>

EmptyStateLabel::EmptyStateLabel(QTableWidget *table, QWidget *parent)
    : QLabel(table->viewport())
    , m_table(table)
{
    Q_UNUSED(parent);
    // 作为表格视口子控件并设置鼠标穿透，可覆盖表格且不拦截交互
    setAlignment(Qt::AlignCenter);
    setWordWrap(true);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet(QStringLiteral("color:#8A97AC; font-size:15px; background:transparent; padding:0 24px;"));
    setGeometry(m_table->viewport()->rect());
    m_table->viewport()->installEventFilter(this);
    hide();
}

void EmptyStateLabel::setMessage(const QString &message)
{
    setText(message);
}

// 空态判定基于“可见行数”而非行数：筛选会隐藏行，仅当所有行都被隐藏才显示占位
void EmptyStateLabel::refresh()
{
    int visible = 0;
    for (int r = 0; r < m_table->rowCount(); ++r) {
        if (!m_table->isRowHidden(r))
            ++visible;
    }
    const bool empty = (visible == 0);
    setVisible(empty);
    if (empty) {
        setGeometry(m_table->viewport()->rect());
        raise();
    }
}

// 视口不参与布局，普通布局无法让标签跟随缩放，须监听视口 Resize 手动同步几何
bool EmptyStateLabel::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_table->viewport() && event->type() == QEvent::Resize)
        setGeometry(m_table->viewport()->rect());
    return QLabel::eventFilter(obj, event);
}
