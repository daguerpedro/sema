#include "netmanager.h"

NetManager::NetManager(QObject *parent)
    : QObject{parent}
{
    cli = new QMqttClient(this);
    cli->setProtocolVersion(QMqttClient::MQTT_3_1_1);

    connect(cli, &QMqttClient::messageReceived,
            this, &NetManager::proccessMessage);

    connect(cli, &QMqttClient::stateChanged, this, [this](QMqttClient::ClientState s){
        QString info ;
        switch(s) {
        case QMqttClient::Disconnected:
            info = "Desconectado";
            break;
        case QMqttClient::Connecting:
            info = "Conectando";
            break;
        case QMqttClient::Connected:
            info = "Conectado";
            break;
        default: break;
        };

        emit log("MQTT " + info);
    });

    connect(cli, &QMqttClient::errorChanged, this, [this](QMqttClient::ClientError e){
        QString info;
        switch (e)
        {
            case QMqttClient::NoError:
                break;
            case QMqttClient::InvalidProtocolVersion:
                info = "Versão do protocolo inválida";
                break;
            case QMqttClient::IdRejected:
                info = "ID Rejeitado";
                break;
            case QMqttClient::ServerUnavailable:
                info = "Servidor indisponível";
                break;
            case QMqttClient::BadUsernameOrPassword:
                info = "Login inválido";
                break;
            case QMqttClient::NotAuthorized:
                info = "Nâo autorizado";
                break;
            case QMqttClient::TransportInvalid:
                info = "Transporte inválido";
                break;
            case QMqttClient::ProtocolViolation:
                info = "Violação de protocolo";
                break;
            case QMqttClient::UnknownError:
                info = "Erro desconhecido";
                break;
            case QMqttClient::Mqtt5SpecificError:
                info = "Erro específico do Mqtt V5";
                break;
            default:
                break;
        };

        emit log("MQTT " + info);

        if(e !=  QMqttClient::NoError)
            emit errorChanged();
    });

    connect(cli, &QMqttClient::connected, this, [this](){
        cli->subscribe(QMqttTopicFilter(m_settingsData.topico));
        emit connected();
    });

    connect(cli, &QMqttClient::disconnected, this, [this](){
        emit disconnected();
    });
}

void NetManager::proccessMessage(const QByteArray &rawMessage,
                                 const QMqttTopicName &topic)
{
    QString raw = QString::fromUtf8(rawMessage);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    emit message(topic.name(), time, raw);
}

void NetManager::connectionRequest()
{
    if(cli == nullptr) return;

    if(cli->state() == QMqttClient::Connected)
    {
        cli->disconnectFromHost();
        return;
    }

    if(cli->state() == QMqttClient::Disconnected)
    {
        //EMQX requer conexão com TLS
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
        sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
        cli->connectToHostEncrypted(sslConfig);
    }
}

void NetManager::updateSettings(const SettingsData& data)
{
    m_settingsData = data;
    if(cli == nullptr) return;

    cli->setUsername(m_settingsData.username);
    cli->setPassword(m_settingsData.pass);
    cli->setHostname(m_settingsData.brokerAddr);
    cli->setPort(static_cast<quint16>(m_settingsData.brokerPort));

    if(cli->state() == QMqttClient::Connected)
        cli->subscribe(QMqttTopicFilter(m_settingsData.topico));
}
