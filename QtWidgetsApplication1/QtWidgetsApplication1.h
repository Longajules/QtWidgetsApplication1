#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include <QApplication>
#include <QListView>
#include <QStringListModel>
#include <qwebsocket.h>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkCookieJar>
#include <QNetworkCookie>

class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget* parent = nullptr);
    ~QtWidgetsApplication1();


private:
    Ui::QtWidgetsApplication1Class ui;

	QWebSocket* socket;
    QNetworkAccessManager* manager;
    QNetworkCookieJar* cookieJar;
    QStringListModel* model;


public slots:
    void inscription();
    void connexion();
    void connectWebSocket();
    void sendMessage();
    void onMessageReceived(const QString& message);

};

