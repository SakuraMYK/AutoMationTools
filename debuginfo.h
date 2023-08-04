#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include <QWidget>
#include <QShortcut>
#include <QDateTime>
#include <QIcon>

namespace Ui
{
    class DebugInfo;
}

class DebugInfo : public QWidget
{
    Q_OBJECT

public:
    explicit DebugInfo(QWidget *parent = nullptr);
    ~DebugInfo();
    void append(const QString &text);

private:
    Ui::DebugInfo *ui;
};

#endif // DEBUGINFO_H
