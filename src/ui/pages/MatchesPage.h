#pragma once

#include <QWidget>

class DataStore;
class QTableWidget;
class QLineEdit;
class EmptyStateLabel;

class MatchesPage : public QWidget
{
    Q_OBJECT

public:
    explicit MatchesPage(DataStore *store, QWidget *parent = nullptr);

public slots:
    // 连接 DataStore::changed 的统一刷新槽：数据变化即重建表格
    void refresh();

signals:
    // 请求跳转到场次详情（参数为场次编号），导航职责留给主窗口
    void matchDetailRequested(const QString &matchId);

private:
    void onAdd();
    void onEdit();
    void onDelete();
    void onDetail();
    // 返回当前选中场次的编号，无选中时为空串
    QString selectedMatchId() const;
    void applyFilter();

    DataStore *m_store = nullptr;
    QTableWidget *m_table = nullptr;
    QLineEdit *m_search = nullptr;
    EmptyStateLabel *m_empty = nullptr;
};
