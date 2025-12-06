#ifndef WINMANAGER_H
#define WINMANAGER_H

#include "settings/settingswin.h"
#include "dashboard/dashboardwin.h"
#include "data/database.h"
#include "networking/netmanager.h"

#include <QObject>

/**
 * @brief WinManager
 * É responsável por gerênciar e conectar todas as janelas e o banco de dados através de signals e slots.
 */
class WinManager : QObject
{
    //Q_OBJECT necessário para signals e slots.
    Q_OBJECT

public:
    Database* database;
    WinManager();

    void showSettings();
    void showDashboard();

    void createSignals();

private:
    SettingsWin* m_settingsWin;
    DashboardWin* m_dashboardWin;
    NetManager* m_netManager;
};

#endif // WINMANAGER_H
