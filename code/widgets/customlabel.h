#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QVariant>
#include <QIcon>
#include <QList>

class CustomLabel : public QWidget
{
private:
    QLabel* m_lTitle;
    QLabel* m_lInfo;
    QBoxLayout* m_layout;

    QString m_dataUnity;

    QIcon* icon;
    QList<QVariant>* list;

    long m_maxValues;
public:
    explicit CustomLabel(QString title, QString iconPath, int iconW, int iconH, QBoxLayout::Direction direction, bool vertical = false, QWidget *parent = nullptr)
    {
        m_lTitle = new QLabel(title);

        icon = new QIcon(iconPath);
        m_lInfo = new QLabel();
        m_lInfo->setPixmap(icon->pixmap(iconW, iconH));
        m_lInfo->setAttribute(Qt::WA_TranslucentBackground);

        if(vertical)
            m_layout = new QVBoxLayout();
        else
            m_layout = new QHBoxLayout();

        m_layout->setDirection(direction);
        m_layout->addWidget(m_lTitle);
        m_layout->addWidget(m_lInfo);
        this->setLayout(m_layout);
    }

    explicit CustomLabel(QString title, QWidget *parent = nullptr)
    {
        m_lTitle = new QLabel(title);
        m_layout = new QHBoxLayout();
        m_layout->addWidget(m_lTitle);
        this->setLayout(m_layout);

    }

    explicit CustomLabel(QString title, QString dataUnity, long maxValues, bool vertical = true, QWidget *parent = nullptr)
    {
        m_dataUnity = dataUnity;
        m_maxValues = maxValues;

        m_lTitle = new QLabel(title);
        m_lInfo = new QLabel(QString("-%1").arg(m_dataUnity));

        if(vertical)
            m_layout = new QVBoxLayout();
        else
            m_layout = new QHBoxLayout();

        m_layout->addWidget(m_lTitle);
        m_layout->addWidget(m_lInfo);

        this->setContentsMargins(0,0,0,0);
        this->setLayout(m_layout);

        list = new QList<QVariant>();
    }

    void updateMaxData(const long& max)
    {
        m_maxValues = max;
    }

    void erase()
    {
        m_lInfo->setText(QString("-%1").arg(m_dataUnity));

        if(list == nullptr) return;
        list->clear();

        m_lInfo->update();
        m_lTitle->update();

        update();
    }

public slots:
    void pushData(QVariant data)
    {
        if(list == nullptr) return;
        list->append(data);
        while (list->count() > m_maxValues)
        {
            list->removeFirst();
        }

        double sum = 0.0;
        int valid_count = 0;

        for (auto& v : *list)
        {
            bool ok = false;
            double value = v.toDouble(&ok);

            if (ok)
            {
                sum += value;
                valid_count++;
            }
        }
        double average = 0.0;
        if (valid_count > 0)
            average = sum / valid_count;


        if(m_lInfo != nullptr && !m_dataUnity.isNull())
        {
            QString avgString = QString::number(average, 'f', 2);
            this->m_lInfo->setText(QString("%1%2").arg(avgString, m_dataUnity));
        }
    }
signals:
};

#endif // CUSTOMLABEL_H
