// kioskwindow.h
#ifndef KIOSKWINDOW_H
#define KIOSKWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>
#include <QProgressBar>
#include <QTimer>
#include <QListWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QDialog>
#include <QMouseEvent>
#include <QLineEdit>
#include <QCheckBox>
#include <QTcpSocket>
#include "kiosknet.h"

class MenuItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuItemWidget(const QString &name, const QString &price, const QString &imagePath, QWidget *parent = nullptr);

signals:
    void itemClicked(const QString &name, const QString &price, const QString &imagePath);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString itemName;
    QString itemPrice;
    QString itemImage;
};

class KioskWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit KioskWindow(QWidget *parent = nullptr);
    ~KioskWindow();

private slots:
    void startLoading();
    void finishLoading();
    void goToMainScreen();
    void goToPrePurchaseScreen();
    void goToPurchaseScreen();
    void goToPurchaseCardScreen();
    void goToPurchaseKakaoPayScreen();
    void goToFinalScreen();
    void goToConnectionScreen();
    void loadMenuItems(const QString &category);
    void removeSelectedItem();
    void showOrderPopup(const QString &itemName, const QString &itemPrice);
    void updateTotalPrice();
    void showRewardPopup();
    void showNumpadScreen();
    void showRewardUsePopup();
    void showCardInputPopup();
    void showWrongCardPopup();
    void showItemDetailPopup(const QString &itemName, const QString &itemPrice, const QString &itemImage);
    void updateFinalScreenCountdown();
    void addToOrderList(const QString &orderDetails);
    void updateAllOrderLists();

private:
    void setupUi();
    void createConnectionScreen();
    void createLoadingPopup();
    void createMainEntryScreen();
    void createPrePurchaseScreen();
    void createPurchaseScreen();
    void createPurchaseCardScreen();
    void createPurchaseKakaoPayScreen();
    void createFinalScreen();

    KioskNet kioskNet;
    QStackedWidget *stackedWidget;
    QWidget *connectionScreen;
    QWidget *mainEntryScreen;
    QWidget *prePurchaseScreen;
    QWidget *purchaseScreen;
    QWidget *purchaseCardScreen;
    QWidget *purchaseKakaoPayScreen;
    QWidget *finalScreen;
    QDialog *loadingPopup;
    QLabel *loadingLabel;
    QProgressBar *loadingBar;
    QPushButton *connectButton;
    QTimer *loadingTimer;
    QListWidget *orderListMain;
    QListWidget *orderListPrePurchase;
    QListWidget *orderListFinal;
    QScrollArea *menuScrollArea;
    QWidget *menuContainer;
    QGridLayout *menuGrid;
    QLabel *totalPriceLabel;
    QPushButton *backButton;
    QTimer *finalScreenTimer;
    int finalScreenCountdown;
};

#endif // KIOSKWINDOW_H
