#include "database.h"

Database::Database()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("sema.db");

    if (!m_db.open()) {
        QMessageBox::warning(nullptr, "Erro ao abrir banco de dados", m_db.lastError().text());
        return;
    }

    m_criarTabelaSettings();
    m_criarTabelaDados();
}

void Database::m_criarTabelaSettings()
{
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS settings ("
                    "key TEXT PRIMARY KEY,"
                    "value TEXT"
                    ");"))
    QMessageBox::warning(nullptr, "Erro ao criar tabela settings", query.lastError().text());

    m_settingsTable = new QSqlTableModel();
    m_settingsTable->setTable("settings");
    m_settingsTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_settingsTable->select();

#ifdef QT_DEBUG
    QTableView *settingsView = new QTableView;
    settingsView->setModel(m_settingsTable);
    settingsView->setWindowTitle("Tabela Settings");
    settingsView->setWindowIcon(QIcon("://img/settings.png"));
    settingsView->show();
#endif
}

void Database::m_criarTabelaDados()
{
    QSqlQuery query;

    if (!query.exec("CREATE TABLE IF NOT EXISTS dados ("
                    "time_stamp TEXT, "
                    "topic TEXT, "
                    "rawdata TEXT)"))
    QMessageBox::warning(nullptr, "Erro ao criar tabela dados", query.lastError().text());


    m_dataTable= new QSqlTableModel();
    m_dataTable->setTable("dados");
    m_dataTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_dataTable->select();

#ifdef QT_DEBUG
    QTableView *dataView = new QTableView;
    dataView->setModel(m_dataTable);
    dataView->setWindowTitle("Tabela Dados");
    dataView->setWindowIcon(QIcon("://img/database.png"));
    dataView->show();
#endif
}

void Database::m_setSetting(const QString &key, const QString &value)
{
    QSqlQuery query;
    query.prepare("UPDATE settings SET value = :value WHERE key = :key");
    query.bindValue(":value", value);
    query.bindValue(":key", key);
    query.exec();

    if (query.numRowsAffected() == 0) {
        QSqlQuery q2;
        q2.prepare("INSERT INTO settings (key, value) VALUES (:key, :value)");
        q2.bindValue(":key", key);
        q2.bindValue(":value", value);
        if (!q2.exec()) {
            QMessageBox::warning(nullptr, "Erro ao inserir setting", q2.lastError().text());
        }
    }
}

void Database::saveSettings(const SettingsData &data)
{
    if (!m_db.isOpen())
    {
        QMessageBox::warning(nullptr, "Erro ao abrir banco de dados", m_db.lastError().text());
        return;
    }
    m_setSetting("brokerAddr", data.brokerAddr);
    m_setSetting("brokerPort", QString::number(data.brokerPort));
    m_setSetting("mqttTopic", data.topico);
    m_setSetting("username", data.username);
    m_setSetting("pass", data.pass);
    m_setSetting("saveData", data.saveData ? "1" : "0");
    m_setSetting("maxData", QString::number(data.maxData));
    m_saveData = data.saveData;
    m_settingsTable->select();
}

SettingsData Database::getSettings()
{
    SettingsData data;

    data.brokerAddr = "";
    data.brokerPort = -1;
    data.saveData   = false;
    data.username = "";
    data.topico = "";
    data.pass = "";
    data.maxData = 30;

    if (!m_db.isOpen())
    {
        QMessageBox::warning(nullptr, "Erro ao abrir banco de dados", m_db.lastError().text());
        return data;
    }

    QSqlQuery query;

    query.prepare("SELECT value FROM settings WHERE key = 'brokerAddr'");
    if (query.exec() && query.next()) {
        data.brokerAddr = query.value(0).toString();
    }

    query.prepare("SELECT value FROM settings WHERE key = 'username'");
    if (query.exec() && query.next()) {
        data.username = query.value(0).toString();
    }

    query.prepare("SELECT value FROM settings WHERE key = 'mqttTopic'");
    if (query.exec() && query.next()) {
        data.topico = query.value(0).toString();
    }

    query.prepare("SELECT value FROM settings WHERE key = 'pass'");
    if (query.exec() && query.next()) {
        data.pass = query.value(0).toString();
    }

    query.prepare("SELECT value FROM settings WHERE key = 'brokerPort'");
    if (query.exec() && query.next()) {
        data.brokerPort = query.value(0).toInt();
    }

    query.prepare("SELECT value FROM settings WHERE key = 'maxData'");
    if (query.exec() && query.next()) {
        data.maxData = query.value(0).toInt();
    }

    query.prepare("SELECT value FROM settings WHERE key = 'saveData'");
    if (query.exec() && query.next()) {
        data.saveData = query.value(0).toString() == "1";
    }
    m_saveData=data.saveData;
    return data;
}

void Database::loadMessages()
{
    auto settings = getSettings();
    if (!m_db.isOpen())
    {
        QMessageBox::warning(nullptr, "Erro ao abrir banco de dados", m_db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT time_stamp, topic, rawdata FROM dados ORDER BY time_stamp DESC LIMIT :max"); //DESC para pegar os mais atuais
    query.bindValue(":max", QString::number(settings.maxData));

    struct MSG {
        QString topico;
        QString time;
        QString raw;
    };

    QList<MSG> lista;

    if (query.exec())
    {
        while (query.next())
            lista.push_front({
                query.value(1).toString(),
                query.value(0).toString(),
                query.value(2).toString()
            }); //Inverte a ordem pois pegamos decrescentemente e temos que plotar crescentemente

        for(auto& v : lista) //Percorre a lista que agora é na ordem crescente
            emit loadedMessage(v.topico, v.time, v.raw);
    }
}

void Database::eraseData()
{
    if (!m_db.isOpen())
    {
        QMessageBox::warning(nullptr, "Erro ao limpar dados", "O banco de dados não está aberto.");
        return;
    }

    auto res = QMessageBox::question(nullptr, "Banco de dados", "Deseja apagar TODOS os dados salvos?", QMessageBox::Yes | QMessageBox::No);
    if(res == QMessageBox::No)
        return;

    QSqlQuery query;
    if (query.exec("DELETE FROM dados")) {
        m_dataTable->select();
        emit erased();
        QMessageBox::information(nullptr, "Banco de dados", "Todos os dados foram apagados");
    } else {
        QMessageBox::warning(nullptr, "Erro ao limpar dados", query.lastError().text());
    }
}

void Database::saveData(const QString &topic, const QString &time, const QString &raw)
{
    if(!m_saveData) return;
    if (!m_db.isOpen())
    {
        QMessageBox::warning(nullptr, "Erro ao abrir banco de dados", m_db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO dados (time_stamp, topic, rawdata) VALUES (:times, :topic, :rawd)");
    query.bindValue(":times", time);
    query.bindValue(":topic", topic);
    query.bindValue(":rawd", raw);

    if (!query.exec() || query.numRowsAffected() == 0) {
        QMessageBox::warning(nullptr, "Erro ao gravar dados no banco de dados", query.lastError().text());
    }
}

void Database::exportData()
{
    if (!m_db.isOpen())
    {
        QMessageBox::warning(nullptr, "Erro ao exportar dados", "O banco de dados não está aberto");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(nullptr,
        tr("Exportar Dados"),
        QDir::homePath(),
        tr("Arquivos CSV (*.csv);;Todos os Arquivos (*)")
    );

    if (filePath.isEmpty() || filePath.isNull()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Erro ao exportar dados",
                             QString("Não foi possível abrir o arquivo para escrita: %1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    QSqlQuery query;

    if (!query.exec("SELECT time_stamp, topic, rawdata FROM dados ORDER BY time_stamp ASC")) {
        QMessageBox::warning(nullptr, "Erro ao consultar dados para exportação", query.lastError().text());
        file.close();
        return;
    }

    out << "Time_Stamp;Topic;RawData\n";
    int rowCount = 0;
    while (query.next()) {
        QString time_stamp = query.value(0).toString();
        QString topic = query.value(1).toString();
        QString rawdata = query.value(2).toString();

        rawdata.replace('\n', ' ').replace('\r', ' ');
        out << time_stamp << ";" << topic << ";" << rawdata << "\n";
        rowCount++;
    }

    file.close();
    QMessageBox::information(nullptr, "Banco de dados", "Todos os dados foram exportados");
    return;
}
