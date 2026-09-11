#include "ui/widgets/EmptyStateLabel.h"

#include <QEvent>
#include <QTableWidget>

EmptyStateLabel::EmptyStateLabel(QTableWidget *table, QWidget *parent)
    : QLabel(table->viewport())
    , m_table(table)
{
    Q_UNUSED(parent);
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

bool EmptyStateLabel::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_table->viewport() && event->type() == QEvent::Resize)
        setGeometry(m_table->viewport()->rect());
    return QLabel::eventFilter(obj, event);
}
