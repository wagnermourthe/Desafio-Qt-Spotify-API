#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtNetworkAuth>
#include <QJsonDocument>
#include <QJsonObject>
#include "clientid.h"
#include <QDesktopServices>
#include <QtDebug>





MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),isGranted(false)
{
    ui->setupUi(this);

    ui->comboBox->addItem("artist");
    ui->comboBox->addItem("track");
    ui->comboBox->addItem("album");

    auto replyHandler = new QOAuthHttpServerReplyHandler(8080, this);
    spotify.setReplyHandler(replyHandler);
    spotify.setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    spotify.setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));
    spotify.setClientIdentifier(clientId);
    spotify.setClientIdentifierSharedKey(clientSecret);
    spotify.setScope("user-read-private user-top-read playlist-read-private playlist-modify-public playlist-modify-private user-read-currently-playing user-read-playback-state user-modify-playback-state");
    //oauth2.setScope("identity read");


    connect(&spotify, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    connect(&spotify, &QOAuth2AuthorizationCodeFlow::statusChanged, this, &MainWindow::authStatusChanged);

    connect(&spotify, &QOAuth2AuthorizationCodeFlow::granted, this, &MainWindow::granted);

    spotify.grant();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetDevices()
{
    //__//GET AVAILABLE DEVICES//__//
    QUrl u ("https://api.spotify.com/v1/me/player/devices");

    auto reply = spotify.get(u);

    connect(reply, &QNetworkReply::finished, [=]() {

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if ( !statusCode.isValid() )
        return;

//        int status = statusCode.toInt();

//        ui->teOutput->appendPlainText(QString::number(status));


        if (reply->error() != QNetworkReply::NoError) {
            ui->teOutput->appendPlainText(reply->errorString());
            return;
        }


        const auto data = reply->readAll();
//        ui->teOutput->appendPlainText(data);

        const auto document = QJsonDocument::fromJson(data);
        const auto root = document.object();
        const auto devices = root.value("devices").toArray();
        const auto devicesArray = root.value("devices");
        devicesNmbr = devices.size();
        for(unsigned int i=0; i<devicesNmbr; i++ )
        {
        deviceID[i] = devicesArray[i].toObject().value("id").toString();
        deviceName[i] = devicesArray[i].toObject().value("name").toString();
        qDebug() << deviceName[i];
        qDebug() << deviceID[i];
        qDebug() << devicesNmbr;
        }




        reply->deleteLater();
    });
}

void MainWindow::GetUserInfo()
{
    //__//GET USER INFORMATION//__//

    QUrl u ("https://api.spotify.com/v1/me");

    auto reply = spotify.get(u);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            ui->teOutput->appendPlainText(reply->errorString());
            return;
        }
        const auto data = reply->readAll();
//        ui->teOutput->appendPlainText(data);

        const auto document = QJsonDocument::fromJson(data);
        const auto root = document.object();
        userName = root.value("id").toString();

        ui->teOutput->appendPlainText("Username: " + userName);

        reply->deleteLater();
    });

}

void MainWindow::granted ()
{
    ui->teOutput->appendPlainText("Signal granted received");

    QString token = spotify.token();
    ui->teOutput->appendPlainText("Token: " + token);

    ui->actionGet_Playlists->setEnabled(true);
    ui->actionGet_User_Information->setEnabled(true);
    isGranted = true;

    MainWindow::GetUserInfo();

    MainWindow::GetDevices();
}

void MainWindow::authStatusChanged(QAbstractOAuth::Status status)
{
    QString s;
    if (status == QAbstractOAuth::Status::Granted)
        s = "granted";

    if (status == QAbstractOAuth::Status::TemporaryCredentialsReceived) {
        s = "temp credentials";
        //oauth2.refreshAccessToken();
    }

//    ui->teOutput->appendPlainText("Auth Status changed: " + s +  "\n");
}

void MainWindow::on_SearchText_returnPressed()
{
    MainWindow::on_SearchButton_clicked();
}

void MainWindow::on_SearchButton_clicked()
{
    QString SearchInput = ui->SearchText->text();

    qDebug() << SearchInput;

    QUrl u ("https://api.spotify.com/v1/search" );

    QUrlQuery myQuery;

    myQuery.addQueryItem("q",SearchInput);

    QString TypeInput = ui->comboBox->currentText();
    myQuery.addQueryItem("type",TypeInput);

    u.setQuery(myQuery);


    auto reply = spotify.get(u);

    connect(reply, &QNetworkReply::finished, [=]()
        {
        if (reply->error() != QNetworkReply::NoError)
        {
            ui->teOutput->appendPlainText(reply->errorString());
            return;
        }
        const auto data = reply->readAll();

        ui->teOutput->appendPlainText(data);


        reply->deleteLater();
        });



}

void MainWindow::on_PreviousButton_clicked()
{
    QUrl u ("https://api.spotify.com/v1/me/player/previous" );
    auto reply = spotify.post(u);

    connect(reply, &QNetworkReply::finished, [=](){
        reply->deleteLater();
        });

}

void MainWindow::on_NextButton_clicked()
{
    QUrl u ("https://api.spotify.com/v1/me/player/next" );
    auto reply = spotify.post(u);

    connect(reply, &QNetworkReply::finished, [=](){
        reply->deleteLater();
        });

}

void MainWindow::on_PauseButton_clicked()
{
    QUrl u ("https://api.spotify.com/v1/me/player/pause" );
    auto reply = spotify.put(u);

    connect(reply, &QNetworkReply::finished, [=]()
        {
        reply->deleteLater();
        });

}

void MainWindow::on_PlayButton_clicked()
{
    QUrl u ("https://api.spotify.com/v1/me/player/play" );

    auto reply = spotify.put(u);

    connect(reply, &QNetworkReply::finished, [=]()
        {

        //teste
        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if ( !statusCode.isValid() )
        return;

        int status = statusCode.toInt();

        ui->teOutput->appendPlainText(QString::number(status));

        if (reply->error() != QNetworkReply::NoError)
        {
            ui->teOutput->appendPlainText(reply->errorString());

            const auto data = reply->readAll();

            ui->teOutput->appendPlainText(data);

            return;
        }
        const auto data = reply->readAll();

        ui->teOutput->appendPlainText(data);

        reply->deleteLater();
        });

}


void MainWindow::on_newPlaylistButton_clicked()
{
    QString NameInput = ui->NewPlaylistName->text();

    qDebug() << NameInput;

    QUrl u ("https://api.spotify.com/v1/users/" + userName + "/playlists");

    QUrlQuery myQuery;

    myQuery.addQueryItem("name",NameInput);

    u.setQuery(myQuery);


    auto reply = spotify.post(u);

    connect(reply, &QNetworkReply::finished, [=]()
        {

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if ( !statusCode.isValid() )
        return;

        int status = statusCode.toInt();

        ui->teOutput->appendPlainText(QString::number(status));

        if (reply->error() != QNetworkReply::NoError)
        {
            ui->teOutput->appendPlainText(reply->errorString());
            return;
        }
        const auto data = reply->readAll();

        ui->teOutput->appendPlainText(data);


        reply->deleteLater();
        });

}


void MainWindow::on_addButton_clicked()
{

}


void MainWindow::on_removeButton_clicked()
{

}

