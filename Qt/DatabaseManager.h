// DatabaseManager.h

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QtSql>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);


    // 차량 구동 제어
    Q_INVOKABLE void go();
    Q_INVOKABLE void back();
    Q_INVOKABLE void left();
    Q_INVOKABLE void right();
    Q_INVOKABLE void mid();
    Q_INVOKABLE void stop();



    // 차량 지시등(왼쪽, 오른쪽, 비상등, 상향등), 와이퍼 제어
    Q_INVOKABLE void turn_on_left();
    Q_INVOKABLE void turn_on_right();
    Q_INVOKABLE void turn_on_emer();
    Q_INVOKABLE void turn_on_high();
    Q_INVOKABLE void turn_on_wiper();

    Q_INVOKABLE void turn_off_left();
    Q_INVOKABLE void turn_off_right();
    Q_INVOKABLE void turn_off_emer();
    Q_INVOKABLE void turn_off_high();
    Q_INVOKABLE void turn_off_wiper();

    Q_INVOKABLE void startPolling();

signals:
    void sensingChanged(const QString &text);

private:
    QSqlDatabase db;
    QTimer timer;

    void init();
    void insertCommand(const QString &cmd_string, const QString &arg_string);
    void pollingQuery();
};

#endif // DATABASEMANAGER_H
