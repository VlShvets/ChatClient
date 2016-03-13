#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    InputDialog(QWidget *_parent = 0);

    QString hostName() const;
    quint16 port() const;

private:
    QLineEdit *txtHostName;
    QLineEdit *txtPort;
};

#endif // INPUTDIALOG_H
