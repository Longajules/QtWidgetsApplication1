#include "QtWidgetsApplication1.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QDebug>

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // 🔹 HTTP
    manager = new QNetworkAccessManager(this);

    cookieJar = new QNetworkCookieJar(this);
    manager->setCookieJar(cookieJar);

    // 🔹 UI (liste messages)
    model = new QStringListModel(this);
    ui.messageList->setModel(model);

    // 🔹 WebSocket
    socket = new QWebSocket();

    connect(socket, &QWebSocket::connected, this, []() {
        qDebug() << "WebSocket connecté";
        });

    connect(socket, &QWebSocket::textMessageReceived,
        this, &QtWidgetsApplication1::onMessageReceived);

    // 🔹 boutons UI (à adapter à ton designer)
    connect(ui.btnInscription, &QPushButton::clicked, this, &QtWidgetsApplication1::inscription);
    connect(ui.btnConnexion, &QPushButton::clicked, this, &QtWidgetsApplication1::connexion);
    connect(ui.btnSend, &QPushButton::clicked, this, &QtWidgetsApplication1::sendMessage);
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{
    if (socket) {
        socket->close();
        delete socket;
    }
}

void QtWidgetsApplication1::sendMessage()
{
    if (socket && socket->isValid()) {

        QJsonObject json;
        
        json["message"] = ui.zoneSaisie->text();

        socket->sendTextMessage(
            QJsonDocument(json).toJson(QJsonDocument::Compact)
        );
    }
}

void QtWidgetsApplication1::onMessageReceived(const QString& message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();

    QString text = obj.value("author").toString() + " : " + obj.value("message").toString();

    QStringList messages = model->stringList();
    messages.append(text);
    model->setStringList(messages);
}

void QtWidgetsApplication1::connectWebSocket()
{
    QUrl url("ws://localhost:3003");

    QList<QNetworkCookie> cookies =
        cookieJar->cookiesForUrl(QUrl("http://localhost:3003"));

    QString cookieHeader;
    for (const QNetworkCookie& cookie : cookies) {
        cookieHeader += cookie.name() + "=" + cookie.value() + "; ";
    }

    QNetworkRequest request(url);
    request.setRawHeader("Cookie", cookieHeader.toUtf8());

    socket->open(request);

    qDebug() << "Cookies envoyés :" << cookieHeader;
}

void QtWidgetsApplication1::connexion()
{
    QUrl url("http://localhost:3003/connexion");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = ui.username->text();
    json["password"] = ui.password->text();

    QNetworkReply* reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Erreur :" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        qDebug() << "Réponse connexion :" << data;

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        QString message = obj.value("message").toString();
        qDebug() << message;

        // 🔥 Connexion WebSocket après login
        connectWebSocket();

        reply->deleteLater();
        });
}

void QtWidgetsApplication1::inscription()
{
    QUrl url("http://localhost:3003/inscription");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = ui.username->text();
    json["password"] = ui.password->text();

    QNetworkReply* reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [reply]() {

        QByteArray data = reply->readAll();
        qDebug() << "Réponse inscription :" << data;

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        QString message = obj.value("message").toString();
        qDebug() << message;

        reply->deleteLater();
        });
}
