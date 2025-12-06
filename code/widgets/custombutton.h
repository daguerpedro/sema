#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QPushButton>
#include <QString>
#include <QIcon>

class CustomButton : public QPushButton
{
    Q_OBJECT

public:
    explicit CustomButton(QString text, QString iconPath, int iconW, int iconH, QWidget* parent = nullptr): QPushButton(text, parent)
    {
        QIcon icon(iconPath);
        setIcon(icon);
        setIconSize(QSize(iconW, iconH));
        setStyleSheet("text-align: left;");
        setLayoutDirection(Qt::LayoutDirection::LeftToRight);
    }

private:

};

#endif // CUSTOMBUTTON_H
