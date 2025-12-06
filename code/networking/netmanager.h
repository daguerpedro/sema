#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>
#include <QMqttClient>
#include <QMqttMessage>
#include <QMqttTopicName>
#include <QString>
#include <QDateTime>
#include <QMessageBox>
#include <QMqttSubscription>
#include "../settings/settingswin.h"

class NetManager : public QObject
{
    Q_OBJECT
private:
    QMqttClient* cli;
    SettingsData m_settingsData;

    void proccessMessage(const QByteArray &message, const QMqttTopicName &topic = QMqttTopicName());
public:
    explicit NetManager(QObject *parent = nullptr);

public slots:
    void connectionRequest();
    void updateSettings(const SettingsData& data);
signals:
    void log(QString message);
    void message(const QString &topic, const QString &time, const QString &raw);
    void connected();
    void disconnected();
    void errorChanged();
};

#endif // NETMANAGER_H
