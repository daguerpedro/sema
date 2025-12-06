#ifndef SETTINGSWIN_H
#define SETTINGSWIN_H

#include <QWidget>
#include <QLineEdit>
#include <QIntValidator>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMessageBox>
#include "../widgets/custombutton.h"
#include <QSpinBox>

struct SettingsData {
    QString brokerAddr;
    QString username;
    QString pass;
    QString topico;
    int brokerPort;
    bool saveData;
    long maxData;
};

class SettingsWin : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsWin(QWidget *parent = nullptr);

    CustomButton* bSave;

    QLineEdit* lAddr;
    QLineEdit* lPort;
    QLineEdit* lTopic;

    QLineEdit* lUser;
    QLineEdit* lPass;
    QSpinBox* sMaxData;

    QCheckBox* cSaveData;

    void loadSettings(const SettingsData& data);

signals:
    void settingsChanged(const SettingsData &data);

private slots:
    void onSaveClicked();

private:
    void createButtons();
    void createInputs();
    void createLayout();

    QIcon* icon;

    QVBoxLayout* layout;

signals:
};

#endif // SETTINGSWIN_H
