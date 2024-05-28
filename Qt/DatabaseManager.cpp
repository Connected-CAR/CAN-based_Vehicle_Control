// DatabaseManager.cpp

#include "DatabaseManager.h"
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    init();
}

void DatabaseManager::init()
{
    qDebug() << "INIT";
    // MySQL 접속 코드
    qDebug()<<"드라이버"<<QSqlDatabase::drivers();
    qDebug()<<QCoreApplication::libraryPaths();
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("13.124.218.110");
    db.setUserName("wooyoung");
    db.setPort(3306);
    db.setPassword("1234");
    db.setDatabaseName("wooDB");

    if (db.open()) {
        // 데이터베이스가 성공적으로 열린 경우
        qDebug() << "Database connected successfully.";
    } else {
        // 데이터베이스 연결이 실패한 경우
        qDebug() << "Failed to connect to database:" << db.lastError().text();
    }

    // Timer setting
    timer.setInterval(500); // 500ms
    connect(&timer, &QTimer::timeout, this, &DatabaseManager::pollingQuery);
}

void DatabaseManager::startPolling() {
    timer.start();
}

void DatabaseManager::pollingQuery()
{
    // 데이터베이스 연결 확인
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return;
    }

    // se_test 스키마에서 temperature와 humidity 값을 가져오기
    QSqlQuery query(db);
    if (!query.exec("SELECT temperature, humidity FROM sensing WHERE id = (SELECT MAX(id) FROM sensing)")) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return;
    }

    // 결과 처리
    QString text;
    if (query.next()) {
        text = QString("Temperature: %1°C \n Humidity: %2%")
                   .arg(query.value(0).toInt())
                   .arg(query.value(1).toInt());
    } else {
        text = "No data available";
    }
    //qDebug()<<text;
    emit sensingChanged(text);
    db.commit();
}

void DatabaseManager::insertCommand(const QString &cmd_string, const QString &arg_string)
{
    QDateTime time = QDateTime::currentDateTime();
    int is_finish = 0;

    QSqlQuery query;
    query.prepare("insert into command(time, cmd_string, arg_string, is_finish) values (:time, :cmd_string, :arg_string, :is_finish)");
    query.bindValue(":time", time);
    query.bindValue(":cmd_string", cmd_string);
    query.bindValue(":arg_string", arg_string);
    query.bindValue(":is_finish", is_finish);
    query.exec();
    db.commit();
}


void DatabaseManager::go()
{
    qDebug() << "go";
    insertCommand("go", "0");
}

void DatabaseManager::back()
{
    qDebug() << "back";
    insertCommand("back", "0");
}

void DatabaseManager::left()
{
    qDebug() << "left";
    insertCommand("left", "0");
}

void DatabaseManager::right()
{
    qDebug() << "right";
    insertCommand("right", "0");
}

void DatabaseManager::mid()
{
    qDebug() << "mid";
    insertCommand("mid", "0");
}

void DatabaseManager::stop()
{
    qDebug() << "stop";
    insertCommand("stop", "0");
}

void DatabaseManager::turn_on_left()
{
    qDebug() << "turn_on_left";
    insertCommand("turn_on_left", "0");
}

void DatabaseManager::turn_on_right()
{
    qDebug() << "turn_on_right";
    insertCommand("turn_on_right", "0");
}

void DatabaseManager::turn_on_emer()
{
    qDebug() << "turn_on_emer";
    insertCommand("turn_on_emer", "0");
}

void DatabaseManager::turn_on_high()
{
    qDebug() << "turn_on_high";
    insertCommand("turn_on_high", "0");
}

void DatabaseManager::turn_on_wiper()
{
    qDebug() << "turn_on_wiper";
    insertCommand("turn_on_wiper", "0");
}

void DatabaseManager::turn_off_left()
{
    qDebug() << "turn_off_left";
    insertCommand("turn_off_left", "0");
}

void DatabaseManager::turn_off_right()
{
    qDebug() << "turn_off_right";
    insertCommand("turn_off_right", "0");
}

void DatabaseManager::turn_off_emer()
{
    qDebug() << "turn_off_emer";
    insertCommand("turn_off_emer", "0");
}

void DatabaseManager::turn_off_high()
{
    qDebug() << "turn_off_high";
    insertCommand("turn_off_high", "0");
}

void DatabaseManager::turn_off_wiper()
{
    qDebug() << "turn_off_wiper";
    insertCommand("turn_off_wiper", "0");
}
