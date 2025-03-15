#include "poswindow.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QFileDialog>
#include <QPixmap>
#include <vector>

POSWindow::POSWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
}

POSWindow::~POSWindow() {}

std::vector<std::vector<QString>> orderData = {
    {"커피 2개", "샌드위치 1개"},
    {"아메리카노 1개", "도넛 2개", "주스 1개"},
    {"라떼 3개"},
    {"핫초코 2개", "쿠키 1개"}
};

OrderDetailPopup::OrderDetailPopup(const std::vector<QString> &orderDetails, QWidget *parent) : QDialog(parent) {
    setWindowTitle("주문 상세 정보");
    setFixedSize(320, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("주문 상세 내역", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titleLabel);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget *scrollContainer = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContainer);

    for (const QString &item : orderDetails) {
        QLabel *orderItemLabel = new QLabel(item, this);
        orderItemLabel->setStyleSheet("font-size: 16px; border-bottom: 1px solid #ccc; padding: 5px;");
        scrollLayout->addWidget(orderItemLabel);
    }
    scrollContainer->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContainer);
    layout->addWidget(scrollArea);

    QPushButton *closeButton = new QPushButton("닫기", this);
    closeButton->setStyleSheet("background-color: #555; color: white; font-size: 16px; padding: 8px;");
    connect(closeButton, &QPushButton::clicked, this, &OrderDetailPopup::close);
    layout->addWidget(closeButton);
}

OrderItemWidget::OrderItemWidget(int orderIndex, QWidget *parent)
    : QWidget(parent), orderIndex(orderIndex) {
    QHBoxLayout *layout = new QHBoxLayout(this);

    orderNumberLabel = new QLabel(QString("주문 %1").arg(orderIndex + 1), this);
    orderNumberLabel->setStyleSheet("font-size: 16px; font-weight: bold; min-width: 50px; border: none;");
    layout->addWidget(orderNumberLabel);

    QStringList orderList;
    for (const QString &item : orderData[orderIndex]) {
        orderList.append(item);
    }
    itemsLabel = new QLabel(orderList.join(", "), this);
    itemsLabel->setStyleSheet("font-size: 14px; color: gray; min-width: 350px; text-decoration: underline; cursor: pointer; border: none;");
    itemsLabel->setWordWrap(true);
    layout->addWidget(itemsLabel);

    readyButton = new QPushButton("준비중", this);
    readyButton->setStyleSheet("background-color: red; color: white; font-size: 14px; padding: 5px;");
    connect(readyButton, &QPushButton::clicked, this, &OrderItemWidget::toggleReadyState);
    layout->addWidget(readyButton);

    setLayout(layout);
}

void OrderItemWidget::mousePressEvent(QMouseEvent *event) {
    if (itemsLabel->geometry().contains(event->pos())) {
        OrderDetailPopup detailPopup(orderData[orderIndex], this);
        detailPopup.exec();
    }
}


void OrderItemWidget::showOrderDetails() {
    OrderDetailPopup detailPopup(orderData[orderIndex], this);
    detailPopup.exec();
}

void OrderItemWidget::toggleReadyState() {
    if (readyButton->text() == "준비중") {
        readyButton->setText("준비완료");
        readyButton->setStyleSheet("background-color: green; color: white; font-size: 14px; padding: 5px;");
    } else {
        readyButton->setText("준비중");
        readyButton->setStyleSheet("background-color: red; color: white; font-size: 14px; padding: 5px;");
    }
}

void POSWindow::setupUi() {
    setFixedSize(640, 360);
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);


    db = new Database(this);
    db->openDatabase();
    db->createTables();
    createMainScreen();
    createEntryListScreen();
    createRevenueScreen();

    stackedWidget->setCurrentWidget(mainScreen);
}

void POSWindow::createMainScreen() {
    mainScreen = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(mainScreen);

    orderListWidget = new QListWidget(mainScreen);
    orderListWidget->setStyleSheet("background-color: white; border: 1px solid #ccc; font-size: 14px;");
    layout->addWidget(orderListWidget);

    QGridLayout *buttonLayout = new QGridLayout();
    QPushButton *entryListButton = new QPushButton("엔트리 정보", mainScreen);
    QPushButton *revenueButton = new QPushButton("매출 확인", mainScreen);

    entryListButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 18px; padding: 10px;");
    revenueButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 18px; padding: 10px;");

    connect(entryListButton, &QPushButton::clicked, this, &POSWindow::goToEntryListScreen);
    connect(revenueButton, &QPushButton::clicked, this, &POSWindow::goToRevenueScreen);

    buttonLayout->addWidget(entryListButton, 0, 0);
    buttonLayout->addWidget(revenueButton, 0, 1);

    layout->addLayout(buttonLayout);
    mainScreen->setLayout(layout);
    stackedWidget->addWidget(mainScreen);

    updateOrderList();
}

void POSWindow::updateOrderList() {
    orderListWidget->clear();
    for (size_t i = 0; i < orderData.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(orderListWidget);
        OrderItemWidget *orderWidget = new OrderItemWidget(i, orderListWidget);
        orderListWidget->setItemWidget(item, orderWidget);
        item->setSizeHint(orderWidget->sizeHint());
    }
}
AddEntryPopup::AddEntryPopup(QWidget *parent) : QDialog(parent) {
    setWindowTitle("상품 추가");
    setFixedSize(320, 350);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("상품 추가", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titleLabel);

    QGridLayout *inputLayout = new QGridLayout();
    QLabel *nameLabel = new QLabel("이름:", this);
    QLabel *priceLabel = new QLabel("가격:", this);
    QLabel *stockLabel = new QLabel("재고:", this);

    nameInput = new QLineEdit(this);
    priceInput = new QLineEdit(this);
    stockInput = new QLineEdit(this);

    inputLayout->addWidget(nameLabel, 0, 0);
    inputLayout->addWidget(nameInput, 0, 1);
    inputLayout->addWidget(priceLabel, 1, 0);
    inputLayout->addWidget(priceInput, 1, 1);
    inputLayout->addWidget(stockLabel, 2, 0);
    inputLayout->addWidget(stockInput, 2, 1);

    layout->addLayout(inputLayout);

    QLabel *imageLabel = new QLabel(this);
    imageLabel->setFixedSize(100, 100);
    imageLabel->setStyleSheet("border: 1px solid black;");
    layout->addWidget(imageLabel, 0, Qt::AlignCenter);

    QPushButton *selectImageButton = new QPushButton("이미지 선택", this);
    connect(selectImageButton, &QPushButton::clicked, this, &AddEntryPopup::selectImage);
    layout->addWidget(selectImageButton);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("저장", this);
    QPushButton *cancelButton = new QPushButton("취소", this);

    saveButton->setStyleSheet("background-color: #028C50; color: white; font-size: 16px; padding: 8px;");
    cancelButton->setStyleSheet("background-color: #555; color: white; font-size: 16px; padding: 8px;");

    connect(saveButton, &QPushButton::clicked, this, &AddEntryPopup::saveChanges);
    connect(cancelButton, &QPushButton::clicked, this, &AddEntryPopup::cancelAdd);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
}

void AddEntryPopup::selectImage() {
    QString filePath = QFileDialog::getOpenFileName(this, "이미지 선택", "", "Images (*.png *.jpg *.jpeg)");
    if (filePath.isEmpty()) {
        return; // Prevents crash if user cancels selection
    }

    selectedImagePath = filePath;
    if (imageLabel) {
        QPixmap pixmap(filePath);
        imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}


void AddEntryPopup::saveChanges() {
    accept();
}

void AddEntryPopup::cancelAdd() {
    reject();
}

ProductItemWidget::ProductItemWidget(const QString &imagePath, const QString &name, const QString &stock, const QString &price, QWidget *parent)
    : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    imageLabel = new QLabel(this);
    QPixmap pixmap(imagePath);
    imageLabel->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio));
    layout->addWidget(imageLabel, 0, Qt::AlignCenter);

    nameLabel = new QLabel(name, this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(nameLabel);

    stockLabel = new QLabel("재고: " + stock, this);
    stockLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(stockLabel);

    priceLabel = new QLabel(price, this);
    priceLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: green;");
    layout->addWidget(priceLabel);

    setLayout(layout);
}

void ProductItemWidget::mousePressEvent(QMouseEvent *event) {
    emit itemClicked(nameLabel->text(), priceLabel->text(), stockLabel->text(), !imageLabel->pixmap().isNull() ? selectedImagePath : "");
}

EditEntryPopup::EditEntryPopup(QWidget *parent) : QDialog(parent) {
    setWindowTitle("상품 수정");
    setFixedSize(320, 350);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("상품 수정", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titleLabel);

    QGridLayout *inputLayout = new QGridLayout();
    QLabel *nameLabel = new QLabel("이름:", this);
    QLabel *priceLabel = new QLabel("가격:", this);
    QLabel *stockLabel = new QLabel("재고:", this);

    nameInput = new QLineEdit(this);
    priceInput = new QLineEdit(this);
    stockInput = new QLineEdit(this);

    inputLayout->addWidget(nameLabel, 0, 0);
    inputLayout->addWidget(nameInput, 0, 1);
    inputLayout->addWidget(priceLabel, 1, 0);
    inputLayout->addWidget(priceInput, 1, 1);
    inputLayout->addWidget(stockLabel, 2, 0);
    inputLayout->addWidget(stockInput, 2, 1);

    layout->addLayout(inputLayout);

    imageLabel = new QLabel(this);
    imageLabel->setFixedSize(100, 100);
    imageLabel->setStyleSheet("border: 1px solid black;");
    layout->addWidget(imageLabel, 0, Qt::AlignCenter);

    QPushButton *selectImageButton = new QPushButton("이미지 변경", this);
    connect(selectImageButton, &QPushButton::clicked, this, &EditEntryPopup::selectImage);
    layout->addWidget(selectImageButton);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("저장", this);
    QPushButton *removeButton = new QPushButton("엔트리 제거", this);
    QPushButton *cancelButton = new QPushButton("취소", this);

    saveButton->setStyleSheet("background-color: #028C50; color: white; font-size: 16px; padding: 8px;");
    removeButton->setStyleSheet("background-color: #FF0000; color: white; font-size: 16px; padding: 8px;");
    cancelButton->setStyleSheet("background-color: #555; color: white; font-size: 16px; padding: 8px;");

    connect(saveButton, &QPushButton::clicked, this, &EditEntryPopup::saveChanges);
    connect(removeButton, &QPushButton::clicked, this, &EditEntryPopup::removeEntry);
    connect(cancelButton, &QPushButton::clicked, this, &EditEntryPopup::cancelEdit);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
}

void EditEntryPopup::setItemDetails(const QString &name, const QString &price, const QString &stock, const QString &imagePath) {
    nameInput->setText(name);
    priceInput->setText(price);
    stockInput->setText(stock);
    selectedImagePath = imagePath;
    if (!imagePath.isEmpty()) {
        QPixmap pixmap(imagePath);
        imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void EditEntryPopup::selectImage() {
    QString filePath = QFileDialog::getOpenFileName(this, "이미지 선택", "", "Images (*.png *.jpg *.jpeg)");
    if (filePath.isEmpty()) {
        return; // Prevents crash if user cancels selection
    }

    selectedImagePath = filePath;
    if (imageLabel) {
        QPixmap pixmap(filePath);
        imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}


void EditEntryPopup::saveChanges() {
    accept();
}

void EditEntryPopup::removeEntry() {
    // Implement actual removal logic
    reject();
}

void EditEntryPopup::cancelEdit() {
    reject();
}

void POSWindow::createEntryListScreen() {
    entryListScreen = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(entryListScreen);

    QLabel *titleLabel = new QLabel("상품 목록", entryListScreen);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(titleLabel);

    QWidget *productContainer = new QWidget(entryListScreen);
    productGridLayout = new QGridLayout(productContainer);
    productContainer->setLayout(productGridLayout);

    QScrollArea *scrollArea = new QScrollArea(entryListScreen);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(productContainer);
    layout->addWidget(scrollArea);

    QPushButton *addButton = new QPushButton("상품 추가", entryListScreen);
    QPushButton *backButton = new QPushButton("뒤로 가기", entryListScreen);

    addButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 18px; padding: 10px;");
    backButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 18px; padding: 10px;");

    connect(addButton, &QPushButton::clicked, this, &POSWindow::goToAddEntryScreen);
    connect(backButton, &QPushButton::clicked, this, &POSWindow::goToMainScreen);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(backButton);
    layout->addLayout(buttonLayout);

    entryListScreen->setLayout(layout);
    stackedWidget->addWidget(entryListScreen);

    editPopup = new EditEntryPopup(this);
    updateProductList();
}

void POSWindow::updateProductList() {
    for (int i = 0; i < 6; ++i) {
        int row = i / 3;
        int col = i % 3;
        ProductItemWidget *product = new ProductItemWidget(":/images/sample.png", "상품" + QString::number(i+1), QString::number(10 - i), "₩" + QString::number(1000 * (i+1)), entryListScreen);
        connect(product, &ProductItemWidget::itemClicked, this, &POSWindow::showEditEntryPopup);
        productGridLayout->addWidget(product, row, col);
    }
}

void POSWindow::showEditEntryPopup(const QString &name, const QString &price, const QString &stock, const QString &imagePath) {
    editPopup->setItemDetails(name, price, stock, imagePath);
    editPopup->exec();
}

RevenueItemWidget::RevenueItemWidget(const QString &orderNum, const QString &items, const QString &price, QWidget *parent)
    : QWidget(parent), orderDetails() {
    QHBoxLayout *layout = new QHBoxLayout(this);

    orderNumberLabel = new QLabel(orderNum, this);
    orderNumberLabel->setStyleSheet("font-size: 16px; font-weight: bold; min-width: 50px; border: none;");
    layout->addWidget(orderNumberLabel);

    itemsLabel = new QLabel(items, this);
    itemsLabel->setStyleSheet("font-size: 14px; color: gray; min-width: 350px; text-decoration: underline; cursor: pointer; border: none;");
    itemsLabel->setWordWrap(true);
    layout->addWidget(itemsLabel);

    priceLabel = new QLabel(price, this);
    priceLabel->setStyleSheet("font-size: 16px; font-weight: bold; min-width: 70px; text-align: right; border: none;");
    layout->addWidget(priceLabel);

    // Convert QStringList to std::vector<QString>
    QStringList itemsList = items.split(", ");
    for (const QString &item : itemsList) {
        orderDetails.push_back(item);
    }

    setLayout(layout);
}

void RevenueItemWidget::mousePressEvent(QMouseEvent *event) {
    if (itemsLabel->geometry().contains(event->pos())) {
        OrderDetailPopup detailPopup(orderDetails, this);
        detailPopup.exec();
    }
}

void RevenueItemWidget::showOrderDetails() {
    OrderDetailPopup detailPopup(orderDetails, this);
    detailPopup.exec();
}

void POSWindow::createRevenueScreen() {
    revenueScreen = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(revenueScreen);

    revenueListWidget = new QListWidget(revenueScreen);
    revenueListWidget->setStyleSheet("background-color: white; border: 1px solid #ccc; font-size: 14px;");
    layout->addWidget(revenueListWidget);

    QLabel *revenueLabel = new QLabel("총 매출 : 80000 원", revenueScreen);
    revenueLabel->setAlignment(Qt::AlignCenter);
    revenueLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(revenueLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *backButton = new QPushButton("메인으로 돌아가기", revenueScreen);
    backButton->setStyleSheet("background-color: #036B3F; color: white; font-size: 18px; padding: 10px;");
    connect(backButton, &QPushButton::clicked, this, &POSWindow::goToMainScreen);

    buttonLayout->addWidget(backButton);
    layout->addLayout(buttonLayout);

    revenueScreen->setLayout(layout);
    stackedWidget->addWidget(revenueScreen);

    updateRevenueList();
}

void POSWindow::updateRevenueList() {
    revenueListWidget->clear();
    for (int i = 1; i <= 8; ++i) {
        QListWidgetItem *item = new QListWidgetItem(revenueListWidget);
        RevenueItemWidget *revenueWidget = new RevenueItemWidget(QString("10%1번").arg(i), "커피 2개", "₩10,000", revenueListWidget);
        revenueListWidget->setItemWidget(item, revenueWidget);
        item->setSizeHint(revenueWidget->sizeHint());
    }
}

void POSWindow::goToMainScreen() {
    stackedWidget->setCurrentWidget(mainScreen);
}

void POSWindow::goToEntryListScreen() {
    stackedWidget->setCurrentWidget(entryListScreen);
}

void POSWindow::goToAddEntryScreen() {
    AddEntryPopup addPopup(this);
    addPopup.exec();
}

void POSWindow::goToEditEntryScreen() {
    EditEntryPopup editPopup(this);
    editPopup.exec();
}

void POSWindow::goToRevenueScreen() {
    stackedWidget->setCurrentWidget(revenueScreen);
}
