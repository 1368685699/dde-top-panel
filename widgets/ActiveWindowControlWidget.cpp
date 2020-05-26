//
// Created by septemberhx on 2020/5/26.
//

#include <QDebug>
#include "ActiveWindowControlWidget.h"
#include "util/XUtils.h"

ActiveWindowControlWidget::ActiveWindowControlWidget(QWidget *parent)
    : QWidget(parent)
    , m_appInter(new DBusDock("com.deepin.dde.daemon.Dock", "/com/deepin/dde/daemon/Dock", QDBusConnection::sessionBus(), this))
{
    this->m_layout = new QHBoxLayout(this);
    this->setLayout(this->m_layout);

    this->closeButton = new QToolButton(this);
    this->closeButton->setIcon(QIcon::fromTheme("close"));
    this->m_layout->addWidget(this->closeButton);

    this->minButton = new QToolButton(this);
    this->minButton->setIcon(QIcon::fromTheme("minimum"));
    this->m_layout->addWidget(this->minButton);

    this->maxButton = new QToolButton(this);
    this->maxButton->setIcon(QIcon::fromTheme("maximum"));
    this->m_layout->addWidget(this->maxButton);

    this->m_winTitleLabel = new QLabel(this);
    this->m_winTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->m_layout->addWidget(this->m_winTitleLabel);

    this->m_layout->addStretch();

    this->setButtonsVisible(false);
    this->setMouseTracking(true);

    connect(this->maxButton, &QToolButton::clicked, this, &ActiveWindowControlWidget::maxButtonClicked);
}

void ActiveWindowControlWidget::activeWindowInfoChanged() {
    int activeWinId = XUtils::getFocusWindowId();
    if (activeWinId < 0) {
        qDebug() << "Failed to get active window id !";
        return;
    }

    if (activeWinId != this->currActiveWinId) {
        this->currActiveWinId = activeWinId;
        // todo
    }

    QString activeWinTitle = XUtils::getWindowName(activeWinId);
    if (activeWinTitle != this->currActiveWinTitle) {
        this->currActiveWinTitle = activeWinTitle;
        this->m_winTitleLabel->setText(this->currActiveWinTitle);
    }

    if (XUtils::checkIfWinMaximum(this->currActiveWinId)) {
        // todo
    }
}

void ActiveWindowControlWidget::setButtonsVisible(bool visible) {
    this->closeButton->setVisible(visible);
    this->minButton->setVisible(visible);
    this->maxButton->setVisible(visible);
}

void ActiveWindowControlWidget::enterEvent(QEvent *event) {
    if (XUtils::checkIfWinMaximum(this->currActiveWinId)) {
        this->setButtonsVisible(true);
    }
    QWidget::enterEvent(event);
}

void ActiveWindowControlWidget::leaveEvent(QEvent *event) {
    this->setButtonsVisible(false);
    QWidget::leaveEvent(event);
}

void ActiveWindowControlWidget::maxButtonClicked() {
    if (XUtils::checkIfWinMaximum(this->currActiveWinId)) {
        XUtils::unmaximizeWindow(this->currActiveWinId);

        // checkIfWinMaximum is MUST needed here.
        //   I don't know whether XSendEvent is designed like this,
        //   my test shows unmaximizeWindow by XSendEvent will work when others try to fetch its properties.
        //   i.e., checkIfWinMaximum
        XUtils::checkIfWinMaximum(this->currActiveWinId);
    } else {
        // sadly the dbus maximizeWindow cannot unmaximize window :(
        this->m_appInter->MaximizeWindow(this->currActiveWinId);
    }
}