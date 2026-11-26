#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;

// 登录页面（对应实验参考图 1）
class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);

    QLineEdit *usernameEdit() const { return m_usernameEdit; }
    QLineEdit *passwordEdit() const { return m_passwordEdit; }
    QPushButton *loginButton() const { return m_loginBtn; }
    QPushButton *registerButton() const { return m_registerBtn; }

private:
    void setupUi();

    QLineEdit *m_usernameEdit = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    QPushButton *m_loginBtn = nullptr;
    QPushButton *m_registerBtn = nullptr;
};

#endif // LOGINWIDGET_H
