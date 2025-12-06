#ifndef CUSTOMCHART_H
#define CUSTOMCHART_H

#include <QStackedLayout>
#include <QLabel>
#include <QIcon>
#include <QChartView>
#include <QString>
#include <QDebug>
#include <QSize>
#include <QLineSeries>
#include <QChart>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QVariant>
#include <QDateTime>
#include <QPainter>

class CustomChart : public QStackedLayout
{
    Q_OBJECT
public:
    CustomChart(const QString& Title, QVariant minY, QVariant maxY, long maxData)
    {
        defaults(Title, minY, maxY, maxData);
    }

    CustomChart(QString Title, QVariant minY, QVariant maxY, long maxData, QString iconPath, int iconW, int iconH)
    {
        icon = new QIcon(iconPath);
        label = new QLabel();
        label->setPixmap(icon->pixmap(QSize(iconW, iconH)));
        label->setAttribute(Qt::WA_TranslucentBackground);
        label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        label->setContentsMargins(15, 15, 0, 0);
        defaults(Title, minY, maxY, maxData);
        addWidget(label);
        setStackingMode(QStackedLayout::StackAll);
    }

    void plotar(const QString& time, QVariant value)
    {
        QDateTime ts = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
        qint64 x = ts.toMSecsSinceEpoch();

        serie->append(static_cast<qreal>(x), value.toReal());

        while (serie->count() > m_maxData) {
            serie->remove(0);
        }

        if (serie->count() > 0) {
            QDateTime maxTs = QDateTime::fromMSecsSinceEpoch(x);
            QPointF minPoint = serie->at(0);
            QDateTime minTs = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(minPoint.x()));
            ax->setRange(minTs, maxTs);
        }

        ay->applyNiceNumbers();
        chart->update();
        view->update();
    }

    void updateMaxData(const long& max)
    {
        m_maxData = max;
    }

    void erase()
    {
        serie->clear();
        ay->applyNiceNumbers();
        chart->update();
        view->update();
    }

private:
    void defaults(const QString& Title, QVariant minY, QVariant maxY, long maxData)
    {
        m_maxData = maxData;

        chart = new QChart();
        serie = new QLineSeries();
        chart->addSeries(serie);

        ax = new QDateTimeAxis();
        ay = new QValueAxis();

        chart->setTitle(Title);
        chart->legend()->setVisible(false);

        ay->setMin(minY.toReal());
        ay->setMax(maxY.toReal());
        ax->setFormat("hh:mm:ss");

        chart->addAxis(ax, Qt::AlignBottom);
        chart->addAxis(ay, Qt::AlignLeft);

        serie->attachAxis(ax);
        serie->attachAxis(ay);

        view = new QChartView();
        view->setChart(chart);
        view->setRenderHint(QPainter::Antialiasing);
        addWidget(view);
    };

    long m_maxData;

    QDateTimeAxis *ax;
    QValueAxis *ay;

    QLineSeries *serie;
    QChartView* view;
    QChart* chart;
    QLabel* label = nullptr;
    QIcon* icon = nullptr;
};

#endif // CUSTOMCHART_H
