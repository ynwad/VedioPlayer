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

    QWidget* createNetworkTab();

    QWidget* createCaptureDevTab();

public slots:
    void on_BtnPlayClicked();

signals:
    void signal_playMedia(QStringList lstPath);

private:
    QTabWidget *m_tabWidget;

    QPushButton *playButton;
    QPushButton *cancelButton;

    QStringList m_lstWaitingList;
};

#endif // SELECTEDMEDIAWIDGET_H
