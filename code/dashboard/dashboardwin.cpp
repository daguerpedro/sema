#include "dashboardwin.h"

DashboardWin::DashboardWin(QWidget *parent)
    : QWidget{parent}
{
    createButtons();
    createLabels();
    createCharts();
    createLayouts();

    resize(1100, 700);

    setWindowIcon(QIcon("://img/sema.png"));
}

void DashboardWin::closeEvent(QCloseEvent *event)
{
    QApplication::quit();
    event->accept();
}

void DashboardWin::createButtons()
{
    bSettings = new CustomButton(" Configurar", "://img/settings.png", 22, 22);
    bData = new CustomButton(" Exportar", "://img/database.png", 22, 22);
    bEraseData = new CustomButton(" Apagar TUDO", "://img/erase.png", 22, 22);
    bConnect = new CustomButton(" Conectar", "://img/connection.png", 22, 22);
    bClearLog = new CustomButton(" Limpar Log", "://img/clear.png", 22, 22);

    connect(bClearLog, &QPushButton::clicked, this, [this](){
        lLog->clear();
    });
}

void DashboardWin::createLabels()
{
    lLog = new QTextEdit();
    int lines = 4;
    int lineHeight = QFontMetrics(lLog->font()).height();
    int charWidth = QFontMetrics(lLog->font()).horizontalAdvance("W");
    int linesWidth = 20 * charWidth;
    lLog->setFixedWidth(linesWidth);
    lLog->setFixedHeight(lines * lineHeight + 12);
    lLog->setReadOnly(true);
    lLog->setPlaceholderText("Log");

    lAvgAirHum = new CustomLabel("Umidade média", "%", 0);
    lAvgAirTemp = new CustomLabel("Temp. Ar média", "°C", 0);
    lAvgWaterTemp = new CustomLabel("Temp. Água média", "°C", 0);
    lAvgUV = new CustomLabel("Índice UV médio", "°C", 0);
}

void DashboardWin::createCharts()
{
    cAirTemp = new CustomChart("Temperatura Atmo", 0, 45, 0);
    cAirHum = new CustomChart("Umidade do Ar", 0, 100, 0);
    cWaterTemp = new CustomChart("Temperatura da Água", -10, 100, 0);
    cUV = new CustomChart("Índice UV", 0, 10, 0);
}

void DashboardWin::createLayouts()
{
    //Layout para o botão de exportar e configurar
    m_buttonsLayout = new QVBoxLayout();
    m_buttonsLayout->addWidget(bConnect);
    m_buttonsLayout->addWidget(bData);
    m_buttonsLayout->addWidget(bSettings);
    m_buttonsLayout->addWidget(lLog);
    m_buttonsLayout->addWidget(bClearLog);
    m_buttonsLayout->addWidget(bEraseData);
    m_buttonsLayout->setAlignment(Qt::AlignBottom);

    //Layout para os dados médios e botões
    m_labelsLayout = new QVBoxLayout();
    m_labelsLayout->addWidget(lAvgAirHum);
    m_labelsLayout->addWidget(lAvgAirTemp);
    m_labelsLayout->addWidget(lAvgUV);
    m_labelsLayout->addWidget(lAvgWaterTemp);

    //Layout para separar labels de buttons
    m_labelsandButtonsLayout = new QVBoxLayout();
    m_labelsandButtonsLayout->addLayout(m_labelsLayout);
    m_labelsandButtonsLayout->addLayout(m_buttonsLayout);

    //Layout em grid para os gráficos
    m_chartsLayout = new QGridLayout();
    m_chartsLayout->addLayout(cAirHum, 0, 0);
    m_chartsLayout->addLayout(cAirTemp, 0, 1);
    m_chartsLayout->addLayout(cUV, 1, 0);
    m_chartsLayout->addLayout(cWaterTemp, 1, 1);
    m_chartsLayout->setSpacing(0);
    m_chartsLayout->setHorizontalSpacing(0);
    m_chartsLayout->setVerticalSpacing(0);
    m_chartsLayout->setContentsMargins(0, 0, 0, 0);

    //Separa o layout dos dados médios VS layout dos gráficos
    m_dataLayout = new QHBoxLayout();
    m_dataLayout->addLayout(m_labelsandButtonsLayout);
    m_dataLayout->addLayout(m_chartsLayout);
    m_dataLayout->setAlignment(Qt::AlignCenter | Qt::AlignHCenter | Qt::AlignVCenter);
    this->setLayout(m_dataLayout);
}

void DashboardWin::log(QString message)
{
    lLog->append(message);
}

void DashboardWin::plotData(const QString &topic, const QString &time, const QString &raw)
{
    //Processa raw que é CSV
    QStringList parts = raw.split(",");
    if (parts.size() < 4)
        return;

    float airHum     = parts[0].toFloat();
    float airTemp    = parts[1].toFloat();
    float waterTemp  = parts[2].toFloat();
    float uv         = parts[3].toFloat();

#ifdef QT_DEBUG
    QString msg = "(" +time + ") " + topic + ": " + raw;
    log(msg);
    qDebug() << msg;
#endif

    cAirHum->plotar(time, airHum);
    cAirTemp->plotar(time, airTemp);
    cWaterTemp->plotar(time, waterTemp);
    cUV->plotar(time, uv);

    lAvgAirHum->pushData(airHum);
    lAvgAirTemp->pushData(airTemp);
    lAvgWaterTemp->pushData(waterTemp);
    lAvgUV->pushData(uv);
}

void DashboardWin::updateMaxData(const long &maxData)
{
    lAvgAirTemp->updateMaxData(maxData);
    lAvgAirHum->updateMaxData(maxData);
    lAvgWaterTemp->updateMaxData(maxData);
    lAvgUV->updateMaxData(maxData);
    cAirTemp->updateMaxData(maxData);
    cAirHum->updateMaxData(maxData);
    cWaterTemp->updateMaxData(maxData);
    cUV->updateMaxData(maxData);
}

void DashboardWin::erase()
{
    lAvgAirTemp->erase();
    lAvgAirHum->erase();
    lAvgWaterTemp->erase();
    lAvgUV->erase();
    cAirTemp->erase();
    cAirHum->erase();
    cWaterTemp->erase();
    cUV->erase();
}
