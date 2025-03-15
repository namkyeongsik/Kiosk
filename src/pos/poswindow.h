#ifndef POSWINDOW_H
#define POSWINDOW_H

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
#include <QFileDialog>
#include <QPixmap>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class POSWindow;
}
QT_END_NAMESPACE

class OrderItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit OrderItemWidget(int orderIndex, QWidget *parent = nullptr);

private slots:
    void toggleReadyState();
    void showOrderDetails();
    void mousePressEvent(QMouseEvent *event);

private:
    int orderIndex;
    QLabel *orderNumberLabel;
    QLabel *itemsLabel;
    QPushButton *readyButton;
};

class RevenueItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit RevenueItemWidget(const QString &orderNum, const QString &items, const QString &price, QWidget *parent = nullptr);

private slots:
    void showOrderDetails();
    void mousePressEvent(QMouseEvent*);

private:
    QLabel *orderNumberLabel;
    QLabel *itemsLabel;
    QLabel *priceLabel;
    std::vector<QString> orderDetails;
};

class ProductItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProductItemWidget(const QString &imagePath, const QString &name, const QString &stock, const QString &price, QWidget *parent = nullptr);

signals:
    void itemClicked(const QString &name, const QString &price, const QString &stock, const QString &imagePath);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *imageLabel;
    QLabel *nameLabel;
    QLabel *stockLabel;
    QLabel *priceLabel;
    QString selectedImagePath;
};

class EditEntryPopup : public QDialog {
    Q_OBJECT
public:
    explicit EditEntryPopup(QWidget *parent = nullptr);
    void setItemDetails(const QString &name, const QString &price, const QString &stock, const QString &imagePath);

private slots:
    void saveChanges();
    void removeEntry();
    void cancelEdit();
    void selectImage();

private:
    QLineEdit *nameInput;
    QLineEdit *priceInput;
    QLineEdit *stockInput;
    QLabel *imageLabel;
    QString selectedImagePath;
};

class AddEntryPopup : public QDialog {
    Q_OBJECT
public:
    explicit AddEntryPopup(QWidget *parent = nullptr);

private slots:
    void saveChanges();
    void cancelAdd();
    void selectImage();

private:
    QLineEdit *nameInput;
    QLineEdit *priceInput;
    QLineEdit *stockInput;
    QLabel *imageLabel;
    QString selectedImagePath;
};

class OrderDetailPopup : public QDialog {
    Q_OBJECT
public:
    explicit OrderDetailPopup(const std::vector<QString> &orderDetails, QWidget *parent = nullptr);
};

class POSWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit POSWindow(QWidget *parent = nullptr);
    ~POSWindow();

private slots:
    void goToMainScreen();
    void goToEntryListScreen();
    void goToAddEntryScreen();
    void goToEditEntryScreen();
    void goToRevenueScreen();
    void showEditEntryPopup(const QString &name, const QString &price, const QString &stock, const QString &imagePath);

private:
    void setupUi();
    void createMainScreen();
    void createEntryListScreen();
    void createRevenueScreen();
    void updateOrderList();
    void updateRevenueList();
    void updateProductList();

    QStackedWidget *stackedWidget;
    QWidget *mainScreen;
    QWidget *entryListScreen;
    QWidget *revenueScreen;

    QListWidget *orderListWidget;
    QListWidget *revenueListWidget;
    QGridLayout *productGridLayout;
    EditEntryPopup *editPopup;
};

#endif // POSWINDOW_H
