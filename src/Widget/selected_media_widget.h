#ifndef SELECTEDMEDIAWIDGET_H
#define SELECTEDMEDIAWIDGET_H

#include <QDialog>
#include <QTabWidget>

class SelectedMediaWidget : public QDialog
{
    Q_OBJECT
public:
    explicit SelectedMediaWidget(QWidget *parent = nullptr);

    // 切换
    void setCurrentIndex(int nIndex){
        m_tabWidget->setCurrentIndex(nIndex);
    }

private:
    QWidget* createFileTab();

signals:

private:
    QTabWidget *m_tabWidget;
};

#endif // SELECTEDMEDIAWIDGET_H
