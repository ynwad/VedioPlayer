#include "Widget/selected_media_widget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include "Base/define.h"

SelectedMediaWidget::SelectedMediaWidget(QWidget *parent)
    : QDialog{parent}{
    setWindowTitle(_ST("打开媒体"));
    setAttribute(Qt::WA_DeleteOnClose);

    // 创建 TabWidget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->insertTab(TableWidget_File_Index, createFileTab(), _ST("文件"));
    m_tabWidget->insertTab(TableWidget_Network_Index, new QWidget(), _ST("网络"));
    m_tabWidget->insertTab(TableWidget_Capture_Index, new QWidget(), _ST("捕获设备"));

    // 底部按钮
    QPushButton *playButton = new QPushButton(_ST("播放"));
    QPushButton *cancelButton = new QPushButton(_ST("取消"));
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(cancelButton);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addLayout(buttonLayout);
    show();
//    connect(playButton, &QPushButton::clicked, this, &SelectedMediaWidget::accept);
//    connect(cancelButton, &QPushButton::clicked, this, &SelectedMediaWidget::reject);

}

QWidget* SelectedMediaWidget::createFileTab() {
    QWidget *fileTab = new QWidget;
    fileTab->setAttribute(Qt::WA_DeleteOnClose);

    // 文件选择区域
    QListWidget *fileList = new QListWidget;
    QPushButton *addButton = new QPushButton(_ST("添加..."));
    QPushButton *removeButton = new QPushButton(_ST("移除"));

    QHBoxLayout *fileButtonLayout = new QHBoxLayout;
    fileButtonLayout->addWidget(addButton);
    fileButtonLayout->addWidget(removeButton);

    QVBoxLayout *fileListLayout = new QVBoxLayout;
    fileListLayout->addWidget(fileList);
    fileListLayout->addLayout(fileButtonLayout);

    // 使用字幕文件
    QCheckBox *useSubtitleCheck = new QCheckBox(_ST("使用字幕文件"));
    QLineEdit *subtitlePath = new QLineEdit;
    QPushButton *browseButton = new QPushButton(_ST("浏览..."));
    QHBoxLayout *subtitleLayout = new QHBoxLayout;
    subtitleLayout->addWidget(useSubtitleCheck);
    subtitleLayout->addWidget(subtitlePath);
    subtitleLayout->addWidget(browseButton);

    // 文件组布局
    QVBoxLayout *fileTabLayout = new QVBoxLayout(fileTab);
    fileTabLayout->addLayout(fileListLayout);
    fileTabLayout->addLayout(subtitleLayout);
    fileTabLayout->addStretch();

    // 添加按钮功能
    connect(addButton, &QPushButton::clicked, this, [fileList] {
        QString filePath = QFileDialog::getOpenFileName();
        if (!filePath.isEmpty()) {
            fileList->addItem(filePath);
        }
    });
    connect(removeButton, &QPushButton::clicked, fileList, [fileList] {
        delete fileList->currentItem();
    });

    return fileTab;
}
