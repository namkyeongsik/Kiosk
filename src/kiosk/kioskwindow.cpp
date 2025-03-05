// kioskwindow.cpp
#include "kioskwindow.h"
#include <QPixmap>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QMouseEvent>
#include <QMainWindow>

QStringList orderListData;

void KioskWindow::addToOrderList(const QString &orderDetails) {
    orderListData.append(orderDetails);
    updateAllOrderLists();
}

void applyButtonStyle(QPushButton *button) {
    if (button) {
        button->setStyleSheet("background-color: #036B3F; color: white; font-size: 16px; border-radius: 8px; padding: 5px;");
    }
}

void KioskWindow::updateAllOrderLists() {
    orderListMain->clear();
    orderListPrePurchase->clear();
    orderListFinal->clear();

    for (const QString &item : orderListData) {
        orderListMain->addItem(item);
        orderListPrePurchase->addItem(item);
        orderListFinal->addItem(item);
    }
    updateTotalPrice();
}

KioskWindow::KioskWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
}

KioskWindow::~KioskWindow() {}

MenuItemWidget::MenuItemWidget(const QString &name, const QString &price, const QString &imagePath, QWidget *parent)
    : QWidget(parent), itemName(name), itemPrice(price), itemImage(imagePath) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *imageLabel = new QLabel();
    QLabel *nameLabel = new QLabel(name);
    QLabel *priceLabel = new QLabel(price);

    imageLabel->setPixmap(QPixmap(imagePath).scaled(80, 80, Qt::KeepAspectRatio));
    nameLabel->setStyleSheet(" font-size: 15px; width: 100%; ");
    priceLabel->setStyleSheet(" font-size: 13px; color: #21BA31; width: 100%; ");
    nameLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(imageLabel);
    layout->addWidget(nameLabel);
    layout->addWidget(priceLabel);
    setLayout(layout);
}

void MenuItemWidget::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    emit itemClicked(itemName, itemPrice, itemImage);
}

void KioskWindow::setupUi() {
    setFixedSize(420, 760);
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setStyleSheet("background-color: white; border-radius: 15px;");

    orderListMain = new QListWidget();
    orderListPrePurchase = new QListWidget();
    orderListFinal = new QListWidget();

    createConnectionScreen();
    createLoadingPopup();
    createMainEntryScreen();
    createPrePurchaseScreen();
    createPurchaseScreen();
    createPurchaseCardScreen();
    createPurchaseKakaoPayScreen();
    createFinalScreen();

    stackedWidget->setCurrentWidget(connectionScreen);
}

void KioskWindow::createPurchaseCardScreen() {
    purchaseCardScreen = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(purchaseCardScreen);

    QLabel *titleLabel = new QLabel("카드 결제", purchaseCardScreen);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QPushButton *enterCardButton = new QPushButton("💳 카드 번호 입력", purchaseCardScreen);
    enterCardButton->setMinimumHeight(50);
    applyButtonStyle(enterCardButton);

    connect(enterCardButton, &QPushButton::clicked, this, &KioskWindow::showCardInputPopup);
    layout->addWidget(enterCardButton);

    QPushButton *backButton = new QPushButton("🔙 뒤로가기", purchaseCardScreen);
    backButton->setMinimumHeight(50);
    applyButtonStyle(backButton);

    connect(backButton, &QPushButton::clicked, this, &KioskWindow::goToPurchaseScreen);
    layout->addWidget(backButton);

    purchaseCardScreen->setLayout(layout);
    stackedWidget->addWidget(purchaseCardScreen);
}

void KioskWindow::createConnectionScreen() {
    connectionScreen = new QWidget();
    QVBoxLayout *connLayout = new QVBoxLayout(connectionScreen);
    QLabel *imageLabel = new QLabel(connectionScreen);
    QPixmap image("../asset/banner.jpg");
    imageLabel->setPixmap(image);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setScaledContents(true);
    connectButton = new QPushButton("연결하기", connectionScreen);
    connectButton->setStyleSheet("background-color: #036B3F; color: white; height: 50px; font-size: 20px; border-radius: 7px;");
    connect(connectButton, &QPushButton::clicked, this, &KioskWindow::startLoading);
    connLayout->addWidget(imageLabel);
    connLayout->addWidget(connectButton);
    connectionScreen->setLayout(connLayout);
    stackedWidget->addWidget(connectionScreen);
}

void KioskWindow::createLoadingPopup() {
    loadingPopup = new QDialog(this, Qt::Dialog | Qt::FramelessWindowHint);
    loadingPopup->setFixedSize(300, 150);
    loadingPopup->setStyleSheet("background-color: white; border-radius: 15px;");

    QVBoxLayout *popupLayout = new QVBoxLayout(loadingPopup);

    QLabel *loadingLabel = new QLabel("로딩 중...", loadingPopup);
    loadingLabel->setAlignment(Qt::AlignCenter);
    loadingLabel->setStyleSheet("font-size: 22px;");

    QProgressBar *loadingBar = new QProgressBar(loadingPopup);
    loadingBar->setRange(0, 100);
    loadingBar->setValue(50);

    loadingBar->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid #036B3F;
            border-radius: 10px;
            background-color: #E0E0E0;
            text-align: center;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: #036B3F;
            border-radius: 8px; /* Round only the left and right ends */
            margin: 0px; /* Remove any spacing between chunks */
        }
    )");

    popupLayout->addWidget(loadingLabel);
    popupLayout->addWidget(loadingBar);
    loadingPopup->setLayout(popupLayout);
}

void KioskWindow::startLoading() {
    kioskNet.connectToPOS();
    connectButton->setEnabled(false);
    loadingPopup->show();
    loadingTimer = new QTimer(this);
    connect(loadingTimer, &QTimer::timeout, this, &KioskWindow::finishLoading);
    loadingTimer->start(2000);
}

void KioskWindow::finishLoading() {
    loadingPopup->hide();
    connectButton->setEnabled(true);
    stackedWidget->setCurrentWidget(mainEntryScreen);
}

void KioskWindow::showOrderPopup(const QString &itemName, const QString &itemPrice) {
    QDialog popup(this);
    popup.setWindowTitle("메뉴 추가");
    popup.setFixedSize(250, 150);

    QVBoxLayout *layout = new QVBoxLayout(&popup);
    QLabel *message = new QLabel(itemName + " - " + itemPrice, &popup);
    QPushButton *confirmButton = new QPushButton("확인", &popup);

    layout->addWidget(message);
    layout->addWidget(confirmButton);

    connect(confirmButton, &QPushButton::clicked, &popup, [&]() {
        addToOrderList(itemName + " - " + itemPrice);
        popup.accept();
    });


    popup.exec();
}

void KioskWindow::createMainEntryScreen() {
    mainEntryScreen = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainEntryScreen);

    QWidget *topMenuBar = new QWidget();
    QHBoxLayout *menuLayout = new QHBoxLayout(topMenuBar);

    QPushButton *coffeeButton = new QPushButton("☕ 커피", topMenuBar);
    QPushButton *beverageButton = new QPushButton("🥤 음료", topMenuBar);
    QPushButton *cakeButton = new QPushButton("🍰 케이크", topMenuBar);
    menuLayout->addWidget(coffeeButton);
    menuLayout->addWidget(beverageButton);
    menuLayout->addWidget(cakeButton);

    coffeeButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 16px; border-radius: 8px; padding: 5px;");
    beverageButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 16px; border-radius: 8px; padding: 5px;");
    cakeButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 16px; border-radius: 8px; padding: 5px;");

    topMenuBar->setLayout(menuLayout);

    menuScrollArea = new QScrollArea();
    menuContainer = new QWidget();
    menuGrid = new QGridLayout(menuContainer);

    loadMenuItems("coffee");

    menuContainer->setLayout(menuGrid);
    menuScrollArea->setWidget(menuContainer);
    menuScrollArea->setWidgetResizable(true);

    QVBoxLayout *orderLayout = new QVBoxLayout();
    QLabel *orderLabel = new QLabel("🛒 주문 목록");
    orderLabel->setStyleSheet("font-size: 16px; font-weight: bold; text-align: center;");

    QScrollArea *orderScrollArea = new QScrollArea();
    orderListMain = new QListWidget();
    orderLabel->setFixedHeight(30);
    orderListMain->setFixedHeight(100);
    orderListMain->setStyleSheet("font-size: 14px; background-color: #f8f8f8; border-radius: 5px; color: black;");
    orderScrollArea->setWidget(orderListMain);
    orderScrollArea->setWidgetResizable(true);
    orderScrollArea->setFixedHeight(100);

    QPushButton *removeItemButton = new QPushButton("🗑 선택 항목 삭제");
    removeItemButton->setStyleSheet("background-color: #FF6F61; color: white; font-size: 14px; height: 35px; border-radius: 8px;");
    connect(removeItemButton, &QPushButton::clicked, this, &KioskWindow::removeSelectedItem);

    QPushButton *purchaseButton = new QPushButton("💳 구매하기");
    purchaseButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 16px; height: 40px; border-radius: 8px;");
    connect(purchaseButton, &QPushButton::clicked, this, &KioskWindow::goToPrePurchaseScreen);

    orderLayout->addWidget(orderLabel);
    orderLayout->addWidget(orderScrollArea);
    orderLayout->addWidget(removeItemButton);
    orderLayout->addWidget(purchaseButton);

    mainLayout->addWidget(topMenuBar);
    mainLayout->addWidget(menuScrollArea);
    mainLayout->addLayout(orderLayout);

    mainEntryScreen->setLayout(mainLayout);
    stackedWidget->addWidget(mainEntryScreen);

    connect(coffeeButton, &QPushButton::clicked, this, [=]() { loadMenuItems("coffee"); });
    connect(beverageButton, &QPushButton::clicked, this, [=]() { loadMenuItems("beverage"); });
    connect(cakeButton, &QPushButton::clicked, this, [=]() { loadMenuItems("cake"); });
}

void KioskWindow::loadMenuItems(const QString &category) {
    QLayoutItem *child;
    while ((child = menuGrid->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QStringList itemNames, itemPrices, itemImages;

    if (category == "coffee") {
        itemNames = {"아메리카노", "카페라떼", "카푸치노", "에스프레소", "모카", "바닐라라떼"};
        itemPrices = {"3000원", "4000원", "4500원", "2500원", "5000원", "4500원"};
        itemImages = {"../asset/coffee1.jpg", "../asset/cafelatte.jpg", "../asset/cappuccino.jpg", "../asset/Espresso.jpg", "../asset/Mocha.jpg", "../asset/Vanilla.jpg"};
    } else if (category == "beverage") {
        itemNames = {"오렌지 주스", "레몬에이드", "청포도 주스", "딸기 스무디"};
        itemPrices = {"3500원", "4000원", "3800원", "4500원"};
        itemImages = {"../asset/Orange.jpg", "../asset/Lemon.jpg", "../asset/Muscat.jpg", "../asset/Strawberry.jpg"};
    } else if (category == "cake") {
        itemNames = {"치즈 케이크", "초코 케이크", "티라미수", "생크림 케이크"};
        itemPrices = {"5000원", "5500원", "6000원", "5800원"};
        itemImages = {"../asset/cheese.jpg", "../asset/choco.jpg", "../asset/Tiramisu.jpg", "../asset/cream.jpg"};
    }

    for (int i = 0; i < itemNames.size(); ++i) {
        MenuItemWidget *itemWidget = new MenuItemWidget(itemNames[i], itemPrices[i], itemImages[i]);
        connect(itemWidget, &MenuItemWidget::itemClicked, this, [=]() {
            showItemDetailPopup(itemNames[i], itemPrices[i], itemImages[i]);
        });
        menuGrid->addWidget(itemWidget, i / 3, i % 3);
    }
}

void KioskWindow::removeSelectedItem() {
    QListWidgetItem *selectedItem = orderListMain->currentItem();

    if (selectedItem) {
        QString itemText = selectedItem->text();

        int indexToRemove = orderListData.indexOf(itemText);
        if (indexToRemove != -1) {
            orderListData.removeAt(indexToRemove);
        }

        delete orderListMain->takeItem(orderListMain->row(selectedItem));

        updateAllOrderLists();
        updateTotalPrice();
    }
}

void KioskWindow::showItemDetailPopup(const QString &itemName, const QString &itemPrice, const QString &itemImage) {
    QDialog itemPopup(this);
    itemPopup.setWindowTitle("상품 정보");
    itemPopup.setFixedSize(300, 400);

    QVBoxLayout *popupLayout = new QVBoxLayout(&itemPopup);

    QLabel *imageLabel = new QLabel(&itemPopup);
    imageLabel->setPixmap(QPixmap(itemImage).scaled(150, 150, Qt::KeepAspectRatio));

    QLabel *nameLabel = new QLabel(itemName, &itemPopup);
    nameLabel->setStyleSheet("font-size: 18px; font-weight: bold; text-align: center;");

    QLabel *priceLabel = new QLabel("가격: " + itemPrice, &itemPopup);
    priceLabel->setStyleSheet("font-size: 16px; color: #21BA31; text-align: center;");

    QLabel *descriptionLabel = new QLabel("이 상품에 대한 설명이 여기에 들어갑니다.", &itemPopup);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setStyleSheet("font-size: 14px; color: #555; text-align: center;");

    QFrame *separator = new QFrame(&itemPopup);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("background-color: #ccc; height: 1px; margin: 10px 0;");

    QLabel *optionsLabel = new QLabel("추가 옵션 선택:", &itemPopup);
    optionsLabel->setStyleSheet("font-size: 16px; font-weight: bold; text-align: center;");

    QCheckBox *option1 = new QCheckBox("샷 추가 (+500원)", &itemPopup);
    QCheckBox *option2 = new QCheckBox("휘핑 크림 추가 (+300원)", &itemPopup);
    QCheckBox *option3 = new QCheckBox("아이스 변경", &itemPopup);

    QPushButton *addButton = new QPushButton("주문 담기", &itemPopup);
    addButton->setStyleSheet("background-color: #FF6F61; color: white; font-size: 16px; height: 40px; border-radius: 8px;");

    connect(addButton, &QPushButton::clicked, &itemPopup, [&]() {
        QString orderDetails = itemName + " - " + itemPrice;
        if (option1->isChecked()) orderDetails += " + 샷 추가";
        if (option2->isChecked()) orderDetails += " + 휘핑 크림";
        if (option3->isChecked()) orderDetails += " + 아이스 변경";

        addToOrderList(orderDetails);
        itemPopup.accept();
    });

    popupLayout->addWidget(imageLabel);
    popupLayout->addWidget(nameLabel);
    popupLayout->addWidget(priceLabel);
    popupLayout->addWidget(descriptionLabel);
    popupLayout->addWidget(separator);
    popupLayout->addWidget(optionsLabel);
    popupLayout->addWidget(option1);
    popupLayout->addWidget(option2);
    popupLayout->addWidget(option3);
    popupLayout->addWidget(addButton);

    itemPopup.setLayout(popupLayout);
    itemPopup.exec();
}

void KioskWindow::updateTotalPrice() {
    int total = 0;
    for (const QString &item : orderListData) {
        QString priceStr = item.split('-').last().trimmed().replace("원", "");
        total += priceStr.toInt();
    }
    totalPriceLabel->setText("총 가격: " + QString::number(total) + "원");
}

void KioskWindow::createPrePurchaseScreen() {
    prePurchaseScreen = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(prePurchaseScreen);

    QLabel *titleLabel = new QLabel("주문 확인", prePurchaseScreen);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    orderListPrePurchase = new QListWidget(prePurchaseScreen);
    orderListPrePurchase->setStyleSheet("font-size: 14px; background-color: #f8f8f8; border-radius: 5px; color: black;");
    layout->addWidget(orderListPrePurchase);

    totalPriceLabel = new QLabel("총 가격: 0원", prePurchaseScreen);
    totalPriceLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(totalPriceLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *backButton = new QPushButton("🔙 돌아가기", prePurchaseScreen);
    QPushButton *takeoutButton = new QPushButton("🛍 포장하기", prePurchaseScreen);
    QPushButton *dineInButton = new QPushButton("🍽 매장이용", prePurchaseScreen);


    backButton->setMinimumHeight(100);
    takeoutButton->setMinimumHeight(100);
    dineInButton->setMinimumHeight(100);

    applyButtonStyle(backButton);
    applyButtonStyle(takeoutButton);
    applyButtonStyle(dineInButton);

    connect(backButton, &QPushButton::clicked, this, &KioskWindow::goToMainScreen);
    connect(takeoutButton, &QPushButton::clicked, this, &KioskWindow::showRewardPopup);
    connect(dineInButton, &QPushButton::clicked, this, &KioskWindow::showRewardPopup);

    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(takeoutButton);
    buttonLayout->addWidget(dineInButton);

    layout->addLayout(buttonLayout);

    prePurchaseScreen->setLayout(layout);
    stackedWidget->addWidget(prePurchaseScreen);
}

void KioskWindow::showRewardPopup() {
    QDialog popup(this);
    popup.setWindowTitle("포인트 적립");
    popup.setFixedSize(250, 150);

    QVBoxLayout layout(&popup);

    QLabel message("포인트를 적립하시겠습니까?", &popup);
    message.setAlignment(Qt::AlignCenter);
    message.setStyleSheet("font-size: 16px; color: black;");


    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton skipButton("그냥 결제", &popup);
    QPushButton rewardButton("포인트 적립", &popup);

    skipButton.setMinimumHeight(50);
    rewardButton.setMinimumHeight(50);

    applyButtonStyle(&skipButton);
    applyButtonStyle(&rewardButton);


    buttonLayout->addWidget(&rewardButton);
    buttonLayout->addWidget(&skipButton);

    layout.addWidget(&message);
    layout.addLayout(buttonLayout);

    connect(&skipButton, &QPushButton::clicked, &popup, [&]() {
        popup.accept();
        goToPurchaseScreen();
    });
    connect(&rewardButton, &QPushButton::clicked, &popup, [&]() {
        popup.accept();
        showNumpadScreen();
    });

    popup.setLayout(&layout);
    popup.exec();
}

void KioskWindow::showNumpadScreen() {
    QDialog numpadPopup(this);
    numpadPopup.setWindowTitle("전화번호 입력");
    numpadPopup.setFixedSize(300, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(&numpadPopup);

    QLabel *message = new QLabel("전화번호를 입력하세요", &numpadPopup);
    message->setAlignment(Qt::AlignCenter);
    message->setStyleSheet("font-size: 16px; color: black;");
    mainLayout->addWidget(message);

    QLineEdit *phoneInput = new QLineEdit(&numpadPopup);
    phoneInput->setReadOnly(true);
    phoneInput->setAlignment(Qt::AlignCenter);
    phoneInput->setStyleSheet("font-size: 18px; padding: 5px; border: 2px solid #036B3F; border-radius: 5px;");
    mainLayout->addWidget(phoneInput);

    QGridLayout *numpadLayout = new QGridLayout();
    QPushButton *buttons[12];
    QString labels[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "←", "0", "확인"};

    auto updateInput = [&](const QString &text) {
        phoneInput->setText(phoneInput->text() + text);
    };

    for (int i = 0; i < 12; ++i) {
        buttons[i] = new QPushButton(labels[i], &numpadPopup);
        buttons[i]->setFixedSize(80, 60);
        applyButtonStyle(buttons[i]);
        numpadLayout->addWidget(buttons[i], i / 3, i % 3);

        if (i != 9 && i != 11) {
            connect(buttons[i], &QPushButton::clicked, this, [=]() {
                if (phoneInput->text().length() < 11) {
                    updateInput(labels[i]);
                }
            });
        }
    }
    connect(buttons[9], &QPushButton::clicked, this, [&]() {
        QString text = phoneInput->text();
        if (!text.isEmpty()) {
            phoneInput->setText(text.left(text.length() - 1));
        }
    });

    connect(buttons[11], &QPushButton::clicked, this, [&]() {
        numpadPopup.accept();
        showRewardUsePopup();
    });

    mainLayout->addLayout(numpadLayout);
    numpadPopup.setLayout(mainLayout);
    numpadPopup.exec();
}

void KioskWindow::showRewardUsePopup() {
    QDialog rewardPopup(this);
    rewardPopup.setWindowTitle("포인트 사용");
    rewardPopup.setFixedSize(300, 400);
    rewardPopup.setStyleSheet("background-color: white; border-radius: 15px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&rewardPopup);

    QLabel *ownedPointsLabel = new QLabel("소유 포인트: 1000p", &rewardPopup);
    ownedPointsLabel->setAlignment(Qt::AlignCenter);
    ownedPointsLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #036B3F;");
    mainLayout->addWidget(ownedPointsLabel);

    QLabel *message = new QLabel("사용할 포인트 입력:", &rewardPopup);
    message->setAlignment(Qt::AlignCenter);
    message->setStyleSheet("font-size: 16px; color: black;");
    mainLayout->addWidget(message);

    QLineEdit *pointInput = new QLineEdit(&rewardPopup);
    pointInput->setReadOnly(true);
    pointInput->setAlignment(Qt::AlignCenter);
    pointInput->setStyleSheet("font-size: 18px; padding: 5px; border: 2px solid #036B3F; border-radius: 5px;");
    mainLayout->addWidget(pointInput);

    QGridLayout *numpadLayout = new QGridLayout();
    QPushButton *buttons[12];
    QString labels[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "←", "0", "확인"};

    for (int i = 0; i < 12; ++i) {
        buttons[i] = new QPushButton(labels[i], &rewardPopup);
        buttons[i]->setFixedSize(80, 60);
        applyButtonStyle(buttons[i]);
        numpadLayout->addWidget(buttons[i], i / 3, i % 3);
    }

    connect(buttons[9], &QPushButton::clicked, [&]() {
        QString text = pointInput->text();
        if (!text.isEmpty()) {
            pointInput->setText(text.left(text.length() - 1));
        }
    });

    connect(buttons[11], &QPushButton::clicked, [&]() {
        rewardPopup.accept();
        goToPurchaseScreen();
    });

    for (int i = 0; i < 11; i++) {
        if (i == 9) continue;
        connect(buttons[i], &QPushButton::clicked, [=]() {
            pointInput->setText(pointInput->text() + labels[i]);
        });
    }

    mainLayout->addLayout(numpadLayout);
    rewardPopup.setLayout(mainLayout);
    rewardPopup.exec();
}

void KioskWindow::createPurchaseScreen() {
    purchaseScreen = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(purchaseScreen);

    QLabel *titleLabel = new QLabel("결제 방식 선택", purchaseScreen);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    layout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *cardPayment = new QPushButton("💳 카드 결제", purchaseScreen);
    QPushButton *kakaoPay = new QPushButton("📱 카카오페이", purchaseScreen);

    cardPayment->setMinimumHeight(150);
    kakaoPay->setMinimumHeight(150);

    applyButtonStyle(cardPayment);
    applyButtonStyle(kakaoPay);

    connect(cardPayment, &QPushButton::clicked, this, &KioskWindow::goToPurchaseCardScreen);
    connect(kakaoPay, &QPushButton::clicked, this, &KioskWindow::goToPurchaseKakaoPayScreen);

    buttonLayout->addWidget(cardPayment);
    buttonLayout->addWidget(kakaoPay);

    layout->addLayout(buttonLayout);

    layout->addStretch();

    backButton = new QPushButton("🔙 뒤로가기", purchaseScreen);
    backButton->setMinimumHeight(50);
    applyButtonStyle(backButton);
    connect(backButton, &QPushButton::clicked, this, &KioskWindow::goToPrePurchaseScreen);

    layout->addWidget(backButton);

    purchaseScreen->setLayout(layout);
    stackedWidget->addWidget(purchaseScreen);
}

void KioskWindow::showCardInputPopup() {
    QDialog cardPopup(this);
    cardPopup.setWindowTitle("카드 번호 입력");
    cardPopup.setFixedSize(300, 400);
    cardPopup.setStyleSheet("background-color: white; border-radius: 15px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&cardPopup);

    QLabel *message = new QLabel("카드 번호를 입력하세요", &cardPopup);
    message->setAlignment(Qt::AlignCenter);
    message->setStyleSheet("font-size: 18px; color: black;");
    mainLayout->addWidget(message);

    QLineEdit *cardInput = new QLineEdit(&cardPopup);
    cardInput->setReadOnly(true);
    cardInput->setAlignment(Qt::AlignCenter);
    cardInput->setStyleSheet("font-size: 18px; padding: 5px; border: 2px solid #036B3F; border-radius: 5px;");
    mainLayout->addWidget(cardInput);

    QGridLayout *numpadLayout = new QGridLayout();
    QPushButton *buttons[12];
    QString labels[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "←", "0", "확인"};

    for (int i = 0; i < 12; ++i) {
        buttons[i] = new QPushButton(labels[i], &cardPopup);
        buttons[i]->setFixedSize(80, 60);
        applyButtonStyle(buttons[i]);
        numpadLayout->addWidget(buttons[i], i / 3, i % 3);
    }

    connect(buttons[9], &QPushButton::clicked, [&]() {
        QString text = cardInput->text();
        if (!text.isEmpty()) {
            cardInput->setText(text.left(text.length() - 1));
        }
    });

    connect(buttons[11], &QPushButton::clicked, [&]() {
        if (cardInput->text().isEmpty()) {
            showWrongCardPopup();
        } else {
            cardPopup.accept();
            goToFinalScreen();
        }
    });

    for (int i = 0; i < 11; i++) {
        if (i == 9) continue;
        connect(buttons[i], &QPushButton::clicked, [=]() {
            cardInput->setText(cardInput->text() + labels[i]);
        });
    }

    mainLayout->addLayout(numpadLayout);
    cardPopup.setLayout(mainLayout);
    cardPopup.exec();
}

void KioskWindow::showWrongCardPopup() {
    QDialog errorPopup(this);
    errorPopup.setWindowTitle("오류");
    errorPopup.setFixedSize(250, 150);

    QVBoxLayout layout(&errorPopup);
    QLabel message("잘못된 카드 번호입니다.", &errorPopup);
    QPushButton confirmButton("확인", &errorPopup);

    layout.addWidget(&message);
    layout.addWidget(&confirmButton);

    connect(&confirmButton, &QPushButton::clicked, &errorPopup, [&]() {
        errorPopup.accept();
    });

    errorPopup.exec();
}

void KioskWindow::createPurchaseKakaoPayScreen() {
    purchaseKakaoPayScreen = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(purchaseKakaoPayScreen);

    QLabel *titleLabel = new QLabel("📱 카카오페이 결제", purchaseKakaoPayScreen);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: black;");
    layout->addWidget(titleLabel);

    QLabel *qrCodeLabel = new QLabel(purchaseKakaoPayScreen);
    QPixmap qrCodePixmap("../asset/kakaopay_qr.png");
    qrCodeLabel->setPixmap(qrCodePixmap.scaled(200, 200, Qt::KeepAspectRatio));
    qrCodeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(qrCodeLabel);

    QPushButton *confirmButton = new QPushButton("✅ 결제 완료", purchaseKakaoPayScreen);
    confirmButton->setMinimumHeight(50);
    applyButtonStyle(confirmButton);

    connect(confirmButton, &QPushButton::clicked, this, &KioskWindow::goToFinalScreen);
    layout->addWidget(confirmButton);

    QPushButton *backButton = new QPushButton("🔙 뒤로가기", purchaseKakaoPayScreen);
    backButton->setMinimumHeight(50);
    applyButtonStyle(backButton);

    connect(backButton, &QPushButton::clicked, this, &KioskWindow::goToPurchaseScreen);
    layout->addWidget(backButton);

    purchaseKakaoPayScreen->setLayout(layout);
    stackedWidget->addWidget(purchaseKakaoPayScreen);
}

void KioskWindow::createFinalScreen() {
    finalScreen = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(finalScreen);

    QLabel *orderNumberLabel = new QLabel("100번", finalScreen);
    orderNumberLabel->setAlignment(Qt::AlignCenter);
    orderNumberLabel->setStyleSheet("font-size: 40px; font-weight: bold;");
    layout->addWidget(orderNumberLabel);

    orderListFinal = new QListWidget(finalScreen);
    orderListFinal->setStyleSheet("font-size: 14px; background-color: #f8f8f8; border-radius: 5px; color: black;");
    layout->addWidget(orderListFinal);

    backButton = new QPushButton("돌아가기(5초)", finalScreen);
    applyButtonStyle(backButton);
    backButton->setFixedHeight(100);

    layout->addWidget(backButton);
    connect(backButton, &QPushButton::clicked, this, &KioskWindow::goToMainScreen);

    finalScreen->setLayout(layout);
    stackedWidget->addWidget(finalScreen);

    finalScreenTimer = new QTimer(this);
    finalScreenCountdown = 5;
    connect(finalScreenTimer, &QTimer::timeout, this, &KioskWindow::updateFinalScreenCountdown);
}

void KioskWindow::updateFinalScreenCountdown() {
    finalScreenCountdown--;
    backButton->setText("돌아가기(" + QString::number(finalScreenCountdown) + "초)");

    if (finalScreenCountdown == 0) {
        finalScreenTimer->stop();
        goToMainScreen();
    }
}

void KioskWindow::goToMainScreen() {
    orderListData.clear();
    orderListMain->clear();
    orderListPrePurchase->clear();
    orderListFinal->clear();

    totalPriceLabel->setText("총 가격: 0원");

    stackedWidget->setCurrentWidget(mainEntryScreen);
}

void KioskWindow::goToPrePurchaseScreen() {
    if (loadingTimer) {
        loadingTimer->stop();
    }
    loadMenuItems("coffee");
    stackedWidget->setCurrentWidget(prePurchaseScreen);
    updateTotalPrice();
}

void KioskWindow::goToPurchaseScreen() {
    stackedWidget->setCurrentWidget(purchaseScreen);
}

void KioskWindow::goToPurchaseCardScreen() {
    stackedWidget->setCurrentWidget(purchaseCardScreen);
}

void KioskWindow::goToPurchaseKakaoPayScreen() {
    stackedWidget->setCurrentWidget(purchaseKakaoPayScreen);
}

void KioskWindow::goToFinalScreen() {
    stackedWidget->setCurrentWidget(finalScreen);
    finalScreenCountdown = 5;
    backButton->setText("돌아가기(5초)");
    finalScreenTimer->start(1000);
}

void KioskWindow::goToConnectionScreen() {
    stackedWidget->setCurrentWidget(connectionScreen);
}
