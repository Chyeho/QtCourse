#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QStackedWidget;
class QLabel;
class QPushButton;
QT_END_NAMESPACE

class LoginWidget;
class WelcomeWidget;
class PatientWidget;
class PatientEditWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    LoginWidget *loginPage() const { return m_loginPage; }
    WelcomeWidget *welcomePage() const { return m_welcomePage; }
    PatientWidget *patientPage() const { return m_patientPage; }
    PatientEditWidget *patientEditPage() const { return m_patientEditPage; }

    QStackedWidget *stack() const { return m_stack; }
    void setPageTitle(const QString &title);

    enum PageIndex { PageLogin = 0, PageWelcome = 1, PagePatient = 2, PagePatientEdit = 3 };

private:
    void setupUi();
    void connectSignals();
    void gotoPage(PageIndex index, const QString &title);
    void goBack();
    void doLogin();
    void doAddPatient();
    void doEditPatient();
    void doSavePatient();
    QString nextPatientId() const;
    static int calcAge(const QDate &dob);

    Ui::MainWindow *ui;
    QStackedWidget *m_stack = nullptr;
    QLabel *m_titleLabel = nullptr;
    QPushButton *m_backBtn = nullptr;
    QPushButton *m_fullBtn = nullptr;

    LoginWidget *m_loginPage = nullptr;
    WelcomeWidget *m_welcomePage = nullptr;
    PatientWidget *m_patientPage = nullptr;
    PatientEditWidget *m_patientEditPage = nullptr;

    QString m_currentUserId;
    QString m_currentUserName;
};

#endif // MAINWINDOW_H
