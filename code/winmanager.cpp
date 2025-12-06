#include "winmanager.h"

WinManager::WinManager()
{
    //Primeiro a db para utilizar configurações salvas nas janelas
    database = new Database();
    auto settings = database->getSettings();

    m_dashboardWin = new DashboardWin();
    m_settingsWin  = new SettingsWin();
    m_netManager = new NetManager();

    m_settingsWin->loadSettings(settings);
    m_netManager->updateSettings(settings);
    m_dashboardWin->updateMaxData(settings.maxData);

    //Conectar todas as janelas entre elas.
    createSignals();

    //Carregar os últimos dados de sensores salvos no banco de dados.
    database->loadMessages();
}

void WinManager::showSettings()
{
    m_settingsWin->show();
}

void WinManager::showDashboard()
{
    m_dashboardWin->show();
}

void WinManager::createSignals()
{
    //------------------------| BOTÕES DO PAINEL PRINCIPAL |------------------------
    connect(m_dashboardWin->bSettings, &CustomButton::clicked, m_settingsWin, &QWidget::show);  //Abre a janela de config
    connect(m_dashboardWin->bConnect, &CustomButton::clicked, this, [this](){
        m_dashboardWin->bConnect->setDisabled(true); //Desativa para evitar duplicatas
        m_netManager->connectionRequest(); //Faz requisição de conexão pro MQTTT
    });
    connect(m_dashboardWin->bData, &CustomButton::clicked, database, &Database::exportData); //Abre a janela de exportar dados salvos
    connect(m_dashboardWin->bEraseData, &CustomButton::clicked, database, &Database::eraseData); //Abre a janela de apagar dados salvos

    //------------------------|    PAINEL CONFIGURAÇÕES    |------------------------
    connect(m_settingsWin, &SettingsWin::settingsChanged, this, [this](const SettingsData &data){
        //Atualiza a config para as classes que usam ela
        database->saveSettings(data);
        m_netManager->updateSettings(data);
        m_dashboardWin->updateMaxData(data.maxData);
    });

    //------------------------|    GERENCIADOR CONEXÃO     |------------------------
    connect(m_netManager, &NetManager::connected, this, [this](){
        //Atualiza no painel estado de conexão e habilita o botão novamente
        m_dashboardWin->bConnect->setEnabled(true);
        m_dashboardWin->bConnect->setText("Desconectar");
        m_dashboardWin->bConnect->update();
    });
    connect(m_netManager, &NetManager::errorChanged, this, [this](){
        //Atualiza no painel estado de conexão e habilita o botão novamente
        m_dashboardWin->bConnect->setEnabled(true);
        m_dashboardWin->bConnect->setText("Conectar");
        m_dashboardWin->bConnect->update();
    });
    connect(m_netManager, &NetManager::disconnected, this, [this](){
        //Atualiza no painel estado de conexão e habilita o botão novamente
        m_dashboardWin->bConnect->setEnabled(true);
        m_dashboardWin->bConnect->setText("Conectar");
        m_dashboardWin->bConnect->update();
    });    
    connect(m_netManager, &NetManager::log, m_dashboardWin, &DashboardWin::log); //Imprime log no painel
    connect(m_netManager, &NetManager::message, database, &Database::saveData); //Salva os dados de sensores recebidos
    connect(m_netManager, &NetManager::message, m_dashboardWin, &DashboardWin::plotData);//Plota os dados de sensores recebidos

    //------------------------|       BANCO DE DADOS       |------------------------
    connect(database, &Database::loadedMessage, this, [this](const QString &topic, const QString &time, const QString &raw){
        m_dashboardWin->plotData(topic, time, raw);//Plota os dados salvos de sensores no dashboard
    });
    connect(database, &Database::erased, m_dashboardWin, &DashboardWin::erase); //Apaga os dados em cache
}
