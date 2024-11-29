#ifndef __SELECTED_MEDIA_WIDGET_H__
#define __SELECTED_MEDIA_WIDGET_H__

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

#endif // __SELECTED_MEDIA_WIDGET_H__
