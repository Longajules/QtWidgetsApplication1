#include "QtWidgetsApplication1.h"

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.centralWidget->setStyleSheet(
        "background-color: #1e1e2e;"
    );

    ui.gridLayoutWidget->setStyleSheet(
        "background-color: #2a2a3e;"
        "border-radius: 8px;"
        "padding: 8px;"
    );

    ui.username->setStyleSheet(
        "background-color: #313145;"
        "color: #cdd6f4;"
        "border: 1px solid #45475a;"
        "border-radius: 4px;"
        "padding: 4px 8px;"
        "font-size: 13px;"
    );

    ui.password->setStyleSheet(
        "background-color: #313145;"
        "color: #cdd6f4;"
        "border: 1px solid #45475a;"
        "border-radius: 4px;"
        "padding: 4px 8px;"
        "font-size: 13px;"
    );

    ui.btnConnexion->setStyleSheet(
        "background-color: #89b4fa;"
        "color: #1e1e2e;"
        "border: none;"
        "border-radius: 4px;"
        "padding: 5px 10px;"
        "font-weight: bold;"
        "font-size: 13px;"
    );

    ui.btnInscription->setStyleSheet(
        "background-color: #a6e3a1;"
        "color: #1e1e2e;"
        "border: none;"
        "border-radius: 4px;"
        "padding: 5px 10px;"
        "font-weight: bold;"
        "font-size: 13px;"
    );

    ui.messageList->setStyleSheet(
        "background-color: #181825;"
        "color: #cdd6f4;"
        "border: 1px solid #45475a;"
        "border-radius: 6px;"
        "padding: 4px;"
        "font-size: 13px;"
    );

    ui.zoneSaisie->setStyleSheet(
        "background-color: #313145;"
        "color: #cdd6f4;"
        "border: 1px solid #45475a;"
        "border-radius: 4px;"
        "padding: 4px 8px;"
        "font-size: 13px;"
    );

    ui.btnSend->setStyleSheet(
        "background-color: #cba6f7;"
        "color: #1e1e2e;"
        "border: none;"
        "border-radius: 4px;"
        "padding: 5px 10px;"
        "font-weight: bold;"
        "font-size: 13px;"
    );

    ui.menuBar->setStyleSheet(
        "background-color: #181825;"
        "color: #cdd6f4;"
        "border-bottom: 1px solid #45475a;"
    );

    ui.mainToolBar->setStyleSheet(
        "background-color: #181825;"
        "border-bottom: 1px solid #45475a;"
    );

    ui.statusBar->setStyleSheet(
        "background-color: #181825;"
        "color: #6c7086;"
        "font-size: 11px;"
    );

    manager = new QNetworkAccessManager(this);

    cookieJar = new QNetworkCookieJar(this);
    manager->setCookieJar(cookieJar);

    model = new QStringListModel(this);
    ui.messageList->setModel(model);

    socket = new QWebSocket();

    connect(socket, &QWebSocket::connected, this, []() {
        qDebug() << "WebSocket connecté";
        });

    connect(socket, &QWebSocket::textMessageReceived,
        this, &QtWidgetsApplication1::onMessageReceived);

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

    QString text = obj.value("username").toString() + " : " + obj.value("message").toString();

    QStringList messages = model->stringList();
    messages.append(text);
    model->setStringList(messages);
}

void QtWidgetsApplication1::connectWebSocket()
{
    QUrl url("ws://172.29.19.8:3003");

    QList<QNetworkCookie> cookies =
        cookieJar->cookiesForUrl(QUrl("http://172.29.19.8:3003"));

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
    QUrl url("http://172.29.19.8:3003/connexion");
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

        connectWebSocket();


        ui.statusLogged->setText(message);

        reply->deleteLater();
        });

}

void QtWidgetsApplication1::inscription()
{
    QUrl url("http://172.29.19.8:3003/inscription");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = ui.username->text();
    json["password"] = ui.password->text();

    QNetworkReply* reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {

        QByteArray data = reply->readAll();
        qDebug() << "Réponse inscription :" << data;
        

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        QString message = obj.value("message").toString();
        qDebug() << message;

        ui.statusLogged->setText(message);

        reply->deleteLater();
        });
}
