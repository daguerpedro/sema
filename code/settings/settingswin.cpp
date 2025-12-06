#include "settingswin.h"

SettingsWin::SettingsWin(QWidget *parent)
    : QWidget{parent}
{
    createButtons();
    createInputs();
    createLayout();

    connect(bSave, &QPushButton::clicked, this, &SettingsWin::onSaveClicked);
}

void SettingsWin::loadSettings(const SettingsData &data)
{
    lAddr->setText(data.brokerAddr);
    lPort->setText(QString::number(data.brokerPort));
    lTopic->setText(data.topico);
    lUser->setText(data.username);
    lPass->setText(data.pass);
    cSaveData->setChecked(data.saveData);
    sMaxData->setValue(data.maxData);
}

void SettingsWin::createButtons()
{
    bSave = new CustomButton(" Salvar", "://img/save.png", 22, 22);
    cSaveData = new QCheckBox("Salvar dados recebidos");
}

void SettingsWin::createInputs()
{
    lAddr = new QLineEdit();
    lAddr->setPlaceholderText("Endereço Broker MQTT");

    lPort =new QLineEdit();
    lPort->setValidator(new QIntValidator(0, 9999, this));
    lPort->setPlaceholderText("Porta Broker MQTT");

    lTopic = new QLineEdit();
    lTopic->setPlaceholderText("Tópico MQTT");

    lUser = new QLineEdit();
    lUser->setPlaceholderText("Usuário MQTT");

    lPass = new QLineEdit();
    lPass->setPlaceholderText("Senha MQTT");
    lPass->setEchoMode(QLineEdit::Password);

    sMaxData = new QSpinBox();
    sMaxData->setMinimum(0);
    sMaxData->setValue(0);
    sMaxData->setSpecialValueText("Máximo dados cache");
}

void SettingsWin::createLayout()
{
    layout = new QVBoxLayout();

    layout->addWidget(lAddr);
    layout->addWidget(lPort);
    layout->addWidget(lTopic);
    layout->addWidget(lUser);
    layout->addWidget(lPass);
    layout->addWidget(sMaxData);
    layout->addWidget(cSaveData);
    layout->addWidget(bSave);

    setLayout(layout);
    setWindowIcon(QIcon("://img/settings.png"));
    setWindowTitle("Configurações");
    setFixedSize(sizeHint().width() + 100, sizeHint().height());
}

void SettingsWin::onSaveClicked()
{
    bool ok;
    int port = lPort->text().toInt(&ok);

    if (!ok || port < 1 || port > 65535) {
        QMessageBox::warning(this, "Porta inválida",
                             "Insira uma porta válida entre 1 e 65535.");
        return;
    }

    if (lAddr->text().isEmpty()) {
        QMessageBox::warning(this, "Endereço vazio",
                             "O endereço do broker não pode estar vazio.");
        return;
    }

    if (lTopic->text().isEmpty()) {
        QMessageBox::warning(this, "Tópico vazio",
                             "Tópico não pode estar vazio.");
        return;
    }

    if (lUser->text().isEmpty()) {
        QMessageBox::warning(this, "Usuário vazio",
                             "Usuário não pode estar vazio.");
        return;
    }

    if (lPass->text().isEmpty()) {
        QMessageBox::warning(this, "Senha vazia",
                             "Senha não pode estar vazia.");
        return;
    }

    SettingsData data;
    data.brokerAddr = lAddr->text();
    data.brokerPort = port;
    data.saveData   = cSaveData->isChecked();
    data.username = lUser->text();
    data.pass = lPass->text();
    data.topico = lTopic->text();
    data.maxData = sMaxData->value();

    emit settingsChanged(data);
    close();
}

